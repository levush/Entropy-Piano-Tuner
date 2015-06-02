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

//======================================================================
//                               Core
//======================================================================

#include "core.h"
#include "messages/message.h"
#include "messages/messagehandler.h"
#include "messages/messagekeyselectionchanged.h"
#include "messages/messagemidievent.h"
#include "messages/messagemodechanged.h"
#include <assert.h>

Core::Core(ProjectManagerAdapter *projectManager,
           AudioRecorderAdapter *recorderAdapter,
           AudioPlayerAdapter *playerAdapter,
           Log *log)
    : mInitialized(false),
      mProjectManager(projectManager),
      mRecorderAdapter(recorderAdapter),
      mPlayerAdapter(playerAdapter),
      mSignalAnalyzer(recorderAdapter),
      mSoundGenerator (playerAdapter)
{
    EptAssert(log, "A log has to be specified during creation of the core");
    INFORMATION("Core created");
}

Core::~Core()
{
    stop();
    exit();  // just to be sure
}

void Core::init(CoreInitialisationAdapter *initAdapter) {
    EptAssert(initAdapter, "At least the default adapter has to be provided");

    if (mInitialized) {
        return;  // already initialized
    }

    initAdapter->create();
    initAdapter->updateProgress(CoreInitialisationAdapter::CORE_INIT_START, 0);
    mRecorderAdapter->init();
    mPlayerAdapter->init();
    initAdapter->updateProgress(CoreInitialisationAdapter::CORE_INIT_PROGRESS, 25);

    // start the player, so that the startup sound is played
    mPlayerAdapter->start();

    // the signal analyzer
    initAdapter->updateProgress(CoreInitialisationAdapter::CORE_INIT_PROGRESS, 30);
    mSignalAnalyzer.init();

    initAdapter->updateProgress(CoreInitialisationAdapter::CORE_INIT_PROGRESS, 60);
    mSoundGenerator.init();

    initAdapter->updateProgress(CoreInitialisationAdapter::CORE_INIT_MIDI, 75);
    mMidi.init();

    std::stringstream ss;
    ss << "We have " << mMidi.GetNumberOfPorts() << " connected Midi devices:" << std::endl << mMidi.GetPortNames();
    INFORMATION ("%s", ss.str().c_str());
    mMidi.OpenPort(); // Open default midi port

    initAdapter->updateProgress(CoreInitialisationAdapter::CORE_INIT_END, 100);

    MessageHandler::send(Message::MSG_INITIALIZE);  // send a message to initialize various modules in indpendent threads

    mInitialized = true;
    initAdapter->destroy();
}





void Core::exit() {
    if (!mInitialized) {
        return;  // already exited
    }

    stop();

    mMidi.exit();

    mSoundGenerator.exit();
    mPlayerAdapter->exit();
    mRecorderAdapter->exit();
    CalculationManager::getSingleton().stop();

    mInitialized = false;
}

void Core::start() {
    mRecorderAdapter->start();
    mPlayerAdapter->start();
}

void Core::stop() {
    //TunerBase::terminated = true;

    // stop the signal analysis
    mSignalAnalyzer.stop();

    // stop audio
    mRecorderAdapter->stop();
    mPlayerAdapter->stop();
}
