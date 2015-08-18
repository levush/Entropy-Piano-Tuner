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
#include "../core/system/simplethreadhandler.h"
#include "settingsforqt.h"

const double QtAudioManager::BufferMilliseconds = 50;

//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor, starting a Qt-thread
/// \param parent : Qt object to which the audio player belongs
///////////////////////////////////////////////////////////////////////////////

AudioPlayerForQt::AudioPlayerForQt(QObject *parent) :
    QObject(parent),
    mQtThread(nullptr),
    mQtAudioManager(nullptr)
{
    setDeviceName(SettingsForQt::getSingleton().getOuputDeviceName().toStdString());
    setSamplingRate(SettingsForQt::getSingleton().getOutputDeviceSamplingRate());
}


//-----------------------------------------------------------------------------
//                    Initialize the Qt-Player thread
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the AudioPlayerForQt
///
/// This function starts an indpendent Qt-compatible thread in which the
/// audio device is instantiated and started.
///////////////////////////////////////////////////////////////////////////////

void AudioPlayerForQt::init()
{
    mQtThread = new QThread;
    mQtAudioManager = new QtAudioManager(this);
    mQtAudioManager->moveToThread(mQtThread);
    connect(mQtAudioManager, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(mQtThread, SIGNAL(started()), mQtAudioManager, SLOT(workerFunction()));
    connect(mQtAudioManager, SIGNAL(finished()), mQtThread, SLOT(quit()));
    connect(mQtAudioManager, SIGNAL(finished()), mQtAudioManager, SLOT(deleteLater()));
    connect(mQtThread, SIGNAL(finished()), mQtThread, SLOT(deleteLater()));

    mQtThread->start(QThread::HighPriority);
}


//-----------------------------------------------------------------------------
//                    Exit from the Qt-Player thread
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief  Exit from the AudioPlayerForQt
///
/// Register the execution thread of the audio player for termination.
/// Wait in idle state until the thread has terminated.
///////////////////////////////////////////////////////////////////////////////

void AudioPlayerForQt::exit()
{
    mQtAudioManager->registerForTermination();
    while (mQtAudioManager->isRunning())
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // deleted automatically upon finished
    mQtAudioManager = nullptr;
    mQtThread = nullptr;
}


void AudioPlayerForQt::start()
{
    if (!mQtAudioManager) {return;}

    mQtAudioManager->setPause(false);
}

void AudioPlayerForQt::stop()
{
    if (!mQtAudioManager) {return;}

    mQtAudioManager->setPause(true);
}

void AudioPlayerForQt::errorString(QString s)
{
    LogE("Error in QtAudioManager: %s", s.toStdString().c_str());
}

//=============================================================================
//                            CLASS QtAudioManager
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor
/// \param audio : Pointer to the AudioPlayerForQt which created the instance.
///////////////////////////////////////////////////////////////////////////////

QtAudioManager::QtAudioManager(AudioPlayerForQt *audio) :
    mAudioSource(audio),
    mThreadRunning(true),
    mPause(false),
    mAudioSink(nullptr),
    mIODevice(nullptr)
{}


//-----------------------------------------------------------------------------
//                              Initialization
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the audio player.
///
/// This function defines the format, looks for the available audio device,
/// manages fallbacks to supported formats, and defines the size of the
/// internal Qt buffer.
///////////////////////////////////////////////////////////////////////////////

void QtAudioManager::init()
{
    SimpleThreadHandler::setThreadName("AudioPlayer");

    // Format specification:
    QAudioFormat format;
    format.setSampleRate(mAudioSource->getSamplingRate());
    format.setChannelCount(mAudioSource->getChannelCount());
    format.setCodec("audio/pcm");
    format.setSampleSize(sizeof(DataFormat) * 8);
    format.setSampleType(QAudioFormat::SignedInt);
    //format.setByteOrder(QAudioFormat::LittleEndian);

    // Find the audio device:
    QAudioDeviceInfo device(QAudioDeviceInfo::defaultOutputDevice());
    if (mAudioSource->getDeviceName().size() > 0)
    {
        QList<QAudioDeviceInfo> devices(QAudioDeviceInfo::availableDevices(QAudio::AudioOutput));
        for (const QAudioDeviceInfo &i : devices)
        {
            if (i.deviceName().toStdString() == mAudioSource->getDeviceName())
            {
                device = i;
                break;
            }

        if (!device.isFormatSupported(format))
            LogE("Selected device settings are not supported!");
        }
    }
    else
    {
        if (!device.isFormatSupported(format))
        {
            LogW("Raw audio format not supported by backend, falling back to default supported");
            format = device.preferredFormat();
            // update sampling rate, buffer type has to stay the same!
            mAudioSource->setSamplingRate(format.sampleRate());
            if (format.sampleSize() != sizeof(DataFormat) * 8)
            {
                LogW("Sample size not supported");
                return;
            }
            if (format.sampleType() != QAudioFormat::SignedInt)
            {
                LogW("Sample format not supported");
                return;
            }
        }
    }
    mAudioSource->setDeviceName(device.deviceName().toStdString());

    // Open the audio output stream
    mAudioSink = new QAudioOutput(device, format);
    if (mAudioSink->error() != QAudio::NoError)
    {
        LogE("Error opening QAudioOutput with error %d", mAudioSink->error());
        return;
    }

    // set volume
    mAudioSink->setVolume(1);


    // Specify the size of the Qt-internal buffer
    const size_t BufferSize = mAudioSource->getChannelCount() *
            ((mAudioSource->getSamplingRate() * BufferMilliseconds)/1000);
    mAudioSink->setBufferSize(BufferSize);
    if (mAudioSink->error() != QAudio::NoError) {
        LogE("Error opening QAudioOutput with error %d", mAudioSink->error());
        return;
    }

    if (mAudioSource->getWriter()) {
        mAudioSource->getWriter()->init(mAudioSource->getSamplingRate(), mAudioSource->getChannelCount());
    }

    LogI("Initialized Qt audio player using device: %s", mAudioSource->getDeviceName().c_str());
}


//-----------------------------------------------------------------------------
//                                  Exit
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Exit from the Qt audio player
///////////////////////////////////////////////////////////////////////////////

void QtAudioManager::exit()
{
    if (mAudioSink)
    {
        mAudioSink->reset();
        delete mAudioSink;
        mAudioSink = nullptr;
        mIODevice = nullptr;
    }

    if (mAudioSource && mAudioSource->getWriter()) {
        mAudioSource->getWriter()->exit();
    }
    LogI("Qt audio player closed.");
}


//-----------------------------------------------------------------------------
//                            Start audio device
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Start the Qt audio device
///////////////////////////////////////////////////////////////////////////////

void QtAudioManager::start()
{
    LogI("Start Qt audio device")
    if (!mAudioSink)
    {
        LogI("Audio device not created, cannot start it.");
        return;
    }
    if (!mIODevice)
    {
        mIODevice = mAudioSink->start();
        if (mAudioSink->error() != QAudio::NoError)
        {
            qWarning() << "Error opening QAudioOutput with error " << mAudioSink->error();
            return;
        }
    }
}


//-----------------------------------------------------------------------------
//                            Stop audio device
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Stop the Qt audio device
///////////////////////////////////////////////////////////////////////////////

void QtAudioManager::stop()
{
    LogI("Stop Qt audio device");
    if (!mAudioSink) return;
    if (mIODevice)
    {
        mAudioSink->stop();
        mIODevice = nullptr;
    }
}

///
/// \brief Pause the audio player
/// \param pause Pause
///
void QtAudioManager::setPause(bool pause) {
    mPause = pause;
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Main worker function of the Qt audio manager
///////////////////////////////////////////////////////////////////////////////

void QtAudioManager::workerFunction()
{
    // Determine the scaling constant
    const auto scaling = std::numeric_limits<QtAudioManager::DataFormat>::max();

    init();
    start();
    if (!mAudioSink) {
        // error while creating audio device
        return;
    }

    mAudioSink->suspend();

    while (mThreadRunning)
    {
        if (mPause or not mAudioSource->getWriter() or mAudioSource->isMuted()) {
            QThread::msleep(500);
            continue;
        }

        AudioBase::PacketType packet(mAudioSink->bytesFree() / sizeof(DataFormat));
        const bool requestPause = !mAudioSource->getWriter()->generateAudioSignal(packet);

        if (requestPause) {
            if (mAudioSink->state() != QAudio::SuspendedState) {
                mAudioSink->suspend();
            }
            QThread::msleep(10);
            continue;
        } else {
            if (mAudioSink->state() == QAudio::SuspendedState) {
                mAudioSink->resume();
            }
            if (packet.size() == 0) {QThread::msleep(5); continue;}

            std::vector<DataFormat> buffer(packet.size());
            for (size_t i=0; i<buffer.size(); ++i) buffer[i] = static_cast<DataFormat>(packet[i] * scaling);
            mIODevice->write((const char*)buffer.data(), buffer.size() * sizeof(DataFormat));
        }
    }
    exit();
    emit finished();
}
