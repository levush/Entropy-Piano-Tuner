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
#include <mutex>
#include <QThread>

class QtAudioManager;

///////////////////////////////////////////////////////////////////////////////
/// \brief The AudioPlayerForQt class
///
/// This class implements the audio player for Qt. Its main purpose is to
/// start an indpendent Qt-compatible thread because the Qt audio player must
/// be operated from a single Qt-thread only.
///////////////////////////////////////////////////////////////////////////////

class AudioPlayerForQt : public QObject, public AudioPlayerAdapter
{
    Q_OBJECT

public:
    AudioPlayerForQt(QObject *parent);
    virtual ~AudioPlayerForQt() {}

    void init() override final;     // Initialize, start thread
    void exit() override final;     // Exit, stop thread

    // No functionality for start and stop, the player starts and stops automatically.
    void start() override final {}
    void stop() override final {}

private:
    QThread* mQtThread;
    QtAudioManager* mQtAudioManager;
};


///////////////////////////////////////////////////////////////////////////////
/// \brief The QtAudioManager class
///
/// This class serves as a container for the workerFunction in which the
/// thread is running
///////////////////////////////////////////////////////////////////////////////

class QtAudioManager : public QObject
{
    Q_OBJECT

public:
    typedef int16_t DataFormat;
    QtAudioManager(AudioPlayerForQt *audio);
    ~QtAudioManager() {}

public slots:
    void workerFunction();

signals:
    void finished();
    void error(QString err);

private:
    AudioPlayerForQt *mAudioSource;
    bool mThreadRunning;
    bool mDeviceActive;
    QAudioOutput *mAudioSink;
    QIODevice *mIODevice;


private:
    void init();
    void exit();
    void start();
    void stop();

public:
    void registerForTermination() { mThreadRunning=false; }
    bool isRunning () { return mThreadRunning; }

    // add your variables here
};



#endif // AUDIOPLAYERFORQT_H
