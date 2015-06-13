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
//                               SYNTHESIZER
//=============================================================================

#include "synthesizer.h"

#include <algorithm>
#include <random>

#include "../system/log.h"
#include "../math/mathtools.h"
#include "../system/eptexception.h"
#include "../system/timer.h"

//-----------------------------------------------------------------------------
//	                             Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor, intitializes the member variables.
///
/// \param audioadapter : Pointer to the implementation of the AudioPlayer
///////////////////////////////////////////////////////////////////////////////

Synthesizer::Synthesizer (AudioPlayerAdapter *audioadapter) :
    mSineWave(),
    mHammerWave(),
    mScheduler(),
    mAudioPlayer(audioadapter)
{}


//-----------------------------------------------------------------------------
//	                      Initialize and start thread
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Initialize and start the synthesizer.
///
/// This function initializes the synthesizer in that it pre-calculates a sine
/// function and starts the main loop of the synthesizer in an indpendent thread.
///////////////////////////////////////////////////////////////////////////////

void Synthesizer::init ()
{
    if (mAudioPlayer)
    {
        // Pre-calculate a sine wave for speedup
        mSineWave.resize(SineLength);
        for (int i=0; i<SineLength; ++i)
            mSineWave[i]=(float)(sin(MathTools::TWO_PI * i / SineLength));

        // Pre-calculate the hammer noise (one second)
        size_t samplerate = mAudioPlayer->getSamplingRate();
        mHammerWave.resize(samplerate);
        mHammerWave.assign(samplerate,0);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> d(-1,1);
        double y=0, f=2*3.14159/samplerate;
        for (size_t i=0; i<samplerate; i++)
        {       y = - 0.1*y + d(gen);
                mHammerWave[i]=
                (sin(f*11.0*i)+sin(f*17.0*i)+sin(f*19.0*i)
                 +sin(f*23.0*i)-sin(f*29.0*i)+sin(f*37.0*i)
                 +0.1*y)
                *exp(-f*0.5*i);
        }
    }
    else LogW("Could not start synthesizer: AudioPlayer not connected.");
}


//-----------------------------------------------------------------------------
//	                             Shut down
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Stop the synthesizer, request its execution thread to terminate.
///////////////////////////////////////////////////////////////////////////////

void Synthesizer::exit ()
{
    stop();
}



//-----------------------------------------------------------------------------
//                        Register a complex sound
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Register a complex sound
///
/// This function registers a complex sound for future use. T
/// \param id : The identification tag or the sound (usually the keynumber).
/// \param frequency : Frequency of lowest partial to be set
/// \param spectrum : The discrete power spectrum (frequency - power).
/// \param stereo : Stereo location ranging from 0 (left) to 1 (right).
/// \param time : Time of the generated PCM sample in seconds.
///////////////////////////////////////////////////////////////////////////////

void Synthesizer::registerSound  (const int id, const double frequency,
                                  const Spectrum &spectrum,
                                  const double stereo,
                                  const double time)
{
    ComplexSound &sound = mSoundCollection[id];
    if (sound.coincidesWith(spectrum))
    {
        std::cout << "*********** Not necessary to add sound " << id << std::endl;
        return;
    }
    sound.stop(); // if necessary interrupt ongoing computation
    sound.init(frequency, spectrum, stereo, mAudioPlayer->getSamplingRate(), mSineWave, time);
    std::cout << "*********** Added sound #  " << id << std::endl;
}


