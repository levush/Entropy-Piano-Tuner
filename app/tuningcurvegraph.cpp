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

#include "tuningcurvegraph.h"

#include <QMouseEvent>

#include "keyboard/keyboardgraphicsview.h"
#include "../core/messages/message.h"
#include "../core/messages/messagehandler.h"
#include "../core/messages/messagekeyselectionchanged.h"


//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor, linking AutoScaledToKeyboardGraphicsView with
///        the TuningCurveGraphDrawer
/// \param parent : The parent widget.
///////////////////////////////////////////////////////////////////////////////

TuningCurveGraph::TuningCurveGraph(QWidget *parent)
    : AutoScaledToKeyboardGraphicsView(parent, this),
      TuningCurveGraphDrawer(this),
      mPressed(false),
      mPressedX(-1)
{
}


//-----------------------------------------------------------------------------
//                          Mouse press event
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Function handling a mouse click. The mouse press event is used
/// to manually edit the tuning curve.
///
/// If the a mouse button is pressed this function sets mPressed to true
/// and stores the mouse x position in the member variable mPressedX.
/// It also passes the actual coordinates to the function handleMouseInteraction.
/// \see handleMouseInteraction
/// \param event : The QMouseEvent
///////////////////////////////////////////////////////////////////////////////

void TuningCurveGraph::mousePressEvent(QMouseEvent *event)
{
    QPointF relP = convertAbsToRel(mapToScene(event->pos()));
    mPressedX = relP.x();
    handleMouseInteraction(relP.x(), relP.y());
    mPressed = true;
}


//-----------------------------------------------------------------------------
//                           Mouse move event
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Function for handling mouse moves. Moving the mouse
/// will continuously change the tuning curve by dragging the green marker.
///
/// The function is only active if mPressed is true.
/// It will use mPressedX as x value and the actual QMouseEvent's y coordinate.
/// These coordinates are passed to the function handleMouseInteraction.
/// \see handleMouseInteraction
/// \param event : The QMouseEvent
///////////////////////////////////////////////////////////////////////////////

void TuningCurveGraph::mouseMoveEvent(QMouseEvent *event)
{
    if (mPressed)
    {
        QPointF relP = convertAbsToRel(mapToScene(event->pos()));
        handleMouseInteraction(relP.x(), relP.y());
    }
}


//-----------------------------------------------------------------------------
//                          Mouse release event
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Mouse release event to stop the change of the tuning curve.
///
/// A mouse release event will set mPressed to false.
/// The final coordinates are passed to the function handleMouseInteraction.
/// \see handleMouseInteraction
/// \param event : The QMouseEvent
///////////////////////////////////////////////////////////////////////////////

void TuningCurveGraph::mouseReleaseEvent(QMouseEvent *event)
{
    if (mPressed)
    {
        QPointF relP = convertAbsToRel(mapToScene(event->pos()));
        handleMouseInteraction(relP.x(), relP.y());
        mPressed = false;
    }
}


//-----------------------------------------------------------------------------
//                          Handle mouse interaction
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief TuningCurveGraph::handleMouseInteraction
///
/// If the mouse is within the valid range the function computes the
/// corresponding key index. In the calculation mode the coordinates are
/// used to manually edit the tuning curve while in all other modes
/// a message is sent to select the corresponding key.
///
/// \param relX : Relative x coordinate of the mouse pointer.
/// \param relY : Relative y coordinate of the mouse pointer.
///////////////////////////////////////////////////////////////////////////////

void TuningCurveGraph::handleMouseInteraction(double relX, double relY)
{
    int key = getNumberOfKeys() * relX;
    if (key < 0 or key >= getNumberOfKeys()) return;
    if (getOperationMode() == MODE_CALCULATION)
    {
        manuallyEditTuningCurveByClick(mPressedX, relY);
    }
    else MessageHandler::send<MessageKeySelectionChanged>(key, &getPiano()->getKey(key));
}
