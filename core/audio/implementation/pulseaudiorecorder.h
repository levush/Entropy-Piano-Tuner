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
//                  Pulse audio input implementation
//======================================================================

#ifndef PULSEAUDIORECORDER_H
#define PULSEAUDIORECORDER_H

#include "../../config.h"

#if CONFIG_ENABLE_PULSEAUDIO

#include "../audiorecorderadapter.h"
#include "../../system/simplethreadhandler.h"
#include <pulse/simple.h>   // Pulse-Audio simplified interface
#include <pulse/error.h>	// Pulse-Audio error handling

class PulseAudioRecorder : public AudioRecorderAdapter, public SimpleThreadHandler
{
public:
    PulseAudioRecorder(void *) {};
    ~PulseAudioRecorder() {};

public:
    void init () override final;
    void exit () override final;
    void start () override final;
    void stop () override final;

    void workerFunction() override final;

    //int getSampleRate () { return SampleRate; };

private:

    pa_simple* instream = nullptr;      ///< Pulse audio input stream

    const int BufferSize = 1024;
};

#endif  // CONFIG_ENABLE_PULSEAUDIO
#endif  // PULSEAUDIORECORDER_H
