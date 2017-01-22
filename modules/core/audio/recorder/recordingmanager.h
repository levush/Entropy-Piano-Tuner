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
//                           Recording manager
//=============================================================================

#ifndef RECORDINGMANAGER_H
#define RECORDINGMANAGER_H

#include "prerequisites.h"
#include "../../messages/messagelistener.h"

class Piano;
class Key;
class AudioRecorder;
class Stroboscope;

////////////////////////////////////////////////////////////////////////////////
/// \brief Recording manager
///
/// The recording manager, whose instance is held by core, manages the
/// messages controlling the recording process. It also takes care of the
/// stroboscopic tuning indicator.
////////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN RecordingManager :  public MessageListener
{
public:
    RecordingManager (AudioRecorder *audioRecorder);

    void init () {}
    void exit () {}

private:
    virtual void handleMessage(MessagePtr m);

    AudioRecorder *mAudioRecorder;            ///< Pointer to the audio device
    Stroboscope *mStroboscope;
    const Piano* mPiano;                    ///< Poitner to the actual piano
    OperationMode mOperationMode;           ///< Current operation mode
    const Key* mSelectedKey;                ///< Currently selected key
    int mKeyNumberOfA4;                      ///< Total number of keys
    int mNumberOfSelectedKey;               ///< Number of actually selected key

    const double FPS_FAST = 30;             ///< Stroboscopic fps during recording
    const double FPS_SLOW = 15;             ///< Stroboscopic fps during non-recording

    void updateStroboscopicFrequencies();
};

#endif // RECORDINGMANAGER_H
