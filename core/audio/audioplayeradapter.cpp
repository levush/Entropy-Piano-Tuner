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

#include <iostream> // kann weg

const uint64_t AudioPlayerAdapter::MIN_BUFFER_SIZE_IN_MSECS = 100;


//-----------------------------------------------------------------------------
//			                    Constructor
//-----------------------------------------------------------------------------

AudioPlayerAdapter::AudioPlayerAdapter()
{
    setChannelCount(2);
}

//-----------------------------------------------------------------------------
//			          Set the current PCM writer interface
//               The old one will be exited, the new one initialized
//-----------------------------------------------------------------------------

void AudioPlayerAdapter::setWriter(PCMWriterInterface *interface)
{
    if (mPCMWriter)
    {
        mPCMWriter->exit();
    }

    mPCMWriter = interface;

    if (mPCMWriter) {
        mPCMWriter->init(getSamplingRate(), getChannelCount());
    }
}
