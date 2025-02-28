////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2000-2024 The Octave Project Developers
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
#include <QRect>
#include <QScreen>

#include "gui-utils.h"

OCTAVE_BEGIN_NAMESPACE(octave)

OCTGUI_API void
combobox_insert_current_item (QComboBox *cb, const QString &text)
{
  QString item_text = text;

  if (item_text.isEmpty ())
    item_text = cb->currentText ();

  int idx;
  while ((idx = cb->findText (item_text)) >= 0)
    cb->removeItem (idx);

  cb->insertItem (0, item_text);
  cb->setCurrentIndex (0);
}

OCTGUI_API void
combobox_update (QComboBox *cb, const int cb_length)
{
  if (! cb || cb_length == 0)
    return;

  // Remove possible empty entries from the cb list
  int index;
  while ((index = cb->findText (QString ())) >= 0)
    cb->removeItem (index);

  // Get current text and return if it is empty
  QString text = cb->currentText ();

  if (text.isEmpty ())
    return;

  // Remove occurrences of the current text in the cb list
  while ((index = cb->findText (text)) >= 0)
    cb->removeItem (index);

  // Remove the last entry from the end if the list is full
  if (cb->count () == cb_length)
    cb->removeItem (cb_length -1);

  // Insert new item at the beginning and set it as current item
  cb->insertItem (0, text);
  cb->setCurrentIndex (0);
}

OCTGUI_API QColor
interpolate_color (const QColor& col1, const QColor& col2,
                   double fs, double fv)
{
#if HAVE_QCOLOR_FLOAT_TYPE
  float h1, s1, v1, h2, s2, v2;
#else
  qreal h1, s1, v1, h2, s2, v2;
#endif

  col1.getHsvF (&h1, &s1, &v1);
  col2.getHsvF (&h2, &s2, &v2);

  return QColor::fromHsvF (h1, s1*fs, v1 + fv*(v2 - v1));
}

OCTGUI_API void
get_screen_geometry (int& width, int& height)
{
  QRect geom = QGuiApplication::primaryScreen ()->availableGeometry ();

  width = geom.width ();
  height = geom.height ();
}

OCTGUI_API void
adjust_to_screen (QRect& actual_geometry, const QRect& default_geometry)
{

  // Get the screen that holds the largest part of the given actual geometry

  const QScreen *actual_screen = nullptr;  // no screen found yet
  QRect actual_screen_geom = QRect ();     // geometry of found screen
  int intersected_area_max = 0;            // max. intersected area

  const int area_actual_geometry
    = actual_geometry.width () * actual_geometry.height ();
  QRect intersection;

  for (const QScreen *screen : QGuiApplication::screens ())
    {
      QRect screen_geom = screen->availableGeometry ();
      intersection = screen_geom.intersected (actual_geometry);
      if (! intersection.isEmpty ())
        {
          int area = intersection.width () * intersection.height ();
          if (area > intersected_area_max)
            {
              actual_screen = screen;
              actual_screen_geom = screen->availableGeometry ();
              if (area == area_actual_geometry)
                return;   // Actual geom is completely on a screen: return
              intersected_area_max = area;
            }
        }
    }

  // If the actual geometry is on no screen, use deault geometry

  if (actual_screen == nullptr)
    {
      actual_geometry = default_geometry;
      return;
    }

  // There is a screen that holds a part of the actual geometry.
  // Now adjust actual geometry to this screen

  // Get some properties of the actual and intersected geometry
  int agx1, agy1, agx2, agy2;
  actual_geometry.getCoords (&agx1, &agy1, &agx2, &agy2);
  int isx1, isy1, isx2, isy2;
  intersection.getCoords (&isx1, &isy1, &isx2, &isy2);

  // Make the intersection the same size as the actual geometry
  if ((agx1 == isx1) && (agx2 != isx2))
    isx1 = isx1 - agx2 + isx2;
  if ((agx1 != isx1) && (agx2 == isx2))
    isx2 = isx2 + agx2 - isx2;
  if ((agy1 == isy1) && (agy2 != isy2))
    isy1 = isy1 - agy2 + isy2;
  if ((agy1 != isy1) && (agy2 == isy2))
    isy2 = isy2 + agy2 - isy2;

  // And compute again the intersection with the screen if this resizing
  // led to corners outside the screen
  actual_geometry
    = actual_screen_geom.intersected (QRect (QPoint (isx1, isy1),
                                             QPoint (isx2, isy2)));
}

OCTAVE_END_NAMESPACE(octave)
