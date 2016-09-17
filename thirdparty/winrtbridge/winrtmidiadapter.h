#ifndef WINRTMIDIADAPTER_H
#define WINRTMIDIADAPTER_H

#include "winrtbridge.h"

class WINRTBRIDGE_EXTERN WinRTMidiAdapterCallback
{
public:
    virtual void sendMidiEvent(int byte0, int byte1, int byte2, double deltaTime) = 0;
};

class WINRTBRIDGE_EXTERN WinRTMidiAdapter
{
public:
    WinRTMidiAdapter(WinRTMidiAdapterCallback *cb);
    ~WinRTMidiAdapter();

    void init();
    void exit();

    int GetNumberOfPorts();
    std::string GetPortName(int i);
    bool OpenPort (int i, std::string AppName="");
    bool OpenPort (std::string AppName="");
    int getCurrentPort() const;

private:
    int mCurrentPort;
};

#endif // WINRTMIDIADAPTER_H
