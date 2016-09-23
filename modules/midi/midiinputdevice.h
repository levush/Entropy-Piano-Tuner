#ifndef MIDIINPUTDEVICE_H
#define MIDIINPUTDEVICE_H

#include <memory>
#include <list>

#include "midiprerequisites.h"
#include "mididevice.h"
#include "midibasecallback.h"
#include "midiinputlistener.h"

namespace midi {

class MidiInputDevice : public MidiDevice, public CallbackManager<MidiInputListener>
{
public:
    MidiInputDevice(MidiDeviceID id);

    virtual void handleRawEvent(int cmd, int arg1, int arg2) override;

private:
};

typedef std::shared_ptr<MidiInputDevice> MidiInputDevicePtr;

}  // namespace midi

#endif // MIDIINPUTDEVICE_H
