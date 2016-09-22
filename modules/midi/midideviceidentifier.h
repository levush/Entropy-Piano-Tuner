#ifndef MIDIDEVICEIDENTIFIER_H
#define MIDIDEVICEIDENTIFIER_H

#include <memory>

#include "midiprerequisites.h"

namespace midi {

class MidiDeviceIdentifier;
typedef std::shared_ptr<MidiDeviceIdentifier> MidiDeviceID;

class MidiDeviceIdentifier
{
public:
    MidiDeviceIdentifier(const MidiType type, const std::string &humanReadable);

    const std::string &humanReadable() const {return mHumanReadable;}
    MidiType type() const {return mType;}

    virtual bool equals(const MidiDeviceID other) = 0;

protected:
    const MidiType mType;

private:
    const std::string mHumanReadable;
};

}  // namespace midi

#endif // MIDIDEVICEIDENTIFIER_H
