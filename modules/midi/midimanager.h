#ifndef MIDIMANAGER_H
#define MIDIMANAGER_H

#include <memory>
#include <list>
#include <string>
#include <vector>
#include <tuple>

#include "midiprerequisites.h"
#include "midiinputdevice.h"
#include "midioutputdevice.h"
#include "midideviceidentifier.h"

namespace midi {

class MidiManager
{
public:
    typedef std::pair<MidiResult, MidiInputDevicePtr> MidiInDevRes;
    typedef std::pair<MidiResult, MidiOutputDevicePtr> MidiOutDevRes;

protected:
    MidiManager();

public:
    virtual ~MidiManager();

public:
    virtual MidiResult init(bool input = true, bool output = true) = 0;
    virtual MidiResult exit() = 0;

    virtual std::vector<MidiDeviceID> listAvailableInputDevices() const = 0;
    virtual std::vector<MidiDeviceID> listAvailableOutputDevices() const = 0;

    MidiInputDevicePtr getConnectedInputDevice() const;
    std::list<MidiInputDevicePtr> getConnectedInputDevices() const {return mMidiInputDevices;}

    MidiOutputDevicePtr getConnectedOutputDevice() const;
    std::list<MidiOutputDevicePtr> getConnectedOutputDevices() const {return mMidiOutputDevices;}

    MidiInDevRes createDefaultInputDevice();
    MidiOutDevRes createDefaultOutputDevice();

    MidiInDevRes createInputDevice(const MidiDeviceID id);
    MidiOutDevRes createOutputDevice(const MidiDeviceID id);

    void deleteDevice(MidiInputDevicePtr device);
    void deleteDevice(MidiOutputDevicePtr device);
    void deleteDevice(const MidiDeviceID id);

protected:
    virtual MidiInDevRes createInputDevice_impl(const MidiDeviceID id) = 0;
    virtual MidiOutDevRes createOutputDevice_impl(const MidiDeviceID id) = 0;

protected:
    std::list<MidiInputDevicePtr> mMidiInputDevices;
    std::list<MidiOutputDevicePtr> mMidiOutputDevices;
};

}  // namespace midi

#endif // MIDIMANAGER_H
