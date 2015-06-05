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
//              MIDI implementaion for various platforms
//======================================================================

#include "RtMidiimplementation.h"

#if CONFIG_ENABLE_RTMIDI

#include <sstream>
#include <chrono>
#include <thread>

#include "../system/log.h"

//-------------------------- Constructor -------------------------------


RtMidiImplementation::RtMidiImplementation () :
    MidiAdapter() {}


//---------------------------- init ------------------------------------

////////////////////////////////////////////////////////////////////////
/// Initialize the RtMidi implementation by calling the creating a
/// new instance. Exception errors are caught and converted into
/// conventional error messages.
////////////////////////////////////////////////////////////////////////

void RtMidiImplementation::init() {
    try {
        // try to create the instance
        mRtMidi.reset(new RtMidiIn());
    }
    catch (const RtMidiError &error) {
        ERROR("%s", error.getMessage().c_str());
    }
}


//------------------------------ exit ----------------------------------

////////////////////////////////////////////////////////////////////////
/// Destroys the RtMidi instance.
////////////////////////////////////////////////////////////////////////

void RtMidiImplementation::exit() {
    mRtMidi.reset();
}

//-------------- Return the number of available Midi devices ------------

////////////////////////////////////////////////////////////////////////
/// \return Number of available Midi ports (0=none).
////////////////////////////////////////////////////////////////////////

int RtMidiImplementation::GetNumberOfPorts ()
{
    if (!mRtMidi) {
        WARNING("Midi has not been initialized");
        return 0;
    }

    int cnt=0;
    try { cnt = mRtMidi->getPortCount(); }
    catch (const RtMidiError &error) ERROR("%s", error.getMessage().c_str());
    return cnt;
}



//------------------------Get the Midi device name ----------------------

////////////////////////////////////////////////////////////////////////
/// \param i : Number of the port starting with 0
/// \return Name of the Midi device connected to port i
////////////////////////////////////////////////////////////////////////

std::string RtMidiImplementation::GetPortName (int i)
{
    std::stringstream ss;
    try
    {
        if (i<0 or i>=static_cast<int>(mRtMidi->getPortCount()))
            ss << "Midi input port #" << i << " not available.";
        else ss << i << mRtMidi->getPortName(i);
    }
    catch (const RtMidiError &error) ERROR("%s", error.getMessage().c_str());
    return ss.str();
}


//---------------- Return a list of all available Midi devices ---------

////////////////////////////////////////////////////////////////////////
/// \return List of all available Midi devices as a single string
////////////////////////////////////////////////////////////////////////

std::string RtMidiImplementation::GetPortNames()
{
    if (!mRtMidi) {
        WARNING("Midi has not been initialized");
        return std::string();
    }

    std::stringstream ss;
    try
    {
        if (mRtMidi->getPortCount()==0) return "No Midi input ports available.";
        for (int i=0; i<static_cast<int>(mRtMidi->getPortCount()); i++)
            ss << "Midi input port #" << i << ": " << mRtMidi->getPortName(i) << '\n';
    }
    catch (const RtMidiError &error) ERROR("%s", error.getMessage().c_str());
    return ss.str();
}


//------------------------ Open a particular port -----------------------

////////////////////////////////////////////////////////////////////////
/// \param i : Number of the Midi port to be opened.
/// \param AppName : Name of the application. Shows up in the list
///                         Midi input devices
/// \return true if successful.
////////////////////////////////////////////////////////////////////////

bool RtMidiImplementation::OpenPort (int i, std::string AppName)
{
    try
    {
        if (i<0 or i>=static_cast<int>(mRtMidi->getPortCount()))
        {
            WARNING ("Invalid port number");
            return false;
        }
        else
        {
            // mRtMidi->closePort();
            // we only need to listen to keypress event, so we can ignore most of the events
            // mRtMidi->ignoreTypes (false,false,false);  // this line would activate all midi messages
            mRtMidi->openPort (i,AppName);
            ClearQueue ();
            mRtMidi->setCallback (&StaticCallback,this);
            mCurrentPort = i;
        }
    }
    catch (const RtMidiError &error) ERROR("%s", error.getMessage().c_str());
    return true;
}


//--------------------- Open the port with the highest number ------------

////////////////////////////////////////////////////////////////////////
/// If Midi devices are connected this function opens the port with the
/// highest number (usually the last one that has been connected to the
/// computer).
///
/// \param AppName : Name of the application. Shows up in the list
///                  Midi input devices
/// \return true if successful.
////////////////////////////////////////////////////////////////////////

bool RtMidiImplementation::OpenPort (std::string AppName)
{
    (void)AppName; // suppresses warning
    int n = GetNumberOfPorts();
    if (n>0) return OpenPort(n-1);
    else return false;
}


//-------------------------- Clear the Midi input queue ----------------

////////////////////////////////////////////////////////////////////////
/// This functions polls data from the queue until it is empty.
/// Here the timing is crucial.
////////////////////////////////////////////////////////////////////////

// Clearing the queue in RtMidi requires some effort. The queue is
// read via getMessage in intervals of 2 us. Unfortunately this
// function does not really tell us wether the queue is empty, rather
// it may respond after some time. To circumvent this problem we
// decalare the queue as empty if 1024 request do not render an event.

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
    catch (const RtMidiError &error) ERROR("%s", error.getMessage().c_str());
}

////////////////////////////////////////////////////////////////////////
/// \brief Static callback function (private)
///
/// The RtMidi package does not admit member functions for callback.
/// To circumvent this restriction, we define a static callback function
/// to which all Midi events are passed. The third user-specific parameter
/// void pointer passes the address of the object. The purpose of the
/// callback function is to send off the received data to the
/// messaging system.
//////////////////////////////////////////////////////////////////////////


void RtMidiImplementation::StaticCallback (double deltatime, std::vector< unsigned char > *message, void* obj)
{
    RtMidiImplementation* myself = (RtMidiImplementation*)obj;

    if (message->size()==3)
    {
        int byte0 = (*message)[0];
        Data data;
        data.event = MIDI_UNDEFINED;
        data.byte1 = (*message)[1];
        data.byte2 = (*message)[2];
        switch (byte0 & 0xF0)
        {
            case 0x80:  data.event = MIDI_KEY_RELEASE; break;
            case 0x90:  data.event = MIDI_KEY_PRESS;
                        if (data.byte2==0) data.event = MIDI_KEY_RELEASE;
                        break;
            case 0xB0:  data.event = MIDI_CONTROL_CHANGE; break;
        }

        data.deltatime = deltatime;
        myself->send(data);
    }
}

#endif  // CONFIG_ENABLE_RTMIDI
