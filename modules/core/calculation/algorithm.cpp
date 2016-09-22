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

#include "algorithm.h"
#include <cmath>
#include "../messages/messagehandler.h"
#include "../messages/messagekeyselectionchanged.h"
#include "../messages/messagechangetuningcurve.h"
#include "../messages/messagecaluclationprogress.h"

//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
/// \brief Constructor of a tuning Algorithm
/// \param piano : Reference to the piano of wich a copy will be create
/// \param description : Description of type AlgorithmFactoryDescription
////////////////////////////////////////////////////////////////////////////////

Algorithm::Algorithm(const Piano &piano, const AlgorithmFactoryDescription &description) :
    mPiano(piano),
    mFactoryDescription(description),
    mParameters(piano.getAlgorithmParameters().getPreparedParameters(description.getAlgorithmName())),
    mKeyboard(mPiano.getKeyboard()),
    mKeys(mKeyboard.getKeys()),
    mNumberOfKeys(mKeyboard.getNumberOfKeys()),
    mKeyNumberOfA4(mKeyboard.getKeyNumberOfA4())
{
}


//-----------------------------------------------------------------------------
//                   Thread worker function of the algorithm
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Thread worker function of the Algorithm
///////////////////////////////////////////////////////////////////////////////

void Algorithm::workerFunction_impl()
{
    setThreadName("Algorithm");
    LogI("Start calculation.");
    MessageHandler::send<MessageCaluclationProgress>
            (MessageCaluclationProgress::CALCULATION_STARTED);

    // Call the worker function of the algorithm from here
    algorithmWorkerFunction();

    // After completion of the algorithm, deselect all keys.
    MessageHandler::send<MessageKeySelectionChanged>(-1,nullptr);

    MessageHandler::send<MessageCaluclationProgress>
            (MessageCaluclationProgress::CALCULATION_ENDED);
    LogI("End of calculation");
}


//-----------------------------------------------------------------------------
//                         Update the tuning curve
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Update the tuning curve
/// \param keynumber : Number of the key where the update takes place
/// \param frequency : Frequency to which the tuning curve is set
///////////////////////////////////////////////////////////////////////////////

void Algorithm::updateTuningCurve(int keynumber, double frequency)
{
    EptAssert (keynumber>=0 and keynumber<mNumberOfKeys,"Range of keynumber");
    mKeyboard[keynumber].setComputedFrequency(frequency);
    MessageHandler::send<MessageChangeTuningCurve>(keynumber,frequency);
}


//-----------------------------------------------------------------------------
//                         Show calculation progress
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Transmit the current percentage of progress to the messaging system
///
/// \param percentage : value in [0,1], corresponding to 0% and 100%
///////////////////////////////////////////////////////////////////////////////

void Algorithm::showCalculationProgress (double fraction)
{
    MessageHandler::send<MessageCaluclationProgress>
            (MessageCaluclationProgress::CALCULATION_PROGRESSED,fraction);
}


//-----------------------------------------------------------------------------
//                         Show calculation progress
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Transmit the current percentage of progress to the messaging system
///
/// \param key : reference to the key that is being processed, converted
/// to a percentage. The progress value is reconstructed from the measured
/// frequency.
/// \param start : The starting value of the progress value
/// \param range : The range of the progress value (start ... start+range)
///////////////////////////////////////////////////////////////////////////////

void Algorithm::showCalculationProgress (const Key &key,
                                                  double start, double range)
{
    double k = mKeyNumberOfA4 + 12 * std::log(key.getRecordedFrequency()/440.0)/log(2);
    showCalculationProgress(start+range*k/mNumberOfKeys);
}
