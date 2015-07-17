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

#include "core/math/mathtools.h"
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
    mSectionSeparatingKey(static_cast<int>(description.getIntParameter("sectionSeparator"))),
    mPitch(piano.getKeyboard().getNumberOfKeys(),0)

{
}


//-----------------------------------------------------------------------------
//                     Update the displayed tuning curve
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Update the tuning curve for a given key number
///
/// This function translates the actual mPitch value into the corresponding
/// frequency, stores the value in the local piano copy mPiano and sends a
/// message that the tuning curve has to be redrawn.
/// \param keynumber : Number of the key to be updated
///////////////////////////////////////////////////////////////////////////////

void PitchRaise::updateTuningcurve (int keynumber)
{
    EptAssert (keynumber>=0 and keynumber<mNumberOfKeys,"Range of keynumber");
    double f = mPiano.getDefiningTempFrequency(keynumber, mPitch[keynumber],440);
    updateTuningCurve(keynumber, f);
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Update the entire tuning curve
///
/// This function translates the all mPitch values into the corresponding
/// frequencies, stores the values in the local piano copy mPiano and sends a
/// sequence of messages that the tuning curve has to be redrawn.
///////////////////////////////////////////////////////////////////////////////

void PitchRaise::updateTuningcurve ()
{
    for (int keynumber = 0; keynumber < mNumberOfKeys; ++keynumber)
        updateTuningcurve(keynumber);
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

    // Define lambda regression function
    auto estimatedInharmonicity = [this,A,B] (int k)
    {
        int s = (k < mSectionSeparatingKey ? 0 : 1);
        double minusLogB = A[s] + k*B[s];
        return exp(-minusLogB);
    };


    for (int i = 0; i < mNumberOfKeys; ++i)
    {
        LogI("B[%d]=%lf",i,estimatedInharmonicity(i));
    }

    // For the computation we need at least two octaves to both sides of A4:
    if (mKeyNumberOfA4<=13 or mNumberOfKeys-mKeyNumberOfA4<=13) return;

    // Compute the expected stretch deviation of the n_th partial of a given key:
    auto cents = [estimatedInharmonicity] (int keynumber, int n)
    { return 600.0  / MathTools::LOG2 *
                log((1+n*n*estimatedInharmonicity(keynumber)) /
                    (1+estimatedInharmonicity(keynumber))); };

    // Define a linear section of the curve in the middle
    int numberA3 = mKeyNumberOfA4-12;
    int numberA4 = mKeyNumberOfA4;
    int numberA5 = mKeyNumberOfA4+12;
    double pitchA5 = 0.5*cents(numberA4,3)+0.5*cents(numberA4,2);
    double pitchA3 = cents(numberA4,2)-cents(numberA3,4);
    for (int k=numberA3; k<mKeyNumberOfA4; ++k)
        mPitch[k] = pitchA3*(mKeyNumberOfA4-k)/12.0;
    for (int k=mKeyNumberOfA4+1; k<=numberA5; ++k)
        mPitch[k] = pitchA5*(k-mKeyNumberOfA4)/12.0;

    // Extend curve to the right by iteration:
    for (int k=numberA5+1; k<mNumberOfKeys; k++)
    {
        double pitch42 = mPitch[k-12] + cents(k-12,4) -cents(k,2);
        double pitch21 = mPitch[k-12] + cents(k-12,2);
        mPitch[k] = 0.3*pitch42 + 0.7*pitch21;
    }

    // Extend the curve to the left by iteration:
    for (int k=numberA3-1; k>=0; k--)
    {
        double pitch42 = mPitch[k+12] + cents(k+12,2) -cents(k,4);
        double pitch105 = mPitch[k+12] + cents(k+12,5) -cents(k,10);
        double fraction = 1.0*k/numberA3;
        mPitch[k] = pitch42*fraction+pitch105*(1-fraction);
    }

    // set the tuning curve
    updateTuningcurve();
}



}  // namespace pitchraise
