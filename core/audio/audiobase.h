/*****************************************************************************
 * Copyright 2016 Haye Hinrichsen, Christoph Wick
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
/// \brief Abstract base class for audio interfaces
///
/// This class defines the basics properties of an audio input or output
/// interface. It defines the sampling rate, the number of used channels
/// and the name of the audio device. Moreover, it defines the basic
/// data type used by the EPT to represent PCM audio signals.
/// \see AudioRecorderAdapter and AudioPlayerAdapter
///////////////////////////////////////////////////////////////////////////////

class AudioBase
{
public:
    /// Floating point data type for a single PCM Value. The PCM values are
    /// assumed to be in [-1,1].
    typedef double PCMDataType;

    /// Type definition of a PCM packet (vector of PCM values).
    typedef std::vector<PCMDataType> PacketType;

public:
    AudioBase();                        ///< Constructor
    virtual ~AudioBase() {}             ///< Destructor (no functionality).

    virtual void init() = 0;            ///< Inizialize the audio device.
    virtual void exit() = 0;            ///< Destroy the audio device.

    virtual void start() = 0;           ///< Start/restart the audio device.
    virtual void stop()  = 0;           ///< Stop the audio device.

    const std::string &getDeviceName() const;   // get the device name
    void setDeviceName(const std::string &n);   // set the device name
    int getSamplingRate() const;                // get actual sampling rate
    virtual void setSamplingRate(int rate);     // set actual sampling rate
    int getChannelCount() const;                // get number of channels
    virtual void setChannelCount(int cnt);      // set number of channels

private:
    std::string mAudioDeviceName;       ///< User-readable string of the used audio device
    int mSamplingRate;                  ///< Current sampling rate
    int mChannelCount;                  ///< Current channel count
};

#endif // AUDIOBASE_H
