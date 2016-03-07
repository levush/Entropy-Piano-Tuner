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
//                 Message reporting an action concerining files
//=============================================================================

#ifndef MESSAGEPROJECTFILE_H
#define MESSAGEPROJECTFILE_H

#include "message.h"

class PianoFile;
class Piano;

///////////////////////////////////////////////////////////////////////////////
/// \brief Message reporting an action concerining files
///
/// Whenever a file is opened, saved, edited or created this message is
/// emitted to inform the other modules of the EPT.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN MessageProjectFile : public Message
{
public:
    enum Types {
        FILE_OPENED,
        FILE_SAVED,

        FILE_EDITED,
        FILE_CREATED,
    };

public:
    MessageProjectFile(Types type, const Piano &piano);
    ~MessageProjectFile();

    Types getFileMessageType() const {return mFileMessageType;}
    const Piano &getPiano() const;

private:
    const Types mFileMessageType;
    const Piano &mPiano;
};

#endif // MESSAGEPROJECTFILE_H
