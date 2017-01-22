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


AudioRecorderForQt::AudioRecorderForQt(QObject *parent)
    : AudioInterfaceForQt(QAudio::AudioInput, parent)
    , mAudioInput(nullptr) {
}

AudioRecorderForQt::~AudioRecorderForQt()
{
}


QAudio::Error AudioRecorderForQt::createDevice(const QAudioFormat &format, const QAudioDeviceInfo &info, int bufferSizeMS) {
    Q_UNUSED(bufferSizeMS);

    mAudioInput = new QAudioInput(info, format);
    if (mAudioInput->error() != QAudio::NoError) {
        LogE("Error creating QAudioInput with error %d", mAudioInput->error());
        return mAudioInput->error();
    }

    LogI("Initialized Qt audio recorder using device: %s", getDeviceName().c_str());

    return mAudioInput->error();
}

void AudioRecorderForQt::exit() {
    stop();
    if (mAudioInput)
    {
        mAudioInput->reset();
        delete mAudioInput;
        mAudioInput = nullptr;
    }

    LogI("Qt audio recorder closed.");
}

void AudioRecorderForQt::start() {
    LogI("Start Qt audio input device")
    if (not mAudioInput)
    {
        LogI("Audio input device was not created and thus cannot be started.");
        return;
    }
    if (!mPCMDevice.isOpen()) {
        if (!mPCMDevice.open(QIODevice::WriteOnly)) {
            LogE("Could not open io device");
        } else {
            mAudioInput->start(&mPCMDevice);
            if (mAudioInput->error() != QAudio::NoError)
            {
                qWarning() << "Error opening QAudioOutput with error " << mAudioInput->error();
            }
        }
    }
    if (isSuspended()) {
        mAudioInput->suspend();
    }
}

void AudioRecorderForQt::stop() {
    LogI("Stop Qt audio device");
    if (!mAudioInput) return;
    mAudioInput->stop();
    mPCMDevice.close();
}

void AudioRecorderForQt::suspendChanged(bool v)
{
    if (mAudioInput) {
        if (v) {mAudioInput->suspend();}
        else {mAudioInput->resume();}
    }
}

void AudioRecorderForQt::setGain(double volume) {
    if (mAudioInput) {
        mAudioInput->setVolume(volume);
    }
}

double AudioRecorderForQt::getGain() const {
    if (mAudioInput) {
        return mAudioInput->volume();
    }
    return 1;
}
