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
//                           Overpull estimator
//=============================================================================

#include "overpull.h"

#include <cmath>
#include <iostream>

#include "../system/log.h"
#include "../math/mathtools.h"

//-----------------------------------------------------------------------------
//                               Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor, resetting the member variables.
///////////////////////////////////////////////////////////////////////////////

OverpullEstimator::OverpullEstimator() :
    mPianoType(piano::PT_COUNT),
    mNumberOfKeys(0),
    mNumberOfBassKeys(0),
    mConcertPitch(0)
{
}


//-----------------------------------------------------------------------------
//                        Initialize interaction matrix
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Initialize
///
/// This function initializes the member variables and calls the function
/// computeInteractionMatrix.
/// \param piano : Pointer to the piano structure
///////////////////////////////////////////////////////////////////////////////

void OverpullEstimator::init (const Piano *piano)
{
    if (not piano) return;
    mNumberOfKeys = piano->getKeyboard().getNumberOfKeys();
    mNumberOfBassKeys = piano->getKeyboard().getNumberOfBassKeys();
    mConcertPitch = piano->getConcertPitch();
    mPianoType = piano->getPianoType();
    computeInteractionMatrix();
}


//-----------------------------------------------------------------------------
//                         Overpull interaction matrix
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Compute the interaction matrix between the string.
///
/// The essential data structure that is needed to operate the overpull system
/// is a response matrix R which tells us how many cents the string number k
/// will fall if we increase string number j by one cent. On average the
/// response is proprtional to the parameter averagePull and should be of
/// the order of 22%.
///
/// This function contains the full overpull theory
///
/// \param averagePull : average pull, of the order of 0.22
///////////////////////////////////////////////////////////////////////////////

void OverpullEstimator::computeInteractionMatrix (double averagePull)
{
    int K = mNumberOfKeys;
    int B = mNumberOfBassKeys;
    if (K<=0 or B<=0 or B>K) return;

    LogI("Compute overpull interaction matrix");

    // resize and reset the interaction matrix:
    R.resize(K);
    for (auto &row : R) row.resize(K);
    for (auto &row : R) row.assign(K,0);

    double DL=0,DR=0,SL=0,SR=0,SB=0,SN=0,shift=0;

    switch(mPianoType)
    {
    case piano::PT_GRAND:
        SL = 1200;      // speaking length treble string left
        SR = 50;        // speaking length treble string right
        SB = 1250;      // average speaking length bass string
        SN = 100;       // average non-speaking length everywhere
        DL = 500;       // distance from bridge to frame left
        DR = 30;        // distance from bridge to frame right
        shift = 3;      // shift between bass and treble
        break;
    case piano::PT_UPRIGHT:
        SL = 1200;      // speaking length treble string left
        SR = 50;        // speaking length treble string right
        SB = 1200;      // average speaking length bass string
        SN = 100;       // average non-speaking length everywhere
        DL = 450;       // distance from bridge to frame left
        DR = 50;        // distance from bridge to frame right
        shift = 10;     // shift between bass and treble
        break;
    default:
        LogW("Undefined piano type encountered");
        break;
    }

    // COMPUTE APPROXIMATE SPEAKING LENGTH OF THE TREBLE STRINGS
    double G =-(pow(2,-0.08333333333333333)*
                (-(SL*pow(2,0.08333333333333333 + B/12.)) +
                  K*SL*pow(2,0.08333333333333333 + B/12.) - B*SR*pow(2,K/12.))*
                pow(1 + B - K,-1));
    double H =((SL*pow(2,1 + B/12.) - SR*pow(2,0.9166666666666666 + K/12.))*
               pow(1 + B - K,-1))/2.;
    auto stringlength = [G,H] (int k) { return (G+H*k)*pow(2,-k/12.0); };

    // COMPUTE APPROXIMATE ARC LENGTH ALONG THE TREBLE BRIDGE
    std::vector<double> arc(K,0);
    const double d = 13.6; // Unison distance in mm
    arc[B]=DL;
    for (int k=B+1; k<K; ++k) arc[k] = arc[k-1] +
            sqrt(d*d + pow(stringlength(k)-stringlength(k-1),2));
    double L = arc[K-1]+DR;

    // DEFLECTION FORMULA FOR A BAR
    auto def = [L,arc] (int j, int k)
    {
        double a  = arc[k];
        double b  = L-a;
        double x  = arc[j];
        return b/L*x*(L*L-x*x-b*b) ;
    };

    // COMPUTE SYMMETRIC DEFLECTION RESPONSE
    auto deflection = [def] (int j, int k)
    {
        if (j<=k) return def(j,k);
        else return def(k,j);
    };

    // COPY BEHAVIOR INTO THE BASS SECTION
    auto epsilon = [deflection,B,shift] (int j, int k)
    {
        if (j<B and k<B) return deflection(j+B+shift,k+B+shift);
        else if (j<B and k>=B) return deflection(j+B+shift,k);
        else if (j>=B and k<B) return deflection(j,k+B+shift);
        else return deflection(j,k);
    };

    // TAKE NUMBER OF STRINGS AND THEIR LENGTH INTO ACCOUNT
    auto response = [B,SB,SN,stringlength,epsilon] (int j, int k)
    {
        double unison=3;
        if (k<8) unison=1; else if (k<B) unison=2;
        double stringlen = (j<B ? SB+SN : stringlength(j)+SN);
        return unison * epsilon(j,k) / stringlen;
    };

    // NORMALIZATION
    double sum = 0;
    double p = 0.8; // weight of sound board deformation 80 %
    for (int j=0; j<K; ++j) for (int k=0; k<K; ++k) sum += (R[j][k] = response(j,k));
    sum /= K;
    for (int j=0; j<K; ++j) for (int k=0; k<K; ++k)
        R[j][k] *= p * averagePull / sum;


    // BRIDGE TILT
    const double sigma = 20;
    const double amplitude = averagePull * (1-p);
    double avstring = B/SB;
    for (int k=B; k<K; k++) avstring += 1.0/stringlength(k);
    avstring /= K;
    double prefactor = amplitude / sqrt(2*3.141) / sigma / avstring / 0.7;
    for (int j=0; j<B; ++j) for (int k=0; k<B; ++k) R[j][k] += prefactor*exp(-0.5*(j-k)*(j-k)/sigma/sigma)/SB;
    for (int j=B; j<K; ++j) for (int k=B; k<K; ++k) R[j][k] += prefactor*exp(-0.5*(j-k)*(j-k)/sigma/sigma)/stringlength(j);

//    // Compute average (only for testing)
//    sum=0;
//    for (int j=0; j<K; ++j) for (int k=0; k<K; ++k) sum += R[j][k];
//    std::cout << "************************" << sum/K << "******************************" << std::endl;

//    // Write data (only for testing purposes)
//    std::ofstream os("/home/hinrichsen/mat.m");
//    os << "M={";
//    for (int j=0; j<K; ++j)
//    {
//        os << "{";
//        for (int k=0; k<K-1; ++k) os << R[j][k] << ",";
//        os << R[j][K-1];
//        if (j!=K-1) os << "},\n";
//        else os << "}};" << std::endl;
//    }
//    os.close();

//    os.open("/home/hinrichsen/response.dat");
//    for (int j=0; j<K; ++j)
//    {
//        double sum=0;
//        for (int k=0; k<K; ++k) sum += R[j][k];
//        os << sum << std::endl;
//    }
//    os.close();

}


