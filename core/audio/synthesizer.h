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
#include "../system/simplethreadhandler.h"

class Sound : public SimpleThreadHandler
{
public:
    using Spectrum = std::map<double,double>;
    using WaveForm = std::vector<double>;

    Sound ();
    ~Sound() {};
    void set (int channels, int samplerate, const WaveForm &sinewave,
              const Spectrum &spectrum, const double stereo, const double time);


private:
    int mChannels;              // number of channels (1,2)
    int mSampleRate;            // sample rate
    WaveForm mSineWave;        // Sine wave pcm data
    Spectrum mSpectrum;         // the spectrum of the sound
    double mStereo;             // its stereo location (0..1)
    double mTime;               // the required sampling time
    WaveForm mWaveForm;         // Computed wave form
    std::atomic<bool> mReady;    // flag that the wave form is ready

    void workerFunction () override final;


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
/// which are known from traditional synthesizers. The ADSR-envelope of each sound
/// (note) can be chosen individually. The release phase is triggered by calling
/// the function ReleaseSound.
///////////////////////////////////////////////////////////////////////////////

class Synthesizer : public SimpleThreadHandler
{
public:
    struct Envelope
    {
        double volume=1;        // overall volume
        double stereo=0.5;      // stereo position (0..1)
        bool hammer=false;      // hammer noise
        double attack=10;       // ADSR attack rate
        double decayrate=0.5;   // ADSR decay rate
        double sustain=0.0;     // ADSR sustain rate
        double release=10;      // ADSR release rate
    };


    using Spectrum = std::map<double,double>;


    void registerSoundForCreation (const int id,
                                   const Spectrum &spectrum,
                                   const double stereo,
                                   const double time=1);
    void play (int id, double frequency, Envelope &env);
    void play (int id, Spectrum &spectrum, Envelope &env);
    void release (int id);

private:

    std::map<int,Sound> mRegistrationQueue;
    std::mutex mRegistrationQueueMutex;

    using WaveForm = std::vector<double>;

    void createWaveforms();

    std::map<int,WaveForm> mWaveForms;


    /////////////////////////////////////////////////////////

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
                      double release=10);   // ADSR release rate

    // Add a Fourier component
    void addFourierComponent (int id, double f, double amplitude);

    // Start playing
    void playSound (int id);

    // Stop playing
    void releaseSound (int id);

    // Check whether sound is still playing
    bool isPlaying (int id);

    // Modify the sustain level of a constantly playing sound
    void ModifySustainLevel (int id, double level);

private:    
    const int SineLength = 16384;           ///< sine value buffer length.
    const double CutoffVolume = 0.00001;    ///< Fade-out volume cutoff.

    AudioBase::PacketType mSineWave;        ///< Sine wave vector.

    struct Tone
    {
        int clock;                          ///< Running time in sample cycles.
        int clock_timeout;                  ///< Timeout when forced to release
        int stage;                          ///< Stage of envelope:  0=off
                                            ///< 1=attack 2=decay 3=sustain 4=release.
        double amplitude;                   ///< Actual time-dependent amplitude.
        double volume;                      ///< Volume of the sound.
        double stereo;                      ///< Stereo position in [0,1].
        double attack;                      ///< Attack rate for envelope.
        double decayrate;                   ///< Decay rate for envelope.
        double sustain;                     ///< Sustain rate for envelope.
        double release;                     ///< Release rate for envelope.
        std::map<float,float> fouriermodes; //********************** kommt weg **********************
    };

    std::map<int,Tone> mChord;             ///< Chord defined as a collection of tones.
    std::mutex mChordMutex;                 ///< Mutex to protect access to the chord.
    AudioPlayerAdapter *mAudioPlayer;       ///< Pointer to the audio player.

    Tone* getSoundPtr (int id);
    void workerFunction () override final;
    void generateWaveform();
};

#endif // SYNTHESIZER_H
