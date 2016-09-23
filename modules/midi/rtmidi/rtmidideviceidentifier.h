#ifndef RTMIDIDEVICEIDENTIFIER_H
#define RTMIDIDEVICEIDENTIFIER_H

#include <string>

#include "midiprerequisites.h"
#include "midideviceidentifier.h"

namespace midi {

class RtMidiDeviceIdentifier : public MidiDeviceIdentifier
{
public:
    RtMidiDeviceIdentifier(const MidiType type, const std::string &id);

    const std::string &id() const {return mID;}

    virtual bool equals(const MidiDeviceID other) override final;
private:
    const std::string mID;
};

}  // namespace midi

#endif // RTMIDIDEVICEIDENTIFIER_H
