#include "platformtoolscore.h"
#include "core/config.h"
#include "core/system/eptexception.h"
#include "core/midi/NoMidiimplementation.h"
#include "core/midi/RtMidiimplementation.h"

PlatformToolsCore* PlatformToolsCore::mSingletonPtr(nullptr);

PlatformToolsCore::PlatformToolsCore() {
    EptAssert(!mSingletonPtr, "Singleton for platform tools core may not be created.");
    mSingletonPtr = this;
}

PlatformToolsCore *PlatformToolsCore::getSingleton() {
    return mSingletonPtr;
}

std::shared_ptr<MidiAdapter> PlatformToolsCore::createMidiAdapter() const {
#if CONFIG_ENABLE_RTMIDI
    return std::make_shared<RtMidiImplementation>();
#else
    return std::make_shared<NoMidiImplementation>();
#endif
}
