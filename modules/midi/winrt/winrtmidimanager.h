#ifndef WINRTMIDIMANAGER_H
#define WINRTMIDIMANAGER_H

#include "midimanager.h"

namespace midi {

class WinRTMidiManager : public MidiManager {
public:
protected:
    virtual std::vector<MidiDeviceID> listAvailableInputDevices() const override final;
    virtual std::vector<MidiDeviceID> listAvailableOutputDevices() const override final;

    virtual MidiResult init_impl() override final;
    virtual MidiResult exit_impl() override final;
    virtual MidiInDevRes createInputDevice_impl(const MidiDeviceID id) override final;
    virtual MidiOutDevRes createOutputDevice_impl(const MidiDeviceID id) override final;
    virtual MidiResult deleteDevice_impl(MidiInputDevicePtr device) override final;
    virtual MidiResult deleteDevice_impl(MidiOutputDevicePtr device) override final;
};

}

#endif // WINRTMIDIMANAGER_H
