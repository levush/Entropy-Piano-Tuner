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

template class EPT_EXTERN std::unique_ptr<ProjectManagerAdapter>;
template class EPT_EXTERN std::unique_ptr<SoundGenerator>;
template class EPT_EXTERN std::unique_ptr<SignalAnalyzer>;

///////////////////////////////////////////////////////////////////////////////
/// \brief CORE : Class managing the core
///
/// The core comprises all components of the EPT which are independent of the
/// GUI. The GUI is connected by impementing a number of virtual adapters.
///////////////////////////////////////////////////////////////////////////////

class Core
{
    // Note msvc: export (EPT_EXTERN) only required fields
public:
    EPT_EXTERN Core(ProjectManagerAdapter *projectManager,
         AudioRecorderAdapter *recorderAdapter,
         AudioPlayerAdapter *playerAdapter,
         Log *log = new Log());
    EPT_EXTERN ~Core();

    EPT_EXTERN void setEnableSoundGenerator(bool enable);


    EPT_EXTERN void init (CoreInitialisationAdapter *initAdapter);
    EPT_EXTERN void exit();
    EPT_EXTERN void start();
    EPT_EXTERN void stop();

    /// Function telling wether the core is already initialized.
    EPT_EXTERN bool isInitialized() const {return mInitialized;}

    // Getter functions
    EPT_EXTERN ProjectManagerAdapter *getProjectManager()  {return mProjectManager.get();}
    EPT_EXTERN AudioRecorderAdapter *getAudioRecorder()    {return mRecorderAdapter;}
    EPT_EXTERN AudioPlayerAdapter *getAudioPlayer()        {return mPlayerAdapter;}
    EPT_EXTERN SoundGenerator *getSoundGenerator()         {return mSoundGenerator.get();}
    EPT_EXTERN PianoManager *getPianoManager()             {return PianoManager::getSingletonPtr().get();}
    EPT_EXTERN MidiAdapter *getMidiInterface()             {return mMidi.get();}

private:
    bool mInitialized;

    bool mEnableSoundGenerator = true;

    // modules
    std::unique_ptr<ProjectManagerAdapter> mProjectManager;
    AudioRecorderAdapter *mRecorderAdapter;
    AudioPlayerAdapter *mPlayerAdapter;
    std::unique_ptr<SoundGenerator> mSoundGenerator;
    RecordingManager mRecordingManager;
    SignalAnalyzer mSignalAnalyzer;
    MidiAdapterPtr mMidi;
};

#endif // CORE_H
