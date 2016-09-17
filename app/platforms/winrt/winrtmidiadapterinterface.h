#ifndef WINRTMIDIADAPTERINTERFACE_H
#define WINRTMIDIADAPTERINTERFACE_H

#include "core/audio/midi/midiadapter.h"

#include <winrtmidiadapter.h>

class WinRTMidiAdapterInterface : public MidiAdapter, WinRTMidiAdapterCallback
{
public:
    WinRTMidiAdapterInterface();

    void init();
    void exit();

    int GetNumberOfPorts();
    std::string GetPortName(int i);
    bool OpenPort (int i, std::string AppName="");
    bool OpenPort (std::string AppName="");
    int getCurrentPort() const;

protected:
    virtual void sendMidiEvent(int byte0, int byte1, int byte2, double deltaTime) override final;

private:
    WinRTMidiAdapter mAdapter;
};

#endif // WINRTMIDIADAPTERINTERFACE_H
