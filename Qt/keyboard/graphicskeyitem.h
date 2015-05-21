#ifndef GRAPHICSKEYITEM_H
#define GRAPHICSKEYITEM_H

#include <QGraphicsRectItem>
#include <QPainterPath>
#include <QFont>
#include <array>

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
