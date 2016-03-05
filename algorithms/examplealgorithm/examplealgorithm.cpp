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

#include "examplealgorithm.h"

ALGORITHM_CPP_START(examplealgorithm)

// constructor that expects the originam piano. This piano will be copied into mPiano which is why
// you may do any changes to mPiano because it doesnt change the actual one.
ExampleAlgorithm::ExampleAlgorithm(const Piano &piano, const AlgorithmFactoryDescription &description) :
    Algorithm(piano, description),
    // load the parameter from the description
    mConcertPitchParam(mParameters->getDoubleParameter("concertPitch"))
{
}

// This is the actual worker function in which you have to define your algorithm.
// It will be started in a separate std::thread.
// It might happen that the user wants to cancel the algorithm even though it is not
// finished yet. Therefore you have to set exit marks using CHECK_CANCEL_THREAD, at
// whom the algorithm can exit and the thread will be stopped.
void ExampleAlgorithm::algorithmWorkerFunction()
{
    // use check cancel thread flags to mark exit points (if the user wants to cancel)
    CHECK_CANCEL_THREAD;

    // do the calculation here

    // for example we just to a equidistant tuning
    for (int i = 0; i < mNumberOfKeys; ++i)
    {
        // usually the exit mark should be set at each time before you make some kind of longer calculation,
        // that can't be interrupted.
        CHECK_CANCEL_THREAD;

        // show the calculation progress to the user
        showCalculationProgress(static_cast<double>(i) / mNumberOfKeys);

        // sleep for 0.2 sec so that you can see the changes in the tuning curve and test to cancel the algorithm
        msleep(200);

        // set the tuning curve
        updateTuningCurve(i, mPiano.getEqualTempFrequency(i, 0, mConcertPitchParam));
    }
}

ALGORITHM_CPP_END
