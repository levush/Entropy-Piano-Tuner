#ifndef GRAPHICSKEYITEM_H
#define GRAPHICSKEYITEM_H

#include <QGraphicsRectItem>
#include <QPainterPath>
#include <QFont>
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

public:
    GraphicsKeyItem(KeyType type, const QString &text, const QColor &textColor);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0) override;
    void setFont(const QFont &font) {mFont = font;}

private:
    const KeyType mKeyType;
    const QString mText;
    const QColor  mTextColor;
    QFont mFont;
};

#endif // GRAPHICSKEYITEM_H
