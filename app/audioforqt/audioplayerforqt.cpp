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

#include "audioplayerforqt.h"

#include <QDebug>

#include "core/system/log.h"

#include "implementations/settingsforqt.h"


//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor, starting a Qt-thread
/// \param parent : Qt object to which the audio player belongs
///////////////////////////////////////////////////////////////////////////////

AudioPlayerForQt::AudioPlayerForQt(QObject *parent) :
    QObject(parent)
{
    setDeviceName(SettingsForQt::getSingleton().getOuputDeviceName().toStdString());
    setSamplingRate(SettingsForQt::getSingleton().getOutputDeviceSamplingRate());
    setBufferSize(SettingsForQt::getSingleton().getAudioPlayerBufferSize());
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
    // (does not work yet) SimpleThreadHandler::setThreadName("AudioPlayer");

    // Format specification:
    QAudioFormat format;
    format.setSampleRate(getSamplingRate());
    format.setChannelCount(getChannelCount());
    format.setCodec("audio/pcm");
    format.setSampleSize(sizeof(DataFormat) * 8);
    format.setSampleType(QAudioFormat::SignedInt);

    // Find the audio device:
    QAudioDeviceInfo device(QAudioDeviceInfo::defaultOutputDevice());
    if (getDeviceName().size() > 0)
    {
        QList<QAudioDeviceInfo> devices(QAudioDeviceInfo::availableDevices(QAudio::AudioOutput));
        for (const QAudioDeviceInfo &i : devices)
        {
            if (i.deviceName().toStdString() == getDeviceName())
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
        if (not device.isFormatSupported(format))
        {
            LogW("Raw audio format not supported by backend, falling back to default supported");
            format = device.preferredFormat();
            // update sampling rate, buffer type has to stay the same!
            if (not device.isFormatSupported(format))
            {
                LogW("Fallback failed. Probably there is no output device available.");
                return;
            }
            setSamplingRate(format.sampleRate());
            if (format.sampleSize() != sizeof(DataFormat) * 8)
            {
                LogW("Sample size of %i not supported", format.sampleSize());
                return;
            }
            if (format.sampleType() != QAudioFormat::SignedInt)
            {
                LogW("Sample format (%i) not supported", format.sampleType());
                return;
            }
        }
    }
    setDeviceName(device.deviceName().toStdString());

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
    const size_t BufferSize = getChannelCount() *
            ((getSamplingRate() * getBufferSize())/1000) * sizeof(DataFormat);
    mAudioSink->setBufferSize(BufferSize);
    if (mAudioSink->error() != QAudio::NoError) {
        LogE("Error opening QAudioOutput with error %d", mAudioSink->error());
        return;
    }

    setWriter(getWriter());
    LogI("Initialized Qt audio player using device: %s", getDeviceName().c_str());
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
    stop();
    if (mAudioSink)
    {
        mAudioSink->reset();
        delete mAudioSink;
        mAudioSink = nullptr;
    }

    LogI("Qt audio player closed.");

    // exit the parent
    AudioPlayerAdapter::exit();
}


void AudioPlayerForQt::start()
{
    LogI("Start Qt audio device")
    if (not mAudioSink)
    {
        LogI("Audio device was not created and thus cannot be started.");
        return;
    }
    if (!mIODevice.isOpen()) {
        mIODevice.setWriter(getWriter());
        if (!mIODevice.open(QIODevice::ReadOnly)) {
            LogE("Could not open io device");
        } else {
            mAudioSink->start(&mIODevice);
            if (mAudioSink->error() != QAudio::NoError)
            {
                qWarning() << "Error opening QAudioOutput with error " << mAudioSink->error();
            }
            if (!getWriter()) {
                mAudioSink->suspend();
            }
        }
    }
}

void AudioPlayerForQt::stop()
{
    LogI("Stop Qt audio device");
    if (not mAudioSink) return;
    mAudioSink->stop();
    mIODevice.close();
}

void AudioPlayerForQt::writerChanged(PCMWriterInterface *writer) {
    mIODevice.setWriter(writer);
    if (mAudioSink) {
        mAudioSink->resume();
    }
}

void AudioPlayerForQt::errorString(QString s)
{
    LogE("Error in QtAudioManager: %s", s.toStdString().c_str());
}
