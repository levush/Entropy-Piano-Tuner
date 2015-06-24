#ifndef ANDROIDPLATFORMTOOLS_H
#define ANDROIDPLATFORMTOOLS_H

#ifndef __ANDROID__
#   error "This file may only be included on android builds."
#endif

#include "Qt/platformtools.h"

class AndroidPlatformTools : public PlatformToolsImplementation<AndroidPlatformTools> {
private:
    bool loadStartupFile(const QStringList args);
};

#endif
