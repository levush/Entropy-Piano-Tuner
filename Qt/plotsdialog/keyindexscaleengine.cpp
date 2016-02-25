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

#include "keyindexscaleengine.h"

KeyIndexScaleEngine::KeyIndexScaleEngine(int maxKeys, int keyOffset) :
    mMaxKeys(maxKeys),
    mKeyOffset(keyOffset) {

}

void KeyIndexScaleEngine::autoScale(int maxNumSteps, double &x1, double &x2, double &stepSize) const {
    Q_UNUSED(maxNumSteps);
    Q_UNUSED(x1);
    Q_UNUSED(x2);
    stepSize = 1;
}

QwtScaleDiv KeyIndexScaleEngine::divideScale(double x1, double x2, int maxMajorSteps, int maxMinorSteps, double stepSize) const {
    Q_UNUSED(maxMajorSteps);
    Q_UNUSED(maxMinorSteps);
    Q_UNUSED(stepSize);

    QList<double> minorTicks, mediumTicks, majorTicks;

    for (int i = std::max(static_cast<int>(x1), mKeyOffset); i < std::min(static_cast<int>(x2 + 1.5), mMaxKeys + 1 + mKeyOffset); ++i) {
        if (i % 12 == 0 || (i - 1) % 12 == 0) {
            majorTicks << i;
        } else {
            mediumTicks << i;
        }
    }

    return QwtScaleDiv(x1, x2, minorTicks, mediumTicks, majorTicks);
}
