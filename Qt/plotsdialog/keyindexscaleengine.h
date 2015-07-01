#ifndef KEYINDEXSCALEENGINE_H
#define KEYINDEXSCALEENGINE_H

#include "qwt_scale_engine.h"
#include "qwt_scale_div.h"

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
