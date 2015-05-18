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

#include "tuningcurvegraph.h"
#include "keyboardgraphicsview.h"
#include "../core/messages/message.h"
#include "../core/messages/messagehandler.h"
#include "../core/messages/messagekeyselectionchanged.h"
#include <QMouseEvent>

TuningCurveGraph::TuningCurveGraph(QWidget *parent)
    : AutoScaledToKeyboardGraphicsView(parent, this),
      TuningCurveGraphDrawer(this),
      mPressed(false),
      mPressedX(-1) {
}

TuningCurveGraph::~TuningCurveGraph()
{

}

void TuningCurveGraph::mousePressEvent(QMouseEvent *event) {
    QPointF relP = convertAbsToRel(mapToScene(event->pos()));
    mPressedX = relP.x();
    handleMouseInteraction(relP.x(), relP.y());
    mPressed = true;
}

void TuningCurveGraph::mouseMoveEvent(QMouseEvent *event) {
    if (mPressed) {
        QPointF relP = convertAbsToRel(mapToScene(event->pos()));
        handleMouseInteraction(relP.x(), relP.y());
    }
}

void TuningCurveGraph::mouseReleaseEvent(QMouseEvent *event) {
    if (mPressed) {
        QPointF relP = convertAbsToRel(mapToScene(event->pos()));
        handleMouseInteraction(relP.x(), relP.y());
        mPressed = false;
    }
}

void TuningCurveGraph::handleMouseInteraction(double relX, double relY) {
    int key = mNumberOfKeys * relX;

    if (key < 0 || key >= mNumberOfKeys) {
        return;
    }

    if (getOperationMode() == MODE_CALCULATION) {
        manuallyEditTuningCurveByClick(mPressedX, relY);
    } else {
       MessageHandler::send<MessageKeySelectionChanged>(key, &mPiano->getKey(key));
    }
}
