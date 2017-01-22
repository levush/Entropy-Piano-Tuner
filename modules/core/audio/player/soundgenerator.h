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
//                 Sound generator driving the synthesizer
//=============================================================================

#ifndef SOUNDGENERATOR_H
#define SOUNDGENERATOR_H

#include "prerequisites.h"
#include "synthesizer.h"
#include "soundgenerator.h"
#include "core/audio/audiointerface.h"
#include "../midi/midiadapter.h"
#include "../../piano/piano.h"
#include "../../messages/messagelistener.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Class for managing the sound generation in the EPT.
///
/// This class manages and composes the sound to be played by the synthesizer.
/// It is completely driven by messages. It is some kind of SoundGenerationManager.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN SoundGenerator : public MessageListener
{
public:
    /// \brief Mode for sound generation
    enum SoundGeneratorMode
    {
        SGM_DEACTIVATED,        ///< No sound generation
        SGM_SYNTHESIZE_KEY,     ///< Produce a sound which imitates the string
        SGM_REFERENCE_TONE,     ///< Produce a simple sine wave as reference
    };

public:
    SoundGenerator (AudioInterface *AudioInterface);
    ~SoundGenerator(){}

    void init ();
    void exit () { mSynthesizer.close(); }   ///< Synthesizer shutdown

    Synthesizer &getSynthesizer() {return mSynthesizer;}

private:
    void handleMessage(MessagePtr m) override final;
    void handleMidiKeypress(MidiAdapter::Data &data);
    void playResonatingSineWave (int keynumber, double frequency, double volume);
    void playResonatingReferenceSound (int keynumber);
    void stopResonatingReferenceSound ();
    void changeVolumeOfResonatingReferenceSound (double level);
    void preCalculateSoundOfKey (const int keynumber);
    void preCalculateSoundOfKey (const int keynumber, Synthesizer::Spectrum &spectrum);
    void preCalculateSoundOfAllKeys ();

private:
    Synthesizer mSynthesizer;                   ///< Instance of the synthesizer.
    const Piano *mPiano;                        ///< Pointer to the piano.
    OperationMode mOperationMode;               ///< Copy of the operation mode.
    int mNumberOfKeys;                          ///< Copy of the number of keys.
    int mKeyNumberOfA4;                         ///< Copy of A-key position.
    int mSelectedKey;                           ///< Copy of selected key.
    int mResonatingKey;                         ///< Keynumber of the resonating sound
    double mResonatingVolume;                   ///< Volume of the resonating sound
};

#endif // SOUNDGENERATOR_H
