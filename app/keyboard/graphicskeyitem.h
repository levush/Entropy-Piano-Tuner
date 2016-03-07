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

#ifndef GRAPHICSKEYITEM_H
#define GRAPHICSKEYITEM_H

#include <QGraphicsRectItem>
#include <QPainterPath>
#include <QFont>
#include <array>

#include "prerequisites.h"

#include "core/piano/pianodefines.h"

class GraphicsKeyItem : public QGraphicsRectItem
{
public:
    static void initShapes(qreal b_w, qreal b_h, qreal w_w, qreal w_h);
    static std::array<QPainterPath, piano::KC_COUNT> mKeyShapes;
    static std::array<QRectF, piano::KC_COUNT> mKeyRects;

    using KeyColor = piano::KeyColor;
    static const int KC_BLACK = piano::KC_BLACK;
    static const int KC_WHITE = piano::KC_WHITE;
    static const int KC_COUNT = piano::KC_COUNT;

public:
    GraphicsKeyItem(KeyColor color, const QString &text, const QColor &textColor);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
    void setFont(const QFont &font) {mFont = font;}

private:
    const KeyColor mKeyColor;
    const QString mText;
    const QColor  mTextColor;
    QFont mFont;
};

#endif // GRAPHICSKEYITEM_H
