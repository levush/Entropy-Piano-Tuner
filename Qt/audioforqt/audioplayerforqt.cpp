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
#include <qdebug.h>

const AudioPlayerForQt::DataFormat AudioPlayerForQt::SIGNAL_SCALING = std::numeric_limits<AudioPlayerForQt::DataFormat>::max();

AudioPlayerForQt::AudioPlayerForQt(QObject *parent)
    : QObject(parent),
      mAudioOutput(nullptr),
      mIODevice(nullptr),
      mNotifyIntervall(MIN_BUFFER_SIZE_IN_MSECS),
      mBufferSize(MIN_BUFFER_SIZE_IN_MSECS / 1000.f * 5) {
    // mSamplingRate = 48000;
#if __ANDROID__
    // on mobile devices, use a bigger value
    mBufferSize *= 5;
#endif

    setDeviceName(SettingsForQt::getSingleton().getOuputDeviceName().toStdString());
    setSamplingRate(SettingsForQt::getSingleton().getOutputDeviceSamplingRate());
}

AudioPlayerForQt::~AudioPlayerForQt()
{
}

void AudioPlayerForQt::init() {
    std::lock_guard<std::mutex> lock(mPacketMutex);

    QAudioFormat format;
    // Set up the format, eg.
    format.setSampleRate(getSamplingRate());
    format.setChannelCount(getChannelCount());
    format.setCodec("audio/pcm");
    format.setSampleSize(sizeof(DataFormat) * 8);
    format.setSampleType(QAudioFormat::SignedInt);
    //format.setByteOrder(QAudioFormat::LittleEndian);


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
            ERROR("Selected device settings are not supported!");
        }
    }
    else {
        if (!device.isFormatSupported(format)) {
            WARNING("Raw audio format not supported by backend, falling back to default supported");
            format = device.preferredFormat();
            // update sampling rate, buffer type has to stay the same!
            setSamplingRate(format.sampleRate());
            if (format.sampleSize() != sizeof(DataFormat) * 8) {
                WARNING("Sample size not supported");
                return;
            }
            if (format.sampleType() != QAudioFormat::SignedInt) {
                WARNING("Sample format not supported");
                return;
            }
        }
    }

    mAudioOutput = new QAudioOutput(device, format);
    if (mAudioOutput->error() != QAudio::NoError) {
        ERROR("Error opening QAudioOutput with error %d", mAudioOutput->error());
        return;
    }


    //mAudioOutput->setNotifyInterval(0);
    mAudioOutput->setBufferSize(getSamplingRate() * mBufferSize * sizeof(DataFormat) * getChannelCount());  // 0.01 seconds buffer size
    //mAudioOutput->setBufferSize(mAudioOutput->periodSize() * 10);
    //mAudioOutput->setBufferSize(getSamplingRate() * sizeof(DataFormat) * 2);


    setDeviceName(device.deviceName().toStdString());

    //QObject::connect(mAudioOutput, SIGNAL(notify()), this, SLOT(onWriteMoreData()));
    if (mAudioOutput->error() != QAudio::NoError) {
        ERROR("Error opening QAudioOutput with error %d", mAudioOutput->error());
        return;
    }


    // time have to be started from the main thread
    QObject::connect(&mWriteTimer, SIGNAL(timeout()), this, SLOT(onWriteMoreData()));

    INFORMATION("Initialized Qt audio player using device: %s", getDeviceName().c_str());
}

void AudioPlayerForQt::exit() {
    if (mAudioOutput) {
        mWriteTimer.stop();
        mAudioOutput->reset();
        delete mAudioOutput;
        mAudioOutput = nullptr;
        mIODevice = nullptr;
    }
    INFORMATION("Qt audio player closed.");
}

void AudioPlayerForQt::start() {
    if (!mAudioOutput) {
        INFORMATION("Audio device not created, cannot start it.");
        return;
    }
    if (!mIODevice) {
        std::lock_guard<std::mutex> lock(mPacketMutex);

        mIODevice = mAudioOutput->start();
        addFreeBufferSize(mAudioOutput->bytesFree() / sizeof(DataFormat));
        if (mAudioOutput->error() != QAudio::NoError) {
            qWarning() << "Error opening QAudioOutput with error " << mAudioOutput->error();
            return;
        }
        if (!mWriteTimer.isActive()) {
            mWriteTimer.start(mNotifyIntervall);
        }
    }
}

void AudioPlayerForQt::stop() {
    if (!mAudioOutput) {
        return;
    }
    if (mIODevice) {
        std::lock_guard<std::mutex> lock(mPacketMutex);
        mAudioOutput->stop();
        mIODevice = nullptr;
    }
    setFreeBufferSize(0);
}


void AudioPlayerForQt::write(const PacketType &packet) {
    // less buffer size is available
    shrinkFreeBufferSize(packet.size());

    // copy the data
    mPacketMutex.lock();
    mPacketToWrite.reserve(mPacketToWrite.size() + packet.size());
    std::move(packet.begin(), packet.end(), std::back_inserter(mPacketToWrite));
    mPacketMutex.unlock();

    // wait for free buffer size
    // TODO terminated flag inactivated, not clear what happens here
//    while (getFreeBufferSize() == 0 && !TunerBase::terminated) {
        /*while (getFreeBufferSize() == 0 ) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }*/
}

void AudioPlayerForQt::flush() {

}

void AudioPlayerForQt::onWriteMoreData() {
    std::lock_guard<std::mutex> lock(mPacketMutex);
    if (!mAudioOutput || !mIODevice) {
        return;
    }

    if (mAudioOutput->state() == QAudio::SuspendedState && mPacketToWrite.size() > 0) {
        // resume the output
        mAudioOutput->resume();
    }

    int nbBytes = mAudioOutput->bytesFree();
    int bufferSize = nbBytes / sizeof(DataFormat);

    if (bufferSize > 0) {
        std::vector<DataFormat> _buffer;

        _buffer.resize(std::min<int>(bufferSize, mPacketToWrite.size()));
        if (_buffer.size() == 0) {
            // no more data available, suspend the output
            mAudioOutput->suspend();
            return;
        }

        for (size_t i = 0; i < _buffer.size(); ++i) {
            _buffer[i] = mPacketToWrite[i] * SIGNAL_SCALING;
        }

        if (mPacketToWrite.size() == _buffer.size()) {
            // all data was written
            mPacketToWrite.clear();
        } else {
            // still data available, crop it
            int remainingSize = mPacketToWrite.size() - _buffer.size();
            int offset = _buffer.size();
            for (size_t i = 0; i < (size_t)remainingSize; ++i) {
                mPacketToWrite[i] = mPacketToWrite[i + offset];
            }
            mPacketToWrite.resize(remainingSize);
        }

        mIODevice->write((const char*)_buffer.data(), _buffer.size() * sizeof(DataFormat));
        addFreeBufferSize(_buffer.size());
    }
}

