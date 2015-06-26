#ifndef PLATFORMTOOLSCORE_H
#define PLATFORMTOOLSCORE_H

#include <memory>

#include "core/midi/midiadapter.h"

class PlatformToolsCore
{
private:
    static PlatformToolsCore* mSingletonPtr;

protected:
    PlatformToolsCore();

public:
    static PlatformToolsCore *getSingleton();

    virtual std::shared_ptr<MidiAdapter> createMidiAdapter() const;
};

#endif // PLATFORMTOOLSCORE_H
