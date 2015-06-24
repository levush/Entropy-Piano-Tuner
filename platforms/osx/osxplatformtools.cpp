#include "osxplatformtools.h"
#include "osxnativewrapper.h"


template<>
std::unique_ptr<OsXPlatformTools> PlatformToolsImplementation<OsXPlatformTools>::mSingleton(new OsXPlatformTools());


void OsXPlatformTools::disableScreensaver() {
    osxDisableScreensaver();
}

void OsXPlatformTools::enableScreensaver() {
    osxReleaseScreensaverLock();
}
