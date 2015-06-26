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

#include "audioplayeradapter.h"
#include "waveformgenerator.h"
#include "../system/simplethreadhandler.h"


//=============================================================================
//                          Structure of a tone
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
/// \brief Structure of a single tone.
///
/// This structure contains all data elements which characterize a single
/// tone. Each tone carries an ID for housekeeping. The tone is characterized
/// mainly by the sound (static properties) and the envelope (dynamics).
///
/// The clock variable counts the number of samples from the beginning of
/// the tone. The clock_timeout limits the maximal duration of a tone.
/// The variables 'stage' indicates the dynamical state of the envelope.
///////////////////////////////////////////////////////////////////////////////

struct Tone
{
    int id;                             ///< Identification tag (negativ=sine)
    double frequency;                   ///< Fundamental frequency
    double volume;
    Envelope envelope;                  ///< Dynamic properties of the tone

    int_fast64_t clock;                 ///< Running time in sample cycles.
    int stage;                          ///< 1=attack 2=decay 3=sustain 4=release.
    double amplitude;                   ///< current envelope amplitude

    WaveformGenerator::Waveform waveform;
};



///////////////////////////////////////////////////////////////////////////////
/// \brief Synthesizer class
///
/// This is the synthesizer of the EPT. It runs in an independent thread.
///////////////////////////////////////////////////////////////////////////////

class Synthesizer : public SimpleThreadHandler
{
public:
    Synthesizer (AudioPlayerAdapter *audioadapter);

    void init ();
    void exit () { stop(); }

    void preCalculateWaveform   (const int id,
                                 const Sound &sound);

    void playSound              (const int id,
                                 const double frequency,
                                 const double volume,
                                 const Envelope &env);

    void ModifySustainLevel     (const int id,
                                 const double level);

    void releaseSound           (const int id);

    bool isPlaying              (const int id) const;


private:

    WaveformGenerator mWaveformGenerator;

    using Waveform = WaveformGenerator::Waveform;

    std::vector<Tone> mPlayingTones;        ///< Chord defined as a collection of tones.
    mutable std::mutex mPlayingMutex;       ///< Mutex to protect access to the chord.

    const int_fast64_t  SineLength = 16384; ///< sine value buffer length.
    const double CutoffVolume = 0.00001;    ///< Fade-out volume cutoff.

    Waveform mSineWave;                     ///< Sine wave vector, computed in init().
    Waveform mHammerWave;                   ///< Hammer noise, computed in init().

    AudioPlayerAdapter *mAudioPlayer;       ///< Pointer to the audio player.

    const Tone* getSoundPointer (const int id) const;
    Tone* getSoundPointer (const int id);
    void workerFunction () override final;
    void generateAudioSignal();
};

#endif // SYNTHESIZER_H
