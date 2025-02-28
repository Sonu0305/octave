////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2013-2024 The Octave Project Developers
//
// See the file COPYRIGHT.md in the top-level directory of this
// distribution or <https://octave.org/copyright/>.
//
// This file is part of Octave.
//
// Octave is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Octave is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Octave; see the file COPYING.  If not, see
// <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////

#if defined (HAVE_CONFIG_H)
#  include "config.h"
#endif

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QDirIterator>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStatusBar>
#include <QTableView>
#include <QTextStream>
#include <QTimer>

#include "find-files-dialog.h"
#include "find-files-model.h"
#include "gui-preferences-global.h"
#include "gui-preferences-ff.h"
#include "gui-settings.h"
#include "gui-utils.h"

OCTAVE_BEGIN_NAMESPACE(octave)

find_files_dialog::find_files_dialog (QWidget *p)
  : QDialog (p)
{
  gui_settings settings;

  setWindowTitle (tr ("Find Files"));
  setWindowIcon (settings.icon ("edit-find"));

  m_dir_iterator = nullptr;

  m_timer = new QTimer (this);
  connect (m_timer, &QTimer::timeout,
           this, &find_files_dialog::look_for_files);

  QLabel *file_name_label = new QLabel (tr ("Named:"));
  m_file_name_edit = new QComboBox;
  m_file_name_edit->setToolTip (tr ("Enter the filename search patterns.\n"
                                    "Several different patterns can be\n"
                                    "separated by ';', e.g. '*.cc ; *.h'"));
  m_file_name_edit->setEditable (true);
  m_file_name_edit->setMaxCount (m_mru_length);

//  m_file_name_edit->setItemText (0, settings.string_value (ff_file_name));
  file_name_label->setBuddy (m_file_name_edit);

  QStringList mru = settings.value (ff_file_name.settings_key ()).toStringList ();
  while (mru.length () > m_mru_length)
    mru.removeLast ();
  m_file_name_edit->addItems (mru);

  QLabel *start_dir_label = new QLabel (tr ("Start in:"));

  m_start_dir_edit = new QComboBox;
  m_start_dir_edit->setEditable (true);
  m_start_dir_edit->setMaxCount (m_mru_length);
  m_start_dir_edit->setToolTip (tr ("Enter the start directory"));
  start_dir_label->setBuddy (m_start_dir_edit);

  mru = settings.value (ff_start_dir.settings_key ()).toStringList ();
  while (mru.length () > m_mru_length)
    mru.removeLast ();
  m_start_dir_edit->addItems (mru);

  m_browse_button = new QPushButton (tr ("Browse..."));
  m_browse_button->setToolTip (tr ("Browse for start directory"));
  connect (m_browse_button, &QPushButton::clicked,
           this, &find_files_dialog::browse_folders);

  m_current_dir_button = new QPushButton (tr ("Current Dir"));
  m_current_dir_button->setToolTip (tr ("Set start directory to current directory"));
  connect (m_current_dir_button, &QPushButton::clicked,
           this, &find_files_dialog::start_dir_to_cwd);

  m_recurse_dirs_check = new QCheckBox (tr ("Search subdirectories"));
  m_recurse_dirs_check->setToolTip (tr ("Search recursively through directories for matching files"));
  m_recurse_dirs_check->setChecked (settings.bool_value (ff_recurse_dirs));

  m_include_dirs_check = new QCheckBox (tr ("Include directory names"));
  m_include_dirs_check->setToolTip (tr ("Include matching directories in search results"));
  m_include_dirs_check->setChecked (settings.bool_value (ff_include_dirs));

  m_name_case_check = new QCheckBox (tr ("Ignore case"));
  m_name_case_check->setToolTip (tr ("Perform case insensitive match"));
  m_name_case_check->setChecked (settings.bool_value (ff_name_case));

  m_contains_text_check = new QCheckBox (tr ("Contains text:"));
  m_contains_text_check->setToolTip (tr ("Include only files containing specified text in search results"));
  m_contains_text_check->setChecked (settings.bool_value (ff_check_text));

  m_contains_text_edit = new QComboBox ();
  m_contains_text_edit->setEditable (true);
  m_contains_text_edit->setMaxCount (m_mru_length);
  m_contains_text_edit->setToolTip (tr ("Text to match"));

  mru = settings.value (ff_contains_text.settings_key ()).toStringList ();
  while (mru.length () > m_mru_length)
    mru.removeLast ();
  m_contains_text_edit->addItems (mru);

  m_content_case_check = new QCheckBox (tr ("Ignore case"));
  m_content_case_check->setToolTip (tr ("Perform case insensitive match"));
  m_content_case_check->setChecked (settings.bool_value (ff_content_case));

  find_files_model *model = new find_files_model (this);
  connect (model, &find_files_model::rowsInserted,
           this, &find_files_dialog::handle_rows_inserted);

  m_file_list = new QTableView;
  m_file_list->setWordWrap (false);
  m_file_list->setModel (model);
  m_file_list->setShowGrid (false);
  m_file_list->setSelectionBehavior (QAbstractItemView::SelectRows);
  m_file_list->setSelectionMode (QAbstractItemView::SingleSelection);
  m_file_list->setAlternatingRowColors (true);
  m_file_list->setSortingEnabled (true);
  m_file_list->horizontalHeader ()->restoreState (settings.value (ff_column_state.settings_key ()).toByteArray ());
  m_file_list->horizontalHeader ()->setSortIndicatorShown (true);
  m_file_list->horizontalHeader ()->setSectionsClickable (true);
  m_file_list->horizontalHeader ()->setStretchLastSection (true);
  m_file_list->horizontalHeader ()->setSectionsMovable (true);
  m_file_list->sortByColumn (settings.int_value (ff_sort_files_by_column),
                             static_cast<Qt::SortOrder>
                             (settings.uint_value (ff_sort_files_by_order)));
  // FIXME: use value<Qt::SortOrder> instead of static cast after
  //        dropping support of Qt 5.4

    connect (m_file_list, &QTableView::doubleClicked,
           this, &find_files_dialog::item_double_clicked);

  m_status_bar = new QStatusBar;
  m_status_bar->showMessage (tr ("Idle."));

  m_find_button = new QPushButton (tr ("Find"));
  m_find_button->setToolTip (tr ("Start search for matching files"));
  connect (m_find_button, &QPushButton::clicked,
           this, &find_files_dialog::start_find);

  m_stop_button = new QPushButton (tr ("Stop"));
  m_stop_button->setToolTip (tr ("Stop search"));
  m_stop_button->setEnabled (false);
  connect (m_stop_button, &QPushButton::clicked,
           this, &find_files_dialog::stop_find);

  // layout everything
  QDialogButtonBox *button_box = new QDialogButtonBox (Qt::Vertical);
  button_box->addButton (m_find_button, QDialogButtonBox::ActionRole);
  button_box->addButton (m_stop_button, QDialogButtonBox::ActionRole);

  // add dialog close button
  m_close_button = button_box->addButton (QDialogButtonBox::Close);
  connect (button_box, &QDialogButtonBox::rejected,
           this, &find_files_dialog::close);

  // name options
  const QString gbox_style_sheet ("QGroupBox { font-weight: bold; } ");
  QGroupBox *name_group = new QGroupBox (tr ("Filename/Location"));
  name_group->setStyleSheet(gbox_style_sheet);
  QGridLayout *name_layout = new QGridLayout;

  name_group->setLayout (name_layout);

  name_layout->addWidget (file_name_label, 0, 0, 1, 1);
  name_layout->addWidget (m_file_name_edit, 0, 1, 1, 3);

  name_layout->addWidget (start_dir_label, 1, 0);
  name_layout->addWidget (m_start_dir_edit, 1, 1, 1, 2);
  name_layout->addWidget (m_browse_button, 1, 3, 1, 1);
  name_layout->addWidget (m_current_dir_button, 2, 3, 1, 1);
  name_layout->setColumnStretch (1, 1);

  QHBoxLayout *name_options_layout = new QHBoxLayout;
  name_options_layout->addWidget (m_recurse_dirs_check);
  name_options_layout->addWidget (m_include_dirs_check);
  name_options_layout->addWidget (m_name_case_check);
  name_layout->addLayout (name_options_layout, 2, 0, 1, 2);

  // content options
  QGroupBox *content_group = new QGroupBox (tr ("File contents"));
  content_group->setStyleSheet(gbox_style_sheet);
  QGridLayout *content_layout = new QGridLayout;
  content_group->setLayout (content_layout);
  content_layout->addWidget (m_contains_text_check, 0, 0, 1, 1);
  content_layout->addWidget (m_contains_text_edit, 0, 1, 1, 1);
  content_layout->setColumnStretch (1, 1);
  content_layout->addWidget (m_content_case_check, 0, 2, 1, 1);

  // results
  QLabel *results_hint = new QLabel (tr ("Results: Double click opens the file"
                                         " or sets the directory"));
  QGroupBox *results_group = new QGroupBox (tr ("Search results"));
  results_group->setStyleSheet(gbox_style_sheet);
  QVBoxLayout *results_layout = new QVBoxLayout ();
  results_group->setLayout (results_layout);
  results_layout->addWidget (results_hint);
  results_layout->addWidget (m_file_list);

  // main layout
  QGridLayout *main_layout = new QGridLayout;
  main_layout->addWidget (name_group, 0, 0);
  main_layout->addWidget (content_group, 1, 0);
  main_layout->addWidget (button_box, 0, 1, 3, 1);
  main_layout->addWidget (results_group, 2, 0);
  main_layout->setRowStretch (2, 1);
  main_layout->addWidget (m_status_bar, 3, 0, 1, -1);

  setLayout (main_layout);

  connect (this, &find_files_dialog::finished,
           this, &find_files_dialog::handle_done);

  m_find_button->setFocus(Qt::OtherFocusReason);

  if (settings.contains (ff_geometry.settings_key ()))
    restoreGeometry (settings.byte_array_value (ff_geometry));
}

