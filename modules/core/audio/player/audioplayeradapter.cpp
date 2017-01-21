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
//                         Adapter for audio output
//=============================================================================

#include "audioplayeradapter.h"

const double AudioPlayerAdapter::DefaultBufferSizeMilliseconds = 100;

//-----------------------------------------------------------------------------
//			                    Constructor
//-----------------------------------------------------------------------------

AudioPlayerAdapter::AudioPlayerAdapter() :
    mPCMWriter(nullptr),
    mMuted(false),
    mBufferSize(DefaultBufferSizeMilliseconds)
{
    setChannelCount(2); // Ouput stereo per default
}


//-----------------------------------------------------------------------------
//  			                    Exit
//-----------------------------------------------------------------------------

void AudioPlayerAdapter::exit()
{
    setWriter(nullptr);
}


//-----------------------------------------------------------------------------
//			          Set the current PCM writer interface
//               The old one will be exited, the new one initialized
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Connect the audio adapter to a PCM writer interface
///
/// The writer interface is a software component that produces sound.
/// In the EPT, the only component producing sound is the Synthesizer.
/// \param interface : Pointer to the PCM writer interface to be connected.
///////////////////////////////////////////////////////////////////////////////

void AudioPlayerAdapter::setWriter(PCMWriterInterface *writerInterface)
{
    // If another writer already exists destroy it
    if (mPCMWriter) mPCMWriter->exit();

    // Set the pointer to the new writer interface
    mPCMWriter = writerInterface;

    // Copy sampling rate and channel count and initialize the writer
    if (mPCMWriter) mPCMWriter->init(getSamplingRate(), getChannelCount());

    writerChanged(mPCMWriter);
}


