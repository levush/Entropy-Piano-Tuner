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
#include <random>

#include "../system/simplethreadhandler.h"

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
///////////////////////////////////////////////////////////////////////////////

class Sound
{
public:

    using Spectrum = std::map<double,double>;   // type of spectrum
    using Partials = std::map<double,double>;   // type of partials list
    using WaveForm = std::vector<float>;        // type of stereo waveform

    Sound();
    ~Sound(){}

    Sound (const Spectrum &spectrum,
           const double stereo, const double volume,
           const int samplerate, const double sampletime);

//        void set (const Partials &spectrum,
//                  const double stereo, const double volume,
//                  const int samplerate, const double sampletime);

//        void set (const Sound &sound,
//                  const int samplerate, const double sampletime);

    void init ();

    //bool computeNextFourierMode (const WaveForm &sinewave, const int phase);

    int getNumberOfPartials() const { return mPartials.size(); }
    int getSampleSize() const { return mSampleSize; }
    void setSampleSize(const int size) { mSampleSize = size; }
    double getStereoLocation() const { return mStereo; }
    double getVolume() const { return mVolume; }
    const Partials &getPartials() const { return mPartials; }
    const WaveForm &getWaveForm() const { return mWaveForm; }


private:
    Partials mPartials;         ///< spectrum of partials
    Partials::iterator mPointer;///< pointer to partials
    double mStereo;             ///< stereo position in [0,1]
    double mVolume;             ///< overall volume in [0,1]
    double mLeftVolume;         ///< left volume (stereo)
    double mRightVolume;        ///< right volume (stereo)
    int64_t mPhaseDifference;   ///< stereo phase difference

    int mSampleRate;            ///< Sample rate
    int64_t mSampleSize;        ///< Number of stereo sample pairs
    WaveForm mWaveForm;         ///< Computed wave form

    void setPartials(const Spectrum &spectrum); // flip spectrum
};




//=============================================================================
//                          Class for a sound library
//=============================================================================

class SoundLibrary : public SimpleThreadHandler
{
public:
    SoundLibrary();
    ~SoundLibrary(){}

    void init() { start(); }
    void exit() { stop(); }

    const Sound::WaveForm getWaveForm (const int id);

private:
    std::map <int,Sound> mSoundLibrary;
    std::mutex mSoundLibraryMutex;
    bool mMutexUnlockingRequest;
    const int64_t mSineLength;
    Sound::WaveForm mSineWave;


    std::default_random_engine rnd;
    std::uniform_int_distribution<int> uniform;


};

#endif // SOUND_H
