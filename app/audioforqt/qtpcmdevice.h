#ifndef QTPCMDEVICE_H
#define QTPCMDEVICE_H

#include <QIODevice>

#include "core/audio/player/pcmwriterinterface.h"

class QtPCMDevice : public QIODevice
{
public:
    void setWriter(PCMWriterInterface *writer);

    virtual bool isSequential() const {return false;}
private:
    virtual qint64 readData(char *data, qint64 maxSize) override final;
    virtual qint64 writeData(const char *data, qint64 maxSize) override final;

private:
    PCMWriterInterface *mWriter = nullptr;
};

#endif // QTPCMDEVICE_H
