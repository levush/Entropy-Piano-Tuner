#include "androidplatformtools.h"
#include "androidnativewrapper.h"

template<>
std::unique_ptr<AndroidPlatformTools> PlatformToolsImplementation<AndroidPlatformTools>::mSingleton(new AndroidPlatformTools());

bool AndroidPlatformTools::loadStartupFile(const QStringList args) {
    Q_UNUSED(args);
    callAndroidVoidTunerApplicationFunction("mainWindowInitialized");

    return false;
}
