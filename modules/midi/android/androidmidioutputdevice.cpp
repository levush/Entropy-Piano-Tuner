#include "androidmidioutputdevice.h"

namespace midi {

AndroidMidiOutputDevice::AndroidMidiOutputDevice(MidiDeviceID id, JNIObject usbDriver)
    : MidiOutputDevice(id)
    , mUsbDriver(usbDriver) {
}

void AndroidMidiOutputDevice::handleRawEvent(int cmd, int arg1, int arg2) {

}

}
