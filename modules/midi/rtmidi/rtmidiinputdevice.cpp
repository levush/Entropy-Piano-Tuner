#include "rtmidiinputdevice.h"

namespace midi {

RtMidiInputDevice::RtMidiInputDevice(MidiDeviceID id)
    : MidiInputDevice(id)
{

}

void RtMidiInputDevice::callback(double deltaTime, std::vector<unsigned char> *data) {
    handleRawEvent(data->at(0), data->at(1), data->at(2));
}

}  // namespace midi
