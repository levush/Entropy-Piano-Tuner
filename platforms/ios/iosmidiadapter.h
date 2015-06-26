#ifndef IOSMIDIADAPTER_H
#define IOSMIDIADAPTER_H

#include "core/midi/midiadapter.h"

class IOsMidiAdapter : public MidiAdapter
{
public:
    IOsMidiAdapter();

    void init() override final;
    void exit() override final;

    int GetNumberOfPorts ();
    std::string GetPortName   (int i);
    bool OpenPort (int i, std::string AppName="");
    bool OpenPort (std::string AppName="");
    int getCurrentPort() const;
};

#endif // IOSMIDIADAPTER_H
