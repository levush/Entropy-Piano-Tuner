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

#ifndef MESSAGEKEYSELECTIONCHANGED_H
#define MESSAGEKEYSELECTIONCHANGED_H

#include "message.h"
#include "../piano/key.h"
#include <cstdint>

class MessageKeySelectionChanged : public Message
{
public:
    MessageKeySelectionChanged(int index, const Key *key, bool force = false);
    ~MessageKeySelectionChanged();

    int getKeyNumber() const {return mIndex;}
    const Key *getKey() const {return mKey;}
    bool isForced() const {return mForce;}

private:
    const int mIndex;
    const Key *mKey;
    const bool mForce;
};

#endif // MESSAGEKEYSELECTIONCHANGED_H
