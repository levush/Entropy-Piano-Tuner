#ifndef RTMIDIMANAGER_H
#define RTMIDIMANAGER_H

#include <vector>

#define __LINUX_ALSA__
#include <RtMidi/RtMidi.h>

#include "midimanager.h"

namespace midi {

class RtMidiManager : public MidiManager
{
public:
    RtMidiManager();

    virtual MidiResult init(bool input, bool output) override final;
    virtual MidiResult exit() override final;

    virtual std::vector<MidiDeviceID> listAvailableInputDevices() const override final;
    virtual std::vector<MidiDeviceID> listAvailableOutputDevices() const override final;

protected:
    virtual MidiInDevRes createInputDevice_impl(const MidiDeviceID id) override final;
    virtual MidiOutDevRes createOutputDevice_impl(const MidiDeviceID id) override final;

private:
    MidiResult clearInputQueue();

    std::shared_ptr<RtMidiIn> mRtMidiIn;
    std::shared_ptr<RtMidiOut> mRtMidiOut;

    static void staticCallback(double deltaTime,
                               std::vector<unsigned char>* data,
                               void *object);
};

}  // namespace midi

#endif // RTMIDIMANAGER_H
