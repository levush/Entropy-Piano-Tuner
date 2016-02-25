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

#include "autoscaledtokeyboardgraphicsview.h"
#include "../core/system/eptexception.h"
#include "keyboardgraphicsview.h"

AutoScaledToKeyboardGraphicsView::AutoScaledToKeyboardGraphicsView(QWidget *parent, DrawerBase *drawer)
    : GraphicsViewAdapterForQt(parent, drawer, QRectF()),
      mKeyboardGraphicsView(nullptr) {

    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

AutoScaledToKeyboardGraphicsView::~AutoScaledToKeyboardGraphicsView()
{

}

void AutoScaledToKeyboardGraphicsView::setKeyboard(KeyboardGraphicsView *keyboardgraphicsview) {
    mKeyboardGraphicsView = keyboardgraphicsview;
    mKeyboardGraphicsView->addAutoScaledGraphicsView(this);
    setSceneRect(mKeyboardGraphicsView->sceneRect());
}


void AutoScaledToKeyboardGraphicsView::highlightKey(int key) {
    GraphicsItem *item = getGraphicItemByRole(ROLE_HIGHLIGHT);
    if (item) {
        delete item;
        item = nullptr;
    }
    if (key >= 0) {
        int numberOfKeys = mKeyboardGraphicsView->getKeyboard()->getNumberOfKeys();
        float deltaX = 1.f / numberOfKeys;
        item = drawFilledRect(key * deltaX, 0, deltaX * 0.99f, 1, GraphicsViewAdapter::PEN_THIN_TRANSPARENT, GraphicsViewAdapter::FILL_LIGHT_GRAY);
        if (item) {
            // key is not out of range
            item->setZOrder(-1);
            item->setKeyIndexAndItemRole(key, ROLE_HIGHLIGHT);
        }
    }
}

void AutoScaledToKeyboardGraphicsView::showEvent(QShowEvent *event) {
    fitInView(mKeyboardGraphicsView->getVisibleContentsRect(), Qt::IgnoreAspectRatio);

    QGraphicsView::showEvent(event);
}

void AutoScaledToKeyboardGraphicsView::resizeEvent(QResizeEvent *event) {
    fitInView(mKeyboardGraphicsView->getVisibleContentsRect(), Qt::IgnoreAspectRatio);

    QGraphicsView::resizeEvent(event);
}
