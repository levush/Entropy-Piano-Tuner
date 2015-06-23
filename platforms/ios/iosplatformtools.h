#ifndef IOSPLATFORMTOOLS_H
#define IOSPLATFORMTOOLS_H

#include "Qt/platformtools.h"

class IOsPlatformTools : PlatformToolsImplementation<IOsPlatformTools> {
private:
    virtual void init() override;
    virtual void disableScreensaver() override;
    virtual void enableScreensaver() override;
};


#endif  // iosplatformtools
