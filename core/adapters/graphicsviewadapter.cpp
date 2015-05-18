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

#include "graphicsviewadapter.h"
#include "../system/eptexception.h"


GraphicsViewAdapter::GraphicsViewAdapter() {

}

GraphicsViewAdapter::~GraphicsViewAdapter() {
    while (mGraphicItems.size() > 0) {
        delete mGraphicItems.front();
    }
}

void GraphicsViewAdapter::clear() {
    while (mGraphicItems.size() > 0) {
        delete mGraphicItems.front();
        // not that the destructor of the graphics item already removes the item
    }
}

GraphicsItem *GraphicsViewAdapter::getGraphicItem(int keyIndex, RoleType role) {
    GraphicItemsList list(getGraphicItems(keyIndex, role));
    EptAssert(list.size() <= 1, "Either the item is unique or does not exist at all");
    if (list.size() > 0) {
        return list.front();
    }
    return nullptr;
}

GraphicsItem *GraphicsViewAdapter::getGraphicItemByRole(RoleType role) {
    GraphicItemsList list(getGraphicItemsByRole(role));
    EptAssert(list.size() <= 1, "Either the item is unique or does not exist at all");
    if (list.size() > 0) {
        return list.front();
    }
    return nullptr;
}

GraphicItemsList GraphicsViewAdapter::getGraphicItems(int keyIndex) {
    GraphicItemsList list;
    for (GraphicsItem *item : mGraphicItems) {
        if (item->getKeyIndex() == keyIndex) {
            list.push_back(item);
        }
    }
    return list;
}

GraphicItemsList GraphicsViewAdapter::getGraphicItemsByRole(RoleType role) {
    GraphicItemsList list;
    for (GraphicsItem *item : mGraphicItems) {
        if ((role & item->getItemRole()) == role) {
            list.push_back(item);
        }
    }
    return list;
}

GraphicItemsList GraphicsViewAdapter::getGraphicItems(int keyIndex, RoleType role) {
    GraphicItemsList list;
    for (GraphicsItem *item : mGraphicItems) {
        if (item->getKeyIndex() == keyIndex && (role & item->getItemRole()) == role) {
            list.push_back(item);
        }
    }
    return list;
}
