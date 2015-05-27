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

#include "resettorecording.h"

// include the factory we will create later
#include "core/calculation/algorithmfactory.h"

template<>
const AlgorithmFactory<resettorecording::ResetToRecording> AlgorithmFactory<resettorecording::ResetToRecording>::mSingleton(
        AlgorithmFactoryDescription("resettorecording"));

namespace resettorecording
{

ResetToRecording::ResetToRecording(const Piano &piano, const AlgorithmFactoryDescription &description) :
    Algorithm(piano, description)
{
}

void ResetToRecording::algorithmWorkerFunction()
{

    for (int i = 0; i < mNumberOfKeys; ++i)
    {
        // set the tuning curve
        updateTuningCurve(i, mPiano.getKey(i).getRecordedFrequency());
    }
}

}  // namespace resettorecording
