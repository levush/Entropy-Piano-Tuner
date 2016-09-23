#ifndef MIDISYSTEM_H
#define MIDISYSTEM_H

#include <memory>

#include "midiprerequisites.h"
#include "midimanager.h"
#include "midiinputlistener.h"

namespace midi {

MIDI_EXTERN_TEMPLATE class std::unique_ptr<MidiManager>;

class MIDI_EXTERN MidiSystem
{
public:
    static MidiSystem &instance();

    MidiManager &manager() {
        return *mManager;
    }

private:
    MidiSystem();

    std::unique_ptr<MidiManager> mManager;
};

inline static MidiManager &manager() {
    return MidiSystem::instance().manager();
}

}  // namespace midi

#endif // MIDISYSTEM_H
