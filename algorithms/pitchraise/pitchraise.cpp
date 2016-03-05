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
//                        Pitch raise algorithm
//=============================================================================

#include "pitchraise.h"

#include "core/math/mathtools.h"
#include "core/messages/messagehandler.h"
#include "core/messages/messagecaluclationprogress.h"

ALGORITHM_CPP_START(pitchraise)

//-----------------------------------------------------------------------------
//                             Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor of the pitch-raise algorithm
///
/// \param piano : Reference to the original piano, will be copied to mPiano
/// \param description : Short description of the algorithm
///////////////////////////////////////////////////////////////////////////////

PitchRaise::PitchRaise(const Piano &piano, const AlgorithmFactoryDescription &description) :
    Algorithm(piano, description),
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

///////////////////////////////////////////////////////////////////////////////
/// \brief PitchRaise::algorithmWorkerFunction
///
/// This is the main worker function of the Pitch-Raise algorithm which is
/// executed in an independent thread. All computations are carried out here.
/// The main parts are the following. First the recorded keys are identified.
/// their measured inharmonicity is loaded and the logarithm is taken. The
/// assumption is that this logarithm varies linearly with the key index in
/// both diagonal sections of the strings. Therefore, an ordinary linear
/// regression is carried out. This allows one to define an approximated
/// inharmonicity for all keys (implemented as a lambda function). With this
/// data a tuning curve is computed by a mixed 4:2 6:3 ... tuning, similar
/// to the initial condition in the entropy minimizer. The whole process
/// is artificially slowed down (msleep) in order to show how the tuning
/// curve grows.
///////////////////////////////////////////////////////////////////////////////
///
void PitchRaise::algorithmWorkerFunction()
{
    LogI("Pitch Raise algorithm started");
    // linear regression data
    std::vector<double> X(2,0),Y(2,0),XX(2,0),XY(2,0),A(2,0),B(2,0);
    std::vector<int> N(2,0);

    // make tuning curve initially flat
    updateTuningcurve();

    for (int i = 0; i < mNumberOfKeys; ++i)
    {
        CHECK_CANCEL_THREAD;
        Key &key = mPiano.getKey(i);
        showCalculationProgress(i*0.25/mNumberOfKeys);
        msleep(5);
        if (key.getMeasuredInharmonicity()>1E-10)
        {
            double B = key.getMeasuredInharmonicity();
            int s = (i < mPiano.getKeyboard().getNumberOfBassKeys() ? 0 : 1);
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
    if (N[0]<2)
    {
        // Error: Not enough recorded keys in the left section
        MessageHandler::send<MessageCaluclationProgress>
                (MessageCaluclationProgress::CALCULATION_FAILED,
                 MessageCaluclationProgress::CALCULATION_ERROR_NO_DATA_LEFTSECTION);// Error left
        return;
    }
    if (N[1]<2)
    {
        // Error: Not enough recorded keys in the right section
        MessageHandler::send<MessageCaluclationProgress>
                (MessageCaluclationProgress::CALCULATION_FAILED,
                 MessageCaluclationProgress::CALCULATION_ERROR_NO_DATA_RIGHTSECTION);// Error left
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
        int s = (k < mPiano.getKeyboard().getNumberOfBassKeys() ? 0 : 1);
        double minusLogB = A[s] + k*B[s];
        return exp(-minusLogB);
    };

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
    {
        CHECK_CANCEL_THREAD;
        mPitch[k] = pitchA3*(mKeyNumberOfA4-k)/12.0;
        showCalculationProgress(0.25+(k-numberA3)*0.125/12);
        updateTuningcurve(k);
        msleep(30);

    }
    for (int k=mKeyNumberOfA4+1; k<=numberA5; ++k)
    {
        CHECK_CANCEL_THREAD;
        mPitch[k] = pitchA5*(k-mKeyNumberOfA4)/12.0;
        showCalculationProgress(0.375+(k-mKeyNumberOfA4)*0.125/12);
        updateTuningcurve(k);
        msleep(30);
    }

    // Extend curve to the right by iteration:
    for (int k=numberA5+1; k<mNumberOfKeys; k++)
    {
        CHECK_CANCEL_THREAD;
        double pitch42 = mPitch[k-12] + cents(k-12,4) -cents(k,2);
        double pitch21 = mPitch[k-12] + cents(k-12,2);
        mPitch[k] = 0.3*pitch42 + 0.7*pitch21;
        updateTuningcurve(k);
        showCalculationProgress(0.5+(k-numberA5)*0.25/(mNumberOfKeys-numberA5));
        msleep(30);
    }

    // Extend the curve to the left by iteration:
    for (int k=numberA3-1; k>=0; k--)
    {
        CHECK_CANCEL_THREAD;
        double pitch42 = mPitch[k+12] + cents(k+12,2) -cents(k,4);
        double pitch105 = mPitch[k+12] + cents(k+12,5) -cents(k,10);
        double fraction = 1.0*k/numberA3;
        mPitch[k] = pitch42*fraction+pitch105*(1-fraction);
        updateTuningcurve(k);
        msleep(30);
        showCalculationProgress(0.75+(numberA3-k)*0.25/(numberA3));
    }
}

ALGORITHM_CPP_END
