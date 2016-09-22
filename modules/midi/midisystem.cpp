#include "midisystem.h"

#if defined(MIDI_USE_RTMIDI)
    #include "rtmidi/rtmidimanager.h"
    using MidiManagerImplementation = midi::RtMidiManager;
#elif defined(MIDI_USE_ANDROID)
    #include "android/androidmidimanager.h"
    using MidiManagerImplementation = midi::AndroidMidiManager;
#endif

namespace midi {

MidiSystem::MidiSystem() {
    mManager.reset(new MidiManagerImplementation());
}

}