find_files_dialog::~find_files_dialog ()
{
  delete m_dir_iterator;
}

void
find_files_dialog::save_settings ()
{
  gui_settings settings;

  int sort_column = m_file_list->horizontalHeader ()->sortIndicatorSection ();
  Qt::SortOrder sort_order
    = m_file_list->horizontalHeader ()->sortIndicatorOrder ();
  settings.setValue (ff_sort_files_by_column.settings_key (), sort_column);
  settings.setValue (ff_sort_files_by_order.settings_key (), sort_order);
  settings.setValue (ff_column_state.settings_key (), m_file_list->horizontalHeader ()->saveState ());

  settings.setValue (ff_geometry.settings_key (), saveGeometry ());

  settings.setValue (ff_recurse_dirs.settings_key (), m_recurse_dirs_check->text ());
  settings.setValue (ff_include_dirs.settings_key (), m_include_dirs_check->text ());
  settings.setValue (ff_name_case.settings_key (), m_name_case_check->text ());

  settings.setValue (ff_check_text.settings_key (), m_contains_text_check->isChecked ());
  settings.setValue (ff_content_case.settings_key (), m_content_case_check->isChecked ());

  combobox_update (m_file_name_edit, m_mru_length);
  combobox_update (m_start_dir_edit, m_mru_length);
  combobox_update (m_contains_text_edit, m_mru_length);

  QStringList mru;
  for (int i = 0; i < m_file_name_edit->count (); i++)
    mru.append (m_file_name_edit->itemText (i));
  settings.setValue (ff_file_name.settings_key (), mru);

  mru.clear ();
  for (int i = 0; i < m_start_dir_edit->count (); i++)
    mru.append (m_start_dir_edit->itemText (i));
  settings.setValue (ff_start_dir.settings_key (), mru);

  mru.clear ();
  for (int i = 0; i < m_contains_text_edit->count (); i++)
    mru.append (m_contains_text_edit->itemText (i));
  settings.setValue (ff_contains_text.settings_key (), mru);

  settings.sync ();
}

