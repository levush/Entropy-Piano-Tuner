#include "androidmidimanager.h"
#include "midisystem.h"
#include "androidnativewrapper.h"

namespace midi {

AndroidMidiManager::AndroidMidiManager() {

}

AndroidMidiManager::~AndroidMidiManager() {

}

void AndroidMidiManager::midiInputDeviceAttached(const std::string &id) {
    invokeCallback(&MidiManagerListener::inputDeviceAttached, std::make_shared<MidiDeviceIdentifier>(INPUT, id));
}

void AndroidMidiManager::midiOutputDeviceAttached(const std::string &id) {
    invokeCallback(&MidiManagerListener::outputDeviceAttached, std::make_shared<MidiDeviceIdentifier>(OUTPUT, id));
}

void AndroidMidiManager::midiInputDeviceDetached(const std::string &id) {
    invokeCallback(&MidiManagerListener::inputDeviceDetached, std::make_shared<MidiDeviceIdentifier>(INPUT, id));
}

void AndroidMidiManager::midiOutputDeviceDetached(const std::string &id) {
    invokeCallback(&MidiManagerListener::outputDeviceDetached, std::make_shared<MidiDeviceIdentifier>(OUTPUT, id));
}

void AndroidMidiManager::receiveMidiEvent(const std::string &id, int cmd, int byte1, int byte2) {
    MidiResult r;
    MidiInputDevicePtr dev;
    std::tie(r, dev) = findMidiInputDevice(std::make_shared<MidiDeviceIdentifier>(INPUT, id));
    if (r == OK) {
        dev->handleRawEvent(cmd, byte1, byte2);
    }
}

MidiResult AndroidMidiManager::init_impl() {
    initAndroidManagerJNI(&mUsbMidiDriver);

    if (mUsbMidiDriver == 0) {
        return BACKEND_CREATION_ERROR;
    }

    return OK;
}

MidiResult AndroidMidiManager::exit_impl() {
    releaseAndroidManagerJNI(mUsbMidiDriver);
    mUsbMidiDriver = 0;

    return OK;
}

std::vector<MidiDeviceID> AndroidMidiManager::listAvailableInputDevices() const {
    std::vector<std::string> device_names(android_listAvailableInputDevices(mUsbMidiDriver));
    std::vector<MidiDeviceID> devices;
    devices.reserve(device_names.size());

    for (const std::string &device_name : device_names) {
        devices.push_back(std::make_shared<MidiDeviceIdentifier>(INPUT, device_name));
    }

    return devices;
}

std::vector<MidiDeviceID> AndroidMidiManager::listAvailableOutputDevices() const {
    std::vector<MidiDeviceID> devices;
    return devices;
}

AndroidMidiManager::MidiInDevRes AndroidMidiManager::createInputDevice_impl(const MidiDeviceID id) {
    return android_createInputDevice(id->humanReadable(), mUsbMidiDriver);
}

AndroidMidiManager::MidiOutDevRes AndroidMidiManager::createOutputDevice_impl(const MidiDeviceID id) {
    return android_createOutputDevice(id->humanReadable(), mUsbMidiDriver);
}

MidiResult AndroidMidiManager::deleteDevice_impl(MidiInputDevicePtr device) {
    return android_deleteInputDevice(device->id()->humanReadable(), mUsbMidiDriver);
}

MidiResult AndroidMidiManager::deleteDevice_impl(MidiOutputDevicePtr device) {
    return android_deleteOutputDevice(device->id()->humanReadable(), mUsbMidiDriver);
}

}  // namespace midi
