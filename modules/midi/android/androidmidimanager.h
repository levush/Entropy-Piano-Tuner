#ifndef ANDROIDMIDIMANAGER_H
#define ANDROIDMIDIMANAGER_H


#include "jniobject.h"
#include "midimanager.h"

namespace midi {

class AndroidMidiManager : public MidiManager {
public:
    AndroidMidiManager();
    virtual ~AndroidMidiManager();

    void midiInputDeviceAttached(const std::string &id);
    void midiOutputDeviceAttached(const std::string &id);

    void midiInputDeviceDetached(const std::string &id);
    void midiOutputDeviceDetached(const std::string &id);

    void receiveMidiEvent(const std::string &id, int cmd, int byte1, int byte2);
protected:
    virtual MidiResult init_impl() override final;
    virtual MidiResult exit() override final;

    virtual std::vector<MidiDeviceID> listAvailableInputDevices() const override final;
    virtual std::vector<MidiDeviceID> listAvailableOutputDevices() const override final;

protected:
    virtual MidiInDevRes createInputDevice_impl(const MidiDeviceID id) override final;
    virtual MidiOutDevRes createOutputDevice_impl(const MidiDeviceID id) override final;

    virtual MidiResult deleteDevice_impl(MidiInputDevicePtr device) override final;
    virtual MidiResult deleteDevice_impl(MidiOutputDevicePtr device) override final;
private:
    mutable JNIObject mUsbMidiDriver = 0;
};

}  // namespace midi

#endif // ANDROIDMIDIMANAGER_H
