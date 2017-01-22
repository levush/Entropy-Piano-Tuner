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

#ifndef AUDIORECORDERFORQT_H
#define AUDIORECORDERFORQT_H

#include <QAudioInput>
#include <QTimer>

#include "prerequisites.h"

#include "audiointerfaceforqt.h"

class AudioRecorderForQt : public AudioInterfaceForQt
{
    Q_OBJECT

public:
    AudioRecorderForQt(QObject *parent);
    virtual ~AudioRecorderForQt();

    void exit() override;

    void start() override;
    void stop() override;

    virtual void suspendChanged(bool v) override final;

    virtual void setGain(double volume) override final;
    virtual double getGain() const override final;

protected:
   virtual QAudio::Error createDevice(const QAudioFormat &format, const QAudioDeviceInfo &info, int bufferSizeMS) override final;

private:
    QAudioInput *mAudioInput;
    QTimer mReadTimer;
};

#endif // AUDIORECORDERFORQT_H
