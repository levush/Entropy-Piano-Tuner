/*****************************************************************************
 * Copyright 2015 Haye Hinrichsen, Christoph Wick
 *
 * This file is part of Entropy Piano Tuner.
 *
 * Entropy Piano Tuner is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * Entropy Piano Tuner is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Entropy Piano Tuner. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

#include "audioplayerforqt.h"
#include "../core/system/log.h"
#include "settingsforqt.h"
#include "core/audio/rawdatawriter.h"
#include <qdebug.h>

// Maximal absolute value by which the signal is limited

const AudioPlayerForQt::DataFormat AudioPlayerForQt::SIGNAL_SCALING =
        std::numeric_limits<AudioPlayerForQt::DataFormat>::max();


///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor
/// \param parent : Qt object to which the audio player belongs
///////////////////////////////////////////////////////////////////////////////

AudioPlayerForQt::AudioPlayerForQt(QObject *parent)
    : QObject(parent),
      mAudioOutput(nullptr),
      mIODevice(nullptr),
      mNotifyIntervall(MIN_BUFFER_SIZE_IN_MSECS),
      mBufferSize(MIN_BUFFER_SIZE_IN_MSECS / 1000.f * 5)
{
#if __ANDROID__
      mBufferSize *= 5;  // on mobile devices, use a bigger value
#endif
    setDeviceName(SettingsForQt::getSingleton().getOuputDeviceName().toStdString());
    setSamplingRate(SettingsForQt::getSingleton().getOutputDeviceSamplingRate());
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the audio player.
///////////////////////////////////////////////////////////////////////////////

void AudioPlayerForQt::init()
{
    // Format specification:
    QAudioFormat format;
    format.setSampleRate(getSamplingRate());
    format.setChannelCount(getChannelCount());
    format.setCodec("audio/pcm");
    format.setSampleSize(sizeof(DataFormat) * 8);
    format.setSampleType(QAudioFormat::SignedInt);
    //format.setByteOrder(QAudioFormat::LittleEndian);

    // Find the audio device:
    QAudioDeviceInfo device(QAudioDeviceInfo::defaultOutputDevice());
    if (getDeviceName().size() > 0) {
        QList<QAudioDeviceInfo> devices(QAudioDeviceInfo::availableDevices(QAudio::AudioOutput));
        for (const QAudioDeviceInfo &i : devices) {
            if (i.deviceName().toStdString() == getDeviceName()) {
                device = i;
                break;
            }
        }

        if (!device.isFormatSupported(format)) {
            LogE("Selected device settings are not supported!");
        }
    }
    else {
        if (!device.isFormatSupported(format)) {
            LogW("Raw audio format not supported by backend, falling back to default supported");
            format = device.preferredFormat();
            // update sampling rate, buffer type has to stay the same!
            setSamplingRate(format.sampleRate());
            if (format.sampleSize() != sizeof(DataFormat) * 8) {
                LogW("Sample size not supported");
                return;
            }
            if (format.sampleType() != QAudioFormat::SignedInt) {
                LogW("Sample format not supported");
                return;
            }
        }
    }

    // Open the audio output stream
    mAudioOutput = new QAudioOutput(device, format);
    if (mAudioOutput->error() != QAudio::NoError) {
        LogE("Error opening QAudioOutput with error %d", mAudioOutput->error());
        return;
    }


    //mAudioOutput->setNotifyInterval(0);
    mAudioOutput->setBufferSize(getSamplingRate() * mBufferSize * sizeof(DataFormat) * getChannelCount());  // 0.01 seconds buffer size
    //mAudioOutput->setBufferSize(mAudioOutput->periodSize() * 10);
    //mAudioOutput->setBufferSize(getSamplingRate() * sizeof(DataFormat) * 2);


    setDeviceName(device.deviceName().toStdString());

    //QObject::connect(mAudioOutput, SIGNAL(notify()), this, SLOT(onWriteMoreData()));
    if (mAudioOutput->error() != QAudio::NoError) {
        LogE("Error opening QAudioOutput with error %d", mAudioOutput->error());
        return;
    }


    // timer have to be started from the main thread
    QObject::connect(&mWriteTimer, SIGNAL(timeout()), this, SLOT(onWriteMoreData()));

    LogI("Initialized Qt audio player using device: %s", getDeviceName().c_str());
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Exit
///////////////////////////////////////////////////////////////////////////////

void AudioPlayerForQt::exit() {
    if (mAudioOutput) {
        mWriteTimer.stop();
        mAudioOutput->reset();
        delete mAudioOutput;
        mAudioOutput = nullptr;
        mIODevice = nullptr;
    }
    LogI("Qt audio player closed.");
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Start
///////////////////////////////////////////////////////////////////////////////

void AudioPlayerForQt::start() {
    if (!mAudioOutput) {
        LogI("Audio device not created, cannot start it.");
        return;
    }
    if (!mIODevice) {
        mIODevice = mAudioOutput->start();
        if (mAudioOutput->error() != QAudio::NoError) {
            qWarning() << "Error opening QAudioOutput with error " << mAudioOutput->error();
            return;
        }
        if (!mWriteTimer.isActive()) {
            mWriteTimer.start(mNotifyIntervall);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Stop
///////////////////////////////////////////////////////////////////////////////

void AudioPlayerForQt::stop() {
    if (!mAudioOutput) {
        return;
    }
    if (mIODevice) {
        mAudioOutput->stop();
        mIODevice = nullptr;
    }
}

void AudioPlayerForQt::onWriteMoreData() {
    if (!mAudioOutput || !mIODevice || !mWriter) {
        return;
    }

    int nbBytes = mAudioOutput->bytesFree();
    int bufferSize = nbBytes / sizeof(DataFormat);

    if (bufferSize > 0) {
        // the pcm data from the synthesize (non scaled)
        PacketType packet(std::move(mWriter->readPacket(bufferSize)));

        // the pcm data ready for the audio device (scaled)
        std::vector<DataFormat> _buffer;

        _buffer.resize(std::min<int>(bufferSize, packet.size()));

        for (size_t i = 0; i < _buffer.size(); ++i) {
            _buffer[i] = packet[i] * SIGNAL_SCALING;
        }

        mIODevice->write((const char*)_buffer.data(), _buffer.size() * sizeof(DataFormat));
    }
}

