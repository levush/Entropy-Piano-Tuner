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

#include "prerequisites.h"

#include "audio/audiointerface.h"
#include "audio/player/soundgenerator.h"
#include "audio/recorder/audiorecorder.h"
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

class EPT_EXTERN Core
{
    // Note msvc: export (EPT_EXTERN) only required fields
public:
    Core(ProjectManagerAdapter *projectManager,
         AudioInterface *recorderInterface,
         AudioInterface *playerInterface
         );
    ~Core();

    void setEnableSoundGenerator(bool enable);


    void init (CoreInitialisationAdapter *initAdapter);
    void exit();
    void start();
    void stop();

    /// Function telling wether the core is already initialized.
    bool isInitialized() const {return mInitialized;}

    // Getter functions
    ProjectManagerAdapter *getProjectManager()  {return mProjectManager.get();}
    AudioInterface *getAudioInput()             {return mRecorderInterface;}
    AudioInterface *getAudioPlayer()            {return mPlayerInterface;}
    AudioRecorder *getAudioRecorder()           {return &mAudioRecoder;}
    SoundGenerator *getSoundGenerator()         {return mSoundGenerator.get();}
    PianoManager *getPianoManager()             {return PianoManager::getSingletonPtr().get();}
    MidiAdapter *getMidiInterface()             {return mMidi.get();}

private:
    bool mInitialized;

    bool mEnableSoundGenerator = true;

    // modules
    std::unique_ptr<ProjectManagerAdapter> mProjectManager;
    AudioInterface *mRecorderInterface;
    AudioInterface *mPlayerInterface;
    AudioRecorder mAudioRecoder;
    std::unique_ptr<SoundGenerator> mSoundGenerator;
    RecordingManager mRecordingManager;
    SignalAnalyzer mSignalAnalyzer;
    MidiAdapterPtr mMidi;
};

#endif // CORE_H
