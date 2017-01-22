#ifndef PCMDEVICE_H
#define PCMDEVICE_H

#include "prerequisites.h"

#include <stdint.h>

class AudioInterface;

class EPT_EXTERN PCMDevice
{
public:
    typedef int16_t DataType;

    virtual void open(AudioInterface *audioInterface);
    virtual void close();
    virtual int64_t read(char *data, int64_t max_bytes) = 0;
    virtual int64_t write(const char *data, int64_t max_bytes) = 0;

    bool isOpen() const {return mAudioInterface != nullptr;}
    int getSampleRate() const;
    int getChannels() const;
protected:
    AudioInterface *mAudioInterface = nullptr;
};

#endif // PCMDEVICE_H
