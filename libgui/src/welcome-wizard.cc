////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2011-2024 The Octave Project Developers
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

#include <QApplication>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

#if defined (OCTAVE_USE_WINDOWS_API)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "gui-preferences-dw.h"
#include "gui-preferences-nr.h"
#include "gui-settings.h"
#include "welcome-wizard.h"

OCTAVE_BEGIN_NAMESPACE(octave)

static QLabel *
make_octave_logo (QWidget *p = nullptr, int height = 100)
{
  QLabel *logo = new QLabel (p);
  QPixmap logo_pixmap (dw_icon_set_names["NONE"]);
  logo->setPixmap (logo_pixmap.scaledToHeight (height));
  return logo;
};

welcome_wizard::welcome_wizard (QWidget *p)
  : QDialog (p), m_page_ctor_list (), m_page_list_iterator (),
    m_current_page (initial_page::create (this)),
    m_allow_web_connect_state (false),
    m_max_height (0), m_max_width (0)
{
  m_page_ctor_list.push_back (initial_page::create);
  m_page_ctor_list.push_back (setup_community_news::create);
  m_page_ctor_list.push_back (final_page::create);

  m_page_list_iterator = m_page_ctor_list.begin ();

  setWindowTitle (tr ("Welcome to GNU Octave"));

  setEnabled (true);

  setSizePolicy (QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

  // Create all pages for pre-setting the minimal required size for all pages
  show_page ();
  adjust_size ();
  next_page ();
  adjust_size ();
  next_page ();
  adjust_size ();
  // now go back to the first page
  previous_page ();
  previous_page ();

  // Set the size determined above
  resize (m_max_width, m_max_height);

#if defined (OCTAVE_USE_WINDOWS_API)
  // HACK to forceshow of dialog if started minimized
  ShowWindow (reinterpret_cast<HWND> (winId ()), SW_SHOWNORMAL);
#endif
}

void
welcome_wizard::adjust_size ()
{
  // Get adjusted size for the current page
  adjustSize ();
  QSize sz = size ();

  // Update the max. size of the three pages if required

  if (sz.height () > m_max_height)
    m_max_height = sz.height ();

  if (sz.width () > m_max_width)
    m_max_width = sz.width ();
}

void
welcome_wizard::handle_web_connect_option (int state)
{
  m_allow_web_connect_state = state == Qt::Checked;
}

void
welcome_wizard::show_page ()
{
  delete m_current_page;
  delete layout ();

  m_current_page = (*m_page_list_iterator) (this);

  QVBoxLayout *new_layout = new QVBoxLayout ();
  setLayout (new_layout);

  new_layout->addWidget (m_current_page);
}

void
welcome_wizard::previous_page ()
{
  --m_page_list_iterator;

  show_page ();
}

void
welcome_wizard::next_page ()
{
  ++m_page_list_iterator;

  show_page ();
}

void
welcome_wizard::accept ()
{
  // Create default settings file.

  gui_settings settings;

  settings.setValue (nr_allow_connection.settings_key (), m_allow_web_connect_state);

  settings.sync ();

  QDialog::accept ();
}

initial_page::initial_page (welcome_wizard *wizard)
  : QWidget (wizard),
    m_title (new QLabel (tr ("Welcome to Octave!"), this)),
    m_message (new QLabel (this)),
    m_logo (make_octave_logo (this)),
    m_next (new QPushButton (tr ("Next"), this)),
    m_cancel (new QPushButton (tr ("Cancel"), this))
{
  QFont ft;
  ft.setPointSize (20);
  m_title->setFont (ft);

  gui_settings settings;

  m_message->setText
    (tr ("<html><body>\n"
         "<p>You seem to be using the Octave graphical interface for the first time on this computer.\n"
         "Click 'Next' to create a configuration file and launch Octave.</p>\n"
         "<p>The configuration file is stored in<br>%1.</p>\n"
         "</body></html>").
     arg (settings.file_name ()));
  m_message->setWordWrap (true);
  m_message->setMinimumWidth (400);

  QVBoxLayout *message_layout = new QVBoxLayout;

  message_layout->addWidget (m_title);
  message_layout->addWidget (m_message);

  QHBoxLayout *message_and_logo = new QHBoxLayout;

  message_and_logo->addLayout (message_layout);
  message_and_logo->addStretch (10);
  message_and_logo->addWidget (m_logo, 0, Qt::AlignTop);

  QHBoxLayout *button_bar = new QHBoxLayout;

  button_bar->addStretch (10);
  button_bar->addWidget (m_next);
  button_bar->addWidget (m_cancel);

  QVBoxLayout *page_layout = new QVBoxLayout (this);
  setLayout (page_layout);

  page_layout->addLayout (message_and_logo);
  page_layout->addStretch (10);
  page_layout->addSpacing (20);
  page_layout->addLayout (button_bar);

  setSizePolicy (QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

  m_next->setDefault (true);
  m_next->setFocus ();

  connect (m_next, &QPushButton::clicked, wizard, &welcome_wizard::next_page);
  connect (m_cancel, &QPushButton::clicked, wizard, &welcome_wizard::reject);
}

setup_community_news::setup_community_news (welcome_wizard *wizard)
  : QWidget (wizard),
    m_title (new QLabel (tr ("Community News"), this)),
    m_message (new QLabel (this)),
    m_checkbox (new QCheckBox (this)),
    m_checkbox_message (new QLabel (this)),
    m_logo (make_octave_logo (this)),
    m_previous (new QPushButton (tr ("Previous"), this)),
    m_next (new QPushButton (tr ("Next"), this)),
    m_cancel (new QPushButton (tr ("Cancel"), this))
{
  QFont ft;
  ft.setPointSize (20);
  m_title->setFont (ft);

  m_message->setText
    (tr ("<html><body>\n"
         "<p>When Octave starts, it will optionally check the Octave web site for current news and information about the Octave community.\n"
         "The check will happen at most once each day and news will only be displayed if there is something new since the last time you viewed the news.</p>\n"
         "<p>You may also view the news by selecting the \"Community News\" item in the \"Help\" menu, or by visiting\n"
         "<a href=\"https://octave.org/community-news.html\">https://octave.org/community-news.html</a>.</p>\n"
         "</body></html>"));
  m_message->setWordWrap (true);
  m_message->setMinimumWidth (400);
  m_message->setOpenExternalLinks (true);

  QVBoxLayout *message_layout = new QVBoxLayout;

  message_layout->addWidget (m_title);
  message_layout->addWidget (m_message);

  QHBoxLayout *message_and_logo = new QHBoxLayout;

  message_and_logo->addLayout (message_layout);
  message_and_logo->addStretch (10);
  message_and_logo->addWidget (m_logo, 0, Qt::AlignTop);

  QHBoxLayout *checkbox_layout = new QHBoxLayout;

  bool allow_connection = nr_allow_connection.def ().toBool ();
  if (allow_connection)
    m_checkbox->setCheckState (Qt::Checked);
  else
    m_checkbox->setCheckState (Qt::Unchecked);

  m_checkbox_message->setText
    (tr ("<html><head>\n"
         "</head><body>\n"
         "<p>Allow Octave to connect to the Octave web site when it starts in order to display current news and information about the Octave community.</p>\n"
         "</body></html>"));
  m_checkbox_message->setWordWrap (true);
  m_checkbox_message->setOpenExternalLinks (true);
  m_checkbox_message->setMinimumWidth (500);

  checkbox_layout->addWidget (m_checkbox, 0, Qt::AlignTop);
  checkbox_layout->addSpacing (20);
  checkbox_layout->addWidget (m_checkbox_message, 0, Qt::AlignTop);
  checkbox_layout->addStretch (10);

  QVBoxLayout *message_logo_and_checkbox = new QVBoxLayout;

  message_logo_and_checkbox->addLayout (message_and_logo);
  message_logo_and_checkbox->addSpacing (20);
  message_logo_and_checkbox->addLayout (checkbox_layout);

  QHBoxLayout *button_bar = new QHBoxLayout;

  button_bar->addStretch (10);
  button_bar->addWidget (m_previous);
  button_bar->addWidget (m_next);
  button_bar->addWidget (m_cancel);

  QVBoxLayout *page_layout = new QVBoxLayout (this);
  setLayout (page_layout);

  page_layout->addLayout (message_logo_and_checkbox);
  page_layout->addStretch (10);
  page_layout->addSpacing (20);
  page_layout->addLayout (button_bar);

  setSizePolicy (QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

  m_next->setDefault (true);
  m_next->setFocus ();

  connect (m_checkbox,
#if defined (HAVE_QCHECKBOX_CHECKSTATECHANGED)
           &QCheckBox::checkStateChanged,
#else
           &QCheckBox::stateChanged,
#endif
           wizard, &welcome_wizard::handle_web_connect_option);

  connect (m_previous, &QPushButton::clicked, wizard, &welcome_wizard::previous_page);
  connect (m_next, &QPushButton::clicked, wizard, &welcome_wizard::next_page);
  connect (m_cancel, &QPushButton::clicked, wizard, &welcome_wizard::reject);
}

final_page::final_page (welcome_wizard *wizard)
  : QWidget (wizard),
    m_title (new QLabel (tr ("Enjoy!"), this)),
    m_message (new QLabel (this)),
    m_logo (make_octave_logo (this)),
    m_links (new QLabel (this)),
    m_previous (new QPushButton (tr ("Previous"), this)),
    m_finish (new QPushButton (tr ("Finish"), this)),
    m_cancel (new QPushButton (tr ("Cancel"), this))
{
  QFont ft;
  ft.setPointSize (20);
  m_title->setFont (ft);

  m_message->setText
    (tr ("<html><body>\n"
         "<p>We hope you find Octave to be a useful tool.</p>\n"
         "<p>If you encounter problems, there are a number of ways to get help, including commercial support options, a discussion board, a wiki, and other community-based support channels.\n"
         "You can find more information about each of these by visiting <a href=\"https://octave.org/support.html\">https://octave.org/support.html</a> (opens in external browser).</p>\n"
         "</body></html>"));
  m_message->setWordWrap (true);
  m_message->setMinimumWidth (400);
  m_message->setOpenExternalLinks (true);

  QVBoxLayout *message_layout = new QVBoxLayout;

  message_layout->addWidget (m_title);
  message_layout->addWidget (m_message);

  QHBoxLayout *message_and_logo = new QHBoxLayout;

  message_and_logo->addLayout (message_layout);
  message_and_logo->addStretch (10);
  message_and_logo->addWidget (m_logo, 0, Qt::AlignTop);

  m_links->setText
    (tr ("<html><head>\n"
         "</head><body>\n"
         "<p>For more information about Octave:</p>\n"
         "<ul>\n"
         "<li>Visit <a href=\"https://octave.org\">https://octave.org</a> (opens in external browser)</li>\n"
         "<li>Get the documentation online in <a href=\"https://www.gnu.org/software/octave/doc/interpreter/index.html\">HTML</a> or <a href=\"https://www.gnu.org/software/octave/octave.pdf\">PDF</a> format (links open in external browser)</li>\n"
         "<li>Open the documentation browser of the Octave GUI with the help menu</li>\n"
         "</ul>\n"
         "</body></html>"));
  m_links->setWordWrap (true);
  m_links->setOpenExternalLinks (true);

  QHBoxLayout *button_bar = new QHBoxLayout;

  button_bar->addStretch (10);
  button_bar->addWidget (m_previous);
  button_bar->addWidget (m_finish);
  button_bar->addWidget (m_cancel);

  QVBoxLayout *page_layout = new QVBoxLayout (this);
  setLayout (page_layout);

  page_layout->addLayout (message_and_logo);
  page_layout->addSpacing (20);
  page_layout->addWidget (m_links);
  page_layout->addStretch (10);
  page_layout->addSpacing (20);
  page_layout->addLayout (button_bar);

  setSizePolicy (QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

  m_finish->setDefault (true);
  m_finish->setFocus ();

  connect (m_previous, &QPushButton::clicked,
           wizard, &welcome_wizard::previous_page);
  connect (m_finish, &QPushButton::clicked, wizard, &welcome_wizard::accept);
  connect (m_cancel, &QPushButton::clicked, wizard, &welcome_wizard::reject);
}

OCTAVE_END_NAMESPACE(octave)
