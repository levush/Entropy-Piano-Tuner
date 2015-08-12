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
#include "../system/eptexception.h"
#include "../math/mathtools.h"

Keyboard::Keyboard(size_t initialSize) :
    mKeys(initialSize),
    mKeyNumberOfA4(48),
    mKeyNumberTrebleBridge(28)
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

piano::KeyColor Keyboard::getKeyColor(int k) const {
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


