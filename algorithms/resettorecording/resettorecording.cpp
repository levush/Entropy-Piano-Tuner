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
//                         Reset to recorded pitches
//=============================================================================

#include "resettorecording.h"
#include "core/system/log.h"

ALGORITHM_CPP_START(resettorecording)

//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor of the copy algorithm
/// \param piano : Reference to a copy of the piano
/// \param description : String describing the algorithm
///////////////////////////////////////////////////////////////////////////////

ResetToRecording::ResetToRecording(const Piano &piano, const AlgorithmFactoryDescription &description) :
    Algorithm(piano, description)
{
}


//-----------------------------------------------------------------------------
//               Worker function carrying out the computation
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Worker function in which the computation thread is carried out
///////////////////////////////////////////////////////////////////////////////

void ResetToRecording::algorithmWorkerFunction()
{
    LogI("Algorithm ResetToRecording carried out, copying the recorded pitches to the tuning curve.")

    const int A4key = mPiano.getKeyboard().getKeyNumberOfA4();
    double fA4 = mPiano.getKey(A4key).getRecordedFrequency();
    // if A4 was not recorded then fall back to 440 Hz
    if (fA4 < 400 or fA4 > 480) fA4=440;

    // Copy all recorded values, normalizing them to the wanted ConcertPitch
    for (int i = 0; i < mNumberOfKeys; ++i)
    {
        msleep(10);
        updateTuningCurve(i, mPiano.getKey(i).getRecordedFrequency()/fA4*440);
    }
}

ALGORITHM_CPP_END
