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

#include <iostream>

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
    mFrequencyRatio(0),
    mPartials(),
    mPointer(),
    mStereo(0),
    mVolume(0),
    mHashTag(0),
    mSampleRate(0),
    mSampleSize(0),
    mWaveForm()
{
    init();
}


//-----------------------------------------------------------------------------
//                              Initialization
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Initialization of a sound
///////////////////////////////////////////////////////////////////////////////

void Sound::init()
{
    computeHashTag();
    mPointer = mPartials.begin();
    mLeftVolume  = mVolume * sqrt(0.9-0.8*mStereo);
    mRightVolume = mVolume * sqrt(0.1+0.8*mStereo);
    mPhaseDifference = round(mSampleRate * (0.5-mStereo) / 200);
}

//-----------------------------------------------------------------------------
//                  Converter from spectrum to partial list
//-----------------------------------------------------------------------------

void Sound::setPartials (const Spectrum &spectrum)
{
    mPartials.clear();
    double norm = 0;
    for (auto &entry : spectrum) norm += entry.second;
    if (norm<=0) return;
    mFrequencyRatio = mFrequency / spectrum.begin()->first;
    for (auto entry : spectrum) mPartials[-entry.second/norm]=entry.first;
}


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
              const double stereo, const double volume,
              const int samplerate, const double sampletime) :
    mFrequency(frequency),
    mFrequencyRatio(0),
    mPartials(),
    mPointer(),
    mStereo(stereo),
    mVolume(volume),
    mSampleRate(samplerate),
    mSampleSize(static_cast<int>(samplerate*sampletime)),
    mWaveForm(2*mSampleSize,0)
{
    setPartials (spectrum);
    init();
}


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

void Sound::set(const double frequency, const Partials &spectrum,
                const double stereo, const double volume,
                const int samplerate, const double sampletime)
{
    mFrequency = frequency;
    mStereo = stereo;
    mVolume = volume;
    mSampleRate = samplerate;
    mSampleSize = static_cast<int>(samplerate*sampletime);
    mWaveForm.resize(2*mSampleSize);
    mWaveForm.assign(2*mSampleSize,0);
    setPartials(spectrum);
    init();
}


//-----------------------------------------------------------------------------
//                     Copy parameters from another sound
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Sound::set
/// \param sound
///////////////////////////////////////////////////////////////////////////////

void Sound::set (const Sound &sound, const int samplerate, const double sampletime)
{
    mFrequency = sound.mFrequency;
    mFrequencyRatio = sound.mFrequencyRatio;
    mPartials = sound.mPartials;
    mPointer = mPartials.begin();
    mStereo = sound.mStereo;
    mVolume = sound.mVolume;
    mSampleRate = samplerate;
    mSampleSize = static_cast<int>(samplerate*sampletime);
    mWaveForm.resize(2*mSampleSize);
    mWaveForm.assign(2*mSampleSize,0);
    init();
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
    mHashTag ^= std::hash<int>() (mSampleRate);
    mHashTag ^= std::hash<int>() (mSampleSize);
    for (auto &f : mPartials)
        mHashTag ^= std::hash<double>() (f.second); // XOR over all peaks
}


bool Sound::computeNextFourierMode (WaveForm &sinewave, int rnd=0)
{
    if (mPointer==mPartials.end()) return false;
    double volume = sqrt(-(mPointer->first));
    const double f = mPointer->second * mFrequencyRatio;
//    std::lock_guard<std::mutex> lock(mMutex);
    if (f>24 and f<10000 and volume>0.0005)
    {
        const int64_t periods = round((mSampleSize * f) / mSampleRate);
        const int64_t leftphase  = rnd;
        const int64_t rightphase = leftphase + mPhaseDifference;
        const int64_t SineLength = sinewave.size();
        for (int64_t i=0; i<mSampleSize; ++i)
        {
            mWaveForm[2*i] += mLeftVolume * volume *
                sinewave[((i*periods*SineLength)/mSampleSize+leftphase)%SineLength];
            mWaveForm[2*i+1] += mRightVolume * volume *
                sinewave[((i*periods*SineLength)/mSampleSize+rightphase)%SineLength];
        }
    }
    mPointer++;
    return true;
}


















//=============================================================================
//                        Class for a sound library
//=============================================================================

SoundLibrary::SoundLibrary() :
    mSoundLibrary(),
    mSineLength(65536),
    mSineWave(mSineLength,0)
{}



//-----------------------------------------------------------------------------
//	                Pre-calculate the PCM waveform of a sound
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Pre-calculate the PCM waveform of a sound
///
/// The purpose of this function is mainly to save time and to ensure a
/// resonable performance of the synthesizer even on small mobile devices.
/// It pre-calculates the wave forms to be played and stores them in
/// the map mPreCalculatedSounds. The sounds are identified by an
/// integer identifier tag (id). The function generates the texture
/// of the sound for the specified sampletime at constant volume.
/// Pre-calculation does not involve any aspects of envelopes and
/// sound dynamics.
///
/// The frequency accuracy is limited by the total sampletime. For example,
/// if we generate a sound with a sampletime of 1 second, then it is expected
/// to differ up to 1 Hz from the true sound, leading to potential beats of
/// 1 second. For a quick survey a sampletime of 1 second would be
/// sufficient, but for longer times 5-10 seconds would be desirable.
///
/// \param id : Identification tag (usually keynumber + offset)
/// \param sound : The sound to be produced (frequency and spectrum)
/// \param sampletime : The total time of the pcm sample
/// \param waitingtime : Wait before starting in an idle state.
///////////////////////////////////////////////////////////////////////////////

void SoundLibrary::addSound (const int id, const Sound &sound, int samplerate, double sampletime)
{

    if (sound.getNumberOfPartials()==0) return;
    Sound &existingSound = mSoundLibrary[id];
    if (existingSound.getHashTag() != sound.getHashTag())
    {
        existingSound.set(sound,samplerate,sampletime);
//        sampledSound.startSampling (mAudioPlayer->getSamplingRate(),mSineWave,samplesize,waitingtime);
        LogI("*********** Added sound #%d  ",id);
    }
}



void SoundLibrary::workerFunction()
{
    setThreadName("WaveFormer");

    // Pre-calculate a sine wave for speedup
    mSineWave.resize(mSineLength);
    for (int i=0; i<mSineLength; ++i)
        mSineWave[i]=static_cast<float>(sin(MathTools::TWO_PI * i / mSineLength));


    while (not cancelThread())
    {
        bool idle = true;
        for (auto &libentry : mSoundLibrary)
        {
            Sound &sound = libentry.second;
            int rnd=0;
            bool newModeGenerated = sound.computeNextFourierMode (mSineWave,rnd);
            if (newModeGenerated) idle=false;
        }
        if (idle) msleep(20);
    }
}




const Sound::WaveForm SoundLibrary::getWaveForm(const int id)
{
    return mSoundLibrary[id].getWaveForm();
}
