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

#include "core/audio/recorder/audiorecorderadapter.h"

class AudioRecorderForQt : public QObject, public AudioRecorderAdapter
{
    Q_OBJECT


    // define the data format of the input/output stream
    // on android float streams are not supported, so we just use
    // in general a singed int stream
    // the int data will be scaled to the intervall [0,1] to allow an
    // internal computation with floats independent on the selected stream
    // size.
    /// Data format of the input/output stream
    typedef int16_t DataFormat;
    /// maximal alloed value of the stream
    static const DataFormat SIGNAL_SCALING;
public:
    AudioRecorderForQt(QObject *parent);
    virtual ~AudioRecorderForQt();

    void init() override;
    void exit() override;

    void start() override;
    void stop() override;

    virtual void setDeviceInputGain(double volume) override final;
    virtual double getDeviceInputGain() const override final;

public slots:
    void onReadPacket();

private:
    QAudioInput *mAudioInput;
    QIODevice *mIODevice;
    QTimer mReadTimer;
};

#endif // AUDIORECORDERFORQT_H
