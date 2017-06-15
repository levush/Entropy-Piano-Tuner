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

#include "keyboard.h"
#include "piano.h"


//-----------------------------------------------------------------------------
//                          Keyboard Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Keyboard constructor
/// \param initialSize : Initial size of the keyboard (standard = 88)
///////////////////////////////////////////////////////////////////////////////

Keyboard::Keyboard(size_t initialSize) :
    mKeys(initialSize),
    mKeyNumberOfA4(Piano::DEFAULT_KEY_NUMBER_OF_A),
    mNumberOfBassKeys(Piano::DEFAULT_KEYS_ON_BASS_BRIDGE)
{
}


//-----------------------------------------------------------------------------
//                              Clearing data
//-----------------------------------------------------------------------------

void Keyboard::clearAllKeys()
{ for (auto &key : mKeys) key.clear(); }

void Keyboard::clearComputedPitches()
{ for (auto &key : mKeys) key.setComputedFrequency(0); }

void Keyboard::clearTunedPitches()
{ for (auto &key : mKeys) key.setTunedFrequency(0); }

void Keyboard::clearOverpulls()
{ for (auto &key : mKeys) key.setOverpull(0); }



//-----------------------------------------------------------------------------
//			              Change keyboard configuration
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Change keyboard configuration
///
/// This function allows the configuration (size and location of A4) to
/// be changed while the application is running. Keys for which data is
/// available are automatically shifted to the new component.
/// \param numberOfKeys : new number of piano keys (standard 88).
/// \param keyNumberOfA : New index of A4 key (standard 48, counting from 0)
///////////////////////////////////////////////////////////////////////////////

void Keyboard::changeKeyboardConfiguration (int numberOfKeys, int keyNumberOfA)
{
    Keys keyCopy(std::move(mKeys));
    mKeys.clear();
    mKeys.resize(numberOfKeys);

    // if keys already exist they are shifted to their new locations:
    for (int toIndex = 0; toIndex < static_cast<int>(mKeys.size()); ++toIndex)
    {
        int fromIndex = toIndex + mKeyNumberOfA4 - keyNumberOfA;
        if (fromIndex >= 0 && fromIndex < static_cast<int>(keyCopy.size()))
        {
            mKeys[toIndex] = keyCopy[fromIndex];
        }
    }
    mKeyNumberOfA4 = keyNumberOfA;
}



//-----------------------------------------------------------------------------
//			              Get pointer to a key
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get pointer to a key with a given number, returning nullptr if
/// the number is out of range (read-only version).
/// \param i : Number of the key
/// \return : Pointer to the key
///////////////////////////////////////////////////////////////////////////////

const Key * Keyboard::getKeyPtr (int i) const
{
    if (i < 0 || i >= static_cast<int>(mKeys.size())) {return nullptr;}
    return &mKeys[i];
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Get pointer to a key with a given number, returning nullptr if
/// the number is out of range (read-write version).
/// \param i : Number of the key
/// \return : Pointer to the key
///////////////////////////////////////////////////////////////////////////////

Key * Keyboard::getKeyPtr(int i)
{
    if (i < 0 || i >= static_cast<int>(mKeys.size())) {return nullptr;}
    return &mKeys[i];
}


//-----------------------------------------------------------------------------
//                        Get the name of the key
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get the name of the key as a string.
///
/// \param keynumber : Number of the key
/// \return : Name of the corresponding note (American notation).
///////////////////////////////////////////////////////////////////////////////

std::string Keyboard::getNoteName (int keynumber) const
{
    const int K = static_cast<int>(size());
    const int Akey = mKeyNumberOfA4;

    if (keynumber < 0 || keynumber >+ K) return "--";
    int Ckey = Akey - 9;
    int octave = (keynumber - Ckey + 120) / 12 - 6;
    int index = (keynumber - Ckey + 120) % 12;
    const std::string names[12]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
    char octchar = (char)('0' + octave);
    return names[index] + octchar;
}


//-----------------------------------------------------------------------------
//                        Get the color of the key
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get the color of the key (black / white)
/// \param keynumber : Number of the key
/// \return Color of the key
///////////////////////////////////////////////////////////////////////////////

piano::KeyColor Keyboard::getKeyColor (int keynumber) const
{
    const piano::KeyColor White = piano::KC_WHITE;
    const piano::KeyColor Black = piano::KC_BLACK;
    const piano::KeyColor scheme[12] =
    {White, Black, White, Black, White, White, Black, White, Black, White, Black, White};
    return scheme[(keynumber + 9 + (12 * 100 - getKeyNumberOfA4())) % 12];
}
