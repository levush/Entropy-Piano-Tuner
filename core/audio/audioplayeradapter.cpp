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
//                         Adapter for audio output
//=============================================================================

#include "audioplayeradapter.h"

#include <assert.h>
#include <vector>

const uint64_t AudioPlayerAdapter::MIN_BUFFER_SIZE_IN_MSECS = 10;


//-----------------------------------------------------------------------------
//			                    Constructor
//-----------------------------------------------------------------------------

AudioPlayerAdapter::AudioPlayerAdapter()
    : mFreeBufferSize(0)
{
    setChannelCount(2);
}


//-----------------------------------------------------------------------------
//                        get the free buffer size
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get the available free buffer size.
///
/// \return Free buffer size
///////////////////////////////////////////////////////////////////////////////

uint64_t AudioPlayerAdapter::getFreeBufferSize() const
{
    std::lock_guard<std::mutex> lock(mDataAccessMutex);
    return mFreeBufferSize;
}


//-----------------------------------------------------------------------------
//            return the minimal number of samples to be written
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get the minimal number of samples to be written.
///
/// The AudioPlayerAdapter does not accept arbitrarily small amounts of data.
/// This function returns the minimal number of samples that have to be
/// transmitted. They are computed on the basis of the constant
/// MIN_BUFFER_SIZE_IN_MSECS
///
/// \return Minimal number of sampels
///////////////////////////////////////////////////////////////////////////////

uint64_t AudioPlayerAdapter::getMinBufferSamples() const
{
    return (getChannelCount() * getSamplingRate() *
            AudioPlayerAdapter::MIN_BUFFER_SIZE_IN_MSECS) / 1000;
}


//-----------------------------------------------------------------------------
//                         set the free buffer size
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Set the free buffer size.
///
/// Called by the implementation if new buffer space is available.
/// \param size : Size to which the buffer size is set.
///////////////////////////////////////////////////////////////////////////////

void AudioPlayerAdapter::setFreeBufferSize(uint64_t size)
{
    std::lock_guard<std::mutex> lock(mDataAccessMutex);
    mFreeBufferSize = size;
}


//-----------------------------------------------------------------------------
//                        increase the free buffer size
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Increase the available buffer size.
///
/// Called by the implementation if new buffer space is available.
/// \param size : Size by which the buffer is increased.
///////////////////////////////////////////////////////////////////////////////

void AudioPlayerAdapter::addFreeBufferSize(uint32_t size)
{
    std::lock_guard<std::mutex> lock(mDataAccessMutex);
    mFreeBufferSize += size;
}


//-----------------------------------------------------------------------------
//                       decrease the free buffer size
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Decrease the available buffer size.
///
/// Called by the implementation as soon as the buffer space shrinks.
/// \param size : Size by which the buffer is decreased.
///////////////////////////////////////////////////////////////////////////////

void AudioPlayerAdapter::shrinkFreeBufferSize(uint32_t size) {
    std::lock_guard<std::mutex> lock(mDataAccessMutex);
    mFreeBufferSize -= size;
}