//-----------------------------------------------------------------------------
//                            Compute overpull
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Compute the required overpull on the basis of the interaction
/// matrix.
///
/// This function first checks whether enough red markers have been set.
/// Then it computed the overpull according to these markers.
/// \param keynumber : Number of the key to be tuned
/// \param piano : Pointer to the piano structure
/// \return Overpull in cents
///////////////////////////////////////////////////////////////////////////////

double OverpullEstimator::getOverpull (int keynumber, const Piano *piano)
{
    // CHECK PARAMETERS FOR CONSISTENCY, OTHERWISE RETURN 0
    if (not piano) return 0;
    int K = piano->getKeyboard().getNumberOfKeys();
    int B = piano->getKeyboard().getNumberOfBassKeys();
    double cpratio = piano->getConcertPitch() / 440.0;
    if (K<=0 or B<=0 or keynumber<0 or keynumber>=K) return 0;

    // IF KEYBOARD PARAMETERS OR PITCH HAS CHANGED RE-INITIALIZE AGAIN
    if (K != mNumberOfKeys or B != mNumberOfBassKeys
        or   piano->getConcertPitch() != mConcertPitch
        or piano->getPianoType() != mPianoType) init(piano);

    //
    std::vector<double> weightedRedMarkers (K,0);
    const int maxGapsize = 7;
    int gapsize = 0;
    int lastkey = 0;
    double lastchi = 0;
    for (int k=0; k<K; ++k)
    {
        double computed   = piano->getKey(k).getComputedFrequency();
        double tuned = piano->getKey(k).getTunedFrequency();
        if (computed>20 and computed<20000 and tuned>20 and tuned<20000 and cpratio>0)
        {
            double chi = 1200.0*log(tuned/computed/cpratio)/MathTools::LOG2;
            weightedRedMarkers[k] = chi*(gapsize+1);
            lastkey = k; lastchi = chi;
            gapsize=0;
        }
        else gapsize++;
        if (gapsize > maxGapsize) return 0;
        if (k==K-1 and gapsize>0)
            weightedRedMarkers[lastkey] += lastchi*gapsize;
    }

    // COMPUTE CURRENTLY NEEDED OVERPULL
    double overpull = 0, totaldeviation = 0;
    for (int k=0; k<K; k++) if (weightedRedMarkers[k])
    {
        totaldeviation += weightedRedMarkers[k];
        overpull -= weightedRedMarkers[k] * R[keynumber][k];
    }


    // THE OVERPULL IS ONLY SHOWN IF THE PIANO IS ON AVERAGE MORE THAN 5 CENTS FLAT
    if (fabs(totaldeviation/K)>5) return overpull;
    else return 0;
}
