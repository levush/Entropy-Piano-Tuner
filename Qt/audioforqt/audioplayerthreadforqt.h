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

#ifndef AUDIOPLAYERTHREADFORQT_H
#define AUDIOPLAYERTHREADFORQT_H

#include "audioplayerforqt.h"
#include <QAudioOutput>
#include <mutex>
#include <atomic>
#include <QThread>

///////////////////////////////////////////////////////////////////////////////
/// \brief Class for the audio player thread
///
/// This class serves as a container for the workerFunction in which the
/// thread is running.
///////////////////////////////////////////////////////////////////////////////

class AudioPlayerThreadForQt : public QObject
{
    Q_OBJECT

public:
    typedef int16_t DataFormat;
    AudioPlayerThreadForQt(AudioPlayerForQt *audio);
    ~AudioPlayerThreadForQt() {}

    void registerForTermination() { mTerminateThread = true; }
    void setPause(bool pause);

public slots:
    void workerFunction();

private:
    void init();
    void exit();
    void start();
    void stop();

signals:
    void finished();
    void error(QString err);

private:
    AudioPlayerForQt *mAudioSource;     ///< Audio source where the data comes from
    QAudioOutput *mAudioSink;           ///< Audio sink to which the data is sent
    QIODevice *mIODevice;               ///< Qt IO device pointer
    std::atomic<bool> mTerminateThread; ///< Boolean indicating that the thread shall be terminated
    std::atomic<bool> mPause;           ///< Boolean indicating that the thread is pausing
};


#endif // AUDIOPLAYERTHREADFORQT_H
