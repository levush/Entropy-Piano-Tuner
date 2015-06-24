#include "windowsplatformtools.h"
#include <Windows.h>

template<>
std::unique_ptr<WindowsPlatformTools> PlatformToolsImplementation<WindowsPlatformTools>::mSingleton(new WindowsPlatformTools());

bool WindowsPlatformTools::loadStartupFile(const QStringList args) {
    return PlatformTools::loadStartupFile(args);
}

void WindowsPlatformTools::disableScreensaver() {
#   if WINAPI_PARTITION_APP
#   elif WINAPI_PARTITION_DESKTOP
        SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED);
#   endif
}

void WindowsPlatformTools::enableScreensaver() {
#   if WINAPI_PARTITION_APP
#   elif WINAPI_PARTITION_DESKTOP
        SetThreadExecutionState(ES_CONTINUOUS);
#   endif
}
