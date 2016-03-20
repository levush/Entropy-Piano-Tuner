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

//=============================================================================
//                      Calculation progress group
//=============================================================================

#include "calculationprogressgroup.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QScrollArea>
#include <QSlider>
#include <QFontMetrics>

#include "core/calculation/calculationmanager.h"
#include "core/calculation/algorithmfactorydescription.h"
#include "core/messages/messagecaluclationprogress.h"
#include "core/piano/pianomanager.h"
#include "core/system/log.h"

#include "dialogs/algorithmdialog/algorithmdialog.h"
#include "implementations/settingsforqt.h"
#include "displaysize.h"

//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief CalculationProgressGroup::CalculationProgressGroup
///
/// The constructor creates the graphics elements shown in the Calculation
/// progress group window. It uses a small progress bar and a small push
/// button which are included here as local classes overriding the
/// Qt standard classes.
/// \param core : Pointer to the core, needed to initialize the adapter
/// \param parent : Pointer to the parent widget
///////////////////////////////////////////////////////////////////////////////

CalculationProgressGroup::CalculationProgressGroup(Core *core, QWidget *parent)
    : DisplaySizeDependingGroupBox(parent, new QVBoxLayout, toFlag(MODE_CALCULATION)),
      MessageListener(),
      CalculationAdapter(core),
      mCalculationInProgress(false)
{
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(mMainWidgetContainer->layout());
    QHBoxLayout *statusTextLayout = new QHBoxLayout;
    mainLayout->addLayout(statusTextLayout);

    statusTextLayout->addWidget(new QLabel(tr("Status:")));
    statusTextLayout->addWidget(mStatusLabel = new QLabel);
    mStatusLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    statusTextLayout->addStretch();

    QHBoxLayout *progressLayout = new QHBoxLayout;
    mainLayout->addLayout(progressLayout);

    // Local class for a small push button
    class MinSizePushButton : public QPushButton
    {
    public:
        MinSizePushButton(QString text = QString()) :
            QPushButton(text)
        {
            setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        }

    private:
        virtual QSize minimumSizeHint() const override
        {
            QFontMetrics f(fontMetrics());
            // add also some extra spacing of 2 chars (x)
            return f.size(Qt::TextSingleLine, text() + "xx");
        }
        virtual QSize sizeHint() const override
        {
            QSize sh(QPushButton::sizeHint());
            sh.setWidth(std::max(minimumSizeHint().width(), sh.width()));
            return sh;
        }
    };

    // Local class for a small progress bar
    class MinSizeProgressBar : public QProgressBar
    {
    private:
        virtual QSize sizeHint() const override {return QSize(0, 0);}
    };

    // Add the progress bar
    progressLayout->addWidget(mCalculationProgressBar = new MinSizeProgressBar);
    mCalculationProgressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    progressLayout->addWidget(mStartCancelButton = new MinSizePushButton);

    // Add the push button
    QPushButton *showAlgorithmInfo = new MinSizePushButton(tr("Info"));
    progressLayout->addWidget(showAlgorithmInfo);
    QObject::connect(showAlgorithmInfo, SIGNAL(clicked()), this, SLOT(showAlgorithmInfo()));

    mainLayout->addStretch();

    // Reset the calculation
    onResetCalculation();

    // show algorithm information in the title
    updateTitle();

    // connect the start/cancel button with the cancel slot
    QObject::connect(mStartCancelButton, SIGNAL(clicked()), this, SLOT(startCancelPressed()));
}


//-----------------------------------------------------------------------------
//                  Message listener and dispatcher
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Message handling.
///
/// This function controls the incoming messages about the calculation status
/// of the algorithm and takes action accordingly.
/// \param m : The incoming message
///////////////////////////////////////////////////////////////////////////////