//-----------------------------------------------------------------------------
//	                          Play a new sound
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Create a new sound (note).
///
/// This function creates or new (or recreates an existing) sound.
/// \param id : Identifier of the sound (usually the piano key number)
/// \param f : Frequency in Hz. If f>0 a pure sine function is generated. For
/// f=0 the program fetches a complex sound from the SoundCollection.
/// \param volume : Overall volume of the sound (intensity of keypress)
/// with typical values between 0 and 1.
/// \param stereo : Stereo position of the sound, ranging from 0 (left) to 1 (right).
/// \param attack : Rate of initial volume increase in units of 1/sec.
/// \param decayrate : Rate of the subsequent volume decrease in units of 1/sec.
/// If this rate is zero the decay phase is omitted and the volume
/// increases directly towards the sustain level controlled by the attack rate.
/// \param sustain : Level at which the volume saturates after decay in (0..1).
/// \param release : Rate at which the sound disappears after release in units of 1/sec.
///////////////////////////////////////////////////////////////////////////////

void Synthesizer::play (int id, double f, double volume, double stereo,
        double attack, double decayrate, double sustain, double release)
{
    Tone tone;
    tone.id=id;
    tone.amplitude=0;
    tone.clock=0;
    tone.stage=0;
    tone.volume=volume;
    tone.stereo=stereo;
    tone.attack=attack;
    tone.decayrate=decayrate;
    tone.sustain=sustain;
    tone.release=release;
    tone.stage = 1;
    tone.frequency = static_cast<int_fast64_t>(100.0*f*SineLength);

    if (f==0) tone.waveform = mSoundCollection[id].getWaveForm();
    mSchedulerMutex.lock();
    mScheduler.push_back(tone);
    mSchedulerMutex.unlock();
}


//-----------------------------------------------------------------------------
//	                    Main thread (worker function)
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Main thread of the synthesizer
///////////////////////////////////////////////////////////////////////////////

void Synthesizer::workerFunction (void)
{
    setThreadName("Synthesizer");
    while (not cancelThread())
    {
        mSchedulerMutex.lock();
        bool active = (mAudioPlayer and mScheduler.size()>0);
        mSchedulerMutex.unlock();
        if (not active) msleep(10); // Synthesizer in idle state
        else
        {
            // first remove all sounds with a volume below cutoff:
            mSchedulerMutex.lock();
            for (auto it = mScheduler.begin(); it != mScheduler.end(); /* no inc */)
                if (it->stage>=2 and it->amplitude<CutoffVolume)
                    it=mScheduler.erase(it);
                else ++it;
            mSchedulerMutex.unlock();

            // then generate the audio signal:
            generateAudioSignal();
        }
    }
}



//-----------------------------------------------------------------------------
//	                        Generate the waveform
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Generate waveform.
///
/// This is the heart of the synthesizer. It fills the circular buffer
/// until it reaches the maximum size. It consists of two parts.
/// First the envelope is computed, rendering the actual amplitude of the
/// sound. Then a loop over all Fourier modes is carried out and a sine
/// wave with the corresponding frequency is added to the buffer.
/// \param snd : Reference of the sound to be converted.
///////////////////////////////////////////////////////////////////////////////

