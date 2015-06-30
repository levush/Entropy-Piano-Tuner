#include "windowsplatformtools.h"
#include <Windows.h>

template<>
std::unique_ptr<WindowsPlatformTools> PlatformToolsImplementation<WindowsPlatformTools>::mSingleton(new WindowsPlatformTools());

bool WindowsPlatformTools::loadStartupFile(const QStringList args) {
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    return PlatformTools::loadStartupFile(args);
#else
    return false;
#endif
}

void WindowsPlatformTools::disableScreensaver() {
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
#else
#endif
}

void WindowsPlatformTools::enableScreensaver() {
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    SetThreadExecutionState(ES_CONTINUOUS);
#else
#endif
}
