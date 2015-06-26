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

    Sound();
    Sound (const Spectrum &spectrum,
           const double volume,
           const double stereo);
    ~Sound(){}

    int getNumberOfPartials()     const { return mSpectrum.size(); }
    double getVolume()            const { return mVolume; }
    double getStereoLocation()    const { return mStereo; }
    const Spectrum &getSpectrum() const { return mSpectrum; }

private:
    Spectrum mSpectrum;         ///< spectrum of partials
    double mVolume;             ///< overall volume in [0,1]
    double mStereo;             ///< stereo position in [0,1]

    double mLeftVolume;         ///< left volume (stereo)
    double mRightVolume;        ///< right volume (stereo)
};


//=============================================================================
//                  Structure describing an envelope
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
/// \brief Structure describing the envelope (dynamics) of a sound
///////////////////////////////////////////////////////////////////////////////

struct Envelope
{
    double attack;      ///< Initial attack rate
    double decay;       ///< Subsequent decay rate
    double sustain;     ///< Sustain level
    double release;     ///< Release rate
    double hammer;      ///< Intensity of hammer noise

    Envelope(double attack=0, double decay=0,
             double sustain=0, double release=0,
             double hammer=0);
};

#endif // SOUND_H
