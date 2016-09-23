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
#include "midimanagerlistener.h"
#include "mididevicewatcher.h"

namespace midi {

class MidiConfiguration {
public:
    bool mEnableInput = true;
    bool mEnableOutput = true;

    bool mAutoConnectInputDevice = true;
    bool mAutoConnectOutputDevice = true;

protected:
    // These variables can not be changed yet, only one device supported
    bool mSingleInputDevice = true;
    bool mSingleOutputDevice = true;
};

class MidiManager
        : protected MidiConfiguration
        , public CallbackManager<MidiManagerListener>
        , public MidiManagerListener                // Listen to itself to handle auto conenct
{
public:
    typedef std::pair<MidiResult, MidiInputDevicePtr> MidiInDevRes;
    typedef std::pair<MidiResult, MidiOutputDevicePtr> MidiOutDevRes;

protected:
    MidiManager();

public:
    virtual ~MidiManager();

public:
    MidiResult init(const MidiConfiguration &config);
    MidiResult exit();

    virtual std::vector<MidiDeviceID> listAvailableInputDevices() const = 0;
    virtual std::vector<MidiDeviceID> listAvailableOutputDevices() const = 0;

    MidiInputDevicePtr getConnectedInputDevice() const;
    MidiDeviceID getConnectedInputDeviceID() const;
    std::list<MidiInputDevicePtr> getConnectedInputDevices() const {return mMidiInputDevices;}

    MidiOutputDevicePtr getConnectedOutputDevice() const;
    MidiDeviceID getConnectedOutputDeviceID() const;
    std::list<MidiOutputDevicePtr> getConnectedOutputDevices() const {return mMidiOutputDevices;}

    MidiInDevRes createDefaultInputDevice();
    MidiOutDevRes createDefaultOutputDevice();

    MidiInDevRes createInputDevice(const MidiDeviceID id);
    MidiOutDevRes createOutputDevice(const MidiDeviceID id);

    MidiResult deleteAllInputDevices();
    MidiResult deleteAllOutputDevices();
    MidiResult deleteDevice(MidiInputDevicePtr device);
    MidiResult deleteDevice(MidiOutputDevicePtr device);
    MidiResult deleteDevice(const MidiDeviceID id);

    MidiInDevRes findMidiInputDevice(MidiDeviceID id);
    MidiOutDevRes findMidiOutputDevice(MidiDeviceID id);

protected:
    virtual MidiResult init_impl() = 0;
    virtual MidiResult exit_impl() = 0;
    virtual MidiInDevRes createInputDevice_impl(const MidiDeviceID id) = 0;
    virtual MidiOutDevRes createOutputDevice_impl(const MidiDeviceID id) = 0;
    virtual MidiResult deleteDevice_impl(MidiInputDevicePtr device) = 0;
    virtual MidiResult deleteDevice_impl(MidiOutputDevicePtr device) = 0;


    virtual void inputDeviceAttached(MidiDeviceID id) override;
    virtual void outputDeviceAttached(MidiDeviceID id) override;

    virtual void inputDeviceDetached(MidiDeviceID id) override;
    virtual void outputDeviceDetached(MidiDeviceID id) override;


protected:
    std::list<MidiInputDevicePtr> mMidiInputDevices;
    std::list<MidiOutputDevicePtr> mMidiOutputDevices;

    MidiDeviceWatcher mMidiDeviceWatcher;
    friend class MidiDeviceWatcher;
};

}  // namespace midi

#endif // MIDIMANAGER_H
