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

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <string>
#include "key.h"

class Keyboard
{
public:
    using Keys = std::vector<Key>;

    enum KeyColor {
        Black,
        White
    };

public:
    Keyboard(size_t initialSize = 88);
    ~Keyboard(){};

    void resize(size_t s);
    void clearKeys();
    size_t size() const {return mKeys.size();}

    const Key &operator[](size_t i) const {return mKeys[i];}
    Key &operator[](size_t i) {return mKeys[i];}

    const Key &at(size_t i) const {return mKeys[i];}
    Key &at(size_t i) {return mKeys[i];}

    const Key *getKeyPtr(int i) const {if (i < 0 || i >= static_cast<int>(mKeys.size())) {return nullptr;} return &mKeys[i];}
    Key *getKeyPtr(int i) {if (i < 0 || i >= static_cast<int>(mKeys.size())) {return nullptr;} return &mKeys[i];}

    Keys &getKeys() {return mKeys;}

    std::string getNoteName(int key) const;
    KeyColor getKeyColor(int k) const;

    void setNumberOfKeys(int keys, int keyNumberOfA);
    int getNumberOfKeys() const {return static_cast<int>(size());}

    int getKeyNumberOfA4() const {return mKeyNumberOfA4;}
    int &getKeyNumberOfA4() {return mKeyNumberOfA4;}

    int convertLocalToGlobal(int index) const;

private:
    Keys mKeys;

    /// total number of keys of the piano
    int mNumberOfKeys;

    /// index of the key A (corresponding with the concert pitch)
    int mKeyNumberOfA4;
};

#endif // KEYBOARD_H
