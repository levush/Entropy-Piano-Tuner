#ifndef IOSNATIVEWRAPPER_H
#define IOSNATIVEWRAPPER_H

#if !defined(__APPLE__)
#   error "This file may only be included on apple platforms"
#endif

#include <TargetConditionals.h>

// includes
#if !defined(TARGET_IPHONE_SIMULATOR) || !defined(TARGET_OS_IPHONE)
#   error "This file may only be include on ios platforms"
#endif

void iosDisableScreensaver();
void iosReleaseScreensaverLock();

#endif // IOSNATIVEWRAPPER_H

