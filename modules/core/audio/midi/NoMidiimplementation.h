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

#ifndef NOMIDIIMPLEMENTATION_H
#define NOMIDIIMPLEMENTATION_H

#include "midiadapter.h"


////////////////////////////////////////////////////////////////////////
/// \brief Inactive Midi interface
///
/// Some devices, e.g. smartphones, do not have a Midi interface.
/// To avoid complicated Macro switches in the code, this class provides
/// an inactive dummy interface. By creating an instance of this interface,
/// the Midi system can be disabled without chaning the main code.
////////////////////////////////////////////////////////////////////////

class NoMidiImplementation : public MidiAdapter
{
public:
    NoMidiImplementation ();

    void init() override final;
    void exit() override final;

    int GetNumberOfPorts () override final;                         ///< Get the number of available input devices
    std::string GetPortName (int i) override final;                 ///< Get the name of device i (starting with zero)
    bool OpenPort (int i, std::string AppName="") override final;   ///< Open Midi input device number i
    bool OpenPort (std::string AppName="") override final;          ///< Open Midi device with the highest port number
    int getCurrentPort() const override final;
};

#endif // NOMIDIIMPLEMENTATION_H




