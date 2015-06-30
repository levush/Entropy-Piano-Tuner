#ifndef OSXPLATFORMTOOLS_H
#define OSXPLATFORMTOOLS_H

#include <TargetConditionals.h>

#ifndef TARGET_OS_MAC
#   error "This file may only be included on Mac Os X devices."
#endif

#include "Qt/platformtools.h"

class OsXPlatformTools : public PlatformToolsImplementation<OsXPlatformTools> {
private:
    virtual void disableScreensaver() override;
    virtual void enableScreensaver() override;
};

#endif  // OSXPLATFORMTOOLS_H
