#include "androidplatformtools.h"
#include "androidnativewrapper.h"

template<>
std::unique_ptr<AndroidPlatformTools> PlatformToolsImplementation<AndroidPlatformTools>::mSingleton(new AndroidPlatformTools());

void AndroidPlatformTools::init() {
    callAndroidVoidTunerApplicationFunction("mainWindowInitialized");
}

bool AndroidPlatformTools::loadStartupFile(const QStringList args) {
    Q_UNUSED(args);
    // done from init call
    return false;
}
