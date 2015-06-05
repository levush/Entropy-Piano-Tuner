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
//                      A simple sine wave synthesizer
//=============================================================================

#ifndef SYNTHESIZER_H
#define SYNTHESIZER_H

#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <thread>
#include <mutex>
#include <chrono>

#include "synthesizeradapter.h"
#include "audioplayeradapter.h"
#include "../system/simplethreadhandler.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Class for a simple synthesizer based on Fourier modes.
///
/// This is a simple software synthesizer that can reproduce any
/// frequency spectrum.
///
/// A sound is produced in three steps. At first the sound has to be created
/// by calling the function CreateSound. Secondly one has to add one or several
/// Fourier modes with a given frequency and amplitude. This allows one to
/// create any sound texture. Finally, by calling the function PlaySound,
/// the waveform is calculated and sent to the actual audio implementation.
///
/// Each sound is identified by an ID, usually the number of the piano key.
///
/// The synthesizer supports basic ADSR-envelopes (attack-decay-sustain-release)
/// which are known from traditional synthesizers. The ADSR-envelope of each sound
/// (note) can be chosen individually. The release phase is triggered by calling
/// the function ReleaseSound.
///////////////////////////////////////////////////////////////////////////////

class Synthesizer : public SynthesizerAdapter, public SimpleThreadHandler
{
public:
    Synthesizer (AudioPlayerAdapter *audioadapter);

    void init ();
    void exit ();

    // Create a new sound (note)
    void createSound (int id,               // Id of the sound
                      double volume=1,      // overall volume
                      double stereo=0.5,    // stereo position (0..1)
                      double attack=10,     // ADSR attack rate
                      double decayrate=0.5, // ADSR decay rate
                      double sustain=0.0,   // ADSR sustain rate
                      double release=10)    // ADSR release rate
                      override final;

    // Add a Fourier component
    void addFourierComponent (int id, double f, double amplitude)
    override final;

    // Start playing
    void playSound (int id) override final;

    // Stop playing
    void releaseSound (int id) override final;

    // Check whether sound is still playing
    bool isPlaying (int id) override final;

    // Modify the sustain level of a constantly playing sound
    void ModifySustainLevel (int id, double level) override final;

private:    
    const int SineLength = 16384;           ///< sine value buffer length.
    const double CutoffVolume = 0.00001;    ///< Fade-out volume cutoff.

    AudioBase::PacketType mSineWave;        ///< Sine wave vector.
    AudioBase::PacketType mBuffer;          ///< Audio buffer.

    struct Sound
    {
        int clock;                          ///< Running time in sample cycles.
        int stage;                          ///< Stage of envelope:  0=off
                                            ///< 1=attack 2=decay 3=sustain 4=release.
        double amplitude;                   ///< Actual time-dependent amplitude.
        double volume;                      ///< Volume of the sound.
        double stereo;                      ///< Stereo position in [0,1].
        double attack;                      ///< Attack rate for envelope.
        double decayrate;                   ///< Decay rate for envelope.
        double sustain;                     ///< Sustain rate for envelope.
        double release;                     ///< Release rate for envelope.
        std::map<float,float> fouriermodes;
    };

    bool mRunning;                      ///< Flag that the thread is running.
    std::map<int,Sound> mChord;         ///< Chord defined as a collection of sounds.
    std::mutex mChordMutex;             ///< Mutex to protect access to the chord.
    AudioPlayerAdapter *mAudioPlayer;   ///< Pointer to the audio player.

    Sound* getSoundPtr (int id);
    void workerFunction () override final;
    void generateWaveform();
};

#endif // SYNTHESIZER_H
