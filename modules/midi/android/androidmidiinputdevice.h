#ifndef ANDROIDMIDIINPUTDEVICE_H
#define ANDROIDMIDIINPUTDEVICE_H


#include "jniobject.h"
#include "midiinputdevice.h"

namespace midi {

class AndroidMidiInputDevice : public MidiInputDevice
{
public:
    AndroidMidiInputDevice(MidiDeviceID id, JNIObject usbDriver);

private:
    mutable JNIObject mUsbMidiDriver;
};

}  // namespace midi

#endif // ANDROIDMIDIINPUTDEVICE_H
