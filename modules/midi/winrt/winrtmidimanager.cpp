#include "winrtmidimanager.h"

namespace midi {

std::vector<MidiDeviceID> WinRTMidiManager::listAvailableInputDevices() const {
    std::vector<MidiDeviceID> res;

    return res;
}

std::vector<MidiDeviceID> WinRTMidiManager::listAvailableOutputDevices() const {
    std::vector<MidiDeviceID> res;

    return res;
}

MidiResult WinRTMidiManager::init_impl() {
    return OK;
}

MidiResult WinRTMidiManager::exit_impl() {
    return OK;
}

WinRTMidiManager::MidiInDevRes WinRTMidiManager::createInputDevice_impl(const MidiDeviceID id) {
    return std::make_pair(OK, MidiInputDevicePtr());
}

WinRTMidiManager::MidiOutDevRes WinRTMidiManager::createOutputDevice_impl(const MidiDeviceID id) {
    return std::make_pair(OK, MidiOutputDevicePtr());
}

MidiResult WinRTMidiManager::deleteDevice_impl(MidiInputDevicePtr device) {
    return OK;
}

MidiResult WinRTMidiManager::deleteDevice_impl(MidiOutputDevicePtr device) {
    return OK;
}

}  // namespace midi
