#ifndef MIDIDEVICE_H
#define MIDIDEVICE_H

#include <memory>

#include "midideviceidentifier.h"

namespace midi {

class MidiDevice
{
public:
    MidiDevice(MidiDeviceID id);

    const MidiDeviceID id() const {return mID;}

    virtual void handleRawEvent(int cmd, int arg1, int arg2) = 0;

private:
    const MidiDeviceID mID;
};

typedef std::shared_ptr<MidiDevice> MidiDevicePtr;

}

#endif // MIDIDEVICE_H
