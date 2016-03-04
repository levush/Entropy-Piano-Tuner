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

#include "NoMidiimplementation.h"

NoMidiImplementation::NoMidiImplementation () : MidiAdapter() {}



void NoMidiImplementation::init() {}
void NoMidiImplementation::exit() {}

//-------------- Return the number of available Midi devices ------------

/// \return Always return that no Midi ports are available
int NoMidiImplementation::GetNumberOfPorts () { return 0; }


//------------------------Get the Midi device name ----------------------

////////////////////////////////////////////////////////////////////////
/// \param i : Number of the port, ignored here
/// \return Returns a string saying that no devices are available
////////////////////////////////////////////////////////////////////////

std::string NoMidiImplementation::GetPortName (int i)
{
    (void)i; // Do not use this arguement
    return "Midi system not available.";
}

//------------------------ Open a particular port -----------------------

////////////////////////////////////////////////////////////////////////
/// \param i : Number of the Midi port to be opened, ignored here
/// \param AppName : Name of the application, ignored here
/// \return always false since there is no port to be opened
////////////////////////////////////////////////////////////////////////

bool NoMidiImplementation::OpenPort (int i, std::string AppName)
{
    (void)i; (void)AppName;  // Do not use these arguments
    return false;
}


//--------------------- Open the port with the highest number ------------

////////////////////////////////////////////////////////////////////////
/// \param AppName : Name of the application, ignored here
/// \return always false since there is no port to be opened
////////////////////////////////////////////////////////////////////////

bool NoMidiImplementation::OpenPort (std::string AppName)
{
    (void)AppName; // Do not use this argument
    return false;
}

int NoMidiImplementation::getCurrentPort() const {
    return -1;
}
