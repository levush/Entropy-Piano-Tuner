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

#include "applenativewrapper.h"
#include "../core/system/log.h"
#include "tunerapplication.h"
#include "platformtools.h"

// includes
#if TARGET_IPHONE_SIMULATOR
#   import <UIKit/UIKit.h>
#elif TARGET_OS_IPHONE
#   import <UIKit/UIKit.h>
#elif TARGET_OS_MAC
#   import <IOKit/pwr_mgt/IOPMLib.h>
#endif

namespace platformtools {

MacUtil macUtil;

void MacUtil::disableScreensaver() {
#if TARGET_IPHONE_SIMULATOR
    [UIApplication sharedApplication].idleTimerDisabled = YES;
#elif TARGET_OS_IPHONE
    [UIApplication sharedApplication].idleTimerDisabled = YES;
#elif TARGET_OS_MAC
    CFStringRef reasonForActivity= CFSTR("Entropy tuner working");
    IOReturn success = IOPMAssertionCreateWithName(kIOPMAssertionTypeNoDisplaySleep,
                                        kIOPMAssertionLevelOn, reasonForActivity, &assertionID);
    if (success == kIOReturnSuccess) {
        //return sucess? todo..
    }
#endif
}

void MacUtil::releaseScreensaverLock() {
#if TARGET_IPHONE_SIMULATOR
    [UIApplication sharedApplication].idleTimerDisabled = NO;
#elif TARGET_OS_IPHONE
    [UIApplication sharedApplication].idleTimerDisabled = NO;
#elif TARGET_OS_MAC
    IOPMAssertionRelease(assertionID);
#endif
}


}  // platformtools

// application extensions
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE


// Make QIOSApplicationDelegate known
@interface QIOSApplicationDelegate
@end

// Add a category to QIOSApplicationDelegate
@interface QIOSApplicationDelegate (CategoryFileOpeningOnStartup)
@end

// Now add new methods
@implementation QIOSApplicationDelegate (CategoryFileOpeningOnStartup)
- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation
{
#pragma unused(application)
#pragma unused(sourceApplication)
#pragma unused(annotation)
    if ([url isFileURL])
    {
        NSString *file = [[NSString alloc] initWithString:[url path]];
        // Handle file being passed in
        platformtools::openFile([file UTF8String], true);
    }
    else
    {
        // Handle custom URL scheme
    }
return TRUE;
}

@end
#endif
