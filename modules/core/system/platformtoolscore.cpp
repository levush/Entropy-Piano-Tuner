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

#include "platformtoolscore.h"
#include "core/config.h"
#include "core/system/eptexception.h"
#include "core/3rdparty/getmemorysize/getmemorysize.h"

PlatformToolsCore* PlatformToolsCore::mSingletonPtr(nullptr);

PlatformToolsCore::PlatformToolsCore() {
    EptAssert(!mSingletonPtr, "Singleton for platform tools core may not be created.");
    mSingletonPtr = this;
}

PlatformToolsCore *PlatformToolsCore::getSingleton() {
    return mSingletonPtr;
}

MidiAdapterPtr PlatformToolsCore::createMidiAdapter() const
{
    return std::make_shared<MidiAdapter>();
}

unsigned long long PlatformToolsCore::getInstalledPhysicalMemoryInB() const {
    return getMemorySize();
}
