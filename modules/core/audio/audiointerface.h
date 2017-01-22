#ifndef AUDIOINTERFACE_H
#define AUDIOINTERFACE_H

#include "prerequisites.h"

#include <string>

#include "pcmdevice.h"

class EPT_EXTERN AudioInterface
{
public:
    /// Floating point data type for a single PCM Value. The PCM values are
    /// assumed to be in [-1,1].
    typedef double PCMDataType;

    /// Type definition of a PCM packet (vector of PCM values).
    typedef std::vector<PCMDataType> PacketType;

public:
    virtual void init() = 0;            ///< Inizialize the audio device.
    virtual void exit() = 0;            ///< Destroy the audio device.

    virtual void start() = 0;           ///< Start/restart the audio device.
    virtual void stop()  = 0;           ///< Stop the audio device.


    void suspend(bool v = true);
    void resume(bool v = true);
    bool isSuspended() const {return mSuspended;}

    virtual const std::string getDeviceName() const = 0;    // get the device name
    virtual int getSamplingRate() const = 0;                // get actual sampling rate
    virtual int getChannelCount() const = 0;                // get number of channels

    virtual PCMDevice *getDevice() const = 0;
    virtual void setDevice(PCMDevice *device) = 0;

    virtual void setGain(double gain) = 0;
    virtual double getGain() const = 0;

protected:
    virtual void suspendChanged(bool s) = 0;

protected:
    bool mSuspended = false;
};

#endif // AUDIOINTERFACE_H
