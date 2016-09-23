#ifndef MIDIINPUTLISTENER_H
#define MIDIINPUTLISTENER_H

#include "midiprerequisites.h"
#include "midibasecallback.h"

namespace midi {

class MIDI_EXTERN MidiInputListener : public CallbackInterface<MidiInputListener> {
public:
    virtual void receiveMessage(int cmd, int byte1, int byte2) {
        MIDI_UNUSED(cmd);
        MIDI_UNUSED(byte1);
        MIDI_UNUSED(byte2);
    }
};

}  // namespace midi

#endif // MIDIINPUTLISTENER_H
