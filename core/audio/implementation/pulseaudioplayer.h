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

//======================================================================
//                Pulse audio output implementation
//======================================================================

#ifndef PULSEAUDIOPLAYER_H
#define PULSEAUDIOPLAYER_H

#include "../../config.h"
#if CONFIG_ENABLE_PULSEAUDIO

#include "../audioplayeradapter.h"
#include "../../system/simplethreadhandler.h"
#include <pulse/simple.h>   // Pulse-Audio simplified interface


class PulseAudioPlayer : public AudioPlayerAdapter, public SimpleThreadHandler
{
public:
    PulseAudioPlayer(void*) {}          ///< Constructor
    ~PulseAudioPlayer() {}              ///< Desctructor
public:
    void init ()  override final;
    void exit () override final;
    void start ()  override final;
    void stop ()  override final;

    void workerFunction() {};


    void write (const PacketType &packet);
    void flush ();

    //int getSampleRate () { return SampleRate; };

private:
    pa_simple* outstream = nullptr;     ///< Pulse audio output stream
    bool mStarted = false;              ///< is the player started
    //const int OptimalPacketSize = 1024; ///< Optimal packet size for reading

};

#endif  // CONFIG_ENABLE_PULSEAUDIO

#endif  // PULSEAUDIOPLAYER_H