void CalculationProgressGroup::handleMessage(MessagePtr m)
{
    if (m->getType() == Message::MSG_CALCULATION_PROGRESS)
    {
        auto mcp(std::static_pointer_cast<MessageCaluclationProgress>(m));
        switch (mcp->getCalculationType())
        {
        case MessageCaluclationProgress::CALCULATION_STARTED:
            mCalculationInProgress = true;
            // we started this calculation
            mStatusLabel->setText(tr("Calculation started"));
            mStartCancelButton->setText(tr("Stop calculation"));
            break;
        case MessageCaluclationProgress::CALCULATION_ENDED:
            if (!mCalculationInProgress) {return;}
            onResetCalculation();
            mCalculationInProgress = false;
            QMessageBox::information(this, tr("Calculation finished"), tr("The calculation finished successfully! Now you can switch to the tuning mode and tune your piano."));
            break;
        case MessageCaluclationProgress::CALCULATION_PROGRESSED:
            if (!mCalculationInProgress) {return;}
            mCalculationProgressBar->setValue(mcp->getValue() * 100);
            break;
        case MessageCaluclationProgress::CALCULATION_ENTROPY_REDUCTION_STARTED:
            if (!mCalculationInProgress) {return;}
            mStatusLabel->setText(tr("Minimizing the entropy"));
            break;
        case MessageCaluclationProgress::CALCULATION_FAILED:
            {
                QString errorText;
                switch (mcp->getErrorCode())
                {
                case MessageCaluclationProgress::CALCULATION_ERROR_NONE:
                    LogW("Calculation error message was sent but no error code was set");
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
                case MessageCaluclationProgress::CALCULATION_ERROR_NO_DATA_LEFTSECTION:
                    errorText = tr("Not enough keys recorded in left section.");
                    break;
                case MessageCaluclationProgress::CALCULATION_ERROR_NO_DATA_RIGHTSECTION:
                    errorText = tr("Not enough keys recorded in right section.");
                    break;
                default:
                    errorText = tr("Undefined error message.");
                    LogI("Undefined error message");
                    break;
                }

                QMessageBox::critical(this, tr("Calculation error"),
                                      QString("%1<br><br><b>%2: %3</b>").arg(errorText, tr("Error code"),
                                                               QString("%1").arg(mcp->getErrorCode())));

                onCancelCalculation();
                break;
            }
        default:
            break;
        }
    } else if (m->getType() == Message::MSG_PROJECT_FILE) {
        updateTitle();
    }
}


//-----------------------------------------------------------------------------
//                         Update algorithm title
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Function to update the title of the algorithm in use
///////////////////////////////////////////////////////////////////////////////

void CalculationProgressGroup::updateTitle()
{
    if (mGroupBox)
    {
        auto algorithm = CalculationManager::getSingleton().getCurrentAlgorithmInformation();
        mGroupBox->setTitle(tr("Calculation with: %1").arg(QString::fromStdWString(algorithm->getName())));
    }
}


//-----------------------------------------------------------------------------
//                            Start calculation
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Function called to start the calculation.
///
/// The function will also change the displayed text of the start button.
///////////////////////////////////////////////////////////////////////////////

void CalculationProgressGroup::onStartCalculation()
{
    CalculationAdapter::startCalculation();
}


//-----------------------------------------------------------------------------
//                           Cancel calculation
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Function called to cancel the calculation.
///
/// The function will also change the displayed text of the start button.
///////////////////////////////////////////////////////////////////////////////

void CalculationProgressGroup::onCancelCalculation()
{
    mStatusLabel->setText(tr("Calculation canceled"));
    mCalculationProgressBar->setValue(0);
    mStartCancelButton->setText(tr("Start calculation"));

    cancelCalculation();
    mCalculationInProgress = false;
}


//-----------------------------------------------------------------------------
//                          Reset calculation
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Function called to reset the calculation.
///
/// The function will also change the displayed text of the start button.
///////////////////////////////////////////////////////////////////////////////

void CalculationProgressGroup::onResetCalculation()
{
    if (DisplaySizeDefines::getSingleton()->isLEq(DS_XSMALL))
    {
        mStatusLabel->setText(QString());  // not enough space for the text
    }
    else
    {
        mStatusLabel->setText(tr("Press the button to start the calculation"));
    }

    mCalculationProgressBar->setValue(0);
    mStartCancelButton->setText(tr("Start calculation"));

    mCalculationInProgress = false;
}


//-----------------------------------------------------------------------------
//                      Start/Cancel button pressed
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Slot called when the start and stop button was pressed
///////////////////////////////////////////////////////////////////////////////

void CalculationProgressGroup::startCancelPressed()
{
    if (mCalculationInProgress) onCancelCalculation();
    else onStartCalculation();
}


//-----------------------------------------------------------------------------
//                           Show algorithm info
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Show information about the algorithm
///////////////////////////////////////////////////////////////////////////////

void CalculationProgressGroup::showAlgorithmInfo()
{
    AlgorithmDialog dialog(CalculationManager::getSingleton().getCurrentAlgorithmInformation(), PianoManager::getSingletonPtr()->getPiano(), parentWidget());
    dialog.exec();
    CalculationManager::getSingleton().setCurrentAlgorithmInformation(dialog.getAlgorithmInformation());
    updateTitle();
}
