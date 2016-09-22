#include "midiinputdevice.h"

namespace midi {

MidiInputDevice::MidiInputDevice(MidiDeviceID id)
    : MidiDevice(id) {

}

void MidiInputDevice::handleRawEvent(int cmd, int arg1, int arg2) {
    invokeCallback(&MidiInputListener::receiveMessage, cmd, arg1, arg2);
}

}  // namespace midi