void Synthesizer::generateAudioSignal ()
{
    mSchedulerMutex.lock();
    size_t N = mScheduler.size();
    mSchedulerMutex.unlock();

    // If there is nothing to play return
    if (N==0) return;

    int_fast64_t SampleRate = mAudioPlayer->getSamplingRate();
    int_fast64_t cycle = 100L * SampleRate;
    int channels = mAudioPlayer->getChannelCount();
    if (channels<=0 or channels>2) return;

    int writtenSamples = 0;

    while (mAudioPlayer->getFreeSize()>=2 and writtenSamples < 10)
    {
        ++writtenSamples;
        double left=0, right=0;
        mSchedulerMutex.lock();
        for (auto &ch : mScheduler)
        {
            //int id = ch.id;
            double y = ch.amplitude;       // get last amplitude
            switch (ch.stage)          // Manage ADSR
            {
                case 1: // ATTACK
                        y += ch.attack*ch.volume/SampleRate;
                        if (ch.decayrate>0)
                        {
                            if (y >= ch.volume) ch.stage++;
                        }
                        else
                        {
                            if (y >= ch.sustain*ch.volume) ch.stage+=2;
                        }
                        break;
                case 2: // DECAY
                        y *= (1-ch.decayrate/SampleRate); // DECAY
                        if (y <= ch.sustain*ch.volume) ch.stage++;
                        break;
                case 3: // SUSTAIN
                        y += (ch.sustain-y) * ch.release/SampleRate;
                        break;
                case 4: // RELEASE
                        y *= (1-ch.release/SampleRate);
                        break;
            }
            ch.amplitude = y;
            ch.clock ++;


            if (ch.frequency>0)
            {
                double sinewave = y * ch.volume * mSineWave[((ch.frequency*ch.clock)/cycle)%SineLength];
                left += (1-ch.stereo) * sinewave;
                right += ch.stereo * sinewave;
            }
            else
            {
                if (ch.clock < static_cast<int>(mHammerWave.size()/2-1))
                {
                    left += 0.2 * ch.volume* (1-ch.stereo) * mHammerWave[2*ch.clock];
                    right += 0.2 * ch.volume *  ch.stereo * mHammerWave[2*ch.clock+1];
                }

                int size = ch.waveform.size();
                if (size>0)
                {
                    left  += y*ch.waveform[(2*ch.clock)%size];
                    right += y*ch.waveform[(2*ch.clock+1)%size];
                }
            }
        }
        mSchedulerMutex.unlock();
        if (channels==1)
        {
            mAudioPlayer->pushSingleSample(static_cast<AudioBase::PacketDataType>((left+right)/2));
        }
        else // if stereo
        {
            mAudioPlayer->pushSingleSample(static_cast<AudioBase::PacketDataType>(left));
            mAudioPlayer->pushSingleSample(static_cast<AudioBase::PacketDataType>(right));
        }
    }
}


//-----------------------------------------------------------------------------
// 	                      Get sound (private)
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get a pointer to the sound addressed by a given ID.
///
/// Note that this function has to be mutexed.
///
/// \param id : Identifier of the sound
///////////////////////////////////////////////////////////////////////////////

Synthesizer::Tone* Synthesizer::getSchedulerPointer (int id)
{
    Tone *snd(nullptr);
    mSchedulerMutex.lock();
    for (auto &ch : mScheduler) if (ch.id==id) { snd=&ch; break; }
    mSchedulerMutex.unlock();
    return snd;
}


//-----------------------------------------------------------------------------
// 	                         Terminate a sound
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Terminate a sound.
///
/// This function forces the sound to fade out, entering the release phase.
/// \param id : identity tag of the sound (number of key).
///////////////////////////////////////////////////////////////////////////////

void Synthesizer::releaseSound (int id)
{
    //Tone *snd(nullptr);
    bool released=false;
    mSchedulerMutex.lock();
    for (auto &ch : mScheduler) if (ch.id==id) { ch.stage=4; released=true; }
    mSchedulerMutex.unlock();
    if (not released) LogW("Sound #%d does not exist.",id);
}


//-----------------------------------------------------------------------------
// 	             Check whether a certain sound is still active
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Check whether a sound with given id is still playing.
///
/// \param id : Identifier of the sound
/// \return Boolean telling whether the sound is still playing.
///////////////////////////////////////////////////////////////////////////////

bool Synthesizer::isPlaying (int id)
{
    return (getSchedulerPointer(id) != nullptr);
}


//-----------------------------------------------------------------------------
// 	                       Change the sustain level
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Change the level (sustain level) of a constantly playing sound
///
/// This function changes the sustain volume of a constantly playing sound
/// The synthesizer will adjust to the new volume adiabatically with the
/// respective decay rate.
///
/// \param id : Identity tag of the sound (number of key).
/// \param level : New sustain level in (0...1).
///////////////////////////////////////////////////////////////////////////////

void Synthesizer::ModifySustainLevel (int id, double level)
{
    auto snd = getSchedulerPointer(id);
    mSchedulerMutex.lock();
    if (snd) snd->sustain = level;
    else LogW ("id does not exist");
    mSchedulerMutex.unlock();
}




