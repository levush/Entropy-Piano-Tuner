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

#include "graphicskeyitem.h"
#include <QPainter>
#include <QPainterPath>
#include <QFontDatabase>
#include <QTextLayout>


const std::array<qreal, GraphicsKeyItem::KC_COUNT> KEY_RADIUS_FACTOR {{0.18, 0.1}};

const std::array<std::array<qreal, 4>, GraphicsKeyItem::KC_COUNT> MARGINS {{ {{0, 0, 0, 0}}, {{0.5, 0, -0.5, 0}} }};

std::array<QPainterPath, GraphicsKeyItem::KC_COUNT> GraphicsKeyItem::mKeyShapes;
std::array<QRectF, GraphicsKeyItem::KC_COUNT> GraphicsKeyItem::mKeyRects;

void GraphicsKeyItem::initShapes(qreal b_w, qreal b_h, qreal w_w, qreal w_h) {
    if (mKeyShapes[KC_BLACK].elementCount() > 0) {return;}  // already initialized

    mKeyRects[KC_BLACK] = QRectF(0, 0, b_w, b_h);
    mKeyRects[KC_WHITE] = QRectF(0, 0, w_w, w_h);

    const std::array<qreal, KC_COUNT> radius {{b_w * KEY_RADIUS_FACTOR[0], w_w * KEY_RADIUS_FACTOR[1]}};
    std::array<QRectF, KC_COUNT> rWithMargin = mKeyRects;

    for (int i = 0; i < KC_COUNT; ++i) {
        const auto &m = MARGINS[i];
        rWithMargin[i].adjust(m[0], m[1], m[2], m[3]);

        mKeyShapes[i].addRoundedRect(rWithMargin[i], radius[i], radius[i]);
        mKeyShapes[i].addRect(rWithMargin[i].left(), rWithMargin[i].top(), rWithMargin[i].width(), radius[i] * 2);

        mKeyShapes[i].setFillRule(Qt::WindingFill);
        mKeyShapes[i] = mKeyShapes[i].simplified();
    }
}


GraphicsKeyItem::GraphicsKeyItem(KeyColor color, const QString &text, const QColor &textColor) :
    QGraphicsRectItem(mKeyRects[color]),
    mKeyColor(color),
    mText(text),
    mTextColor(textColor)
{
}

void GraphicsKeyItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    // QGraphicsRectItem::paint(painter, option, widget);
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->setPen(pen());
    painter->setBrush(brush());
    painter->drawPath(mKeyShapes[mKeyColor]);

    // draw text
    painter->rotate(-90);
    const qreal textFieldWidth = rect().height() * 0.94;
    const qreal textFieldHeight = rect().width();
    painter->translate(-textFieldWidth, 0);
    QPen p;
    p.setBrush(QBrush(mTextColor));
    painter->setPen(p);
    painter->setBrush(Qt::NoBrush);
    painter->setFont(mFont);

    painter->drawText(QRectF(0, 0, textFieldWidth, textFieldHeight), Qt::AlignVCenter | Qt::AlignLeft, mText);
}
