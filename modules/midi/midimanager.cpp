#include "midimanager.h"

namespace midi {

MidiManager::MidiManager() {

}

MidiManager::~MidiManager() {
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
    auto r = createInputDevice_impl(id);
    if (r.first == OK) {
        mMidiInputDevices.push_back(r.second);
    }

    return r;
}

MidiManager::MidiOutDevRes MidiManager::createOutputDevice(const MidiDeviceID id) {
    auto r = createOutputDevice_impl(id);
    if (r.first == OK) {
        mMidiOutputDevices.push_back(r.second);
    }

    return r;
}

}  // namespace midi
