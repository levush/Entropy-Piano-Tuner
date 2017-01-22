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

#ifndef AUDIOPLAYERFORQT_H
#define AUDIOPLAYERFORQT_H

#include <QAudioOutput>

#include "prerequisites.h"

#include "audiointerfaceforqt.h"

class AudioPlayerThreadForQt;

///////////////////////////////////////////////////////////////////////////////
/// \brief The AudioPlayerForQt class
///
/// This class implements the audio player for Qt. Its main purpose is to
/// start an indpendent Qt-compatible thread because the Qt audio player must
/// be operated from a single Qt-thread only.
///////////////////////////////////////////////////////////////////////////////

class AudioPlayerForQt : public AudioInterfaceForQt
{
    Q_OBJECT

public:
    AudioPlayerForQt(QObject *parent);
    ~AudioPlayerForQt() {}


    void exit() override final;

    void start() override final;
    void stop() override final;

    virtual void suspendChanged(bool s) override final;

    virtual void setGain(double gain) override final;
    virtual double getGain() const override final;
protected:
   virtual QAudio::Error createDevice(const QAudioFormat &format, const QAudioDeviceInfo &info, int bufferSizeMS) override final;

private slots:
    void errorString(QString);
    void stateChanged(QAudio::State state);

private:
    QAudioOutput *mAudioSink;           ///< Audio sink to which the data is sent
};

#endif // AUDIOPLAYERFORQT_H
