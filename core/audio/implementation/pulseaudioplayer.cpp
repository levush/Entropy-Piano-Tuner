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

#include "pulseaudioplayer.h"

#if CONFIG_ENABLE_PULSEAUDIO

#include <pulse/error.h>
#include "../../system/log.h"
#include "iostream"

//------------------ Initialize the audio output -----------------------

void PulseAudioPlayer::init ()
{
    // Pulse audio format specification for output:
    static pa_sample_spec out;
    out.format = PA_SAMPLE_FLOAT32LE;	 // use 32 bit floating point in [-1,+1]
    out.rate = getSamplingRate();	     // Sample rate
    out.channels = getChannelCount();    // Channels

    // Pulse audio low latency settings for reading (magic):
    static pa_buffer_attr outattr;
    outattr.tlength   = pa_usec_to_bytes(50*1000, &out);
    outattr.minreq    = pa_usec_to_bytes(0, &out);
    outattr.maxlength = pa_usec_to_bytes(50*1000, &out);
    outattr.fragsize  = sizeof(uint32_t) -1;

    // start the pulse audio output stream:
    int errorcode=0;	                // error variable
    outstream = pa_simple_new (nullptr, "ENTROPY TUNER PLAYBACK",
                               PA_STREAM_PLAYBACK, nullptr,"playback",
                               &out, nullptr, &outattr, &errorcode);
    if (outstream) { INFORMATION("Pulse audio player initialized"); }
    else { ERROR("Player: Could not open pulse audio output stream: %s",pa_strerror(errorcode)); }

}


//-------------------- Shut down pulse audio output --------------------

void PulseAudioPlayer::exit ()
{
    if (outstream)
    {
        pa_simple_free (outstream);
        outstream = nullptr;
        INFORMATION("Pulse audio player shutdown");
    }
}




void PulseAudioPlayer::start ()
{
    mStarted = true;
    // set default free buffer size, so that the synthesizer know how much can be written
    // this is constant during runtime, since in the pulse audio player the pa_simple_write
    // function returns when the sound has been played. This behaviour is different in other
    // audio libraries (e.g. Qt)

    // I dont understand yet why size has to be + 1 to work
    setFreeBufferSize(getMinBufferSamples() + 1);
}

void PulseAudioPlayer::stop ()
{
    mStarted = false;
    setFreeBufferSize(0);
}


//======================================================================
//                          Write a packet
//======================================================================

void PulseAudioPlayer::write (const PacketType &packet)
{
    if (!mStarted) {return;}

    int errorcode=0;
    std::vector<float> copy(packet.size());
    for (size_t i=0; i<packet.size(); ++i) copy[i]=packet[i];

//    std::ofstream os;
//    os.open("00.dat",std::fstream::app);
//    for (auto &c : copy) os << c << "\n";
//    os.close();

    int result = pa_simple_write(outstream, copy.data(), (size_t) copy.size()*sizeof(float), &errorcode);
    if (result < 0) {
        ERROR("Could not write to pulse audio stream: ");
        ERROR(pa_strerror(errorcode));
    }
}


//======================================================================
//                          flush buffer
//======================================================================

void PulseAudioPlayer::flush ()
{
    if (!mStarted) {return;}

    pa_simple_flush(outstream,nullptr);
}

#endif  // CONFIG_ENABLE_PULSEAUDIO
