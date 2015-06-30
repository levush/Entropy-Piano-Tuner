#include "keyindexscaledraw.h"
#include <QPainter>

KeyIndexScaleDraw::KeyIndexScaleDraw()
{

}

void KeyIndexScaleDraw::drawLabel(QPainter *p, double val) const {
    int index = static_cast<int>(val + 0.5);

    p->translate(-mKeyWidth / 2, 0);

    if ((index - 1) % 12 == 0) {
        QwtScaleDraw::drawLabel(p, val);
    }

    p->translate(mKeyWidth / 2, 0);
}
