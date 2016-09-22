#include "midisystem.h"

#include "rtmidi/rtmidimanager.h"

namespace midi {

MidiSystem::MidiSystem() {
    mManager.reset(new RtMidiManager());
}

}
