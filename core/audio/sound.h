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
//                        Class describing a sound
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
/// \brief The Sound class
///
/// A sound is a tone with a spectrum containing many higher partials.
/// By defining a suitable spectrum one can generate different textures of
/// a sound. In the EPT, sounds are used to mimic the original sound of
/// the recorded piano key on the basis of the actually measured spectrum.
///
/// This class describes the static properties of a sound, in particular its
/// fundamental frequency, its spectral content, the stereo location and
/// its overall volume. It does not tell us anything about the dynamics
/// (duration, envelope). This will be accounted for in the class Synthesizer.
///
/// The class is equipped with a hash tag, a long integer which is some kind
/// of an identifying fingerprint of the spectrum. This allows to quickly
/// compare whether two sounds are identical.
///////////////////////////////////////////////////////////////////////////////

class Sound
{
public:

    using Spectrum = std::map<double,double>;   // type of spectrum

    Sound();
    ~Sound(){}

    Sound (const double frequency, const Spectrum &spectrum,
           const double stereo, const double volume);

    void set (const double frequency, const Spectrum &spectrum,
              const double stereo, const double volume);

    void set (const Sound &sound);

    long getHashTag () const { return mHashTag; }

    // allow the synthesizer to access the member variables
    friend class Synthesizer;

protected:
    double mFrequency;          ///< fundamental frequency
    Spectrum mSpectrum;         ///< spectrum of partials
    double mStereo;             ///< stereo position in [0,1]
    double mVolume;             ///< overall volume in [0,1]
    long mHashTag;              ///< hash tag, binary value

    void computeHashTag();      // compute hash tag
};



//=============================================================================
//        Class for a sound which automatically produces a waveform
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
/// \brief Class for a sound that automatically produces a smapled waveform.
///
/// The playback of complex sounds is computationally expensive. This class,
/// derived from Sound, computes the waveform in advance: Whenever a sound
/// is initialized by the function init(...), an independent thread is
/// started automatically which computes the waveform
/// and stores it locally in the member variable mWaveForm.
///////////////////////////////////////////////////////////////////////////////

class SampledSound : public SimpleThreadHandler, public Sound
{
public:

using WaveForm = std::vector<float>;            // type of stereo waveform

    SampledSound ();                            // constructor
    ~SampledSound() {}                          // empty destructor

    void startSampling (const int samplerate,   // start calculation
                        const WaveForm &sinewave,
                        const double sampletime,
                        const double waitingtime = 0);

    bool differsFrom (const Sound &sound) const; // compare by hash tag

    bool isReady() { return mReady; }           // calculation complete?

    WaveForm getWaveForm();

private:

    int mSampleRate;                ///< Sample rate
    WaveForm mSineWave;             ///< Copy of the sine wave pcm data
    double mTime;                   ///< The required sampling time
    int64_t mSampleLength;          ///< Number of stereo sample pairs
    WaveForm mWaveForm;             ///< Computed wave form
    std::mutex mMutex;              ///< Access mutex
    std::atomic<bool> mReady;       ///< Flag that the wave form is ready
    double mWaitingTime;            ///< Waiting time before computation

    static std::atomic<int> numberOfThreads;        ///< Actual number of threads

    void workerFunction () override final;          ///< Thread worker function
    void generateWaveform ();                       ///< PCM waveform generator
};

#endif // SOUND_H
