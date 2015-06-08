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

#ifndef AUDIOPLAYERADAPTER_H
#define AUDIOPLAYERADAPTER_H

#include "audiobase.h"
#include "circularbuffer.h"

#include <mutex>
#include <cstdint>

class RawDataWriter;

///////////////////////////////////////////////////////////////////////////////
/// \brief Abstract adapter class for playing audio signals
///
/// This class has to be implemented by the actual sound device implementation.
///
/// The sound device implementation tells the adapter how large the available
/// buffer size is. Depending on the implementation this may vary with time.
///
/// With the function 'write' it is possible to write a raw audio data
/// to the stream. You are only write a packet with a size that does not
/// exceed mFreeBufferSize.
///////////////////////////////////////////////////////////////////////////////

class AudioPlayerAdapter : public AudioBase
{
public:
    /// Minimal buffer size in milliseconds:
    static const uint64_t MIN_BUFFER_SIZE_IN_MSECS;

public:
    AudioPlayerAdapter();
    virtual ~AudioPlayerAdapter() {};

//    void setRawDataWriter(RawDataWriter *writer);

    size_t getSize (void);
    size_t getFreeSize (void);
    size_t getMaximalSize (void);
    void   setMaximalSize(size_t s);
    void pushSingleSample (AudioBase::PacketDataType);

//    virtual void start() {};
//    virtual void stop() {};
//    virtual void init() {};
//    virtual void exit() {};


public:

    PacketType getPacket(size_t n);

private:

    CircularBuffer<AudioBase::PacketDataType> mBuffer;
    std::mutex mBufferMutex;

    //RawDataWriter *mWriter;                     ///< Writer of the pcm data
};

#endif // AUDIOPLAYERADAPTER_H
