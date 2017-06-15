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
//      Class describing the piano keyboard, holding a collection of keys
//=============================================================================

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "prerequisites.h"
#include "key.h"
#include "pianodefines.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Class describing the piano keyboard, holding a collection of keys
///
/// This class describes all data related to the keyboard of the piano. The
/// most important data is a vector of keys. In addition, this class
/// tells us where the key A4 is located and how many of the keys belong
/// to the bass bridge.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN Keyboard
{
public:
    using Keys = std::vector<Key>;

public:
    Keyboard(size_t initialSize);
    ~Keyboard(){}

    size_t size() const         {return mKeys.size();}
    void resize(size_t newSize) { mKeys.resize(newSize); }

    void changeKeyboardConfiguration (int numberOfKeys, int keyNumberOfA);

    void clearAllKeys();            ///< Clear all keys completely
    void clearComputedPitches();    ///< Set all computed pitches to zero
    void clearTunedPitches();       ///< Set all tuned pitches to zero
    void clearOverpulls();          ///< Set all overpull markers to zero

    // Access operators:
    const Key &operator[](size_t i) const   {return mKeys[i];}
    Key &operator[](size_t i)               {return mKeys[i];}
    const Key &at(size_t i) const           {return mKeys[i];}
    Key &at(size_t i)                       {return mKeys[i];}

    // get pointer to a particular key
    const Key *getKeyPtr(int i) const;
    Key *getKeyPtr(int i);

    // get a reference to the key vector
    Keys &getKeys() {return mKeys;}

    // access functions
    int getNumberOfKeys() const     {return static_cast<int>(size());}
    int getKeyNumberOfA4() const    {return mKeyNumberOfA4;}
    int &getKeyNumberOfA4()         {return mKeyNumberOfA4;}
    int getNumberOfBassKeys() const {return mNumberOfBassKeys;}
    int &getNumberOfBassKeys()      {return mNumberOfBassKeys;}
    void setNumberOfBassKeys(int keys) {mNumberOfBassKeys = keys;}


    // compute note name and color
    std::string getNoteName (int keynumber) const;
    piano::KeyColor getKeyColor (int keynumber) const;

private:
    Keys mKeys;             ///< Vector holding the keys
    int mKeyNumberOfA4;     ///< Index of the key A4 (440Hz)
    int mNumberOfBassKeys;  ///< Number of keys on bass bridge
};

#endif // KEYBOARD_H
