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

#ifndef PLATFORMTOOLSCORE_H
#define PLATFORMTOOLSCORE_H

#include <memory>

#include "prerequisites.h"
#include "core/audio/midi/midiadapter.h"

class EPT_EXTERN PlatformToolsCore
{
private:
    static PlatformToolsCore* mSingletonPtr;

protected:
    PlatformToolsCore();

public:
    static PlatformToolsCore *getSingleton();

    virtual MidiAdapterPtr createMidiAdapter() const;

    /// Returns the physical memory, can be used to detect low memory devices
    virtual unsigned long long getInstalledPhysicalMemoryInB() const;
};

#endif // PLATFORMTOOLSCORE_H
