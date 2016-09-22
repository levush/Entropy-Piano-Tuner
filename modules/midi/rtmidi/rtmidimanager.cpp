#include "rtmidimanager.h"
#include "rtmidideviceidentifier.h"
#include "rtmidiinputdevice.h"

#include <thread>
#include <chrono>

namespace midi {

RtMidiManager::RtMidiManager() {
}

MidiResult RtMidiManager::init(bool input, bool output) {
    if (input && !mRtMidiIn) {
        try {
            mRtMidiIn.reset(new RtMidiIn());
        } catch (const RtMidiError &error) {
            mRtMidiIn.reset();
            return BACKEND_CREATION_ERROR;
        }

        if (mRtMidiIn->getCurrentApi() == RtMidi::UNSPECIFIED) {
            return UNKNOWN_API;
        }
    }

    if (output && !mRtMidiOut) {
        try {
            mRtMidiOut.reset(new RtMidiOut());
        } catch (const RtMidiError &error) {
            mRtMidiOut.reset();
            return BACKEND_CREATION_ERROR;
        }

        if (mRtMidiOut->getCurrentApi() == RtMidi::UNSPECIFIED) {
            return UNKNOWN_API;
        }
    }

    return OK;
}

MidiResult RtMidiManager::exit() {
    mRtMidiIn.reset();
    mRtMidiOut.reset();

    return OK;
}

std::vector<MidiDeviceID> RtMidiManager::listAvailableInputDevices() const {
    std::vector<MidiDeviceID> devices;

    if (!mRtMidiIn) {
        return devices;
    }

    try {
        for (unsigned int i = 0; i < mRtMidiIn->getPortCount(); ++i) {
            auto name = mRtMidiIn->getPortName(i);
            if (name.find("RtMidi") == std::string::npos) {
                // skip RtMidi device
                devices.push_back(std::make_shared<RtMidiDeviceIdentifier>(name));
            }
        }
    } catch (const RtMidiError& error) {
    }

    return devices;
}

std::vector<MidiDeviceID> RtMidiManager::listAvailableOutputDevices() const {
    std::vector<MidiDeviceID> devices;

    if (!mRtMidiOut) {
        return devices;
    }

    try {
        for (unsigned int i = 0; i < mRtMidiOut->getPortCount(); ++i) {
            auto name = mRtMidiOut->getPortName(i);
            if (name.find("RtMidi") == std::string::npos) {
                // skip RtMidi device
                devices.push_back(std::make_shared<RtMidiDeviceIdentifier>(name));
            }
        }
    } catch (const RtMidiError& error) {
    }

    return devices;
}

MidiManager::MidiInDevRes RtMidiManager::createInputDevice_impl(const MidiDeviceID id) {
    if (!mRtMidiIn) {return std::make_pair(MIDI_INPUT_DISABLED, MidiInputDevicePtr());}

    auto rawStringId = std::static_pointer_cast<const RtMidiDeviceIdentifier>(id)->id();
    if (mRtMidiIn->isPortOpen()) {
        mRtMidiIn->closePort();
    }

    if (mRtMidiIn->getPortCount() == 0) {
        return std::make_pair(MIDI_INPUT_NO_DEVICES, MidiInputDevicePtr());
    }

    // find position of port
    for (unsigned int i = 0; i < mRtMidiIn->getPortCount(); ++i) {
        const std::string deviceId(mRtMidiIn->getPortName(i));
        if (deviceId == rawStringId) {
            // Trick to get connected, not understood
            mRtMidiIn.reset(new RtMidiIn());
            // connect
            mRtMidiIn->openPort(i);

            if (!mRtMidiIn->isPortOpen()) {
                return std::make_pair(BACKEND_ERROR, MidiInputDevicePtr());
            }

            // clear events
            clearInputQueue();
            // create device
            auto device = std::make_shared<RtMidiInputDevice>(std::make_shared<RtMidiDeviceIdentifier>(deviceId));
            // connect it to the input source
            mRtMidiIn->setCallback(&staticCallback, device.get());
            // return the newly created input device
            return std::make_pair(OK, MidiInputDevicePtr(device));
        }
    }

    return std::make_pair(MIDI_INPUT_DEVICE_ID_NOT_FOUND, MidiInputDevicePtr());
}

MidiManager::MidiOutDevRes RtMidiManager::createOutputDevice_impl(const MidiDeviceID id) {
    if (!mRtMidiIn) {return std::make_pair(MIDI_OUTPUT_DISABLED, MidiOutputDevicePtr());}

    return std::make_pair(MIDI_UNIMPLEMENTED, MidiOutputDevicePtr());
}


MidiResult RtMidiManager::clearInputQueue() {
    try {
        std::vector<unsigned char> message;
        bool clearing = true;
        int timeout = 1000;
        while (clearing and --timeout > 0) {
            clearing = false;
            for (int i = 0; i < 256; ++i) {
                std::this_thread::sleep_for(std::chrono::microseconds(2));
                mRtMidiIn->getMessage(&message);
                if (message.size() > 0) {clearing = true;}
            }
        }
    } catch (const RtMidiError &error) {
        return BACKEND_ERROR;
    }

    return OK;
}

void RtMidiManager::staticCallback(double deltaTime, std::vector<unsigned char> *data, void *object) {
    RtMidiInputDevice* device = static_cast<RtMidiInputDevice*>(object);
    device->callback(deltaTime, data);
}

}  // namespace midi
