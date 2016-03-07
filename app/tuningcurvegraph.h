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
//          Qt implementation of the tuning curve graph drawer
//=============================================================================

#ifndef TUNINGCURVEGRAPH_H
#define TUNINGCURVEGRAPH_H

#include <QGraphicsPathItem>

#include "prerequisites.h"

#include "core/drawers/tuningcurvegraphdrawer.h"

#include "keyboard/autoscaledtokeyboardgraphicsview.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief The QGraphicsView to display the tuning curve and the inharmonicity.
///
/// This is the Qt implementation of the TuningCurveGraphDrawer.
/// It forwards the drawings of TuningCurveGraphDrawer to a
/// AutoScaledToKeyboardGraphicsView.
///
/// The user can change the computed tuning curve with a mouse click on the
/// bars. This class is basically managing these mouse clicks.
/// A click selects the corresponding key on the keyboard by sending a
/// MSG_KEY_SELECTION_CHANGED message.
///
/// \see AutoScaledToKeyboardGraphicsView
/// \see TuningCurveGraphDrawer
///////////////////////////////////////////////////////////////////////////////

class TuningCurveGraph : public AutoScaledToKeyboardGraphicsView,
                         public TuningCurveGraphDrawer
{
public:
    explicit TuningCurveGraph (QWidget *parent);
    virtual ~TuningCurveGraph() {}               ///< Empty virtual destructor

protected:
    virtual QSize sizeHint()        const override final {return QSize(0, 400);}
    virtual QSize minimumSizeHint() const override final {return QSize(0, 100);}

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void handleMouseInteraction(double relX, double relY);

private:
    /// Is the mouse pressed?
    bool mPressed;

    /// The x coordinate where the mouse was pressed in first instance.
    float mPressedX;
};

#endif // TUNINGCURVEGRAPH_H
