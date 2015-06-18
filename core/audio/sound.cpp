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

#include "sound.h"

#include "../math/mathtools.h"
#include "../system/log.h"


//=============================================================================
//                              Class of a  Sound
//=============================================================================

//-----------------------------------------------------------------------------
//                           Empty constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor: Create an empty sound
///////////////////////////////////////////////////////////////////////////////

Sound::Sound() :
    mFrequency(0),
    mSpectrum(),
    mStereo(0),
    mVolume(0),
    mHashTag(0)
{}


//-----------------------------------------------------------------------------
//                         Parametrized constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Parametrized constructor for a sound
/// \param frequency : Fundamental frequency in Hz
/// \param spectrum : Power spectrum: f -> intensity
/// \param stereo : Stereo position in [0,1] (0=left, 1=right)
/// \param volume : Overall volume in [0,1]
///////////////////////////////////////////////////////////////////////////////

Sound::Sound (const double frequency, const Spectrum &spectrum,
              const double stereo, const double volume) :
    mFrequency(frequency),
    mSpectrum(spectrum),
    mStereo(stereo),
    mVolume(volume)
{   computeHashTag(); }


//-----------------------------------------------------------------------------
//                        Set parameters individually
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Set parameters of a sound individually
/// \param frequency : Fundamental frequency in Hz
/// \param spectrum : Power spectrum: f -> intensity
/// \param stereo : Stereo position in [0,1] (0=left, 1=right)
/// \param volume : Overall volume in [0,1]
///////////////////////////////////////////////////////////////////////////////

void Sound::set(const double frequency, const Spectrum &spectrum,
                const double stereo, const double volume)
{
    mFrequency = frequency;
    mSpectrum = spectrum;
    mStereo = stereo;
    mVolume = volume;
    computeHashTag();
}


//-----------------------------------------------------------------------------
//                     Copy parameters from another sound
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Sound::set
/// \param sound
///////////////////////////////////////////////////////////////////////////////

void Sound::set (const Sound &sound)
{
    mFrequency = sound.mFrequency;
    mSpectrum = sound.mSpectrum;
    mStereo = sound.mStereo;
    mVolume = sound.mVolume;
    computeHashTag();
}


//-----------------------------------------------------------------------------
//                       Compute has tag of the sound
//-----------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////
/// \brief Compute hash tag of the sound
///
/// As soon as the frequency of a key changes, the waveform of a sound
/// has to be computed once again. However, if the spectrum happens to
/// coincide with the actual spectrum, then there is no need for a renewed
/// computation. To detect changes easily and to avoid unnecessary mutex
/// blocking, the Sound class provides a hash tag which is some kind of
/// XOR over the relevant data.
///
/// Since the frequency of the spectral lines may change during
/// tuning, the hash tag is computed exclusively from the fundamental
/// frequency and the peaks heights.
///
/// \return Corresponding hash tag.
///////////////////////////////////////////////////////////////////////////////

void Sound::computeHashTag ()
{
    mHashTag = std::hash<double>() (mFrequency);
    for (auto &f : mSpectrum)
        mHashTag ^= std::hash<double>() (f.second); // XOR over all peaks
}





//=============================================================================
//                        Class for a complex sound
//=============================================================================

//-----------------------------------------------------------------------------
//                               Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor, initializes the member variables
///////////////////////////////////////////////////////////////////////////////

SampledSound::SampledSound() :
    mSampleRate(0),
    mSineWave(),
    mSampleTime(0),
    mSampleLength(0),
    mWaveForm(),
    mReady(false)
{}


//-----------------------------------------------------------------------------
//                         Initialize a new ComplexSound
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Initialize a new complex sound
///
/// This function initializes a newly created complex sound, depending on the
/// parameters which characterize the sound. After initialization, the
/// computation of the PCM wave form is started in an independent thread.
///
/// \param samplerate : Sample rate to be used for generating PCM
/// \param sinewave : Reference to a pre-calculated sine wave
/// \param time : Duration of the PCM waveform to be generated in seconds
/// \param waitingtime : Technical sleep time before compuatation (seconds).
///////////////////////////////////////////////////////////////////////////////

void SampledSound::startSampling (const int samplerate,
                                  const WaveForm &sinewave,
                                  const double sampletime,
                                  const double waitingtime)
{
    stop(); // if necessary interrupt ongoing computation
    std::lock_guard<std::mutex> lock(mMutex);

    mSampleRate = samplerate;
    mSineWave = sinewave;
    mSampleTime = sampletime;
    mSampleLength = MathTools::roundToInteger(mSampleRate * sampletime);
    mWaitingTime = waitingtime;

    mWaveForm.clear();
    mReady = false;
    start();                        // start the thread (workerFunction)
}



