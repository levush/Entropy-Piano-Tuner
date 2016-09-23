#include "androidmidiinputdevice.h"

namespace midi {

AndroidMidiInputDevice::AndroidMidiInputDevice(MidiDeviceID id, JNIObject usbDriver)
    : MidiInputDevice(id)
    , mUsbMidiDriver(usbDriver)
{

}

}  // namespace midi
