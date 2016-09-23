#ifndef ANDROIDMIDIOUTPUTDEVICE_H
#define ANDROIDMIDIOUTPUTDEVICE_H

#include "midioutputdevice.h"
#include "jniobject.h"

namespace midi {

class AndroidMidiOutputDevice
        : public MidiOutputDevice {
public:
    AndroidMidiOutputDevice(MidiDeviceID id, JNIObject usbDriver);

    virtual void handleRawEvent(int cmd, int arg1, int arg2) override final;

private:
    JNIObject mUsbDriver;
};

}  // namespace midi

#endif // ANDROIDMIDIOUTPUTDEVICE_H
