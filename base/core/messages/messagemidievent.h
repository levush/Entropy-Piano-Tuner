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
//                          Message for a MIDI event
//=============================================================================

#ifndef MESSAGEMIDIEVENT_H
#define MESSAGEMIDIEVENT_H

#include "../config.h"

#include "message.h"
#include "../audio/midi/midiadapter.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Class for a MIDI event
///
/// Whenever an incoming MIDI event occurs. The structure of the MIDI event
/// is defined in the MIDI adapter
///////////////////////////////////////////////////////////////////////////////

class MessageMidiEvent : public Message
{
public:
    MessageMidiEvent(const MidiAdapter::Data &data);
    ~MessageMidiEvent();

    const MidiAdapter::Data &getData() const;

private:
    const MidiAdapter::Data data;
};

#endif // MESSAGEMIDIEVENT_H

