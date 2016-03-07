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
//                    Message reporting final key detection
//=============================================================================

#ifndef MESSAGEFINALKEY_H
#define MESSAGEFINALKEY_H

#include "message.h"
#include "../piano/key.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Class for a message reporting a finally detected key.
///
/// When a piano key is played in the recording mode, the EPT continuously
/// determines the 'rolling FFT' and uses this information to make a guess
/// about the key which is pressed. This guess should be the same as long as
/// the same key is played, but due to inaccuracies it may occasionally fail
/// or fluctuate. The system keeps track of these changes and, upon completion
/// of the recording, determines the 'final key' by a majority decision. This
/// final key is then emitted by this message to inform the other components
/// of the software.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN MessageFinalKey : public Message
{
public:
    MessageFinalKey(int keyNumber, std::shared_ptr<Key> key);
    ~MessageFinalKey() {}

    int getKeyNumber() { return mKeyNumber; }
    std::shared_ptr<Key> getFinalKey() const { return mKey; }

private:
    std::shared_ptr<Key> mKey;
    int mKeyNumber;
};

#endif // MESSAGEFINALKEY_H
