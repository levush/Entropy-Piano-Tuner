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
//     Message indicating the change of data associated with a single key
//=============================================================================

#ifndef MESSAGEKEYDATACHANGED_H
#define MESSAGEKEYDATACHANGED_H

#include "message.h"
#include "../piano/key.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Message sent whenever the data associated with a single key changes.
///////////////////////////////////////////////////////////////////////////////

class MessageKeyDataChanged : public Message
{
public:
    MessageKeyDataChanged(int index, const Key *key);
    ~MessageKeyDataChanged();

    int getIndex() const {return mIndex;}
    const Key *getKey() const {return mKey;}

private:
    const int mIndex;
    const Key *mKey;
};

#endif // MESSAGEKEYDATACHANGED_H
