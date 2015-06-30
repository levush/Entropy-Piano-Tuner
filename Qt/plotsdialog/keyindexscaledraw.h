#ifndef KEYINDEXSCALEDRAW_H
#define KEYINDEXSCALEDRAW_H

#include "qwt_scale_draw.h"

class KeyIndexScaleDraw : public QObject, public QwtScaleDraw
{
    Q_OBJECT
public:
    KeyIndexScaleDraw();


protected:
    virtual void drawLabel(QPainter *p, double val) const override;

public slots:
    void setKeyWidth(double d) {mKeyWidth = d;}

private:
    double mKeyWidth = 0;
};

#endif // KEYINDEXSCALEDRAW_H
