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
//       Class for a complex sound consisting of various Fourier modes
//=============================================================================

#ifndef SOUND_H
#define SOUND_H

#include <map>
#include <vector>
#include <cmath>

#include "../system/simplethreadhandler.h"
#include "../system/log.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Class for a complex sound
///
/// A complex sound is a tone with a complex spectrum of higher partials.
/// By defining a suitable spectrum one can generate different textures of
/// the sound. In the EPT complex sounds are used to mimic the original sound
/// of the recorded piano key on the basis of the actual spectrum.
///
/// This class describes the spectral content of a complex sound. It does not
/// tell us anything about the dynamics (volume, duration envelope). This
/// will be accounted for in the class Synthesizer.
///
/// The playback of complex sounds is computationally expensive. Therefore, the
/// waveform is computed in advance: Whenever a sound is initialized, an
/// independent thread is automatically started which computes the waveform.
///////////////////////////////////////////////////////////////////////////////

class ComplexSound : public SimpleThreadHandler
{
public:
    using Spectrum = std::map<double,double>;   // frequency-power spectrum
    using WaveForm = std::vector<double>;       // stereo waveform

    ComplexSound ();
    ~ComplexSound() {};

    // Initialize a newly created complex sound:
    void init (const Spectrum &spectrum, const double stereo, const int samplerate,
               const WaveForm &sinewave, const double time, const double waitingtime = 0);

    bool coincidesWith (const Spectrum &spectrum)
    { return mHash == computeHashTag(spectrum); }

    WaveForm getWaveForm();

private:
    int mSampleRate;                // Sample rate
    WaveForm mSineWave;             // Copy of the sine wave pcm data
    Spectrum mSpectrum;             // The spectrum of the sound
    double mStereo;                 // Its stereo position (0..1)
    double mTime;                   // The required sampling time
    WaveForm mWaveForm;             // Computed wave form
    std::mutex mMutex;              // Access mutex
    std::atomic<bool> mReady;       // Flag that the wave form is ready
    std::atomic<long> mHash;        // Hash value for comparison
    double mWaitingTime;            // Waiting time before computation

    static std::atomic<int> numberOfThreads;        // Actual number of threads

    void workerFunction () override final;          // Thread worker function
    void generateWaveform ();                       // PCM waveform generator

    long computeHashTag (const Spectrum &spectrum);
};

#endif // SOUND_H
