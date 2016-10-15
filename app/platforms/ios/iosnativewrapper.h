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

#ifndef IOSNATIVEWRAPPER_H
#define IOSNATIVEWRAPPER_H

#if !defined(__APPLE__)
#   error "This file may only be included on apple platforms"
#endif

#include <TargetConditionals.h>
#include <string>

// includes
#if !defined(TARGET_IPHONE_SIMULATOR) || !defined(TARGET_OS_IPHONE)
#   error "This file may only be include on ios platforms"
#endif

void iosInit();
void iosDisableScreensaver();
void iosReleaseScreensaverLock();
unsigned long long iosGetInstalledPhysicalMemoryInB();

#endif // IOSNATIVEWRAPPER_H

