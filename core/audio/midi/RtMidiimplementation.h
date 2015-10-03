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

//=============================================================================
//                MIDI implementaion for various platforms
//=============================================================================

#ifndef RTMIDIIMPLEMENTATION_H
#define RTMIDIIMPLEMENTATION_H
#include "../../config.h"
#include "midiadapter.h"
#include <memory>

// so not forget to set PREDEFINED = CONFIG_ENABLE_RTMIDI in Doxyfile
// in order to include this class in the doxygen documentation.

#if CONFIG_ENABLE_RTMIDI

#include "../../thirdparty/RtMidi/RtMidi.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Interface for reading an externally connected MIDI keyboard.
///
/// This class provides a simplified interface for reading external
/// MIDI keyboards.
/// It uses the third-party software \b RtMidi \b 2.1.0 which is published
/// under the same license.
/// \see Source: http://www.music.mcgill.ca/~gary/rtmidi/
/// A documentation of RtMidi is not included here. If you need more
/// information please refer to the above website.
///
/// Requirements for Linux:
/// alsa_devel, library asound, #define __LINUX_ALSA__
///////////////////////////////////////////////////////////////////////////////

class RtMidiImplementation : public MidiAdapter
{
public:
    RtMidiImplementation ();                                        ///< Constructor without functionality
    ~RtMidiImplementation () {}                                     ///< Destructor without functionality

    void init() override final;                                     ///< Initialize RtMidi implementation
    void exit() override final;                                     ///< Shut down RtMidi implementation

    int GetNumberOfPorts() override final;                          ///< Get the number of available MIDI input ports
    std::string GetPortName(int i) override final;                  ///< Get the name of device number \e  i (starting with zero)
    std::string GetPortNames() override final;                      ///< Get a list of all available input devices
    bool OpenPort (int i, std::string AppName="") override final;   ///< Open MIDI input device number \e i
    bool OpenPort (std::string AppName="")override final;           ///< Open MIDI device with the highest port number
     int getCurrentPort() const override final {return mCurrentPort;}///< Get the current port number

private:
    void ClearQueue ();                 ///< Clear the MIDI input queue
    std::unique_ptr<RtMidiIn> mRtMidi;  ///< Instance of the third-party MIDI interface
    int mCurrentPort;                   ///< The current port that is used or -1 of none

    ///< Static callback function, required by RtMidi.
    static void StaticCallback (double deltatime, std::vector< unsigned char > *message, void *);
};

#endif // CONFIG_ENABLE_RTMIDI
#endif // RTMIDIIMPLEMENTATION_H
