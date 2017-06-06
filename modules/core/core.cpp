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

//============================================================================
//                                  Core
//============================================================================

#include "core.h"
#include "calculation/calculationmanager.h"
#include "messages/message.h"
#include "messages/messagehandler.h"
#include "messages/messagekeyselectionchanged.h"
#include "messages/messagemidievent.h"
#include "messages/messagemodechanged.h"
#include "system/platformtoolscore.h"

//-----------------------------------------------------------------------------
//                              Core constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Core constructor
/// \param projectManager : Pointer to the project manager
/// \param recorderAdapter : Pointer to the recorder adapter
/// \param playerAdapter : Pointer to the player adapter
/// \param log : Pointer to the system log
///////////////////////////////////////////////////////////////////////////////

Core::Core(ProjectManagerAdapter *projectManager,
           AudioInterface *recorderInterface,
           AudioInterface *playerInterface)
    : mInitialized(false),                      // Initially not initialized
      mProjectManager(projectManager),
      mRecorderInterface(recorderInterface),
      mPlayerInterface(playerInterface),
      mRecordingManager (&mAudioRecoder),
      mSignalAnalyzer(&mAudioRecoder)
{
    new PianoManager;
    mMidi = std::make_shared<MidiAdapter>();

    // load algorithm plugins
#ifdef EPT_ALL_STATIC
#else
    CalculationManager::getSingleton().loadAlgorithms();
#endif

    LogI("Core created");
}


//-----------------------------------------------------------------------------
//                              Destructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Core destructor, calls stop and exit.
///////////////////////////////////////////////////////////////////////////////

Core::~Core()
{
    stop();
    exit();  // just to be sure
    // destroy piano manager (here, because it inherits MessageListener)
    PianoManager::getSingletonPtr().reset();
    LogI("PianoManager destroyed");
    CalculationManager::selfDelete();
    LogI("Core destroyed");
}

void Core::setEnableSoundGenerator(bool enable)
{
    mEnableSoundGenerator = enable;
    if (!enable) {
        mSignalAnalyzer.stop();
        if (mSoundGenerator) {
            mSoundGenerator->exit();
        }
    }
}


//-----------------------------------------------------------------------------
//                      Core initialization procedure
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Core initialization
///
/// On startup the core components have to be initialized by calling this
/// function. Since the user has should see that something happens, a
/// message box with a progress bar will be shown. The handling of this
/// message box is carried out by the initialization adapter passed here
/// as a pointer.
/// \param initAdapter : Pointer to the initialization adapter
///////////////////////////////////////////////////////////////////////////////

void Core::init(CoreInitialisationAdapter *initAdapter)
{
    EptAssert(initAdapter, "At least the default adapter has to be provided");

    if (mInitialized) return;           // If already initialized return

    initAdapter->create();              // Open initialization message box

    initAdapter->updateProgress (0);    // empty

    initAdapter->updateProgress (11);   // Initialize recorder
    mRecorderInterface->init();
    mRecorderInterface->setDevice(&mAudioRecoder);

    initAdapter->updateProgress (22);   // Initialize player
    mPlayerInterface->init();

    initAdapter->updateProgress (33);   // Start the player, so that the startup sound is played
    mPlayerInterface->start();

    initAdapter->updateProgress (44);   // Initialize the signal analyzer
    mSignalAnalyzer.init();

    initAdapter->updateProgress (55);   // Initialize the sound generator
    if (mEnableSoundGenerator) {
        mSoundGenerator.reset(new SoundGenerator(mPlayerInterface));
        mSoundGenerator->init();
    } else {
        LogI("SoundGenerator is disabled. Possibly low physical memory!");
    }

    initAdapter->updateProgress (65);   // Initialize the recording manager
    mRecordingManager.init();

    initAdapter->updateProgress (75);   // Initialze the MIDI system

    initAdapter->updateProgress (87);   // Open the default MIDI port

    initAdapter->updateProgress (100);

    mInitialized = true;                // set initialization flag and
    initAdapter->destroy();             // remove the init message box
}



//-----------------------------------------------------------------------------
//                                  Exit
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Thus function calls exit() of all components that were initialized.
///////////////////////////////////////////////////////////////////////////////

void Core::exit()
{
    if (not mInitialized) return;
    stop();

    mRecordingManager.exit();
    if (mSoundGenerator) {mSoundGenerator->exit();}
    mSignalAnalyzer.exit();
    mPlayerInterface->exit();
    mRecorderInterface->exit();
    CalculationManager::getSingleton().stop();

    mInitialized = false;
}


//-----------------------------------------------------------------------------
//                              Start the core
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Start the core
///
/// Start all members which are running while the core is up.
///////////////////////////////////////////////////////////////////////////////

void Core::start()
{
    LogI("Starting the core");
    mRecorderInterface->start();
    mPlayerInterface->start();
}



//-----------------------------------------------------------------------------
//                               Stop the core
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Stop the core
///
/// Stop core components when stopping the core.
///////////////////////////////////////////////////////////////////////////////

void Core::stop()
{
    mSignalAnalyzer.stop();
    mRecorderInterface->stop();
    mPlayerInterface->stop();
}
