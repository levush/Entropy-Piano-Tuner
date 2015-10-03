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

#ifndef IOSMIDIADAPTER_H
#define IOSMIDIADAPTER_H

#include "core/audio/midi/midiadapter.h"

class IOsMidiAdapter : public MidiAdapter
{
public:
    IOsMidiAdapter();

    void init() override final;
    void exit() override final;

    int GetNumberOfPorts () override;
    std::string GetPortName   (int i) override;
    bool OpenPort (int i, std::string AppName="") override;
    bool OpenPort (std::string AppName="") override;
    int getCurrentPort() const override;
};

#endif // IOSMIDIADAPTER_H
