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

#include "doubleslider.h"
#include <cmath>

DoubleSlider::DoubleSlider(double min, double max, int precision) :
    mPrecFactor(std::pow(10, precision)) {
    setRange(min * mPrecFactor, max * mPrecFactor);

    QObject::connect(this, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
}

void DoubleSlider::setValue(double value) {
    QSlider::setValue(round(value * mPrecFactor));
}

void DoubleSlider::setValue(QString value) {
    setValue(value.toDouble());
}

void DoubleSlider::onValueChanged(int value) {
    emit valueChanged(value / mPrecFactor);
    emit valueChanged(QString::number(value / mPrecFactor));
}

