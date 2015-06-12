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

#ifndef MESSAGEFINALKEY_H
#define MESSAGEFINALKEY_H

#include "message.h"
#include "../piano/key.h"

class MessageFinalKey : public Message
{
public:
    MessageFinalKey(int keyNumber, std::shared_ptr<Key> key);
    ~MessageFinalKey();

    int getKeyNumber() { return mKeyNumber; }
    std::shared_ptr<Key> getFinalKey() const { return mKey; }

private:
    std::shared_ptr<Key> mKey;
    int mKeyNumber;
};

#endif // MESSAGEFINALKEY_H
