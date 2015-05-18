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

#ifndef APPLENATIVEWRAPPER_H
#define APPLENATIVEWRAPPER_H

#if !defined(__APPLE__)
#   error "This file may only be included on apple platforms"
#endif

#include <TargetConditionals.h>

// includes
#if TARGET_IPHONE_SIMULATOR
#elif TARGET_OS_IPHONE
#elif TARGET_OS_MAC
#   import <IOKit/pwr_mgt/IOPMLib.h>
#endif


namespace platformtools {

/// helper class for mac
class MacUtil {
public:
    void disableScreensaver();
    void releaseScreensaverLock();
private:
#if TARGET_IPHONE_SIMULATOR
#elif TARGET_OS_IPHONE
#elif TARGET_OS_MAC
    IOPMAssertionID assertionID;
#endif
};

// variable of the helper class
extern MacUtil macUtil;

}  // platformtools

#endif // APPLENATIVEWRAPPER_H
