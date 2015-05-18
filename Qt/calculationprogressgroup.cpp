/*****************************************************************************
 * Copyright 2015 Haye Hinrichsen, Christoph Wick
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

#include "calculationprogressgroup.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QScrollArea>
#include <QSlider>
#include "../core/messages/messagecaluclationprogress.h"
#include "../core/system/log.h"
#include "../core/calculation/calculationmanager.h"
#include "../core/calculation/algorithmfactorydescription.h"
#include "settingsforqt.h"
#include "algorithmdialog.h"

CalculationProgressGroup::CalculationProgressGroup(Core *core, bool smallDevice, QWidget *parent)
    : QGroupBox(parent),
      MessageListener(false),
      CalculationAdapter(core),
      mCalculationInProgress(false)
{
    setTitle(tr("Calculation"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    QHBoxLayout *statusTextLayout = new QHBoxLayout;
    mainLayout->addLayout(statusTextLayout);

    statusTextLayout->addWidget(new QLabel(tr("Status:")));
    statusTextLayout->addWidget(mStatusLabel = new QLabel);
    mStatusLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    statusTextLayout->addStretch();

    QHBoxLayout *progressLayout = new QHBoxLayout;
    mainLayout->addLayout(progressLayout);

    progressLayout->addWidget(mCalculationProgressBar = new QProgressBar);
    progressLayout->addWidget(mStartCancelButton = new QPushButton);

    QPushButton *showAlgorithmInfo = new QPushButton(tr("Info"));
    progressLayout->addWidget(showAlgorithmInfo);
    showAlgorithmInfo->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    QObject::connect(showAlgorithmInfo, SIGNAL(clicked()), this, SLOT(showAlgorithmInfo()));


    mainLayout->addStretch();

    resetCalculation();

    mAlgorithmSelection = QString::fromStdString(SettingsForQt::getSingleton().getLastUsedAlgorithm());
    // check if algorithm exists
    if (CalculationManager::getSingleton().hasAlgorithm(mAlgorithmSelection.toStdString()) == false) {
        // select default algorithm
        mAlgorithmSelection = QString::fromStdString(CalculationManager::getSingleton().getDefaultAlgorithmId());
    }

    QObject::connect(mStartCancelButton, SIGNAL(clicked()), this, SLOT(startCancelPressed()));
}

CalculationProgressGroup::~CalculationProgressGroup()
{

}

void CalculationProgressGroup::handleMessage(MessagePtr m) {
    switch (m->getType()) {
    case Message::MSG_CALCULATION_PROGRESS: {
        auto mcp(std::static_pointer_cast<MessageCaluclationProgress>(m));
        switch (mcp->getCalculationType()) {
        case MessageCaluclationProgress::CALCULATION_FAILED: {
            QString errorText;
            switch (mcp->getErrorCode()) {
            case MessageCaluclationProgress::CALCULATION_ERROR_NONE:
                WARNING("Calculation error message was sent but no error code was set");
                errorText = tr("An unknown error occured during the calculation.");
                break;
            case MessageCaluclationProgress::CALCULATION_ERROR_NO_DATA:
                errorText = tr("No data available.");
                break;
            case MessageCaluclationProgress::CALCULATION_ERROR_NOT_ALL_KEYS_RECORDED:
                errorText = tr("Not all keys recorded");
                break;
            case MessageCaluclationProgress::CALCULATION_ERROR_KEY_DATA_INCONSISTENT:
                errorText = tr("Key data inconsistent.");
                break;
            }

            QMessageBox::critical(this, tr("Calculation error"), QString("%1<br><br><b>%2: %3</b>").arg(errorText, tr("Error code"), QString("%1").arg(mcp->getErrorCode())));

            cancelCalculation();
            break;
        }
        case MessageCaluclationProgress::CALCULATION_STARTED:
            mCalculationInProgress = true;
            // we started this calculation
            mStatusLabel->setText(tr("Calculation started"));
            mStartCancelButton->setText(tr("Stop calculation"));
            break;
        case MessageCaluclationProgress::CALCULATION_ENDED:
            resetCalculation();
            mCalculationInProgress = false;
            break;
        case MessageCaluclationProgress::CALCULATION_PROGRESSED:
            mCalculationProgressBar->setValue(mcp->getValue() * 100);
            break;
        case MessageCaluclationProgress::CALCULATION_ENTROPY_REDUCTION_STARTED:
            //mCalculationProgressBar->setDisabled(true);
            mStatusLabel->setText(tr("Minimizing the entropy"));
            break;
        }
        break;
    }
    default:
        break;
    }
}

void CalculationProgressGroup::startCalculation() {
    CalculationAdapter::startCalculation(mAlgorithmSelection.toStdString());

    activateMessageListener();
}

void CalculationProgressGroup::cancelCalculation() {
    mStatusLabel->setText(tr("Calculation canceled"));
    mCalculationProgressBar->setValue(0);
    mStartCancelButton->setText(tr("Start calculation"));

    CalculationAdapter::cancelCalculation();
    deactivateMessageListener();
    mCalculationInProgress = false;
}

void CalculationProgressGroup::resetCalculation() {
    mStatusLabel->setText(tr("Press the button to start the calculation"));
    mCalculationProgressBar->setValue(0);
    mStartCancelButton->setText(tr("Start calculation"));

    deactivateMessageListener();
    mCalculationInProgress = false;
}

void CalculationProgressGroup::startCancelPressed() {
    if (mCalculationInProgress) {
        cancelCalculation();
    } else {
        startCalculation();
    }
}

void CalculationProgressGroup::showAlgorithmInfo() {
    AlgorithmDialog dialog(mAlgorithmSelection, parentWidget());
    dialog.exec();
    mAlgorithmSelection = dialog.getAlgorithm();
    Settings::getSingleton().setLastUsedAlgorithm(mAlgorithmSelection.toStdString());
}
