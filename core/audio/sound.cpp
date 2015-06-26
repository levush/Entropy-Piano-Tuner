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

Sound::Sound() : mSpectrum(),mVolume(0),mStereo(0),
                 mLeftVolume(0),mRightVolume(0)
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
/// \param samplerate : Sampling rate
/// \param sampletime : Time of the generated sample in seconds.
///////////////////////////////////////////////////////////////////////////////

Sound::Sound (const Spectrum &spectrum,
              const double stereo, const double volume) :
    mSpectrum(spectrum),
    mVolume(volume),
    mStereo(stereo)
{
    double norm = 0;
    for (auto &entry : spectrum) norm += entry.second;
    if (norm<=0)
    {
        LogW("Norm of the power spectrum non-positive: norm=%lf",norm);
        return;
    }
    for (auto &y : mSpectrum) y.second /= norm;
    mLeftVolume  = mVolume * sqrt(0.9-0.8*mStereo);
    mRightVolume = mVolume * sqrt(0.1+0.8*mStereo);
}


//=============================================================================
//                  Structure describing an envelope
//=============================================================================

//-----------------------------------------------------------------------------
//	                             Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Envelope::Envelope
/// \param attack : Rate of initial volume increase in units of 1/sec.
/// \param decay : Rate of the subsequent volume decrease in units of 1/sec.
///        If this rate is zero the decay phase is omitted and the volume
/// increases directly towards the sustain level controlled by the attack rate.
/// \param sustain : Level at which the volume saturates after decay in (0..1).
/// \param release : Rate at which the sound disappears after release in
/// units of 1/sec.
/// \param hammer : Volume of a hammer-like noise at the beginning
///////////////////////////////////////////////////////////////////////////////

Envelope::Envelope(double attack, double decay,
                   double sustain, double release, double hammer) :
    attack(attack),
    decay(decay),
    sustain(sustain),
    release(release),
    hammer(hammer)
{};



