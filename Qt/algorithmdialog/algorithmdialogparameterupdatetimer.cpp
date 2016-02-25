#include "algorithmdialogparameterupdatetimer.h"
#include <QTimer>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include "core/system/eptexception.h"
#include "core/system/log.h"

AlgorithmDialogParameterUpdateTimer::AlgorithmDialogParameterUpdateTimer(
            const AlgorithmParameter &description,
            ConstSingleAlgorithmParametersPtr parameters,
            QWidget *outputWidget,
            QObject *parent) :
    QObject(parent),
    mOutputWidget(outputWidget),
    mParameterDescription(description),
    mAlgorithmParameters(parameters)
{
    EptAssert(description.updateIntervalInMS() >= 0, "To create a update timer, an update timer greater 0 is required");

    QTimer *timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(description.updateIntervalInMS());
}

void AlgorithmDialogParameterUpdateTimer::update()
{
    switch (mParameterDescription.getType()) {
    case AlgorithmParameter::TYPE_DOUBLE:
        if (mParameterDescription.displayLineEdit()) {
            dynamic_cast<QLineEdit*>(mOutputWidget)->setText(QString::number(mAlgorithmParameters->getDoubleParameter(mParameterDescription.getID())));
        }
        if (mParameterDescription.displaySpinBox()) {
            dynamic_cast<QDoubleSpinBox*>(mOutputWidget)->setValue(mAlgorithmParameters->getDoubleParameter(mParameterDescription.getID()));
        }
        break;
    case AlgorithmParameter::TYPE_INT:
        if (mParameterDescription.displayLineEdit()) {
            dynamic_cast<QLineEdit*>(mOutputWidget)->setText(QString::number(mAlgorithmParameters->getIntParameter(mParameterDescription.getID())));
        }
        if (mParameterDescription.displaySpinBox()) {
            dynamic_cast<QSpinBox*>(mOutputWidget)->setValue(mAlgorithmParameters->getIntParameter(mParameterDescription.getID()));
        }
        break;
    case AlgorithmParameter::TYPE_LIST: {
        QComboBox *cb = dynamic_cast<QComboBox*>(mOutputWidget);
        cb->setCurrentIndex(cb->findData(QString::fromStdString(mAlgorithmParameters->getStringParameter(mParameterDescription.getID()))));
        break; }
    default:
        EPT_EXCEPT(EptException::ERR_NOT_IMPLEMENTED, "Default button does not implement type.");
    }
}
