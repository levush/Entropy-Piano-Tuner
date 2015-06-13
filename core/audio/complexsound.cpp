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
//                        Class for a complex sound
//=============================================================================

#include "complexsound.h"

//-----------------------------------------------------------------------------
//                               Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor, initializes the member variables
///////////////////////////////////////////////////////////////////////////////

ComplexSound::ComplexSound() :
    mSampleRate(0),
    mSineWave(),
    mSpectrum(),
    mStereo(0.5),
    mTime(0),
    mWaveForm(),
    mReady(false),
    mHash(0)
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
/// \param spectrum : The power spectrum of the sound (frequency - power)
/// \param stereo : Stereo location in the range 0 (left) to 1 (right)
/// \param samplerate : Sample rate to be used for generating PCM
/// \param sinewave : Referemce to a pre-calculated sine wave
/// \param time : Duration of the PCM waveform to be generated in seconds
/// \param waitingtime : Technical sleep time before compuatation (seconds).
///////////////////////////////////////////////////////////////////////////////

void ComplexSound::init (const double frequency,
                         const Spectrum &spectrum,
                         const double stereo,
                         const int samplerate,
                         const WaveForm &sinewave,
                         const double time,
                         const double waitingtime)
{
    if (frequency<=0) return;
    std::lock_guard<std::mutex> lock(mMutex);
    mSampleRate = samplerate;
    mSineWave = sinewave;
    mSpectrum.clear();
    if (spectrum.size()>0)
    {
        double f1 = spectrum.begin()->first;
        for (auto &s : spectrum) mSpectrum[s.first*frequency/f1]=s.second;
    }
    mStereo = stereo;
    mTime = time;
    mWaveForm.clear();
    mReady = false;
    mHash = computeHashTag(spectrum);
    mWaitingTime = waitingtime;
    start();
}


//-----------------------------------------------------------------------------
//                Generate the PCM waveform of a complex sound
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Generate the PCM waveform of a complex sound.
///
/// This function generates the PCM data according to the specified power
/// spectrum for the predetermined length. By default a stereo singnal is
/// generated (format LRLR...)
///////////////////////////////////////////////////////////////////////////////

void ComplexSound::generateWaveform()
{
    std::lock_guard<std::mutex> lock(mMutex);
    auto round = [] (double x) { return static_cast<int64_t>(x+0.5); };
    const int64_t SampleLength = round(mSampleRate * mTime);
    const double leftvol  = sqrt(0.9-0.8*mStereo);
    const double rightvol = sqrt(0.1+0.8*mStereo);
    mWaveForm.resize(2*SampleLength);
    mWaveForm.assign(2*SampleLength,0);

    double sum=0;
    for (auto &mode : mSpectrum) sum+=mode.second;
    if (sum<=0) return;

    const int64_t SineLength = mSineWave.size();
    for (auto &mode : mSpectrum)
    {
        const double f = mode.first;
        const double volume = pow(mode.second / sum,0.5);

        if (f>24 and f<10000 and volume>0.001)
        {
            const int64_t periods = round((SampleLength * f) / mSampleRate);
            const int64_t phasediff = round(periods * mSampleRate *
                                            (0.5-mStereo) / 500);
            const int64_t leftphase  = rand();
            const int64_t rightphase = leftphase + phasediff;
            for (int64_t i=0; i<SampleLength; ++i)
            {
                mWaveForm[2*i] += volume * leftvol *
                    mSineWave[((i*periods*SineLength)/SampleLength+leftphase)%SineLength];
                mWaveForm[2*i+1] += volume * rightvol *
                    mSineWave[((i*periods*SineLength)/SampleLength+rightphase)%SineLength];
            }
        }
        if (cancelThread()) return;
    }
    mReady = true;
    LogI ("Created waveform with %d partials", (int)(mSpectrum.size()));

//    if (id !=0) return;
//    std::ofstream os("0000-waveform.dat");
//    for (int64_t i=0; i<SampleLength; ++i)  os << mWaveForms[0][2*i] << std::endl;
//    os << "&" << std::endl;
//    for (int64_t i=0; i<SampleLength; ++i)  os << mWaveForms[0][2*i+1] << std::endl;
//    os.close();
}


//-----------------------------------------------------------------------------
//           Worker function executing the thread for PCM generation
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief ComplexSound::workerFunction
///
/// This function executes the thread which is started in the initialization
/// procedure. To avoid an overload of the CPU, only a certain maximal number
/// of threads is allows to be active at the same time. The worker function
/// manages this thread limitation and calls the function generateWafeform().
///////////////////////////////////////////////////////////////////////////////

std::atomic<int> ComplexSound::numberOfThreads(0); // static thread counter

void ComplexSound::workerFunction()
{
    if (mWaitingTime > 0) msleep (1000.0 * mWaitingTime);
    if (cancelThread()) return;
    const int maxNumberOfThreads = 8;
    while (numberOfThreads >= maxNumberOfThreads) msleep(1);
    if (cancelThread()) return;
    numberOfThreads++;
    generateWaveform();
    numberOfThreads--;
}


//-----------------------------------------------------------------------------
//                      Private: compute hash value
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Compute hash value of the spectrum
///
/// As soon as the frequency of a key changes, the waveform of the ComplexSound
/// has to be computed once again. However, if the spectrum happens to
/// coincide with the actual spectrum, then there is no need for a renewed
/// computation.
///
/// Since during computation a comparison of the spectrum would be difficult
/// because of mutex blocking, the class saves a hash tag of the spectrum as
/// a necessary condition for coincidence.
/// \param spectrum : Reference to the spectrum.
/// \return Corresponding hash tag.
///////////////////////////////////////////////////////////////////////////////

long ComplexSound::computeHashTag (const Spectrum &spectrum)
{
    long hash = 0;
    for (auto &f : spectrum) hash ^= std::hash<double>() (f.second);
    return hash;
}


//----------------------------------------------------------------------------
//     Return the PCM waveform of the complex sound (constant volume)
//----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get the current PCM waveform of the complex sound
/// \return PCM vector in stereo (LRLRLR...), zero size if not yet computed
///////////////////////////////////////////////////////////////////////////////

ComplexSound::WaveForm ComplexSound::getWaveForm()
{
    std::lock_guard<std::mutex> lock(mMutex);
    if (not mReady) return WaveForm();
    else return mWaveForm;
}

