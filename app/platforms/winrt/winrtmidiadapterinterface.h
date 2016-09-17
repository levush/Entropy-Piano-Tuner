#ifndef WINRTMIDIADAPTERINTERFACE_H
#define WINRTMIDIADAPTERINTERFACE_H

#include "core/audio/midi/midiadapter.h"

#include <winrtmidiadapter.h>

class WinRTMidiAdapterInterface : public MidiAdapter, WinRTMidiAdapterCallback
{
public:
    WinRTMidiAdapterInterface();

    void init() {mAdapter.init();}
    void exit() {mAdapter.exit();}

    int GetNumberOfPorts() {return mAdapter.GetNumberOfPorts();}
    std::string GetPortName(int i) {return mAdapter.GetPortName(i);}
    bool OpenPort (int i, std::string AppName="") {return mAdapter.OpenPort(i, AppName);}
    bool OpenPort (std::string AppName="") {return mAdapter.OpenPort(AppName);}
    int getCurrentPort() const {return mAdapter.getCurrentPort();}

protected:
    virtual void sendMidiEvent(int byte0, int byte1, int byte2, double deltaTime) override final;

private:
    WinRTMidiAdapter mAdapter;
};

#endif // WINRTMIDIADAPTERINTERFACE_H