void
find_files_dialog::set_search_dir (const QString& dir)
{
  stop_find ();
  combobox_insert_current_item (m_start_dir_edit, dir);
}

void
find_files_dialog::start_find ()
{
  stop_find ();

  combobox_update (m_file_name_edit, m_mru_length);
  combobox_update (m_start_dir_edit, m_mru_length);
  combobox_update (m_contains_text_edit, m_mru_length);

  find_files_model *m = static_cast<find_files_model *> (m_file_list->model ());
  m->clear ();

  QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags;
  if (m_recurse_dirs_check->isChecked ())
    flags |= QDirIterator::Subdirectories;

  QDir::Filters filters = QDir::Dirs | QDir::NoDotAndDotDot | QDir::Files;
  if (! m_name_case_check->isChecked ())
    filters |= QDir::CaseSensitive;

  const QStringList nameFilters =
    m_file_name_edit->currentText ().split (QRegularExpression("\\s*;\\s*"),
#if defined (HAVE_QT_SPLITBEHAVIOR_ENUM)
                                            Qt::SkipEmptyParts);
#else
                                            QString::SkipEmptyParts);
#endif

  if (m_dir_iterator)
    delete m_dir_iterator;

  m_dir_iterator = new QDirIterator (m_start_dir_edit->currentText (),
                                     nameFilters, filters, flags);

  // enable/disable widgets
  m_find_button->setEnabled (false);
  m_stop_button->setEnabled (true);
  m_close_button->setEnabled (false);
  m_browse_button->setEnabled (false);
  m_current_dir_button->setEnabled (false);
  m_start_dir_edit->setEnabled (false);
  m_file_name_edit->setEnabled (false);
  m_recurse_dirs_check->setEnabled (false);
  m_include_dirs_check->setEnabled (false);
  m_name_case_check->setEnabled (false);
  m_contains_text_check->setEnabled (false);
  m_content_case_check->setEnabled (false);
  m_contains_text_edit->setEnabled (false);

  m_status_bar->showMessage (tr ("Searching..."));
  m_timer->start (0);
}

