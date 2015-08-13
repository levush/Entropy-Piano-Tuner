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
//                           Overpull algorithm
//=============================================================================

#include "overpull.h"

#include <cmath>
#include <iostream>

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

void OverpullEstimator::computeInteractionMatrix (double average)
{
    int K = mNumberOfKeys;
    int B = mNumberOfBassKeys;
    int T = K-B;
    if (K<=0 or B<=0 or B>K) return;

    LogI("Compute overpull interaction matrix");

    // resize and reset the interaction matrix:
    R.resize(K);
    for (auto &row : R) row.resize(K);
    for (auto &row : R) row.assign(K,0);

    double DL,DR,SL,SR,alpha;

    switch(mPianoType)
    {
    case piano::PT_GRAND:
        SL = 1200;      // speaking length treble string left
        SR = 50;        // speaking length treble string right
        DL = 450;       // distance from bridge to frame left
        DR = 100;       // distance from bridge to frame right
        alpha = 0.5;
        break;
    case piano::PT_UPRIGHT:
        SL = 1200;      // speaking length treble string left
        SR = 50;        // speaking length treble string right
        DL = 450;       // distance from bridge to frame left
        DR = 100;       // distance from bridge to frame right
        alpha = 0.5;
        break;
    default:
        LogW("Undefined piano type encountered");
        break;
    }

    // (G+H(K-1))2^((1-K)/12)==SR
    // (G+H(B-1))2^((1-B)/12)==SL
    double G = (-(pow(2,(1 + B)/12.)*(-1 + K)*SL) + pow(2,K/12.)*B*SR)/
            (pow(2,0.08333333333333333)*(1 + B - K));
    double H = (pow(2,B/12.)*SL - pow(2,(-1 + K)/12.)*SR)/(1 + B - K);
    auto stringlength = [G,H] (int k) { return (G+H*k)*pow(2,-k/12.0); };

    for (int k=B; k<K; ++k) std::cout << "SL(" << k << ")=" << stringlength(k) << std::endl;

    auto xcoord = [DL,DR,K,T] (int k)
    { return (K+1-k)*(DL*(K-k)+DR*(2*T+k-K))/2/T; };

    auto case2 = [K,B,T,DL,DR,SL,SR,xcoord] (int j, int k)
    {
        double LT = 0.5*(T+1)*(DL+DR);
        double L  = LT+SL+SR;
        double a  = xcoord(k)+SR;
        double b  = L-a;
        double x  = xcoord(j)+SR;
        return b/L*x*(L*L-x*x-b*b) * pow(2.0,(j+k)/12.0);
    };

    auto TM = [case2] (int j, int k)
    {
        if (j>=k) return case2(j+1,k+1);
        else return case2(k+1,j+1);
    };

    auto RM = [TM,alpha,B] (int j, int k)
    {
        if (j<B and k<B) return alpha * alpha * TM(j+B,k+B);
        else if (j<B and k>=B) return alpha * TM(j+B,k);
        else if (j>=B and k<B) return alpha * TM(j,k+B);
        else return TM(j,k);
    };

    double sum = 0;
    for (int j=0; j<K; ++j) for (int k=0; k<K; ++k) sum += (R[j][k] = RM(j,k));
    sum /= K;
    for (int j=0; j<K; ++j) for (int k=0; k<K; ++k) R[j][k] *= average / sum;

    std::ofstream os("/home/hinrichsen/mat.m");
    os << "M={";
    for (int j=0; j<K; ++j)
    {
        os << "{";
        for (int k=0; k<K-1; ++k) os << R[j][k] << ",";
        os << R[j][K-1];
        if (j!=K-1) os << "},\n";
        else os << "}};" << std::endl;
    }
    os.close();
}


//-----------------------------------------------------------------------------
//                            Compute overpull
//-----------------------------------------------------------------------------

double OverpullEstimator::getOverpull (int keynumber, const Piano *piano)
{
    if (not piano) return 0;
    int K = piano->getKeyboard().getNumberOfKeys();
    int B = piano->getKeyboard().getNumberOfBassKeys();
    double cpitch = piano->getConcertPitch();
    if (K<=0 or B<=0 or keynumber<0 or keynumber>=K) return 0;

    // If parameter have changed recalculate the matrix
    if (K != mNumberOfKeys or B != mNumberOfBassKeys or  cpitch != mConcertPitch
        or piano->getPianoType() != mPianoType) init(piano);

    // check whether key provides valid data
    auto valid = [this] (const Key &key)
    {
        bool okay = (key.getComputedFrequency() > 20 and
                     key.getTunedFrequency() > 20);
        return okay;
    };
    auto isvalid = [this,valid,piano] (int k) { return valid(piano->getKey(k)); };

    const int maxGapsize = 10;
    int gapsize = 0;
    for (int k=0; k<K; ++k)
    {
        if (not isvalid(k)) gapsize++;
        else gapsize=0;
        if (gapsize > maxGapsize) return 0;
    }

    // compute tuned deviation against computed levels in cents
    auto deviation = [this,piano,cpitch] (int k)
    {
        auto key = piano->getKey(k);
        double computed = key.getComputedFrequency();
        double tuned = key.getTunedFrequency();
        double cpratio = cpitch/440.0;
        if (tuned<=0 or computed<=0 or cpratio<=0) return 0.0;
        return 1200.0*log2(tuned/computed/cpratio);
    };

    // If so, create a piecewise linear interpolation of tuning levels
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

    // Interpolate the two bridges separately.
    if (not interpolate (0,B-1)) return 0;
    if (not interpolate (B-1,K-1)) return 0;

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

    if (fabs(totaldeviation/K)>5) return overpull;
    else return 0;
}
