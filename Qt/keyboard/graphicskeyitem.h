#ifndef GRAPHICSKEYITEM_H
#define GRAPHICSKEYITEM_H

#include <QGraphicsRectItem>
#include <QPainterPath>
#include <array>

class GraphicsKeyItem : public QGraphicsRectItem
{
public:
    enum KeyType {
        BLACK = 0,
        WHITE = 1,

        COUNT,
    };

public:
    static void initShapes(qreal b_w, qreal b_h, qreal w_w, qreal w_h);
    static std::array<QPainterPath, KeyType::COUNT> mKeyShapes;
    static std::array<QRectF, KeyType::COUNT> mKeyRects;
    static QFont mFont;

public:
    GraphicsKeyItem(KeyType type, const QString &text, const QColor &textColor);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;

private:
    const KeyType mKeyType;
    const QString mText;
    const QColor  mTextColor;
};

#endif // GRAPHICSKEYITEM_H
