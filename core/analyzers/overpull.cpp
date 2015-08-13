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
#include "cmath"

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

    double DL,DR,d,a;

    switch(mPianoType)
    {
    case piano::PT_GRAND:
        DL = 16;
        DR = 45;
        d = 450;
        a = 0.5;
        break;
    case piano::PT_UPRIGHT:
        DL = 16;
        DR = 25;
        d = 150;
        a = 0.4;
        break;
    default:
        LogW("Undefined piano type encountered");
        break;
    }

    auto case2 = [K,B,T,DL,DR,d] (int j, int k)
    {
    return
    ((-1 + j - K)*(DL*(j - K) + DR*(-j + K - 2*T))*
         (d - ((-1 + k - K)*(DL*(k - K) + DR*(-k + K - 2*T)))/(2.*T) +
           ((DL + DR)*(1 + T))/2.)*
         (-(pow(-1 + j - K,2)*pow(DL*(j - K) + DR*(-j + K - 2*T),2))/
            (4.*pow(T,2)) + pow(d + ((DL + DR)*(1 + T))/2.,2) -
           pow(d - ((-1 + k - K)*(DL*(k - K) + DR*(-k + K - 2*T)))/
              (2.*T) + ((DL + DR)*(1 + T))/2.,2)))/
       (2.*T*(d + ((DL + DR)*(1 + T))/2.));
    };

    auto TM = [case2] (int j, int k)
    {
        if (j>=k) return case2(j+1,k+1);
        else return case2(k+1,j+1);
    };

    auto RM = [TM,a,B] (int j, int k)
    {
        if (j<B and k<B) return a * a * TM(j+B,k+B);
        else if (j<B and k>=B) return a * TM(j+B,k);
        else if (j>=B and k<B) return a * TM(j,k+B);
        else return TM(j,k);
    };

    double sum = 0;
    for (int j=0; j<K; ++j) for (int k=0; k<K; ++k) sum += (R[j][k] = RM(j,k));
    sum /= K;
    for (int j=0; j<K; ++j) for (int k=0; k<K; ++k) R[j][k] *= average / sum;

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

//    double overpull = -deviation(keynumber);
    double overpull = 0;
    for (int k=0; k<K; k++) if (k != keynumber)
    {
        double delta = deviation(k);
        overpull -= delta * R[k][keynumber];
    }

//    // Write to HDD for testing purposes
//    std::ofstream os("000-overpull.dat");
//    for (int key=0; key<mNumberOfKeys; key++) if (deviation(key)) os << key << " " << deviation(key) << std::endl;
//    os << "&" << std::endl;
//    for (int key=0; key<mNumberOfKeys; key++) os << key << " " << interpolation[key] << std::endl;
//    os.close();


    return overpull;
}
