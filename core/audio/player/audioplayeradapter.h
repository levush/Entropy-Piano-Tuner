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
//                        Adapter for audio playback
//=============================================================================

#ifndef AUDIOPLAYERADAPTER_H
#define AUDIOPLAYERADAPTER_H

#include "pcmwriterinterface.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Abstract adapter class for PCM-based audio output drivers.
///
/// This class has to be implemented by the actual sound system. In the
/// EPT this is the AudioPlayerForQt
///
/// Note that the sound-producing component (the EPT synthesizer) does not call
/// the audio player by itself to deliver data, instead the AudioPlayer
/// manages the workflow and requests data from the sound-producing component.
///
/// To establish this mechanism the sound-producing component (the synthesizer)
/// has to be derived from an abstract class called PCMWriterInterface.
/// The request is carried out by calling a function named generateAudioPacket
/// which is defined virtually in the PCMWriterInterface.
/// \see AudioPlayerForQt
/// \see PCMWriterInterface
///
/// The present class does nothing but holding a pointer to the
/// PCMWriterInterface. In addition it handels a mute flag.
///////////////////////////////////////////////////////////////////////////////

class AudioPlayerAdapter : public AudioBase
{
public:
    AudioPlayerAdapter();                           ///< Constructor, resetting default values
    virtual ~AudioPlayerAdapter() {}                ///< Destructor without functionality

    virtual void exit() override;

    void setWriter (PCMWriterInterface *interface);
    PCMWriterInterface *getWriter() {return mPCMWriter;} ///< Get pointer to the PCM writer

    void setMuted (bool muted) { mMuted = muted; }  ///< Mute the audio player.
    bool isMuted() { return mMuted; }               ///< Returns true if the player is muted
private:
    PCMWriterInterface *mPCMWriter;                 ///< Pointer to PCM writer
    bool   mMuted;                                  ///< Flag which is true if the ouput device muted
};

#endif // AUDIOPLAYERADAPTER_H
