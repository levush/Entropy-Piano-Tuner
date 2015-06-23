#include "iosplatformtools.h"
#include "iosnativewrapper.h"

template<>
std::unique_ptr<IOsPlatformTools> PlatformToolsImplementation<IOsPlatformTools>::mSingleton(new IOsPlatformTools());

void IOsPlatformTools::init() {
    // init midi components
}

void IOsPlatformTools::disableScreensaver() {
    iosDisableScreensaver();
}

void IOsPlatformTools::enableScreensaver() {
    iosReleaseScreensaverLock();
}
