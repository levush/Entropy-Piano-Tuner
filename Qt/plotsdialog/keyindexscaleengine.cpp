#include "keyindexscaleengine.h"

KeyIndexScaleEngine::KeyIndexScaleEngine(int maxKeys) :
    mMaxKeys(maxKeys) {

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

    for (int i = std::max(static_cast<int>(x1), 0); i < std::min(static_cast<int>(x2 + 1.5), mMaxKeys + 1); ++i) {
        if (i % 12 == 0 || (i - 1) % 12 == 0) {
            majorTicks << i;
        } else {
            mediumTicks << i;
        }
    }

    return QwtScaleDiv(x1, x2, minorTicks, mediumTicks, majorTicks);
}
