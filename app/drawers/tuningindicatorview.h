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

#ifndef TUNINGINDICATORVIEW_H
#define TUNINGINDICATORVIEW_H

#include "prerequisites.h"

#include "core/drawers/tuningindicatordrawer.h"

#include "drawers/stroboscopicviewadapterforqt.h"
#include "implementations/graphicsviewadapterforqt.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Class for the Qt implementation of the tuning indicator
///
/// It forwards the drawings of the TuningIndicatorDrawer to a
/// AutoScaledToKeyboardGraphicsView.
///////////////////////////////////////////////////////////////////////////////

class TuningIndicatorView : public StroboscopicViewAdapterForQt,
                            public TuningIndicatorDrawer
{
public:
    TuningIndicatorView (QWidget *parent);
    virtual ~TuningIndicatorView() {}      ///< Empty virtual destructor.

private:
    void mousePressEvent (QMouseEvent *event) override final;
    void mouseDoubleClickEvent (QMouseEvent * event) override final;
    virtual QSize sizeHint() const override final { return QSize(0, 0);}
};

#endif // TUNINGINDICATORVIEW_H
