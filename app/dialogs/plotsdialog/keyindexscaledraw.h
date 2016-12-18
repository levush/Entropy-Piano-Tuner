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

#ifndef KEYINDEXSCALEDRAW_H
#define KEYINDEXSCALEDRAW_H

#include <qwt_scale_draw.h>

#include "prerequisites.h"

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
