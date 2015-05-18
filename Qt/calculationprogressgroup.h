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

#ifndef CALCULATIONPROGRESSGROUP_H
#define CALCULATIONPROGRESSGROUP_H

#include <QGroupBox>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QComboBox>
#include <QMap>
#include "../core/messages/messagelistener.h"
#include "../core/adapters/calculationadapter.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Visible QGroupBox to handle the calculation.
///
/// This group box will have the progress bar and a button to start and stop
/// the calculation.
///////////////////////////////////////////////////////////////////////////////
class CalculationProgressGroup : public QGroupBox, public MessageListener, public CalculationAdapter
{
    Q_OBJECT
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Constructor.
    /// \param core : Pointer to the Core
    /// \param parent : Paren widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    CalculationProgressGroup(Core *core, bool smallDevice, QWidget *parent = nullptr);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Destructor.
    ///////////////////////////////////////////////////////////////////////////////
    ~CalculationProgressGroup();

private:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Message handling.
    /// \param m : The message
    ///////////////////////////////////////////////////////////////////////////////
    virtual void handleMessage(MessagePtr m) override final;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function called to start the calculation.
    ///
    /// The function will also change the displayed text of the start button.
    ///////////////////////////////////////////////////////////////////////////////
    void startCalculation();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function called to cancel the calculation.
    ///
    /// The function will also change the displayed text of the start button.
    ///////////////////////////////////////////////////////////////////////////////
    void cancelCalculation();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function called to reset the calculation.
    ///
    /// The function will also change the displayed text of the start button.
    ///////////////////////////////////////////////////////////////////////////////
    void resetCalculation();

private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Slot called when the start and stop button was pressed
    ///////////////////////////////////////////////////////////////////////////////
    void startCancelPressed();


    void showAlgorithmInfo();

private:
    /// Variable to store whether the calculation is in progress.
    bool mCalculationInProgress;

    /// Pointer to the status label
    QLabel *mStatusLabel;

    /// Pointer to the progress bar
    QProgressBar *mCalculationProgressBar;

    /// Pointer to the start and cancel button
    QPushButton *mStartCancelButton;

    /// Pointer to the algorithm selector
    QString mAlgorithmSelection;
};

#endif // CALCULATIONPROGRESSGROUP_H
