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

//=============================================================================
//                                   Core
//=============================================================================

#ifndef CORE_H
#define CORE_H

#include "audio/player/soundgenerator.h"
#include "audio/recorder/audiorecorderadapter.h"
#include "audio/recorder/recordingmanager.h"
#include "audio/midi/midiadapter.h"
#include "analyzers/signalanalyzer.h"
#include "calculation/calculationmanager.h"
#include "adapters/coreinitialisationadapter.h"
#include "adapters/projectmanageradapter.h"
#include "piano/pianomanager.h"
#include "system/log.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief CORE : Class managing the core
///
/// The core comprises all components of the EPT which are independent of the
/// GUI. The GUI is connected by impementing a number of virtual adapters.
///////////////////////////////////////////////////////////////////////////////

class Core
{
public:
    Core(ProjectManagerAdapter *projectManager,
         AudioRecorderAdapter *recorderAdapter,
         AudioPlayerAdapter *playerAdapter,
         Log *log = new Log());
    ~Core();


    void init (CoreInitialisationAdapter *initAdapter);
    void exit();
    void start();
    void stop();

    /// Function telling wether the core is already initialized.
    bool isInitialized() const {return mInitialized;}

    // Getter functions
    ProjectManagerAdapter *getProjectManager()  {return mProjectManager.get();}
    AudioRecorderAdapter *getAudioRecorder()    {return mRecorderAdapter;}
    AudioPlayerAdapter *getAudioPlayer()        {return mPlayerAdapter;}
    SoundGenerator &getSoundGenerator()         {return mSoundGenerator;}
    PianoManager *getPianoManager()             {return PianoManager::getSingletonPtr().get();}
    MidiAdapter *getMidiInterface()             {return mMidi.get();}

private:
    bool mInitialized;
    std::unique_ptr<ProjectManagerAdapter> mProjectManager;
    AudioRecorderAdapter *mRecorderAdapter;
    AudioPlayerAdapter *mPlayerAdapter;
    SoundGenerator mSoundGenerator;
    RecordingManager mRecordingManager;
    SignalAnalyzer mSignalAnalyzer;
    std::shared_ptr<MidiAdapter> mMidi;
};

#endif // CORE_H
