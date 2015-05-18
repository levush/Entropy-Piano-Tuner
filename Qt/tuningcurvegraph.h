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

#ifndef TUNINGCURVEGRAPH_H
#define TUNINGCURVEGRAPH_H

#include "autoscaledtokeyboardgraphicsview.h"
#include <QGraphicsPathItem>
#include "../core/drawers/tuningcurvegraphdrawer.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief The QGraphicsView to display the tuning curve and the inharmonicity.
///
/// It will displayed the drawings of TuningCurveGraphDrawer to a
/// AutoScaledToKeyboardGraphicsView.
/// The user can change the computed tuning curve with a mouse click on the
/// bars.
/// A click selects the corresponding key on the keyboard by sending a
/// MSG_KEY_SELECTION_CHANGED message.
///////////////////////////////////////////////////////////////////////////////
class TuningCurveGraph :
        public AutoScaledToKeyboardGraphicsView,
        public TuningCurveGraphDrawer
{
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Constructor to link AutoScaledToKeyboardGraphicsView with
    ///        the TuningCurveGraphDrawer
    /// \param parent : The parent widget.
    ///////////////////////////////////////////////////////////////////////////////
    explicit TuningCurveGraph(QWidget *parent);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Empty virtual destructor.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~TuningCurveGraph();
protected:

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Mouse press event to select the manually edit the tuning curve.
    /// \param event : The QMouseEvent
    ///
    /// It will set mPressed to true and set mPressedX to the mouse x position.
    ///////////////////////////////////////////////////////////////////////////////
    void mousePressEvent(QMouseEvent *event) override;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Mouse move will change the tuning curve.
    /// \param event : The QMouseEvent
    ///
    /// The function is only active if mPressed is true.
    /// It will use mPressedX as x value and the QMouseEvent's y coordinate.
    ///////////////////////////////////////////////////////////////////////////////
    void mouseMoveEvent(QMouseEvent *event) override;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Mouse release event to stop the change of the tuning curve.
    /// \param event : The QMouseEvent
    ///
    /// This will set mPressed to false.
    ///////////////////////////////////////////////////////////////////////////////
    void mouseReleaseEvent(QMouseEvent *event) override;

    void handleMouseInteraction(double relX, double relY);

private:
    /// Is the mouse pressed?
    bool mPressed;

    /// The x coordinate where the mouse was pressed in first instance.
    float mPressedX;
};

#endif // TUNINGCURVEGRAPH_H
