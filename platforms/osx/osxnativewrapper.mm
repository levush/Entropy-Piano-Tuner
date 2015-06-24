#include "osxnativewrapper.h"
#import <IOKit/pwr_mgt/IOPMLib.h>

IOPMAssertionID osxAssertionID;

void osxDisableScreensaver() {
    CFStringRef reasonForActivity= CFSTR("Entropy tuner working");
    IOReturn success = IOPMAssertionCreateWithName(kIOPMAssertionTypeNoDisplaySleep,
                                        kIOPMAssertionLevelOn, reasonForActivity, &osxAssertionID);
    if (success == kIOReturnSuccess) {
        //return sucess? todo..
    }

}

void osxReleaseScreensaverLock() {
    IOPMAssertionRelease(osxAssertionID);
}
