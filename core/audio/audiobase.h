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

#ifndef AUDIOBASE_H
#define AUDIOBASE_H

#include <vector>
#include <cstdint>
#include <string>
#include "../system/prerequisites.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Abstract base class for handling audio signals
///
/// This class defines the basics properties of an audio input or output stream.
/// It defines the basic samling rate, the used channels and the audio format.
///////////////////////////////////////////////////////////////////////////////

class AudioBase
{
public:
    /// Data type for a packet: floating point PCM value in [0,1].
    typedef double PacketDataType;

    /// Type definition for a packet.
    typedef std::vector<PacketDataType> PacketType;

public:
    AudioBase();                        ///< Constructor
    virtual ~AudioBase() {}             ///< Destructor (no functionality).


    virtual void init() = 0;            ///< Inizialize the audio device.
    virtual void exit() = 0;            ///< Destroy the audio device.

    virtual void start() = 0;           ///< Start/restart the audio device.
    virtual void stop()  = 0;           ///< Stop the audio device.

    const std::string &getDeviceName() const;     // get the device name
    void setDeviceName(const std::string &n);     // set the device name
    uint16_t getSamplingRate() const;             // get actual sampling rate
    virtual void setSamplingRate(uint16_t rate);  // set sampling rate
    uint8_t  getChannelCount() const;             // get number of channels
    virtual void setChannelCount(uint8_t c);      // set number of channels

    // Adjust the volume of the audio device.
    virtual void setVolume(double volume);         // set the volume
    virtual double getVolume() const;              // get the volume

private:
    std::string mAudioDeviceName;       ///< User readable string of the used audio device
    uint16_t mSamplingRate;             ///< Current sampling rate
    uint8_t mChannelCount;              ///< Current channel count
};

#endif // AUDIOBASE_H