//-----------------------------------------------------------------------------
//           Worker function executing the thread for PCM generation
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Thread function in which the waveform is calculated
///
/// This function executes the thread which is started in the
/// PCM waveform of the sound is calculated.
///
/// The function does not compute the new waveform immediately, instead it
/// waits for mWaitingTime (double in seconds) before the computation is
/// started. Whenever the the same thread is started once again during the
/// waiting period, the current thread is cancelled. This ensures that only
/// the newest thread survives. On the other hand, a high CPU load in
/// situations with many changes (during tuning) is avoided.
///
/// To further avoid an overload of the CPU, only a certain maximal number
/// of threads is allows to be active at the same time. The worker function
/// manages this thread limitation and calls the function generateWafeform().
///////////////////////////////////////////////////////////////////////////////

void SampledSound::workerFunction()
{
    setThreadName("Wafeform Creation");

    // First wait and cancel if retriggered
    for (double t=0; t<mWaitingTime and not cancelThread(); t+=0.001) msleep (1);
    if (cancelThread()) return;

    // Avoid CPU overload by limiting the number of threads
    const int maxNumberOfThreads = 8;
    while (numberOfThreads >= maxNumberOfThreads) msleep(1);
    if (cancelThread()) return;
    numberOfThreads++;
    generateWaveform();
    numberOfThreads--;
}

/// \brief Static thread counter counting the number of running threads

std::atomic<int> SampledSound::numberOfThreads(0); // static thread counter


//-----------------------------------------------------------------------------
//                Generate the PCM waveform of the sound
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Generate the PCM waveform of the sound.
///
/// This function generates the PCM waveform according to the specified power
/// spectrum for the predetermined length. By default a stereo singnal is
/// generated (format LRLR...).
///
/// The wave consists of a superposition of sine functions. In order to avoid
/// an initial "klick" all sine waves are randomly phase-shifted. Moreover,
/// the phase between the two stereo channels is phase-shifted to mimic the
/// phase delay in the real world. This leads to a more stereo-like sound.
///////////////////////////////////////////////////////////////////////////////

void SampledSound::generateWaveform()
{
    std::lock_guard<std::mutex> lock(mMutex);
    const double leftvol  = sqrt(0.9-0.8*mStereo);
    const double rightvol = sqrt(0.1+0.8*mStereo);
    mWaveForm.resize(2*mSampleLength);
    mWaveForm.assign(2*mSampleLength,0);
    std::default_random_engine generator;
    std::uniform_int_distribution<int> flatDistribution(0,mSampleLength);


    double norm = 0;
    for (auto &s : mSpectrum) norm += s.second;
    if (norm<=0) return;

    const double frequencyshift = mFrequency / mSpectrum.begin()->first;

    const int64_t SineLength = mSineWave.size();
    for (auto &mode : mSpectrum)
    {
        const double f = mode.first * frequencyshift;
        const double volume = sqrt(mode.second/norm);

        if (f>24 and f<10000 and volume>0.0005)
        {
            const int64_t periods = round((mSampleLength * f) / mSampleRate);
            const int64_t phasediff = round(mSampleRate *
                                            (0.5-mStereo) / 200);
            const int64_t leftphase  = flatDistribution(generator);;
            const int64_t rightphase = leftphase + phasediff;
            for (int64_t i=0; i<mSampleLength; ++i)
            {
                mWaveForm[2*i] += volume * leftvol *
                    mSineWave[((i*periods*SineLength)/mSampleLength+leftphase)%SineLength];
                mWaveForm[2*i+1] += volume * rightvol *
                    mSineWave[((i*periods*SineLength)/mSampleLength+rightphase)%SineLength];
            }
        }
        if (cancelThread()) return;
    }
    mReady = true;
    LogI ("Created waveform with %d partials", (int)(mSpectrum.size()));
}


///////////////////////////////////////////////////////////////////////////////
/// \brief Find out whether another sound is different from the present one.
/// \param sound : Sound structure to be compared.
/// \return True if the sounds are different.
///////////////////////////////////////////////////////////////////////////////

bool SampledSound::differsFrom (const Sound &sound) const
{ return (getHashTag() != sound.getHashTag()); }


//----------------------------------------------------------------------------
//     Return the PCM waveform of the complex sound (constant volume)
//----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get the current PCM waveform of the complex sound
/// \return PCM vector in stereo (LRLRLR...), zero size if not yet computed
///////////////////////////////////////////////////////////////////////////////

SampledSound::WaveForm SampledSound::getWaveForm()
{
    std::lock_guard<std::mutex> lock(mMutex);
    if (not mReady) return WaveForm();
    else return mWaveForm;
}





