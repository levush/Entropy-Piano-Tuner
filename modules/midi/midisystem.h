#ifndef MIDISYSTEM_H
#define MIDISYSTEM_H

#include <memory>

#include "midimanager.h"
#include "midiinputlistener.h"

namespace midi {

class MidiSystem
{
public:
    static MidiSystem &instance() {
        static MidiSystem singleton;
        return singleton;
    }

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
