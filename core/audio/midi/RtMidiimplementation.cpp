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

#include "RtMidiimplementation.h"

#if CONFIG_ENABLE_RTMIDI

#include <sstream>
#include <thread>

#include "../../system/log.h"

//-----------------------------------------------------------------------------
//                    Constructor without functionality
//-----------------------------------------------------------------------------

RtMidiImplementation::RtMidiImplementation () :
    MidiAdapter(),
    mRtMidi(nullptr),
    mCurrentPort(0)
{}


//-----------------------------------------------------------------------------
//                   Initialize the RtMidi-Implementation
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Initializes the RtMidi implementation by trying to create a new instance.
/// Exception errors are caught and converted into conventional error messages.
///////////////////////////////////////////////////////////////////////////////

void RtMidiImplementation::init()
{
    try { mRtMidi.reset(new RtMidiIn()); }  // try to create the instance
    catch (const RtMidiError &error) LogE("%s", error.getMessage().c_str());
}


//-----------------------------------------------------------------------------
//                       Exit from RtMidi-Implementation
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Destroys the RtMidi instance, resetting the pointer mRtMidi.
///////////////////////////////////////////////////////////////////////////////

void RtMidiImplementation::exit()
{
    mRtMidi.reset();
}


//-----------------------------------------------------------------------------
//                 Return the number of available Midi ports
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \return Number of available MIDI ports (0=none).
///////////////////////////////////////////////////////////////////////////////

int RtMidiImplementation::GetNumberOfPorts ()
{
    if (not mRtMidi)
    {
        LogW("Midi has not been initialized");
        return 0;
    }

    int cnt = 0;
    try { cnt = mRtMidi->getPortCount(); }
    catch (const RtMidiError &error) LogE("%s", error.getMessage().c_str());
    return cnt;
}



//-----------------------------------------------------------------------------
//                 Get the MIDI device name for a given port
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \param i : Number of the port starting with 0
/// \return Name of the MIDI device connected to port \e i
///////////////////////////////////////////////////////////////////////////////

std::string RtMidiImplementation::GetPortName (int i)
{
    std::stringstream ss;
    try
    {
        if (i<0 or i>=static_cast<int>(mRtMidi->getPortCount()))
            ss << "Midi input port #" << i << " not available.";
        else ss << i << mRtMidi->getPortName(i);
    }
    catch (const RtMidiError &error) LogE("%s", error.getMessage().c_str());
    return ss.str();
}


//-----------------------------------------------------------------------------
//               Return a list of all available Midi devices
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \return List of all available MIDI devices as a single string
///////////////////////////////////////////////////////////////////////////////

std::string RtMidiImplementation::GetPortNames()
{
    if (not mRtMidi)
    {
        LogW("Midi has not been initialized");
        return std::string();
    }

    std::stringstream ss;
    try
    {
        if (mRtMidi->getPortCount()==0) return "No Midi input ports available.";
        for (int i=0; i<static_cast<int>(mRtMidi->getPortCount()); i++)
            ss << "Midi input port #" << i << ": " << mRtMidi->getPortName(i) << '\n';
    }
    catch (const RtMidiError &error) LogE("%s", error.getMessage().c_str());
    return ss.str();
}


//-----------------------------------------------------------------------------
//                             Open port number i
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \param i : Number of the MIDI port to be opened.
/// \param AppName : Name of the application (e.g. Entropy Piano Tuner Midi in).
///                  Shows up in the list MIDI input devices.
/// \return True if successful.
///////////////////////////////////////////////////////////////////////////////

bool RtMidiImplementation::OpenPort (int i, std::string AppName)
{
    try
    {
        if (i<0 or i>=static_cast<int>(mRtMidi->getPortCount()))
        {
            LogW ("Invalid port number");
            return false;
        }
        else
        {
            mRtMidi->openPort (i,AppName);
            ClearQueue ();
            mRtMidi->setCallback (&StaticCallback,this);
            mCurrentPort = i;
        }
    }
    catch (const RtMidiError &error) LogE("%s", error.getMessage().c_str());
    return true;
}


//-----------------------------------------------------------------------------
//                   Open the port with the highest number
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// If MIDI devices are connected, this function opens the port with the
/// highest number (usually this is the last one that has been connected to the
/// computer).
///
/// \param AppName : Name of the application (e.g. Entropy Piano Tuner Midi in).
///                  Shows up in the list MIDI input devices.
/// \return True if successful.
///////////////////////////////////////////////////////////////////////////////

bool RtMidiImplementation::OpenPort (std::string AppName)
{
    int n = GetNumberOfPorts();
    if (n>0) return OpenPort(n-1,AppName);
    else return false;
}


//-----------------------------------------------------------------------------
//                        Clear the MIDI input queue
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// This functions polls data from the queue until it is empty.
/// Here the timing is crucial.
///
/// Details: Clearing the queue in RtMidi requires some effort. The queue is
/// read via getMessage in intervals of 2 us. Unfortunately this
/// function does not really tell us wether the queue is empty, rather
/// it may respond after some time. To circumvent this problem we
/// decalare the queue as empty if 1024 request do not render an event.
///////////////////////////////////////////////////////////////////////////////

void RtMidiImplementation::ClearQueue ()
{
    try
    {
        std::vector<unsigned char> message;
        bool clearing;
        int timeout=1000;
        do
        {
            clearing=false;
            for (int i=0; i<256; ++i)
            {
                std::this_thread::sleep_for(
                            std::chrono::microseconds(2));
                mRtMidi->getMessage(&message);
                if (message.size()>0) clearing=true;
            }
        }
        while (clearing and --timeout>0);
    }
    catch (const RtMidiError &error) LogE("%s", error.getMessage().c_str());
}


//-----------------------------------------------------------------------------
//                           STATIC CALLBACK FUNCTION
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Static callback function (private)
///
/// The RtMidi package does not admit member functions for callback.
/// To circumvent this restriction, we define a static callback function
/// to which all MIDI events are passed. The third user-specific parameter
/// void pointer passes the address of the object. The purpose of the
/// callback function is to send off the received data to the
/// messaging system.
/// \param deltatime : passes the elapsed time since the last callback
/// \param message : Pointer to a vector containing the MIDI event data.
/// \param obj : Pointer to the instance of this class. See also:
///              RtMidiImplementation::OpenPort
///////////////////////////////////////////////////////////////////////////////

void RtMidiImplementation::StaticCallback (double deltatime,
                                           std::vector< unsigned char > *message,
                                           void* obj)
{
    RtMidiImplementation* myself = (RtMidiImplementation*)obj;

    if (message->size()==3)
    {
        int byte0 = (*message)[0];
        Data data;
        data.event = MIDI_UNDEFINED;
        data.byte1 = (*message)[1];
        data.byte2 = (*message)[2];
        data.event = byteToEvent(byte0);
        if (data.event == MIDI_KEY_PRESS && data.byte2 == 0)
        {
            data.event = MIDI_KEY_RELEASE;
        }

        data.deltatime = deltatime;
        myself->send(data);
    }
}

#endif  // CONFIG_ENABLE_RTMIDI
