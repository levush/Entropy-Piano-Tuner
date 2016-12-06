#include "winrtbridge.h"
#include "winrtnativehelper.h"

using namespace Windows::System;

WinRTBridge::WinRTBridge()
{
}

unsigned long long WinRTBridge::getInstalledPhysicalMemoryInB() const {
    return Windows::System::MemoryManager::AppMemoryUsageLimit;
}
