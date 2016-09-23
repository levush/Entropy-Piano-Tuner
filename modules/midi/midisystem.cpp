#include "midisystem.h"

#if defined(MIDI_USE_RTMIDI)
    #include "rtmidi/rtmidimanager.h"
    using MidiManagerImplementation = midi::RtMidiManager;
#elif defined(MIDI_USE_ANDROID)
    #include "android/androidmidimanager.h"
    using MidiManagerImplementation = midi::AndroidMidiManager;
#endif

namespace midi {

MidiSystem &MidiSystem::instance() {
  static std::unique_ptr<MidiSystem> singleton(new MidiSystem);
  return *singleton;
}

MidiSystem::MidiSystem() {
    mManager.reset(new MidiManagerImplementation());
}

}
