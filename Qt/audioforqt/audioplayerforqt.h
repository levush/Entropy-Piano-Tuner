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

#ifndef AUDIOPLAYERFORQT_H
#define AUDIOPLAYERFORQT_H

#include "../../core/audio/audioplayeradapter.h"
#include <QAudioOutput>
#include <QTimer>
#include <mutex>

class AudioPlayerForQt : public QObject, public AudioPlayerAdapter
{
    Q_OBJECT

    // define the data format of the input/output stream
    // on android float streams are not supported, so we just use
    // in general a singed int stream
    // the int data will be scaled to the intervall [0,1] to allow an
    // internal computation with floats independent on the selected stream
    // size.
    // note that if you chance the data format to float, then set
    // SIGNAL_SCALING to 1 (commented line below)
    /// Data format of the input/output stream
    typedef int16_t DataFormat;
    /// maximal alloed value of the stream
    static const DataFormat SIGNAL_SCALING;
    //static constexpr DataFormat SIGNAL_SCALING  = 1;
public:
    AudioPlayerForQt(QObject *parent);
    virtual ~AudioPlayerForQt();

    void init() override;
    void exit() override;

    void start() override;
    void stop() override;


    virtual void write(const PacketType &packet) override final;
    virtual void flush() override;
public slots:
    void onWriteMoreData();

private:


    QAudioOutput *mAudioOutput;
    QIODevice *mIODevice;
    int mNotifyIntervall;
    float mBufferSize; /// buffer size of qt in seconds, on slow devices use a bigger value
    PacketType mPacketToWrite;
    std::mutex mPacketMutex;
    QTimer mWriteTimer;
};

#endif // AUDIOPLAYERFORQT_H
