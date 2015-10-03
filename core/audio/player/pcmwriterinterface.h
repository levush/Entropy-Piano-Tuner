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

#ifndef PCMWRITERINTERFACE
#define PCMWRITERINTERFACE

#include "../audiobase.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Abstract base class for sound-producing software components which
/// transmit data to the AudioPlayerAdapter.
///
/// This class is the base class for any derived class that produces sound
/// and sends it to the audio device. In the EPT the Synthesizer is the only
/// class that produces sound. This is a header file only without
/// implementation (cpp) file.
/// \see AudioPlayerAdapter
/// \see AudioPlayerForQt
///////////////////////////////////////////////////////////////////////////////

class PCMWriterInterface
{
public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Initialize the PCM-Writer with given sample rate and channel number
    /// \param sampleRate : The sample rate
    /// \param channels : The number of channels
    ///////////////////////////////////////////////////////////////////////////

    virtual void init(const int sampleRate, const int channels)
    { mSampleRate = sampleRate; mChannels = channels; }

    virtual void exit()=0;      ///< Exit, shut down writer interface

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Function that generates new sound and copies it to the outputPacket
    ///
    /// This function is called by the implementation of the AudioPlayer
    /// as a request to generate new sound and to copy it to the
    /// outputPacket passed by reference.
    /// \param outputPacket The output packet (newly generated sound) to be filled.
    /// The length of the packet can be chosen freely.
    /// \returns True on success, false if the player should pause.
    ///////////////////////////////////////////////////////////////////////////

    virtual bool generateAudioSignal (AudioBase::PacketType &outputPacket) = 0;

protected:
    int mSampleRate = 0;    ///< Sample rate
    int mChannels = 0;      ///< Number of channels
};

#endif // PCMWRITERINTERFACE
