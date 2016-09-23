#ifndef MIDIDEVICEWATCHER_H
#define MIDIDEVICEWATCHER_H

#include <chrono>
#include <vector>
#include <thread>
#include <atomic>

#include "midiprerequisites.h"
#include "midideviceidentifier.h"

namespace midi {

class MidiDeviceWatcher
{
public:
    void start(std::chrono::milliseconds updateInterval = std::chrono::milliseconds(1000));
    void stop();

private:
    void loop();

private:
    std::chrono::milliseconds mUpdateInterval;
    std::thread mThread;
    std::atomic<bool> mStopThread;

    std::vector<MidiDeviceID> mLastInputDevices;
    std::vector<MidiDeviceID> mLastOutputDevices;
};

}  // namespace midi

#endif // MIDIDEVICEWATCHER_H
