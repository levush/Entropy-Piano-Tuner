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

#ifndef ALGORITHMDIALOGPARAMETERUPDATETIMER_H
#define ALGORITHMDIALOGPARAMETERUPDATETIMER_H

#include <QObject>
#include <QWidget>

#include "prerequisites.h"

#include "core/calculation/algorithmparameterdescription.h"
#include "core/calculation/algorithmparameters.h"

class AlgorithmDialogParameterUpdateTimer : public QObject
{
    Q_OBJECT
public:
    explicit AlgorithmDialogParameterUpdateTimer(
            const AlgorithmParameterDescription &description,
            ConstSingleAlgorithmParametersPtr parameters,
            QWidget *outputWidget,
            QObject *parent = 0);

signals:

private slots:
    void update();

private:
    QWidget *mOutputWidget;

    const AlgorithmParameterDescription mParameterDescription;
    ConstSingleAlgorithmParametersPtr mAlgorithmParameters;
};

#endif // ALGORITHMDIALOGPARAMETERUPDATETIMER_H
