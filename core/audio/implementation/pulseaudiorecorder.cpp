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

#include "pulseaudiorecorder.h"

#if CONFIG_ENABLE_PULSEAUDIO
#include "../../system/log.h"
#include <iostream>


//-------------------- Initialize pulse audio input --------------------

void PulseAudioRecorder::init ()
{
    // Pulse audio format specification for input:
    static pa_sample_spec in;
    in.format = PA_SAMPLE_FLOAT32LE;	// use 32 bit floating point in [-1,+1]
    in.rate = getSamplingRate();        // default sample rate
    in.channels = 1;        // mono recording

    // Pulse audio settings for reading
    static pa_buffer_attr inattr;
    inattr.maxlength=(uint32_t)-1;	    // default value
    inattr.tlength  =(uint32_t)-1;	    // default value
    inattr.prebuf   =(uint32_t)-1;      // default value
    inattr.minreq   =(uint32_t)-1;      // default value
    inattr.fragsize =BufferSize;         // improved recording latency

    int errorcode=0;	                // error variable
    instream = pa_simple_new (nullptr, "ENTROPY TUNER RECORDING",
                              PA_STREAM_RECORD, nullptr,"record",
                              &in, nullptr, &inattr, &errorcode);

    if (instream) { LogI("Pulse audio recorder initialized"); }
    else { LogE("Recorder: Could not open pulse audio input stream: %s",pa_strerror(errorcode)); }
}


//--------------------- Shut down pulse audio input --------------------

void PulseAudioRecorder::exit ()
{
    // we have to stop the thread first!
    SimpleThreadHandler::stop();

    if (instream)  {
        pa_simple_free (instream);
        instream = nullptr;
        LogI("Pulse audio recorder shutdown");
    }
}


//----------------------- Start the audio recorder ----------------------

void PulseAudioRecorder::start ()
{
    std::cout << "starting pulse audio recorder thread" << std::endl;
    SimpleThreadHandler::start();
}


//----------------------- Stop the audio recorder -----------------------

void PulseAudioRecorder::stop ()
{
    std::cout << "stopping pulse audio recorder thread" << std::endl;
    SimpleThreadHandler::stop();
}



void PulseAudioRecorder::workerFunction()
{
    setThreadName("PulseAudioRecorder");
    std::vector<float> buffer;
    buffer.resize(BufferSize);
    while (not cancelThread())
    {
        int errorcode=0;
        int result = pa_simple_read(instream, (void*)(buffer.data()), (size_t) BufferSize*sizeof(float), &errorcode);
        if (result < 0) { LogE("Could not read from audio stream: %s",pa_strerror(errorcode));}
        else pushRawData(buffer);
    }
}

#endif  // CONFIG_ENABLE_PULSEAUDIO


