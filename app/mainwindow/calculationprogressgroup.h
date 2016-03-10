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

#ifndef CALCULATIONPROGRESSGROUP_H
#define CALCULATIONPROGRESSGROUP_H

#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QComboBox>
#include <QMap>

#include "prerequisites.h"

#include "core/messages/messagelistener.h"
#include "core/adapters/calculationadapter.h"

#include "widgets/displaysizedependinggroupbox.h"

class AlgorithmInformation;

///////////////////////////////////////////////////////////////////////////////
/// \brief Visible QGroupBox to handle the calculation.
///
/// This group box shows the progress bar , the algorithm selector box,
/// and a button to start and stop the calculation.
///
/// Note that on small devices there will be no group box, just a widget!
/// Therefore this class does not inherit QGroupBox, instead it owns
/// a member variable of it.
///
/// While the algorithm is running it may emit various messages about its
/// status. Therefore, the CalculationProgressGroup is a MessageListener
///////////////////////////////////////////////////////////////////////////////

class CalculationProgressGroup : public DisplaySizeDependingGroupBox,
                                 public MessageListener,
                                 public CalculationAdapter
{
    Q_OBJECT

public:
    CalculationProgressGroup(Core *core, QWidget *parent = nullptr);
    ~CalculationProgressGroup() {} ///< Destructor without function

private:
    virtual void handleMessage(MessagePtr m) override final;

private slots:
    void updateTitle();
    void onStartCalculation();
    void onCancelCalculation();
    void onResetCalculation();
    void startCancelPressed();
    void showAlgorithmInfo();

private:

    bool mCalculationInProgress;            ///< Flag: calculation is in progress.
    QLabel *mStatusLabel;                   ///< Pointer to the status label
    QProgressBar *mCalculationProgressBar;  ///< Pointer to the progress bar
    QPushButton *mStartCancelButton;        ///< Pointer to the start and cancel button
};

#endif // CALCULATIONPROGRESSGROUP_H
