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

#include "algorithmdialogparameterupdatetimer.h"
#include <QTimer>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include "core/system/eptexception.h"
#include "core/system/log.h"

AlgorithmDialogParameterUpdateTimer::AlgorithmDialogParameterUpdateTimer(
        const AlgorithmParameterDescription &description,
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
    case AlgorithmParameterDescription::TYPE_DOUBLE:
        if (mParameterDescription.displayLineEdit()) {
            dynamic_cast<QLineEdit*>(mOutputWidget)->setText(QString::number(mAlgorithmParameters->getDoubleParameter(mParameterDescription.getID())));
        }
        if (mParameterDescription.displaySpinBox()) {
            dynamic_cast<QDoubleSpinBox*>(mOutputWidget)->setValue(mAlgorithmParameters->getDoubleParameter(mParameterDescription.getID()));
        }
        break;
    case AlgorithmParameterDescription::TYPE_INT:
        if (mParameterDescription.displayLineEdit()) {
            dynamic_cast<QLineEdit*>(mOutputWidget)->setText(QString::number(mAlgorithmParameters->getIntParameter(mParameterDescription.getID())));
        }
        if (mParameterDescription.displaySpinBox()) {
            dynamic_cast<QSpinBox*>(mOutputWidget)->setValue(mAlgorithmParameters->getIntParameter(mParameterDescription.getID()));
        }
        break;
    case AlgorithmParameterDescription::TYPE_LIST: {
        QComboBox *cb = dynamic_cast<QComboBox*>(mOutputWidget);
        cb->setCurrentIndex(cb->findData(QString::fromStdString(mAlgorithmParameters->getStringParameter(mParameterDescription.getID()))));
        break; }
    default:
        EPT_EXCEPT(EptException::ERR_NOT_IMPLEMENTED, "Default button does not implement type.");
    }
}
