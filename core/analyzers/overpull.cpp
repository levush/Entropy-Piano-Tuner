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

//=============================================================================
//                           Overpull estimator
//=============================================================================

#include "overpull.h"

#include <cmath>
#include <iostream>

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
/// the order of 20%.
///
/// \param averagePull : average pull, of the order of 0.2
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

    double DL,DR,SL,SR,SB,SN,shift;

    switch(mPianoType)
    {
    case piano::PT_GRAND:
        SL = 1200;      // speaking length treble string left
        SR = 50;        // speaking length treble string right
        SB = 1250;      // average speaking length bass string
        SN = 100;       // average non-speaking length in treble
        DL = 500;       // distance from bridge to frame left
        DR = 80;       // distance from bridge to frame right
        shift = 3;      // shift between bass and treble
        break;
    case piano::PT_UPRIGHT:
        SL = 1200;      // speaking length treble string left
        SR = 50;        // speaking length treble string right
        SB = 1200;      // average speaking length bass string
        SN = 100;       // average non-speaking length in treble
        DL = 450;       // distance from bridge to frame left
        DR = 100;       // distance from bridge to frame right
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
        if (j<8) unison=1; else if (j<B) unison=2;
        double stringlen = (k<B ? SB+SN : stringlength(k)+SN);
        return unison * epsilon(j,k) / stringlen;
    };

    // NORMALIZATION
    double sum = 0;
    for (int j=0; j<K; ++j) for (int k=0; k<K; ++k) sum += (R[j][k] = response(j,k));
    sum /= K;
    for (int j=0; j<K; ++j) for (int k=0; k<K; ++k) R[j][k] *= averagePull / sum;

    for (int k=0; k<K; ++k) std::cout << R[40][k] << std::endl;

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
}


//-----------------------------------------------------------------------------
//                            Compute overpull
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Compute the required overpull on the basis of the interaction
/// matrix.
///
/// This function first checks whether enough red markers have been set.
/// If so, it connects the red markers internally by linear interpolation.
/// This interpolation is then used to feed the interaction matrix, which
/// returns the required overpull.
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
    double cpitch = piano->getConcertPitch();
    if (K<=0 or B<=0 or keynumber<0 or keynumber>=K) return 0;

    // IF KEYBOARD PARAMETERS OR PITCH HAS CHANGED RE-INITIALIZE AGAIN
    if (K != mNumberOfKeys or B != mNumberOfBassKeys or  cpitch != mConcertPitch
        or piano->getPianoType() != mPianoType) init(piano);

    // CHECK WHETHER A GIVEN KEY PROVIDES VALID DATA
    auto valid = [this] (const Key &key)
    {return (key.getComputedFrequency() > 20 and key.getTunedFrequency() > 20);};
    auto isvalid = [this,valid,piano] (int k) { return valid(piano->getKey(k)); };

    // RED MARKERS HAVE TO BE PLACED WITH GAPS SMALLER THAN 7 HALFTONES
    // OTHERWISE RETURN 0 (DO NOT SHOW OVERPULL)
    const int maxGapsize = 7;
    int gapsize = 0;
    for (int k=0; k<K; ++k)
    {
        if (not isvalid(k)) gapsize++;
        else gapsize=0;
        if (gapsize > maxGapsize) return 0;
    }

    // COMPUTE HOW MUCH THE PITCH DEVIATES FROM THE COMPUTED TUNING CURVE
    auto deviation = [this,piano,cpitch] (int k)
    {
        auto key = piano->getKey(k);
        double computed = key.getComputedFrequency();
        double tuned = key.getTunedFrequency();
        double cpratio = cpitch/440.0;
        if (tuned<=0 or computed<=0 or cpratio<=0) return 0.0;
        return 1200.0*log2(tuned/computed/cpratio);
    };

    // FOR THE MISSING RED MARKERS CREATE A LINEAR INTERPOLATION
    std::vector<double> interpolation (K,0);
    auto interpolate = [this,isvalid,deviation,&interpolation] (int start, int end)
    {
        int leftmost = start, rightmost = end-1;
        while (not isvalid(leftmost) and leftmost < end) leftmost++;
        while (not isvalid(rightmost) and rightmost >= start) rightmost--;
        if (leftmost >= rightmost) return false;

        for (int i=start; i<leftmost; i++) interpolation[i]=deviation(leftmost);
        int left = leftmost, right;
        do
        {
            right = left+1;
            while (not isvalid(right) and right<=rightmost) right++;
            for (int i=left; i<right; ++i) interpolation[i]=
                    ((i-left)*deviation(right)+(right-i)*deviation(left))/(right-left);
            left=right;
        }
        while (right <= rightmost);
        for (int i=rightmost; i<end; i++) interpolation[i]=deviation(rightmost);
        return true;
    };

    // THE INTERPOLATION IS CARRIED OUT SEPARATELY ON BOTH BRIDGES
    if (not interpolate (0,B-1)) return 0;
    if (not interpolate (B-1,K-1)) return 0;

    // COMPUTE CURRENTLY NEEDED OVERPULL
    double overpull = 0, totaldeviation = 0;
    for (int k=0; k<K; k++) if (k != keynumber)
    {
        double delta = interpolation[k];
        if (delta>-2 and delta<10) delta=0;
        totaldeviation += delta;
        overpull -= delta * R[k][keynumber];
    }

//    // Write to HDD for testing purposes
//    std::ofstream os("000-overpull.dat");
//    for (int key=0; key<mNumberOfKeys; key++) if (deviation(key)) os << key << " " << deviation(key) << std::endl;
//    os << "&" << std::endl;
//    for (int key=0; key<mNumberOfKeys; key++) os << key << " " << interpolation[key] << std::endl;
//    os.close();

    // THE OVERPULL IS ONLY SHOWN IF THE PIANO IS ON AVERAGE MORE THAN 5 CENTS FLAT
    if (fabs(totaldeviation/K)>5) return overpull;
    else return 0;
}
