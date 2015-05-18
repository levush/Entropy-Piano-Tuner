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

//======================================================================
//                           Midi adapter
//======================================================================

#ifndef MIDIADAPTER_H
#define MIDIADAPTER_H

#include <string>

/////////////////////////////////////////////////////////////////////////
/// \brief Adapter class for reading an externally connected MIDI keyboard.
///
/// This abstract class defines the interface between the piano tuner
/// and an external MIDI keyboard. It has to be inherited in a specific
/// platform-dependent implementation (see class RtMidiImplementation).
///
/// A midi command consists of 3 bytes, as explained
/// e.g. in https://www.nyu.edu/classes/bello/FMT_files/9_MIDI_code.pdf
/// In addition, the callback function provides a parameter called 'deltatime',
/// which is basically the time in seconds elapsed since the last event.
////////////////////////////////////////////////////////////////////////

class MidiAdapter
{
public:

    MidiAdapter() {};
    ~MidiAdapter() {};

    virtual void init() = 0;                                    ///< Initialisation of the implementation
    virtual void exit() = 0;                                    ///< Shut down the implementation

    virtual int GetNumberOfPorts () = 0;                        ///< Get the number of available input devices
    virtual std::string GetPortName   (int i) = 0;              ///< Get the name of device i (starting with zero)
    virtual std::string GetPortNames  () = 0;                   ///< Get a list of all available input devices
    virtual bool OpenPort (int i, std::string AppName="")=0;    ///< Open Midi input device number i
    virtual bool OpenPort (std::string AppName="") = 0;         ///< Open Midi device with the highest port number

    virtual int getCurrentPort() const = 0;                     ///< Get the current port number

    /// \brief Enumeration of the possible MIDI events
    ///
    /// Midi events processed by the tuner,
    /// representing the MIDI operation code in a simplified manner

    enum Event {
            MIDI_UNDEFINED,     ///< Midi event undefined
            MIDI_KEY_PRESS,     ///< Midi event when a key is released
            MIDI_KEY_RELEASE,   ///< Midi event for everything else
            MIDI_CONTROL_CHANGE,  ///< Midi event for chaning voice
        };

    /// \brief Structure of the data associated with a MIDI event
    ///
    /// MIDI commands consist of three bytes (see e.g.
    /// https://ccrma.stanford.edu/~craig/articles/linuxmidi/misc/essenmidi.html)
    /// The first byte represents a command, which is translated into one of the
    /// enums listed in MidiEvent (see above). The second byte usually represents
    /// the key index while the third byte represents the velocity (intensity)
    /// of the keystroke.

    struct Data {

        Event event;        ///< Midi event, encoded by the enumeration MidiEvent
        int byte1;          ///< Data byte, usually representing the MIDI key index.
        int byte2;          ///< Data byte, usually representing the keystroke intensity.
        double deltatime;   ///< Time elapsed since the last MIDI event.
        };

protected:

    void send (Data &data); ///< Send new MIDI data to the messaging system
};

#endif // MIDIADAPTER_H
