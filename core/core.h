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

#ifndef CORE_H
#define CORE_H

#include <thread>
#include "analyzers/signalanalyzer.h"
#include "calculation/calculationmanager.h"
#include "audio/audiorecorderadapter.h"
#include "audio/soundgenerator.h"
#include "messages/messagelistener.h"
#include "system/log.h"
#include "adapters/coreinitialisationadapter.h"
#include "adapters/projectmanageradapter.h"
#include "piano/pianomanager.h"

#include "midi/midiadapter.h"

////////////////////////////////////////////////////////////////////////
/// \brief class for initialising the core
///
////////////////////////////////////////////////////////////////////////

class Core
{
public:
    Core(ProjectManagerAdapter *projectManager,
         AudioRecorderAdapter *recorderAdapter,
         AudioPlayerAdapter *playerAdapter,
         Log *log = new Log());
    ~Core();


    void init(CoreInitialisationAdapter *initAdapter);
    void exit();


    void start();                           ///< function to start the threads
    void stop();                            ///< function to stop all threads

    bool isInitialized() const {return mInitialized;}

    ProjectManagerAdapter *getProjectManager() {return mProjectManager.get();}
    AudioRecorderAdapter *getAudioRecorder() {return mRecorderAdapter;}
    AudioPlayerAdapter *getAudioPlayer() {return mPlayerAdapter;}
    //SignalAnalyzer *getSingalAnalyzer() {return &mSignalAnalyzer;}
    MidiAdapter *getMidiInterface() {return mMidi.get();}
    PianoManager *getPianoManager() {return &mPianoManager;}

private:
    bool mInitialized;
    std::unique_ptr<ProjectManagerAdapter> mProjectManager;
    AudioRecorderAdapter *mRecorderAdapter;
    AudioPlayerAdapter *mPlayerAdapter;
    SignalAnalyzer mSignalAnalyzer;
    SoundGenerator mSoundGenerator;
    std::shared_ptr<MidiAdapter> mMidi;

    PianoManager mPianoManager;

};

#endif // CORE_H
