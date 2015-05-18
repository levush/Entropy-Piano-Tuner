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

#include "graphicsitemforqt.h"
#include "../core/system/eptexception.h"
#include "graphicsviewadapterforqt.h"
#include <QGraphicsScene>

GraphicsItemForQt::GraphicsItemForQt(GraphicsViewAdapter *graphicsView, QGraphicsItem *item) :
    GraphicsItem(graphicsView),
    mItem(item)
{
    EptAssert(item, "The item must exist");
}

GraphicsItemForQt::~GraphicsItemForQt()
{
    if (mItem) {
        delete mItem;
        mItem = nullptr;
    }
}

void GraphicsItemForQt::setPosition(float x, float y) {
    QPointF pos(static_cast<GraphicsViewAdapterForQt*>(mGraphicsView)->convertRelToAbs(QPointF(x, y)));
    mItem->setPos(pos);
}

void GraphicsItemForQt::setZOrder(float z) {
    mItem->setZValue(z);
}
