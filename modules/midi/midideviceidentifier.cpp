#include "midideviceidentifier.h"

namespace midi {

MidiDeviceIdentifier::MidiDeviceIdentifier(const MidiType type, const std::string &humanReadable)
    : mType(type)
    , mHumanReadable(humanReadable) {

}

bool MidiDeviceIdentifier::equals(const MidiDeviceID other) {
    if (!other) {return false;}
    return other->mHumanReadable == mHumanReadable && other->mType == mType;
}

}  // namespace midi
