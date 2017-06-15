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

AudioPlayerForQt::AudioPlayerForQt(QObject *parent)
    : AudioInterfaceForQt(QAudio::AudioOutput, parent)
    , mAudioSink(nullptr)
{
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

QAudio::Error AudioPlayerForQt::createDevice(const QAudioFormat &format, const QAudioDeviceInfo &info, int bufferSizeMS)
{
    // Open the audio output stream
    mAudioSink = new QAudioOutput(info, format);
    QObject::connect(mAudioSink, SIGNAL(stateChanged(QAudio::State)), this, SLOT(stateChanged(QAudio::State)));
    if (mAudioSink->error() != QAudio::NoError)
    {
        LogE("Error opening QAudioOutput with error %d", mAudioSink->error());
        return mAudioSink->error();
    }

    // set volume
    mAudioSink->setVolume(1);


    // Specify the size of the Qt-internal buffer
    const int bufferSize = format.bytesForDuration(bufferSizeMS * 1000);
    mAudioSink->setBufferSize(bufferSize);
    if (mAudioSink->error() != QAudio::NoError) {
        LogE("Error opening QAudioOutput with error %d", mAudioSink->error());
        return mAudioSink->error();
    }

    return mAudioSink->error();
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
}


void AudioPlayerForQt::start()
{
    LogI("Start Qt audio device")
    if (not mAudioSink)
    {
        LogI("Audio device was not created and thus cannot be started.");
        return;
    }
    if (!mPCMDevice.isOpen()) {
        if (!mPCMDevice.open(QIODevice::ReadOnly)) {
            LogE("Could not open io device");
        } else {
            mAudioSink->start(&mPCMDevice);
            if (mAudioSink->error() != QAudio::NoError)
            {
                qWarning() << "Error opening QAudioOutput with error " << mAudioSink->error();
            }
        }
    }
    if (isSuspended()) {
        mAudioSink->suspend();
    }
}

void AudioPlayerForQt::stop()
{
    LogI("Stop Qt audio device");
    if (not mAudioSink) return;
    mAudioSink->stop();
    mPCMDevice.close();
}

void AudioPlayerForQt::suspendChanged(bool v)
{
    if (mAudioSink) {
        if (v) {mAudioSink->suspend();}
        else {mAudioSink->resume();}
    }
}

void AudioPlayerForQt::setGain(double gain)
{
    if (mAudioSink) {mAudioSink->setVolume(gain);}
}

double AudioPlayerForQt::getGain() const
{
    if (mAudioSink) {return mAudioSink->volume();}
    return 1;
}

void AudioPlayerForQt::errorString(QString s)
{
    LogE("Error in QtAudioManager: %s", s.toStdString().c_str());
}

void AudioPlayerForQt::stateChanged(QAudio::State state)
{
    qDebug() << "Audio player state changed: " << state;
}
