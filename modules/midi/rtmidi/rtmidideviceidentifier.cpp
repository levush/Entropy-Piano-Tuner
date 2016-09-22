#include "rtmidideviceidentifier.h"

namespace midi {

RtMidiDeviceIdentifier::RtMidiDeviceIdentifier(const std::string &id)
    : MidiDeviceIdentifier(id)
    , mID(id) {
}

bool RtMidiDeviceIdentifier::equals(const MidiDeviceID other) {
    if (!other) {return false;}
    auto otherRaw = dynamic_cast<const RtMidiDeviceIdentifier*>(other.get());
    if (!otherRaw) {return false;}

    return mID == otherRaw->mID;
}

}  // namespace midi
