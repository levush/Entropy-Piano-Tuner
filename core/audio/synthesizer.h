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
#include <queue>
#include <cmath>
#include <thread>
#include <mutex>
#include <chrono>

#include "audioplayeradapter.h"
#include "complexsound.h"
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

class Synthesizer : public SimpleThreadHandler
{
public:


    using Spectrum = std::map<double,double>;


    void registerSound (const int id,
                        const double f1,
                        const Spectrum &spectrum,
                        const double stereo,
                        const double time=1);
    void play (int id,               // Id of the sound
               double f, //Sine frequ
                      double volume=1,      // overall volume
                      double stereo=0.5,    // stereo position (0..1)
                      double attack=10,     // ADSR attack rate
                      double decayrate=0.5, // ADSR decay rate
                      double sustain=0.0,   // ADSR sustain rate
                      double release=10);   // ADSR release rate
    void release (int id);

private:

    using WaveForm = std::vector<float>;
    std::map<int,ComplexSound> mSoundCollection;

    void createWaveforms();


    /////////////////////////////////////////////////////////

public:
    Synthesizer (AudioPlayerAdapter *audioadapter);

    void init ();
    void exit ();

    // Create a new sound (note)

    // Stop playing
    void releaseSound (int id);

    // Check whether sound is still playing
    bool isPlaying (int id);

    // Modify the sustain level of a constantly playing sound
    void ModifySustainLevel (int id, double level);

private:    
    const int_fast64_t  SineLength = 16384;           ///< sine value buffer length.
    const double CutoffVolume = 0.00001;    ///< Fade-out volume cutoff.

    std::vector<float> mSineWave;           ///< Sine wave vector.
    std::vector<float> mHammerWave;         ///< Hammer noise PCM data

    struct Tone
    {
        int id;                             ///< Identification tag
        int_fast64_t  clock;                          ///< Running time in sample cycles.
        int_fast64_t  clock_timeout;                  ///< Timeout when forced to release
        int stage;                          ///< Stage of envelope:  0=off
                                            ///< 1=attack 2=decay 3=sustain 4=release.
        double amplitude;                   ///< Actual time-dependent amplitude.
        double volume;                      ///< Volume of the sound.
        double stereo;                      ///< Stereo position in [0,1].
        double attack;                      ///< Attack rate for envelope.
        double decayrate;                   ///< Decay rate for envelope.
        double sustain;                     ///< Sustain rate for envelope.
        double release;                     ///< Release rate for envelope.
        int_fast64_t frequency;             ///< converted sine frequency, 0 for waveform
        WaveForm waveform;                     // Computed wave form stored here
    };

    std::vector<Tone> mScheduler;             ///< Chord defined as a collection of tones.
    std::mutex mSchedulerMutex;                 ///< Mutex to protect access to the chord.
    AudioPlayerAdapter *mAudioPlayer;       ///< Pointer to the audio player.

    Tone* getSchedulerPointer (int id);
    void workerFunction () override final;
    void generateAudioSignal();
};

#endif // SYNTHESIZER_H
