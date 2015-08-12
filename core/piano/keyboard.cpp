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

#include "keyboard.h"
#include "../system/log.h"
#include "../system/eptexception.h"
#include "../math/mathtools.h"

Keyboard::Keyboard(size_t initialSize) :
    mKeys(initialSize),
    mKeyNumberOfA4(48),
    mNumberOfBassKeys(28)
{
}


void Keyboard::resize(size_t s)
{
    mKeys.resize(s);
}

void Keyboard::clearKeys()
{
    for (auto &key : mKeys) key.clear();
}

void Keyboard::clearComputedPitches()
{
    for (auto &key : mKeys) key.setComputedFrequency(0);
}

void Keyboard::clearTunedPitches()
{
    for (auto &key : mKeys) key.setTunedFrequency(0);
}

//----------------------------------------------------------------------
//			             set the number of keys
//----------------------------------------------------------------------


void Keyboard::setNumberOfKeys(int keys, int keyNumberOfA) {
    Keys keyCopy(std::move(mKeys));
    mKeys.resize(0);
    mKeys.resize(keys);

    int startIndex = mKeyNumberOfA4 - keyNumberOfA;

    // copy keys
    for (int toIndex = 0; toIndex < static_cast<int>(mKeys.size()); ++toIndex) {
        int fromIndex = toIndex + startIndex;
        if (fromIndex >= 0 && fromIndex < static_cast<int>(keyCopy.size())) {
            mKeys[toIndex] = keyCopy[fromIndex];
        }
    }

    mKeyNumberOfA4 = keyNumberOfA;
}

// Tells the graphics how the keys look like
// This function should not be static

std::string Keyboard::getNoteName(int key) const {
    // TODO
    const int K = size();
    const int Akey = mKeyNumberOfA4;

    if (key < 0 || key >+ K) return "--";
    int Ckey = Akey - 9;
    int octave = (key - Ckey + 120) / 12 - 6;
    int index = (key - Ckey + 120) % 12;
    const std::string names[12]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
    char octchar = (char)('0' + octave);
    return names[index] + octchar;
}

// Tells whether the key index k is black or white

piano::KeyColor Keyboard::getKeyColor(int k) const
{
    const piano::KeyColor White = piano::KC_WHITE;
    const piano::KeyColor Black = piano::KC_BLACK;
    const piano::KeyColor scheme[12] =
    {White, Black, White, Black, White, White, Black, White, Black, White, Black, White};
    return scheme[(k + 9 + (12 * 100 - getKeyNumberOfA4())) % 12];
}

// converts a key index on the local scale [0, numberOfKeys] to the global [0, 87] scale

int Keyboard::convertLocalToGlobal(int index) const {
    const int scaleOffset = mKeyNumberOfA4 - 48;

    return index - scaleOffset;
}


//-----------------------------------------------------------------------------
//                         Overpull interaction matrix
//-----------------------------------------------------------------------------

double Keyboard::overpullMatrix (const int keyin, const int keyout,
                                 piano::PianoType pt, int basskeys) const
{
    if (keyin+keyout+basskeys>0){}
    switch(pt)
    {
    case piano::PT_GRAND:
        return 0.02;
        break;
    case piano::PT_UPRIGHT:
        return 0.02;
        break;
    default:
        LogW("Undefined piano type encountered");
        break;
    }
    return 0;
}

//-----------------------------------------------------------------------------
//                            Compute overpull
//-----------------------------------------------------------------------------

double Keyboard::computeOverpull (int keynumber, double concertPitch, piano::PianoType pt) const
{
    // First we have to check whether enough measured values exist
    int numberofkeys = mKeys.size();
    if (numberofkeys <= 0) return 0;

    // check whether key provides valid data
    auto valid = [this] (const Key &key)
    {
        return ( key.getComputedFrequency() > 20 and
                 key.getTunedFrequency() > 20 );
    };
    auto isvalid = [this,valid] (int key) { return valid(mKeys[key]); };

    const int maxGapsize = 10;
    int gapsize = 0;
    for (auto &key : mKeys)
    {
        if (valid(key)) gapsize++; else gapsize=0;
        if (gapsize > maxGapsize) return 0;
    }

    // compute tuned deviation against computed levels in cents
    auto deviation = [this,concertPitch] (const Key &key)
    {
        double computed = key.getComputedFrequency();
        double tuned = key.getTunedFrequency();
        double cpratio = concertPitch/440.0;
        if (tuned<=0 or computed<=0 or cpratio<=0) return 0.0;
        return 1200.0*log2(tuned/computed/cpratio);
    };


    // If so, create a piecewise linear interpolation of tuning levels
    std::vector<double> interpolation (numberofkeys,0);
    auto interpolate = [this,&isvalid,&deviation,&interpolation] (int start, int end)
    {
        int leftmost = start, rightmost = end-1;
        while (not isvalid(leftmost) and leftmost < end) leftmost++;
        while (not isvalid(rightmost) and rightmost >= start) rightmost--;
        if (leftmost >= rightmost) return false;

        for (int i=start; i<leftmost; i++) interpolation[i]=deviation(mKeys[leftmost]);
        int left = leftmost, right;
        do
        {
            right = left+1;
            while (not isvalid(right) and right<=rightmost) right++;
            for (int i=left; i<right; ++i) interpolation[i]=
                    ((i-left)*deviation(mKeys[right])+
                     (right-i)*deviation(mKeys[left]))/(right-left);
            left=right;
        }
        while (right <= rightmost);
        for (int i=rightmost; i<end; i++) interpolation[i]=deviation(mKeys[rightmost]);
        return true;
    };
    // Interpolate the two bridges separately.
    if (not interpolate (0,mNumberOfBassKeys)) return 0;
    if (not interpolate (mNumberOfBassKeys,numberofkeys)) return 0;

    double overpull = 0;
    for (int key=0; key<numberofkeys; key++) if (key != keynumber)
    {
        double delta = deviation(mKeys[key]);
        overpull -= delta * overpullMatrix(key,keynumber,pt,mNumberOfBassKeys);
    }

//    // Write to HDD for testing purposes
//    std::ofstream os("000-overpull.dat");
//    for (int key=0; key<mNumberOfKeys; key++) if (deviation(key)) os << key << " " << deviation(key) << std::endl;
//    os << "&" << std::endl;
//    for (int key=0; key<mNumberOfKeys; key++) os << key << " " << interpolation[key] << std::endl;
//    os.close();


    return overpull;
}
