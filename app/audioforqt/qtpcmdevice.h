#ifndef QTPCMDEVICE_H
#define QTPCMDEVICE_H

#include <QIODevice>

#include "core/audio/pcmdevice.h"

class AudioInterfaceForQt;

class QtPCMDevice : public QIODevice
{
public:
    QtPCMDevice(AudioInterfaceForQt *audioInterface);

    virtual bool open(OpenMode mode) override final;
    virtual void close() override final;

    void setDevice(PCMDevice *device);
    PCMDevice *getDevice() const {return mDevice;}


    virtual bool isSequential() const override {return false;}
private:
    virtual qint64 readData(char *data, qint64 maxSize) override final;
    virtual qint64 writeData(const char *data, qint64 maxSize) override final;

private:
    PCMDevice *mDevice = nullptr;
    AudioInterfaceForQt *mAudioInterface;
};

#endif // QTPCMDEVICE_H
