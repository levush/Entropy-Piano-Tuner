#include "midideviceidentifier.h"

namespace midi {

MidiDeviceIdentifier::MidiDeviceIdentifier(const MidiType type, const std::string &humanReadable)
    : mType(type)
    , mHumanReadable(humanReadable) {

}

}  // namespace midi
