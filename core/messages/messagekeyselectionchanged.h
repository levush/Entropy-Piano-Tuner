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
//            Message indicating that the selected key has changed
//=============================================================================

#ifndef MESSAGEKEYSELECTIONCHANGED_H
#define MESSAGEKEYSELECTIONCHANGED_H

#include "message.h"
#include "../piano/pianodefines.h"
#include "../piano/key.h"
#include <cstdint>

///////////////////////////////////////////////////////////////////////////////
/// \brief Class of a message indicating a change of the selected key
///
/// The EPT has the notion of key selection. Keys can be selected automatically
/// by 'hearing' or by taps and mouse clicks. Whenever the key selection
/// changes this message is emitted to inform all other modules of the EPT.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN MessageKeySelectionChanged : public Message
{
public:
    MessageKeySelectionChanged(int index, const Key *key, piano::KeyState keyState = piano::STATE_NORMAL);
    ~MessageKeySelectionChanged() {}

    int getKeyNumber() const {return mIndex;}
    const Key *getKey() const {return mKey;}
    piano::KeyState getKeyState() const {return mKeyState;}

    bool isForced() const {return mKeyState == piano::STATE_FORCED;}

private:
    const int mIndex;
    const Key *mKey;
    const piano::KeyState mKeyState;
};

#endif // MESSAGEKEYSELECTIONCHANGED_H
