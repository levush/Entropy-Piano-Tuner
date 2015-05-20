#include "graphicskeyitem.h"
#include <QPainter>
#include <QPainterPath>
#include <QFontDatabase>
#include <QTextLayout>

const std::array<qreal, GraphicsKeyItem::COUNT> KEY_RADIUS_FACTOR {{0.1, 0.1}};
const qreal WHITE_KEY_RADIUS_FACTOR = 0.4;

const std::array<std::array<qreal, 4>, GraphicsKeyItem::COUNT> MARGINS {{ {{0, 0, 0, 0}}, {{0.5, 0, -0.5, 0}} }};

void GraphicsKeyItem::initShapes(qreal b_w, qreal b_h, qreal w_w, qreal w_h) {
    if (mKeyShapes[BLACK].elementCount() > 0) {return;}  // already initialized

    mKeyRects[BLACK] = QRectF(0, 0, b_w, b_h);
    mKeyRects[WHITE] = QRectF(0, 0, w_w, w_h);

    const std::array<qreal, COUNT> radius {{b_w * KEY_RADIUS_FACTOR[0], w_w * KEY_RADIUS_FACTOR[1]}};
    std::array<QRectF, COUNT> rWithMargin = mKeyRects;

    for (size_t i = 0; i < COUNT; ++i) {
        const auto &m = MARGINS[i];
        rWithMargin[i].adjust(m[0], m[1], m[2], m[3]);

        mKeyShapes[i].addRoundedRect(rWithMargin[i], radius[i], radius[i]);
        mKeyShapes[i].addRect(rWithMargin[i].left(), rWithMargin[i].top(), rWithMargin[i].width(), radius[i] * 2);

        mKeyShapes[i].setFillRule(Qt::WindingFill);
        mKeyShapes[i] = mKeyShapes[i].simplified();
    }
}

std::array<QPainterPath, GraphicsKeyItem::COUNT> GraphicsKeyItem::mKeyShapes;
std::array<QRectF, GraphicsKeyItem::COUNT> GraphicsKeyItem::mKeyRects;

GraphicsKeyItem::GraphicsKeyItem(KeyType type, const QString &text, const QColor &textColor) :
    QGraphicsRectItem(mKeyRects[type]),
    mKeyType(type),
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
    painter->drawPath(mKeyShapes[mKeyType]);

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
