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
//                              Audio base class
//=============================================================================

#include "audiobase.h"

//-----------------------------------------------------------------------------
//                               Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// The constructor initializes the default values for the
/// default sampling rate and the channels count for the sound device
///////////////////////////////////////////////////////////////////////////////

AudioBase::AudioBase()
    : mAudioDeviceName(),
      mSamplingRate(44100),
      mChannelCount(1)
{}


//-----------------------------------------------------------------------------
//                          Get the device name
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get a readable string of the name of the audio device.
/// \returns mAudioDeviceName.
///////////////////////////////////////////////////////////////////////////////

const std::string &AudioBase::getDeviceName() const
{ return mAudioDeviceName; }


//-----------------------------------------------------------------------------
//                          Set the device name
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Set the device name.
/// \param n : Name of the audio device.
///////////////////////////////////////////////////////////////////////////////

void AudioBase::setDeviceName(const std::string &n)
{ mAudioDeviceName = n; }


//-----------------------------------------------------------------------------
//                       Get the actual sampling rate
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get the actual sampling rate.
/// \returns mSamplingRate
///////////////////////////////////////////////////////////////////////////////

uint16_t AudioBase::getSamplingRate() const
{ return mSamplingRate; }


//-----------------------------------------------------------------------------
//                             Set sampling rate
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Allow the implementation to change the sampling rate during operation
///
/// Some audio devices do not support arbitrary sampling rates.
/// Therefore, the actual implementation of the audio device should
/// be able to change the sampling rate if necessary.
/// This function stores the new sampling rate in the local member variable.
/// \param rate : New sampling rate to be used.
///////////////////////////////////////////////////////////////////////////////

void AudioBase::setSamplingRate(uint16_t rate)
{ mSamplingRate = rate; }


//-----------------------------------------------------------------------------
//                          Get number of channels
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get the actual number of channels (1=mono, 2=stereo).
/// \returns mChannelCount
///////////////////////////////////////////////////////////////////////////////

uint8_t AudioBase::getChannelCount() const
{ return mChannelCount; }


//-----------------------------------------------------------------------------
//                          Set number of channels
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Set number of channels (1=mono, 2=stereo).
/// \param c : number of channels
///////////////////////////////////////////////////////////////////////////////

void AudioBase::setChannelCount(uint8_t c)
{ mChannelCount = c; }


//-----------------------------------------------------------------------------
//                         Set the device volume
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Sets the system volume of the audio device. Default is 1.
/// \param volume The volume
///////////////////////////////////////////////////////////////////////////////

void AudioBase::setVolume(double volume)
{ (void)volume; }


//-----------------------------------------------------------------------------
//                         Get the device volume
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Gets the system volume of the audio device. Default is 1.
/// \returns The volume
///////////////////////////////////////////////////////////////////////////////

double AudioBase::getVolume() const
{ return 1; }
