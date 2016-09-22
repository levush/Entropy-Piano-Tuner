#include "rtmidiinputdevice.h"

namespace midi {

RtMidiInputDevice::RtMidiInputDevice(MidiDeviceID id)
    : MidiInputDevice(id)
{

}

void RtMidiInputDevice::callback(double deltaTime, std::vector<unsigned char> *data) {
    MIDI_UNUSED(deltaTime);
    if (data->size() == 1) {
        handleRawEvent(data->at(0), 0, 0);
    } else if (data->size() == 2) {
        handleRawEvent(data->at(0), data->at(1), 0);
    } else {
        handleRawEvent(data->at(0), data->at(1), data->at(2));
    }
}

}  // namespace midi
