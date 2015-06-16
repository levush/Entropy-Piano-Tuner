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

#include <iostream> // ******************** später weg
#include <vector>
#include <map>
#include <cmath>
#include <thread>
#include <mutex>
#include <chrono>

#include "audioplayeradapter.h"
#include "sound.h"
#include "../system/simplethreadhandler.h"


struct Envelope
{
    double attack;
    double decay;
    double sustain;
    double release;
    double hammer;

    Envelope(double attack=0, double decay=0,
             double sustain=0, double release=0,
             double hammer=0);
};



struct Tone
{
    int id;                             ///< Identification tag
    Sound sound;                        ///< Sound texture information

    int_fast64_t frequency;             ///< converted sine frequency
    int_fast64_t clock;                 ///< Running time in sample cycles.
    int_fast64_t clock_timeout;         ///< Timeout when forced to release
    int stage;                          ///< Stage of envelope:  0=off
                                        ///< 1=attack 2=decay 3=sustain 4=release.
    double amplitude;
    Envelope envelope;
    SampledSound::WaveForm waveform;                  ///< Computed wave form stored here
};




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
/// which are known from traditional synthesizers. The ADSR-envelope of each
/// sound can be chosen individually. The release phase is triggered by calling
/// the function ReleaseSound.
///////////////////////////////////////////////////////////////////////////////

class Synthesizer : public SimpleThreadHandler
{
public:
    using Spectrum = std::map<double,double>;


    Synthesizer (AudioPlayerAdapter *audioadapter);

    void init ();
    void exit () { stop(); }

    void registerSound  (const int id, const Sound &sound, double const waitingtime);

    void playSound (const int id, const Sound &sound, const Envelope &env, const int maxplaytime=60);

    void releaseSound (const int id);

    // Check whether sound is still playing
    bool isPlaying (const int id);

    // Modify the sustain level of a constantly playing sound
    void ModifySustainLevel (int id, double level);


private:

    using WaveForm = SampledSound::WaveForm;

    std::map <int,SampledSound> mSoundCollection;
    std::vector<Tone> mPlayingTones;        ///< Chord defined as a collection of tones.
    std::mutex mPlayingMutex;               ///< Mutex to protect access to the chord.

    const int_fast64_t  SineLength = 16384; ///< sine value buffer length.
    const double CutoffVolume = 0.00001;    ///< Fade-out volume cutoff.

    WaveForm mSineWave;           ///< Sine wave vector.
    WaveForm mHammerWave;         ///< Hammer noise PCM data

    AudioPlayerAdapter *mAudioPlayer;       ///< Pointer to the audio player.

    Tone* getSchedulerPointer (int id);
    void workerFunction () override final;
    void generateAudioSignal();
};

#endif // SYNTHESIZER_H
