#ifndef RTMIDIINPUTDEVICE_H
#define RTMIDIINPUTDEVICE_H

#include <vector>

#include "midiprerequisites.h"
#include "midiinputdevice.h"

namespace midi {

class RtMidiInputDevice : public MidiInputDevice
{
public:
    RtMidiInputDevice(MidiDeviceID id);

    void callback(double deltaTime, std::vector<unsigned char> *data);
};

}  // namespace midi

#endif // RTMIDIINPUTDEVICE_H
