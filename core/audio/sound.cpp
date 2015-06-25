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
#include <mutex>

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
    mPartials(),
    mStereo(0),
    mVolume(0),
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
    mLeftVolume  = mVolume * sqrt(0.9-0.8*mStereo);
    mRightVolume = mVolume * sqrt(0.1+0.8*mStereo);
    mPhaseDifference = round(mSampleRate * (0.5-mStereo) / 200);
}

//-----------------------------------------------------------------------------
//                  Converter from spectrum to partial list
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Convert the spectrum into a list of partials sorted by intensity
///
/// The calculation of the wave forms takes time. Therefore, the generation
/// starts with the most important Fourier modes. To this end, the spectrum
/// (list of peaks), which is usually organized as a map from frequency ->
/// intensity is flipped, mapping -intensity -> frequency. In this way
/// the most intense partials are in front of the map.
///
/// \param spectrum : List of peaks (frequency -> intensity)
///////////////////////////////////////////////////////////////////////////////

void Sound::setPartials (const Spectrum &spectrum)
{
    mPartials.clear();
    double norm = 0;
    for (auto &entry : spectrum) norm += entry.second;
    if (norm<=0) return;
    mPartials = spectrum;
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
/// \param samplerate : Sampling rate
/// \param sampletime : Time of the generated sample in seconds.
///////////////////////////////////////////////////////////////////////////////

Sound::Sound (const Spectrum &spectrum,
              const double stereo, const double volume,
              const int samplerate, const double sampletime) :
    mPartials(),
    mStereo(stereo),
    mVolume(volume),
    mSampleRate(samplerate),
    mSampleSize(static_cast<int>(samplerate*sampletime)),
    mWaveForm(2*mSampleSize,0)
{
    setPartials (spectrum);
    init();
}




////-----------------------------------------------------------------------------
////                    Calculate the next Fourier mode
////-----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////
///// \brief Calculate the next Fourier mode
///// \param sinewave : Reference to a pre-calculated sine wave for acceleration
///// \param phase : total phase shift of the Fourier component (should be random)
///// \return true if a mode was constructed, false if nothing to do
/////////////////////////////////////////////////////////////////////////////////

//bool Sound::computeNextFourierMode (const WaveForm &sinewave, const int phase=0)
//{
//    if (mPointer==mPartials.end()) return false;
//    double volume = sqrt(-(mPointer->first));
//    const double f = mPointer->second;
//    if (f>24 and f<10000 and volume>0.0005)
//    {
//        const int64_t periods = round((mSampleSize * f) / mSampleRate);
//        const int64_t leftphase  = phase;
//        const int64_t rightphase = leftphase + mPhaseDifference;
//        const int64_t SineLength = sinewave.size();
//        for (int64_t i=0; i<mSampleSize; ++i)
//        {
//            mWaveForm[2*i] += mLeftVolume * volume *
//                sinewave[((i*periods*SineLength)/mSampleSize+leftphase)%SineLength];
//            mWaveForm[2*i+1] += mRightVolume * volume *
//                sinewave[((i*periods*SineLength)/mSampleSize+rightphase)%SineLength];
//        }
//    }
//    mPointer++;
//    return true;
//}




//=============================================================================
//                        Class for a sound library
//=============================================================================

//-----------------------------------------------------------------------------
//	                            Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor
///////////////////////////////////////////////////////////////////////////////

SoundLibrary::SoundLibrary() :
    mSoundLibrary(),
    mSoundLibraryMutex(),
    mMutexUnlockingRequest(false),
    mSineLength(65536),
    mSineWave(mSineLength,0),
    rnd(),
    uniform(0,mSineLength-1)
{}






//-----------------------------------------------------------------------------
//	            Function for getting the precalculated waveform
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Function for getting the precalculated waveform
/// \param id : Identity tag of the waveform
/// \return Copy of the waveform
///////////////////////////////////////////////////////////////////////////////

const Sound::WaveForm SoundLibrary::getWaveForm(const int id)
{
    std::lock_guard<std::mutex> lock(mSoundLibraryMutex);
    return mSoundLibrary[id].getWaveForm();
}
