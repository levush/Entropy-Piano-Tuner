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
//                               Midi adapter
//=============================================================================

#include "midiadapter.h"

#include <sstream>

#include "../../system/log.h"
#include "../../messages/messagehandler.h"
#include "../../messages/messagemidievent.h"


//---------------------0--------------------------------------------------------
//                Return a list of all available Midi devices
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \return List all available Midi devices in a single string
///////////////////////////////////////////////////////////////////////////////

std::string MidiAdapter::GetPortNames()
{
    const int ports = GetNumberOfPorts();
    std::stringstream s;
    if (ports > 0)
    {
        s << GetPortName(0);
        for (int i = 1; i < GetNumberOfPorts(); ++i) s << ", " << GetPortName(i);
        return s.str();
    }
    else return "No MIDI ports available.";
}


//-----------------------------------------------------------------------------
//                Convert the MIDI command byte to a MidiEvent
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \return MidiEvent code, MIDI_UNDEFINED if not recognized
///////////////////////////////////////////////////////////////////////////////

MidiAdapter::MidiEvent MidiAdapter::byteToEvent(int byte)
{
    switch (byte & 0xF0)
    {
        case 0x80:  return MIDI_KEY_RELEASE;
        case 0x90:  return MIDI_KEY_PRESS;
        case 0xB0:  return MIDI_CONTROL_CHANGE;
    }
    return MIDI_UNDEFINED;
}


//-----------------------------------------------------------------------------
//                            Send MIDI message
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// This function sends a new data set received from the MIDI implementation
/// as a message to the message handler.
///////////////////////////////////////////////////////////////////////////////

void MidiAdapter::send (Data &data)
{
    LogI("Midi event with data %d %d %d %lf",
         (int)(data.event), data.byte1, data.byte2, data.deltatime);
    MessageHandler::send<MessageMidiEvent>(data);
}
