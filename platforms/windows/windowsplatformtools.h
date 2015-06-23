#ifndef WINDOWSPLATFORMTOOLS_H
#define WINDOWSPLATFORMTOOLS_H

#ifndef _WIN32
#   error "This file may only be included on windows platforms."
#endif

#include "Qt/platformtools.h"

class WindowsPlatformTools : public PlatformToolsImplementation<WindowsPlatformTools> {
public:
    virtual bool loadStartupFile(const QStringList args) override;
    virtual void disableScreensaver() override;
    virtual void enableScreensaver() override;
};


#endif  // WINDOWSPLATFORMTOOLS_H
