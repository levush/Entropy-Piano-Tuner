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

#include <qdebug.h>
#include <iostream>

#include "../core/system/log.h"
#include "settingsforqt.h"
#include "core/audio/rawdatawriter.h"



// Maximal absolute value by which the signal is limited

const AudioPlayerForQt::DataFormat AudioPlayerForQt::SIGNAL_SCALING =
        std::numeric_limits<AudioPlayerForQt::DataFormat>::max();


///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor, setting
/// \param parent : Qt object to which the audio player belongs
///////////////////////////////////////////////////////////////////////////////

AudioPlayerForQt::AudioPlayerForQt(QObject *parent) :
    QObject(parent),
    mThread(nullptr),
    mWorkingInstance(nullptr)
{
    setDeviceName(SettingsForQt::getSingleton().getOuputDeviceName().toStdString());
    setSamplingRate(SettingsForQt::getSingleton().getOutputDeviceSamplingRate());
}


void AudioPlayerForQt::init()
{
    mThread = new QThread;
    mWorkingInstance = new Worker(this);
    mWorkingInstance->moveToThread(mThread);
    connect(mWorkingInstance, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(mThread, SIGNAL(started()), mWorkingInstance, SLOT(process()));
    connect(mWorkingInstance, SIGNAL(finished()), mThread, SLOT(quit()));
    connect(mWorkingInstance, SIGNAL(finished()), mWorkingInstance, SLOT(deleteLater()));
    connect(mThread, SIGNAL(finished()), mThread, SLOT(deleteLater()));
    mThread->start();
}


void AudioPlayerForQt::exit()
{
    mWorkingInstance->registerForTermination();
    while (mWorkingInstance->isRunning()); // Wait
}





///////////////////////////////////////////////////////////////////////////////
/// \brief Start the Qt audio player in an independent thread
///////////////////////////////////////////////////////////////////////////////

void AudioPlayerForQt::start()
{
    LogI("*********************Qt Audio player starting up **********************");
    //mThread = std::thread(&AudioPlayerForQt::workerFunction, this);
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Stop the Qt audio player
///////////////////////////////////////////////////////////////////////////////

void AudioPlayerForQt::stop()
{
    //if (mThread.joinable()) mThread.join(); // Wait for thread to terminate
    LogI("*********************Qt Audio player shutting down**********************")
}




void AudioPlayerForQt::workerFunction ()
{
//    ////////////////////// Dann Schleife


//    ////////////////////// Dann Schleife



//void AudioPlayerForQt::onWriteMoreData() {
//    if (!mAudioOutput || !mIODevice || !mWriter) {
//        return;
//    }

//    int nbBytes = mAudioOutput->bytesFree();
//    int bufferSize = nbBytes / sizeof(DataFormat);

//    if (bufferSize > 0) {
//        // the pcm data from the synthesize (non scaled)
//        PacketType packet(std::move(mWriter->readPacket(bufferSize)));

//        // the pcm data ready for the audio device (scaled)
//        std::vector<DataFormat> _buffer;

//        _buffer.resize(std::min<int>(bufferSize, packet.size()));

//        for (size_t i = 0; i < _buffer.size(); ++i) {
//            _buffer[i] = packet[i] * SIGNAL_SCALING;
//        }

//        mIODevice->write((const char*)_buffer.data(), _buffer.size() * sizeof(DataFormat));
//    }
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////



// --- CONSTRUCTOR ---
Worker::Worker(AudioPlayerForQt *audio) :
    mAudio(audio),
    mRunning(false),
    mAudioOutput(nullptr),
    mIODevice(nullptr),
    mNotifyIntervall(1000)

{
#if __ANDROID__
      mBufferSize *= 2;  // on mobile devices, use a bigger value  ////////////////////////// where defined?
#endif
}




// --- DECONSTRUCTOR ---
Worker::~Worker() {
    // free resources
}



///////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the audio player.
///////////////////////////////////////////////////////////////////////////////

void Worker::init()
{
    // Format specification:
    QAudioFormat format;
    format.setSampleRate(mAudio->getSamplingRate());
    format.setChannelCount(mAudio->getChannelCount());
    format.setCodec("audio/pcm");
    format.setSampleSize(sizeof(AudioPlayerForQt::DataFormat) * 8);
    format.setSampleType(QAudioFormat::SignedInt);
    //format.setByteOrder(QAudioFormat::LittleEndian);

    // Find the audio device:
    QAudioDeviceInfo device(QAudioDeviceInfo::defaultOutputDevice());
    if (mAudio->getDeviceName().size() > 0) {
        QList<QAudioDeviceInfo> devices(QAudioDeviceInfo::availableDevices(QAudio::AudioOutput));
        for (const QAudioDeviceInfo &i : devices) {
            if (i.deviceName().toStdString() == mAudio->getDeviceName()) {
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
            mAudio->setSamplingRate(format.sampleRate());
            if (format.sampleSize() != sizeof(AudioPlayerForQt::DataFormat) * 8) {
                LogW("Sample size not supported");
                return;
            }
            if (format.sampleType() != QAudioFormat::SignedInt) {
                LogW("Sample format not supported");
                return;
            }
        }
    }
    mAudio->setDeviceName(device.deviceName().toStdString());

    // Open the audio output stream
    mAudioOutput = new QAudioOutput(device, format);
    if (mAudioOutput->error() != QAudio::NoError) {
        LogE("Error opening QAudioOutput with error %d", mAudioOutput->error());
        return;
    }


    // Specify the size of the Qt-internal buffer
    // The buffer size is twice of the interval time
    mAudioOutput->setBufferSize(mAudio->getSamplingRate() * mAudio->getChannelCount() * 2 * mNotifyIntervall / 1000.f);
    if (mAudioOutput->error() != QAudio::NoError) {
        LogE("Error opening QAudioOutput with error %d", mAudioOutput->error());
        return;
    }

    LogI("Initialized Qt audio player using device: %s", mAudio->getDeviceName().c_str());
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Exit from the Qt audio player
///////////////////////////////////////////////////////////////////////////////

void Worker::exit()
{
    if (mAudioOutput)
    {
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


void Worker::start()
{
    LogI("*********************Qt Audio player create audio **********************")
    if (!mAudioOutput)
    {
        LogI("Audio device not created, cannot start it.");
        return;
    }
    if (!mIODevice)
    {
        mIODevice = mAudioOutput->start();
        if (mAudioOutput->error() != QAudio::NoError)
        {
            qWarning() << "Error opening QAudioOutput with error " << mAudioOutput->error();
            return;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Stop
///////////////////////////////////////////////////////////////////////////////


void Worker::stop()
{
    LogI("*********************Qt Audio player destroy audio dev **********************")

    if (!mAudioOutput) {
        return;
    }
    if (mIODevice) {
        mAudioOutput->stop();
        mIODevice = nullptr;
    }
}



///////////////////////////////////////////////////////////////////////////////
/// \brief MAIN
///////////////////////////////////////////////////////////////////////////////

void Worker::process()
{
    mRunning=true;
    init();
    start();


    while (mRunning)
    {

        {
            size_t n = mAudioOutput->bytesFree();
            if (n==0) QThread::msleep(1);
            else
            {
                auto buffer = mAudio->getPacket(n);
                size_t m = buffer.size();
                if (m==0) continue;
                QByteArray pcm;
                pcm.resize(m);
                for (int i=0; i<m; ++i) pcm[i]=(char)(buffer[i]*256);
                mIODevice->write(pcm.data(),m);
            }
        }

    }
    stop();
    exit();
    emit finished();
}
