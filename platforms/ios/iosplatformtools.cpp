#include "iosplatformtools.h"
#include "iosnativewrapper.h"
#include "iosmidiadapter.h"

template<>
std::unique_ptr<IOsPlatformTools> PlatformToolsImplementation<IOsPlatformTools>::mSingleton(new IOsPlatformTools());

void IOsPlatformTools::init() {
    // init midi components
    iosInit();
}

void IOsPlatformTools::disableScreensaver() {
    iosDisableScreensaver();
}

void IOsPlatformTools::enableScreensaver() {
    iosReleaseScreensaverLock();
}

std::shared_ptr<MidiAdapter> IOsPlatformTools::createMidiAdapter() const {
    return std::make_shared<IOsMidiAdapter>();
}
