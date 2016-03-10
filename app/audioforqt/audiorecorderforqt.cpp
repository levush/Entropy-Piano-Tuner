/*****************************************************************************
 * Copyright 2016 Haye Hinrichsen, Christoph Wick
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

#include "audiorecorderforqt.h"
#include <qdebug.h>
#include <QTimer>
#include <assert.h>

#include "core/system/log.h"

#include "implementations/settingsforqt.h"
#include "dialogs/donotshowagainmessagebox.h"

const AudioRecorderForQt::DataFormat AudioRecorderForQt::SIGNAL_SCALING = std::numeric_limits<AudioRecorderForQt::DataFormat>::max();
// const AudioRecorderForQt::DataFormat AudioRecorderForQt::SIGNAL_SCALING  = 1;

AudioRecorderForQt::AudioRecorderForQt(QObject *parent)
    : QObject(parent),
      mAudioInput(nullptr),
      mIODevice(nullptr) {

    setDeviceName(SettingsForQt::getSingleton().getInputDeviceName().toStdString());
    setSamplingRate(SettingsForQt::getSingleton().getInputDeviceSamplingRate());
}

AudioRecorderForQt::~AudioRecorderForQt()
{
}


void AudioRecorderForQt::init() {
    QAudioFormat format;
    // Set up the format, eg.
    format.setSampleRate(getSamplingRate());
    format.setChannelCount(getChannelCount());
    format.setCodec("audio/pcm");
    format.setSampleSize(sizeof(DataFormat) * 8);
    format.setSampleType(QAudioFormat::SignedInt);
    //format.setByteOrder(QAudioFormat::LittleEndian);

    QAudioDeviceInfo device(QAudioDeviceInfo::defaultInputDevice());
    if (getDeviceName().size() > 0) {
        QList<QAudioDeviceInfo> devices(QAudioDeviceInfo::availableDevices(QAudio::AudioInput));
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
        // only necessary if default settings
        if (!device.isFormatSupported(format)) {
            LogW("Raw audio format not supported by backend, falling back to nearest supported");
            format = device.nearestFormat(format);
            // update sampling rate, buffer type has to stay the same!
            if (not device.isFormatSupported(format))
            {
                LogW("Fallback failed. Probably there is no input device available. Did you connect your microphone?");
                return;
            }
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

    mAudioInput = new QAudioInput(device, format);
    if (mAudioInput->error() != QAudio::NoError) {
        LogE("Error creating QAudioInput with error %d", mAudioInput->error());
        return;
    }
    mAudioInput->setNotifyInterval(20);

    setDeviceName(device.deviceName().toStdString());


    QObject::connect(&mReadTimer, SIGNAL(timeout()), this, SLOT(onReadPacket()));

    LogI("Initialized Qt audio recorder using device: %s", getDeviceName().c_str());
}

void AudioRecorderForQt::exit() {
    mReadTimer.stop();

    if (mAudioInput) {
        if (mIODevice) {
            mIODevice->close();
            mIODevice = nullptr;
        }
        mAudioInput->reset();
        delete mAudioInput;
        mAudioInput = nullptr;
    }
    LogI("Qt audio recorder closed.");
}

void AudioRecorderForQt::start() {
    if (!mAudioInput) {
        LogI("Audio device not created, cannot start it.");
        return;
    }
    mIODevice = mAudioInput->start();
    if (mAudioInput->error() != QAudio::NoError) {
        qWarning() << "Error starting QAudioInput with error " << mAudioInput->error();
        return;
    }
    mReadTimer.start(mAudioInput->notifyInterval());
}

void AudioRecorderForQt::stop() {
    if (!mAudioInput) {
        return;
    }
    mReadTimer.stop();
    if (mAudioInput) {
        mAudioInput->stop();
        mIODevice = nullptr;
    }
}

void AudioRecorderForQt::setDeviceInputGain(double volume) {
    if (mAudioInput) {
        mAudioInput->setVolume(volume);
    }
}

double AudioRecorderForQt::getDeviceInputGain() const {
    if (mAudioInput) {
        return mAudioInput->volume();
    }
    return 1;
}

void AudioRecorderForQt::onReadPacket() {
    QByteArray rawdata = mIODevice->readAll();
    std::vector<DataFormat> data(rawdata.size() / sizeof(DataFormat));
    memcpy(data.data(), rawdata.data(), data.size() * sizeof(DataFormat));

    PacketType realData(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        realData[i] = data[i] * 1.0 / SIGNAL_SCALING;
    }

    pushRawData(realData);
}
