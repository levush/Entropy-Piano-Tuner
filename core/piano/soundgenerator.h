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
//                 Sound generator driving the synthesizer
//=============================================================================

#ifndef SOUNDGENERATOR_H
#define SOUNDGENERATOR_H

#include "soundgeneratormode.h"

#include "../audio/synthesizer.h"
#include "../midi/midiadapter.h"
#include "../piano/piano.h"
#include "../messages/messagelistener.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Class for managing the arrangement of sounds.
///
/// This class manages and composes the sound to be played by the synthesizer.
/// It is completely driven by messages.
///////////////////////////////////////////////////////////////////////////////

class SoundGenerator : public MessageListener
{
public:
    SoundGenerator (AudioPlayerAdapter *audioadapter);
    ~SoundGenerator(){}

    void init () {mSynthesizer.init();}
    void exit () {mSynthesizer.exit();}

    void start() {mSynthesizer.start();}
    void stop()  {mSynthesizer.stop();}

private:
    void handleMessage(MessagePtr m) override final;
    void handleMidiKeypress(MidiAdapter::Data &data);
    double getStereoPosition (int keynumber);
    void playSineWave(int keynumber, double frequency, double volume);
    void playResonatingSineWave (int keynumber, double frequency, double volume);
    void playResonatingReferenceSound (int keynumber);
    void stopResonatingReferenceSound ();
    void changeVolumeOfResonatingReferenceSound (double level);

    void playOriginalSoundOfKey (const int keynumber, const double volume);
    void playEchoSound (const int keynumber);

    void preCalculateSoundOfKey (const int keynumber, const OperationMode operationmode, const double frequency);
    void preCalculateSoundOfAllKeys ();

private:
    Synthesizer mSynthesizer;               ///< Instance of the synthesizer.
    const AudioPlayerAdapter *mAudioAdapter;///< Pointer to the audio device
    const Piano *mPiano;                    ///< Pointer to the piano.
    OperationMode mOperationMode;           ///< Copy of the operation mode.
    double mConcertPitch;                   ///< Copy of the concert pitch.
    int mNumberOfKeys;                      ///< Copy of the number of keys.
    int mKeyNumberOfA4;                     ///< Copy of A-key position.
    int mSelectedKey;                       ///< Copy of selected key.
    int mResonatingKey;                     ///< Key of the resonating sound
    double mResonatingVolume;               ///< Volume of the resonating sound
};

#endif // SOUNDGENERATOR_H
