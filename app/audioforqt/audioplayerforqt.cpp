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

#include "core/system/log.h"

#include "audioplayerthreadforqt.h"
#include "implementations/settingsforqt.h"

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
    mQtAudioManager = new AudioPlayerThreadForQt(this);
    mQtAudioManager->moveToThread(mQtThread);
    connect(mQtAudioManager, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(mQtAudioManager, SIGNAL(finished()), mQtThread, SLOT(deleteLater()));
    connect(mQtThread, SIGNAL(started()), mQtAudioManager, SLOT(workerFunction()));
    connect(mQtAudioManager, SIGNAL(finished()), mQtThread, SLOT(quit()));
    connect(mQtAudioManager, SIGNAL(finished()), mQtAudioManager, SLOT(deleteLater()));

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
    // quit and wait for thread termination
    mQtAudioManager->registerForTermination();
    mQtThread->quit();
    mQtThread->wait();

    // objects are deleted automatically upon finished, so just set pointers to nullptr
    mQtAudioManager = nullptr;
    mQtThread = nullptr;

    // exit the parent
    AudioPlayerAdapter::exit();
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
