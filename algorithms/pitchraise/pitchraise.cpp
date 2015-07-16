/*****************************************************************************
 * Copyright 2015 Haye Hinrichsen, Christoph Wick
 *
 * This file is s of Entropy Piano Tuner.
 *
 * Entropy Piano Tuner is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * Entropy Piano Tuner is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A sICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Entropy Piano Tuner. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

// Berenung mittels

// Fehlercode Rückgabe

// Why after start of algorithm
// Application gone active?

//=============================================================================
//                        Pitch raise algorithm
//=============================================================================

#include "pitchraise.h"

#include "core/calculation/algorithmfactory.h"

//=============================================================================
//                        Create algorithm factory
//=============================================================================

template<>
const AlgorithmFactory<pitchraise::PitchRaise> AlgorithmFactory<pitchraise::PitchRaise>::mSingleton(
        AlgorithmFactoryDescription("pitchraise"));

namespace pitchraise
{

//-----------------------------------------------------------------------------
//                             Constructor
//-----------------------------------------------------------------------------

// constructor that expects the originam piano. This piano will be copied into mPiano which is why
// you may do any changes to mPiano because it doesnt change the actual one.

PitchRaise::PitchRaise(const Piano &piano, const AlgorithmFactoryDescription &description) :
    Algorithm(piano, description),
    // load the parameter from the description
    mSectionSeparatingKey(static_cast<int>(description.getDoubleParameter("sectionSeparator")))
{
}


//-----------------------------------------------------------------------------
//                             Worker function
//-----------------------------------------------------------------------------

void PitchRaise::algorithmWorkerFunction()
{
    LogI("Pitch Raise algorithm started");
    // linear regression data
    std::vector<double> X(2,0),Y(2,0),XX(2,0),XY(2,0),A(2,0),B(2,0);
    std::vector<int> N(2,0);

    for (int i = 0; i < mNumberOfKeys; ++i)
    {
        CHECK_CANCEL_THREAD;
        Key &key = mPiano.getKey(i);
        showCalculationProgress(key, 0.0, 0.2);
        if (key.getMeasuredInharmonicity()>1E-10)
        {
            double B = key.getMeasuredInharmonicity();
            int s = (i < mSectionSeparatingKey ? 0 : 1);
            if (s==0) LogI("Found recorded key number %d in the  left section with B=%lf.",i,B)
            else         LogI("Found recorded key number %d in the right section with B=%lf.",i,B);
            double x = i, y = -log(B);
            N[s]++;
            X[s] += x;
            Y[s] += y;
            XX[s] += x*x;
            XY[s] += x*y;
        }
    }
    if (N[0]==0)
    {
        // Error left
        LogW("Not enough keys recorded in the left section (key 1...%d)",mSectionSeparatingKey);
        return;
    }
    if (N[1]==0)
    {
        // Error right
        LogW("Not enough keys recorded in the right section (key %d...%d)",mSectionSeparatingKey,mNumberOfKeys);
        return;
    }

    // Perform linear regression (see https://de.wikipedia.org/wiki/Lineare_Regression)
    for (int s=0; s<2; ++s)
    {
        A[s] = (XX[s]*Y[s] -X[s]*XY[s]) / (N[s]*XX[s] - X[s]*X[s]);
        B[s] = (N[s]*XY[s] - X[s]*Y[s]) / (N[s]*XX[s] - X[s]*X[s]);
    }

    for (int i = 0; i < mNumberOfKeys; ++i)
    {
        int s = (i < mSectionSeparatingKey ? 0 : 1);
        double logB = A[s]+i*B[s];
        LogI("B[%d]=%lf",i,exp(-logB));
    }

        // set the tuning curve
        //updateTuningCurve(i, mPiano.getEqualTempFrequency(i, 0, mConcertPitchParam));
    }

}  // namespace pitchraise