void
find_files_dialog::stop_find ()
{
  m_timer->stop ();

  m_find_button->setEnabled (true);
  m_stop_button->setEnabled (false);
  m_close_button->setEnabled (true);
  m_browse_button->setEnabled (true);
  m_current_dir_button->setEnabled (true);
  m_start_dir_edit->setEnabled (true);
  m_file_name_edit->setEnabled (true);
  m_recurse_dirs_check->setEnabled (true);
  m_include_dirs_check->setEnabled (true);
  m_name_case_check->setEnabled (true);
  m_contains_text_check->setEnabled (true);
  m_content_case_check->setEnabled (true);
  m_contains_text_edit->setEnabled (true);

  find_files_model *m = static_cast<find_files_model *> (m_file_list->model ());
  int rows = m->rowCount ();

  m_file_list->resizeRowToContents (rows-1);  // Resize last row

  QString res_str = QString (tr ("%1 match (es)")).arg (rows);
  m_status_bar->showMessage (res_str);
}

void
find_files_dialog::browse_folders ()
{
  int opts = 0;  // No options by default.

  // FIXME: Remove, if for all common KDE versions (bug #54607) is resolved.

  gui_settings settings;

  if (! settings.bool_value (global_use_native_dialogs))
    opts = QFileDialog::DontUseNativeDialog;

  QString dir =
    QFileDialog::getExistingDirectory (this, tr ("Set search directory"),
                                       m_start_dir_edit->itemText (0),
                                       QFileDialog::Option (opts));

  if (! dir.isEmpty ())
    combobox_insert_current_item  (m_start_dir_edit, dir);
}

void
find_files_dialog::start_dir_to_cwd ()
{
  combobox_insert_current_item (m_start_dir_edit, QDir::currentPath ());
}

void
find_files_dialog::item_double_clicked (const QModelIndex& idx)
{
  find_files_model *m = static_cast<find_files_model *> (m_file_list->model ());

  QFileInfo info = m->fileInfo (idx);

  if (idx.column () == 1)
    {
      // clicked in directory part
      Q_EMIT dir_selected (info.absolutePath ());
    }
  else
    {
      // clicked in filename part
      if (info.isDir ())
        Q_EMIT dir_selected (info.absoluteFilePath ());
      else
        Q_EMIT file_selected (info.absoluteFilePath ());
    }
}

void
find_files_dialog::look_for_files ()
{
  if (m_dir_iterator && m_dir_iterator->hasNext ())
    {
      QFileInfo info (m_dir_iterator->next ());

      find_files_model *m
        = static_cast<find_files_model *> (m_file_list->model ());

      if (is_match (info))
        m->addFile (info);
    }
  else
    {
      stop_find ();
    }
}

void
find_files_dialog::handle_done (int)
{
  // make sure we stopped processing
  stop_find ();
}

bool
find_files_dialog::is_match (const QFileInfo& info)
{
  bool match = true;
  if (info.isDir ())
    {
      if (! m_include_dirs_check->isChecked ()) match = false;
      if (m_contains_text_check->isChecked ()) match = false;
    }
  else
    {
      // a file
      if (m_contains_text_check->isChecked ())
        {
          match = false;

          QFile file (info.absoluteFilePath ());
          if (file.open (QIODevice::ReadOnly))
            {
              QTextStream stream (&file);

              QString line;
              QString match_str = m_contains_text_edit->itemText (0);

              Qt::CaseSensitivity cs = m_content_case_check->isChecked ()
                                       ? Qt::CaseInsensitive
                                       : Qt::CaseSensitive;

              do
                {
                  line = stream.readLine ();
                  match = line.contains (match_str, cs);
                }
              while (! line.isNull () && match == false);
            }

        }
    }

  return match;
}

void find_files_dialog::handle_rows_inserted (const QModelIndex&, int first, int)
{
  // Only one row is inserted at a time: This signal is emitted directly after
  // the (empty) row was inserted but before the row contents is inserted.
  // The last row is resized after the find process stopped.
  if (first > 0)
    m_file_list->resizeRowToContents (first-1);
}

void find_files_dialog::closeEvent (QCloseEvent *e)
{
  save_settings ();
  e->accept ();
}

OCTAVE_END_NAMESPACE(octave)
