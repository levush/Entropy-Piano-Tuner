#include "mididevicewatcher.h"
#include "midisystem.h"

namespace midi {

void MidiDeviceWatcher::start(std::chrono::milliseconds updateInterval) {
    stop();

    mLastInputDevices = std::move(manager().listAvailableInputDevices());
    mLastOutputDevices = std::move(manager().listAvailableOutputDevices());

    mUpdateInterval = updateInterval;
    mStopThread = false;

    mThread = std::thread(&MidiDeviceWatcher::loop, this);
#if defined(__linux__) && !defined(__ANDROID__)
    // Note: If you encounter a compiler error in the following line
    // please comment it out. It serves only for debugging purposes on Qt.
    pthread_setname_np(mThread.native_handle(), "MIDIDeviceWatcher Thread");
#endif


}

void MidiDeviceWatcher::stop() {
    if (!mThread.joinable()) {return;}  // already stopped
    mStopThread = true;
    mThread.join();
}

void MidiDeviceWatcher::loop() {
    std::chrono::milliseconds waited;
    const std::chrono::milliseconds minWaitInterval(100);
    while (true) {
        while (waited < mUpdateInterval) {
            waited += minWaitInterval;
            std::this_thread::sleep_for(minWaitInterval);
            if (mStopThread) {
                return;
            }
        }
        waited = std::chrono::milliseconds(0);

        // compare lists
        const std::vector<MidiDeviceID> newInputDevices(manager().listAvailableInputDevices());
        const std::vector<MidiDeviceID> newOutputDevices(manager().listAvailableOutputDevices());

        // check for new deviced
        for (MidiDeviceID newID : newInputDevices) {
            auto it = std::find_if(mLastInputDevices.begin(), mLastInputDevices.end(),
                                   [newID](const MidiDeviceID id) {return newID->equals(id);});
            if (it == mLastInputDevices.end()) {
                manager().invokeCallback(&MidiManagerListener::inputDeviceAttached, newID);
            }
        }

        for (MidiDeviceID newID : newOutputDevices) {
            auto it = std::find_if(mLastOutputDevices.begin(), mLastOutputDevices.end(),
                                   [newID](const MidiDeviceID id) {return newID->equals(id);});
            if (it == mLastOutputDevices.end()) {
                manager().invokeCallback(&MidiManagerListener::outputDeviceAttached, newID);
            }
        }

        // check for removed devices
        for (MidiDeviceID oldID : mLastInputDevices) {
            auto it = std::find_if(newInputDevices.begin(), newInputDevices.end(),
                                   [oldID](const MidiDeviceID id) {return oldID->equals(id);});
            if (it == newInputDevices.end()) {
                manager().invokeCallback(&MidiManagerListener::inputDeviceDetached, oldID);
            }
        }

        for (MidiDeviceID oldID : mLastOutputDevices) {
            auto it = std::find_if(newOutputDevices.begin(), newOutputDevices.end(),
                                   [oldID](const MidiDeviceID id) {return oldID->equals(id);});
            if (it == newOutputDevices.end()) {
                manager().invokeCallback(&MidiManagerListener::outputDeviceAttached, oldID);
            }
        }

        // update lists
        mLastInputDevices = std::move(newInputDevices);
        mLastOutputDevices = std::move(newOutputDevices);
    }
}

}  // namespace midi
