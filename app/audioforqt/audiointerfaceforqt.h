#ifndef AUDIOINTERFACEFORQT_H
#define AUDIOINTERFACEFORQT_H

#include <QObject>
#include <QAudioDeviceInfo>

#include "core/audio/audiointerface.h"
#include "qtpcmdevice.h"

class AudioInterfaceForQt : public QObject, public AudioInterface
{
    Q_OBJECT

public:
    static const int DEFAULT_BUFFER_SIZE_MS;

public:
    AudioInterfaceForQt(QAudio::Mode mode, QObject *parent);

    void reinitialize(int samplingRate, int channelCount, QAudioDeviceInfo deviceInfo, int bufferSizeMS);
    virtual void init() override final;

    const QAudioFormat &getFormat() const {return mFormat;}
    const QAudioDeviceInfo &getDeviceInfo() const {return mDeviceInfo;}
    int getBufferSizeMS() const;

    virtual const std::string getDeviceName() const override final;
    virtual int getSamplingRate() const override final;
    virtual int getChannelCount() const override final;

    virtual PCMDevice *getDevice() const override final;
    virtual void setDevice(PCMDevice *device) override final;

protected:
    virtual QAudio::Error createDevice(const QAudioFormat &format, const QAudioDeviceInfo &info, int bufferSizeMS = -1) = 0;

protected:
    const QAudio::Mode mMode;
    const QString mSettingsPrefix;
    QtPCMDevice mPCMDevice;
    QAudioFormat mFormat;
    QAudioDeviceInfo mDeviceInfo;
};

#endif // AUDIOINTERFACEFORQT_H
