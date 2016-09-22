#include "midimanager.h"

namespace midi {

MidiManager::MidiManager() {

}

MidiManager::~MidiManager() {
}

MidiResult MidiManager::init(const MidiConfiguration &config) {
    // copy config
    *((MidiConfiguration*)this) = config;

    // init platform dependent manager
    return init_impl();
}

MidiInputDevicePtr MidiManager::getConnectedInputDevice() const {
    if (mMidiInputDevices.size() == 0) {return MidiInputDevicePtr();}
    return mMidiInputDevices.front();
}

MidiOutputDevicePtr MidiManager::getConnectedOutputDevice() const {
    if (mMidiOutputDevices.size() == 0) {return MidiOutputDevicePtr();}
    return mMidiOutputDevices.front();
}

MidiManager::MidiInDevRes MidiManager::createDefaultInputDevice() {
    auto devices = listAvailableInputDevices();
    if (devices.size() == 0) {
        return std::make_pair(MIDI_OUTPUT_NO_DEVICES, MidiInputDevicePtr());
    } else {
        return createInputDevice(devices.back());
    }

}

MidiManager::MidiOutDevRes MidiManager::createDefaultOutputDevice() {
    auto devices = listAvailableOutputDevices();
    if (devices.size() == 0) {
        return std::make_pair(MIDI_OUTPUT_NO_DEVICES, MidiOutputDevicePtr());
    } else {
        return createOutputDevice(devices.back());
    }
}

MidiManager::MidiInDevRes MidiManager::createInputDevice(const MidiDeviceID id) {
    if (!id) {
        return std::make_pair(MIDI_INPUT_DEVICE_ID_NOT_FOUND, MidiInputDevicePtr());
    }

    auto it_r = std::find_if(mMidiInputDevices.begin(), mMidiInputDevices.end(),
                          [id](MidiInputDevicePtr const &device) {return device->id()->equals(id);});
    if (it_r != mMidiInputDevices.end()) {
        return std::make_pair(OK, *it_r);
    }

    if (mSingleInputDevice) {
        // delete old devices
        deleteAllInputDevices();
    }


    auto r = createInputDevice_impl(id);
    if (r.first == OK) {
        mMidiInputDevices.push_back(r.second);
    }

    return r;
}

MidiManager::MidiOutDevRes MidiManager::createOutputDevice(const MidiDeviceID id) {
    if (!id) {
        return std::make_pair(MIDI_OUTPUT_DEVICE_ID_NOT_FOUND, MidiOutputDevicePtr());
    }

    auto it_r = std::find_if(mMidiOutputDevices.begin(), mMidiOutputDevices.end(),
                          [id](MidiOutputDevicePtr device) {return device->id()->equals(id);});
    if (it_r != mMidiOutputDevices.end()) {
        return std::make_pair(OK, *it_r);
    }

    if (mSingleOutputDevice) {
        // delete old devices
        deleteAllOutputDevices();
    }

    auto r = createOutputDevice_impl(id);
    if (r.first == OK) {
        mMidiOutputDevices.push_back(r.second);
    }

    return r;
}

MidiResult MidiManager::deleteAllInputDevices() {
    MidiResult global_r = OK;
    for (MidiInputDevicePtr dev : mMidiInputDevices) {
        MidiResult r = deleteDevice_impl(dev);
        if (!r) {
            global_r = r;
        }
    }
    mMidiInputDevices.clear();
    return global_r;
}

MidiResult MidiManager::deleteAllOutputDevices() {
    MidiResult global_r = OK;
    for (MidiOutputDevicePtr dev : mMidiOutputDevices) {
        MidiResult r = deleteDevice_impl(dev);
        if (!r) {
            global_r = r;
        }
    }
    mMidiOutputDevices.clear();
    return global_r;
}

MidiResult MidiManager::deleteDevice(MidiInputDevicePtr device) {
    auto it_r = std::find(mMidiInputDevices.begin(), mMidiInputDevices.end(), device);
    if (it_r == mMidiInputDevices.end()) {
        // already deleted
        return OK;
    }

    mMidiInputDevices.erase(it_r);
    return deleteDevice_impl(device);
}

MidiResult MidiManager::deleteDevice(MidiOutputDevicePtr device) {
    auto it_r = std::find(mMidiOutputDevices.begin(), mMidiOutputDevices.end(), device);
    if (it_r == mMidiOutputDevices.end()) {
        // already deleted
        return OK;
    }

    mMidiOutputDevices.erase(it_r);
    return deleteDevice_impl(device);
}

MidiResult MidiManager::deleteDevice(const MidiDeviceID id) {
    if (!id) {
        return OK;
    }

    if (id->type() == INPUT) {
        for (MidiInputDevicePtr dev : mMidiInputDevices) {
            if (dev->id()->equals(id)) {
                return deleteDevice(dev);
            }
        }
    } else if (id->type() == OUTPUT) {
        for (MidiOutputDevicePtr dev : mMidiOutputDevices) {
            if (dev->id()->equals(id)) {
                return deleteDevice(dev);
            }
        }
    }

    return MIDI_DEVICE_ID_NOT_FOUND;
}

}  // namespace midi
