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

#ifndef SOUND_H
#define SOUND_H

#include <map>
#include <vector>

#include "../system/simplethreadhandler.h"

class Synthesizer;

//=============================================================================
//                             Class for a sound
//=============================================================================

class Sound
{
public:
    using Spectrum = std::map<double,double>;

    Sound();
    ~Sound(){}

    void set (const double frequency, const Spectrum &spectrum,
              const double stereo, const double volume);

    Sound (const double frequency, const Spectrum &spectrum,
           const double stereo, const double volume);

    void set(const Sound &sound);

    long getHashTag() const;

    friend class Synthesizer;
protected:

    double mFrequency;         // fundamental frequency
    Spectrum mSpectrum;       // spectrum of partials
    double mStereo;            // stereo position (0..1)
    double mVolume;            // overall volume
    long mHashTag;    // hash tag

    void computeHashTag();
};



//=============================================================================
//       Class for a complex sound consisting of various Fourier modes
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
/// \brief Class for a complex sound
///
/// A complex sound is a tone with a spectrum containing many higher partials.
/// By defining a suitable spectrum one can generate different textures of
/// the sound. In the EPT, complex sounds are used to mimic the original sound
/// of the recorded piano key on the basis of the actually measured spectrum.
///
/// This class describes the spectral content of a complex sound. It does not
/// tell us anything about the dynamics (volume, duration envelope). This
/// will be accounted for in the class Synthesizer.
///
/// The playback of complex sounds is computationally expensive. Therefore, the
/// waveform is computed in advance: Whenever a sound is initialized, an
/// independent thread is automatically started which computes the waveform
/// and stores it locally in the member variable mWaveForm.
///////////////////////////////////////////////////////////////////////////////

class SampledSound : public SimpleThreadHandler, public Sound
{
public:
using WaveForm = std::vector<float>;        // stereo waveform

    SampledSound ();                            // constructor
    ~SampledSound() {}                          // empty destructor

    // Initialize a newly created instance of a complex sound:
    void init (const Sound &sound,
               const int samplerate,
               const WaveForm &sinewave,
               const double playingtime,
               const double waitingtime = 0);

    bool differsFrom (const Sound &sound) const
    { return (getHashTag() != sound.getHashTag()); }

    bool isReady() { return mReady; }

    WaveForm getWaveForm();

private:
    int mSampleRate;                // Sample rate
    WaveForm mSineWave;             // Copy of the sine wave pcm data
    double mTime;                   // The required sampling time
    int64_t mSampleLength;          // Number of stereo sample pairs
    WaveForm mWaveForm;             // Computed wave form
    std::mutex mMutex;              // Access mutex
    std::atomic<bool> mReady;       // Flag that the wave form is ready
    double mWaitingTime;            // Waiting time before computation

    static std::atomic<int> numberOfThreads;        // Actual number of threads

    void workerFunction () override final;          // Thread worker function
    void generateWaveform ();                       // PCM waveform generator
};

#endif // SOUND_H
