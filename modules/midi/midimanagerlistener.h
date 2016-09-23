#ifndef MIDIMANAGERLISTENER_H
#define MIDIMANAGERLISTENER_H

#include "midibasecallback.h"
#include "midideviceidentifier.h"
#include "midiinputdevice.h"
#include "midioutputdevice.h"

namespace midi {

class MidiManagerListener : public CallbackInterface<MidiManagerListener> {
public:
    virtual void inputDeviceAttached(MidiDeviceID id) {MIDI_UNUSED(id);}
    virtual void outputDeviceAttached(MidiDeviceID id) {MIDI_UNUSED(id);}

    virtual void inputDeviceDetached(MidiDeviceID id) {MIDI_UNUSED(id);}
    virtual void outputDeviceDetached(MidiDeviceID id) {MIDI_UNUSED(id);}

    virtual void inputDeviceCreated(MidiInputDevicePtr device) {MIDI_UNUSED(device);}
    virtual void outputDeviceCreated(MidiOutputDevicePtr device) {MIDI_UNUSED(device);}

    virtual void inputDeviceDeleted(MidiDeviceID id) {MIDI_UNUSED(id);}
    virtual void outputDeviceDeleted(MidiDeviceID id) {MIDI_UNUSED(id);}
};

}  // namespace midi

#endif // MIDIMANAGERLISTENER_H
