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


// midi functions
void iosMidiInit();
void iosMidiExit();
int iosMidiGetNumberOfPorts();
std::string iosMidiGetPortName(int i);
bool iosMidiOpenPort(int i);
bool iosMidiOpenPort();
int iosMidiGetCurrentPort();


#endif // IOSNATIVEWRAPPER_H

