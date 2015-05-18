/*****************************************************************************
 * Copyright 2015 Haye Hinrichsen, Christoph Wick
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

#ifndef ZOOMEDSPECTRUMGRAPHICSVIEW_H
#define ZOOMEDSPECTRUMGRAPHICSVIEW_H

#include "graphicsviewadapterforqt.h"
#include "../core/drawers/zoomedspectrumdrawer.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief The QGraphicsView to display the zoomed fourier spectrum.
///
/// It will displayed the drawings of ZoomedSpectrumDrawer to a
/// AutoScaledToKeyboardGraphicsView.
///////////////////////////////////////////////////////////////////////////////
class ZoomedSpectrumGraphicsView : public GraphicsViewAdapterForQt, public ZoomedSpectrumDrawer
{
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Constructor to link AutoScaledToKeyboardGraphicsView with
    ///        the ZoomedSpectrumDrawer
    /// \param parent : The parent widget.
    ///////////////////////////////////////////////////////////////////////////////
    ZoomedSpectrumGraphicsView(QWidget *parent);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Empty virtual destructor.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~ZoomedSpectrumGraphicsView();
};

#endif // ZOOMEDSPECTRUMGRAPHICSVIEW_H
