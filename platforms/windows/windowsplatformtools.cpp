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

#include "windowsplatformtools.h"
#include <Windows.h>

template<>
std::unique_ptr<WindowsPlatformTools> PlatformToolsImplementation<WindowsPlatformTools>::mSingleton(new WindowsPlatformTools());

bool WindowsPlatformTools::loadStartupFile(const QStringList args) {
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    return PlatformTools::loadStartupFile(args);
#else
    return false;
#endif
}

void WindowsPlatformTools::disableScreensaver() {
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
#else
#endif
}

void WindowsPlatformTools::enableScreensaver() {
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    SetThreadExecutionState(ES_CONTINUOUS);
#else
#endif
}

unsigned long long WindowsPlatformTools::getInstalledPhysicalMemoryInB() const {
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    return PlatformToolsImplementation<WindowsPlatformTools>::getInstalledPhysicalMemoryInB();
#else
    // TODO: Implement correctly for windows RT and Windows Phone
    return PlatformToolsImplementation<WindowsPlatformTools>::getInstalledPhysicalMemoryInB();
#endif
}
