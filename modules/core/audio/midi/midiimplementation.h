#ifndef MIDIIMPLEMENTATION_H
#define MIDIIMPLEMENTATION_H

#include "midi/midisystem.h"
#include "midi/midimanagerlistener.h"
#include "midi/midimanager.h"
#include "midi/midiinputlistener.h"

#include "midiadapter.h"

class MidiImplementation : public MidiAdapter, public midi::MidiInputListener
{
public:
    MidiImplementation();

    void init() override final;
    void exit() override final;

    int GetNumberOfPorts () override final;                         ///< Get the number of available input devices
    std::string GetPortName (int i) override final;                 ///< Get the name of device i (starting with zero)
    bool OpenPort (int i, std::string AppName="") override final;   ///< Open Midi input device number i
    bool OpenPort (std::string AppName="") override final;          ///< Open Midi device with the highest port number
    int getCurrentPort() const override final;

    virtual void receiveMessage(int cmd, int byte1, int byte2) override final;
};

#endif // MIDIIMPLEMENTATION_H
