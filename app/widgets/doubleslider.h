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

#ifndef DOUBLESLIDER_H
#define DOUBLESLIDER_H

#include <QSlider>

#include "prerequisites.h"

class DoubleSlider : public QSlider
{
    Q_OBJECT
public:
    DoubleSlider(double min, double max, int precision);

private:
    const double mPrecFactor;

public slots:
    void setValue(double);
    void setValue(QString);

private slots:
    void onValueChanged(int);

signals:
    void valueChanged(double);
    void valueChanged(QString);
};

#endif // DOUBLESLIDER_H
