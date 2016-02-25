/*****************************************************************************
 * Copyright 2016 Haye Hinrichsen, Christoph Wick
 *
 * This file is part of Entropy Piano Tuner.
 *
 * Entropy Piano Tuner is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * Entropy Piano Tuner is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Entropy Piano Tuner. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

//=============================================================================
//             Graphical Qt interface for the tuning indicator
//=============================================================================

#include "tuningindicatorview.h"

#include <QMouseEvent>

#include "../core/system/log.h"
#include "../core/settings.h"

//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor to link AutoScaledToKeyboardGraphicsView with
///        the TuningIndicatorDrawer
/// \param parent : The parent widget.
///////////////////////////////////////////////////////////////////////////////

TuningIndicatorView::TuningIndicatorView(QWidget *parent)
    : StroboscopicViewAdapterForQt (parent, this, QRect(0, 0, 200, 200)),
      TuningIndicatorDrawer(this)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setWhatsThis(tr("This is the tuning indicator. Touch this field to toggle between spectral and stroboscopic mode. In the spectral mode you should bring the peak to the center of the window for optimal tuning. When tuning several strings of unisons at once, several peaks might appear and all of them should be tuned to match the center. In the stroboscopic mode several stripes of drifiting rainbows are shown. The stripes represent the partials. Optimal tuning of single strings is obtained when the rainbows come to a halt."));

}


//-----------------------------------------------------------------------------
//      A mouse click toggles the operation mode of the tuning indicator
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Function called when the tuning indicator is clicked or touched.
///
/// By clicking the tuning indicator the EPT toggles between the spectral
/// and the stroboscopic mode. The modes can also be changed in the settings.
/// \param event : Mouse event that tells us which button has been pressed.
///////////////////////////////////////////////////////////////////////////////

void TuningIndicatorView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::NoButton)
    {
        toggleSpectralAndStroboscopeMode();
    }
}


//-----------------------------------------------------------------------------
//    double click also toggles the operation mode of the tuning indicator
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Function called when the tuning indicator is double-clicked.
///
/// For convenience this function toggles the mode like mousePressEvent
/// in the case of a double click. Otherwise, klicking twice would only
/// toggle once which is might me confusing.
/// \param event : Mouse event that tells us which button has been pressed.
///////////////////////////////////////////////////////////////////////////////

void TuningIndicatorView::mouseDoubleClickEvent (QMouseEvent * event)
{
    mousePressEvent(event);
}
