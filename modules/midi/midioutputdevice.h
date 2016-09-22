#ifndef MIDIOUTPUTDEVICE_H
#define MIDIOUTPUTDEVICE_H

#include <memory>

#include "mididevice.h"

namespace midi {

class MidiOutputDevice : public MidiDevice
{
public:
    MidiOutputDevice(MidiDeviceID id);
};

typedef std::shared_ptr<MidiOutputDevice> MidiOutputDevicePtr;

}  // namespace midi

#endif // MIDIOUTPUTDEVICE_H
