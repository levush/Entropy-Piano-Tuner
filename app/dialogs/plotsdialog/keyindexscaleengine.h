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

#ifndef KEYINDEXSCALEENGINE_H
#define KEYINDEXSCALEENGINE_H

#include <qwt_scale_engine.h>
#include <qwt_scale_div.h>

#include "prerequisites.h"

class KeyIndexScaleEngine : public QwtScaleEngine
{
public:
    KeyIndexScaleEngine(int maxKeys, int keyOffset);

    virtual void autoScale(int maxNumSteps, double &x1, double &x2, double &stepSize) const override final;
    virtual QwtScaleDiv divideScale(double x1, double x2, int maxMajorSteps, int maxMinorSteps, double stepSize=0.0) const override final;

private:
    const int mMaxKeys;
    const int mKeyOffset;
};

#endif // KEYINDEXSCALEENGINE_H
