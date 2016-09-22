#include "rtmidideviceidentifier.h"

namespace midi {

RtMidiDeviceIdentifier::RtMidiDeviceIdentifier(const MidiType type, const std::string &id)
    : MidiDeviceIdentifier(type, id)
    , mID(id) {
}

bool RtMidiDeviceIdentifier::equals(const MidiDeviceID other) {
    if (!other) {return false;}
    auto otherRaw = dynamic_cast<const RtMidiDeviceIdentifier*>(other.get());
    if (!otherRaw) {return false;}

    return mID == otherRaw->mID && mType == other->type();
}

}  // namespace midi
