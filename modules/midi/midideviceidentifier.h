#ifndef MIDIDEVICEIDENTIFIER_H
#define MIDIDEVICEIDENTIFIER_H

#include <memory>

namespace midi {

class MidiDeviceIdentifier;
typedef std::shared_ptr<MidiDeviceIdentifier> MidiDeviceID;

class MidiDeviceIdentifier
{
public:
    MidiDeviceIdentifier(const std::string &humanReadable);

    const std::string &humanReadable() const {return mHumanReadable;}

    virtual bool equals(const MidiDeviceID other) = 0;

private:
    const std::string mHumanReadable;
};

}  // namespace midi

#endif // MIDIDEVICEIDENTIFIER_H
