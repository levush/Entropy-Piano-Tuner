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
#include <thread>

const uint64_t AudioPlayerAdapter::MIN_BUFFER_SIZE_IN_MSECS = 10;


//-----------------------------------------------------------------------------
//			                    Constructor
//-----------------------------------------------------------------------------

AudioPlayerAdapter::AudioPlayerAdapter(RawDataWriter *writer) :
    mBuffer(1000)
{
    setChannelCount(2);
}


//-----------------------------------------------------------------------------
//			   Transmit a single PCM value, wait if not ready
//                  To be called by the user (synthesizer)
//-----------------------------------------------------------------------------

void AudioPlayerAdapter::writeSample (AudioBase::PacketDataType s)
{
    mBufferMutex.lock();
    size_t free = mBuffer.maximum_size()-mBuffer.size();
    if (free>0) mBuffer.push_back(s);
    mBufferMutex.unlock();
    if (free==0) std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

//-----------------------------------------------------------------------------
//                      Get a packet of a certain size
//                    To be called by the implementation
//-----------------------------------------------------------------------------

AudioBase::PacketType & AudioPlayerAdapter::getPacket (size_t n)
{
    mBufferMutex.lock();
    AudioBase::PacketType packet = mBuffer.readData(n);
    mBufferMutex.unlock();
    return packet;
}



//void AudioPlayerAdapter::setRawDataWriter(RawDataWriter *writer) {
//    mWriter = writer;
//    if (mWriter) {
//        start();
//    } else {
//        stop();
//    }
//}
