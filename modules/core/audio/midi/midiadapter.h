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

#ifndef MIDIADAPTER_H
#define MIDIADAPTER_H

#include "prerequisites.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Adapter class for reading an externally connected MIDI keyboard.
///
/// This abstract class defines the interface between the EPT
/// and an external MIDI keyboard. It has to be inherited in a specific
/// platform-dependent implementation.
/// \see class RtMidiImplementation and NoMidiImplementation
///
/// A midi command consists of three bytes, as explained e.g. in
/// https://www.nyu.edu/classes/bello/FMT_files/9_MIDI_code.pdf .
/// In addition, the callback function provides a parameter called 'deltatime',
/// which is basically the time in seconds elapsed since the last event.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN MidiAdapter
{
public:

    MidiAdapter() {}                                            ///< Constructor without function
    ~MidiAdapter() {}                                           ///< Destructor without function

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Enumeration of the possible MIDI events
    ///
    /// Midi events processed by the EPT,
    /// representing the MIDI operation code in a simplified manner.
    ///////////////////////////////////////////////////////////////////////////

    enum MidiEvent
    {
        MIDI_UNDEFINED = 0,     ///< Midi event undefined
        MIDI_KEY_PRESS,         ///< Midi event when a key is pressed
        MIDI_KEY_RELEASE,       ///< Midi event when a key is released
        MIDI_CONTROL_CHANGE,    ///< Midi event for changing voice
    };


    ///////////////////////////////////////////////////////////////////////////
    /// \brief Structure of the data associated with a MIDI event
    ///
    /// MIDI commands consist of three bytes (see e.g.
    /// https://ccrma.stanford.edu/~craig/articles/linuxmidi/misc/essenmidi.html)
    /// The first byte represents a command, which is translated into one of the
    /// enums listed in MidiEvent (see above). The second byte usually represents
    /// the key index while the third byte represents the velocity (intensity)
    /// of the keystroke.
    ///////////////////////////////////////////////////////////////////////////

    struct Data
    {
        MidiEvent event;    ///< Midi event, encoded by the enumeration MidiEvent
        int byte1;          ///< Data byte, usually representing the MIDI key index.
        int byte2;          ///< Data byte, usually representing the keystroke intensity.
        double deltatime;   ///< Time elapsed since the last MIDI event.
    };

public:

    // The following functions have to be public because of access from iosnativewrapper
    static MidiEvent byteToEvent (int byte); ///< Convert MIDI code to MidiEvent

    void send (Data &data); ///< Send new MIDI data to the messaging system

    void receiveMessage(int cmd, int byte1, int byte2, double timestamp = 0);
};

typedef std::shared_ptr<MidiAdapter> MidiAdapterPtr;
template class EPT_EXTERN std::shared_ptr<MidiAdapter>;

#endif // MIDIADAPTER_H
