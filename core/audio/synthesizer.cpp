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


#include "synthesizer.h"

//#include <algorithm>
#include <random>

#include "../system/log.h"
#include "../math/mathtools.h"


Envelope::Envelope(double attack, double decay, double sustain, double release, double hammer) :
    attack(attack), decay(decay), sustain(sustain), release(release), hammer(hammer) {};


//=============================================================================
//                               SYNTHESIZER
//=============================================================================



/// \param attack : Rate of initial volume increase in units of 1/sec.
/// \param decayrate : Rate of the subsequent volume decrease in units of 1/sec.
///        If this rate is zero the decay phase is omitted and the volume
/// increases directly towards the sustain level controlled by the attack rate.
/// \param sustain : Level at which the volume saturates after decay in (0..1).
/// \param release : Rate at which the sound disappears after release in units of 1/sec.



//-----------------------------------------------------------------------------
//	                             Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor, intitializes the member variables.
///
/// \param audioadapter : Pointer to the implementation of the AudioPlayer
///////////////////////////////////////////////////////////////////////////////

Synthesizer::Synthesizer (AudioPlayerAdapter *audioadapter) :
    mSoundCollection(),
    mPlayingTones(),
    mPlayingMutex(),
    mSineWave(),
    mHammerWave(),
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
        double f=2*3.14159/samplerate;
        for (size_t i=0; i<samplerate; i++)
            {
                mHammerWave[i] = 2*d(gen);
                mHammerWave[i] *= exp(-f*0.5*i);;
            }
    }
    else LogW("Could not start synthesizer: AudioPlayer not connected.");
}


void Synthesizer::registerSound  (const int id,
                                  const Sound &sound,
                                  const double sampletime,
                                  const double waitingtime)
{
    if (sound.mSpectrum.size()==0) return;
    SampledSound &sampledSound = mSoundCollection[id];
    if (sampledSound.differsFrom(sound))
    {
        sampledSound.set(sound);
        sampledSound.startSampling (mAudioPlayer->getSamplingRate(),mSineWave,sampletime,waitingtime);
        LogI("*********** Added sound #%d  ",id);
    }
}


//-----------------------------------------------------------------------------
//	                          Play a new sound
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Create a new sound (note).
///
/// This function creates or new (or recreates an existing) sound.
///
/// \param id : Identification tag or the sound (usually the keynumber).
/// \param frequency : Frequency of lowest partial
/// \param spectrum : The discrete power spectrum (frequency - power).
///                   The base frequency of the spectrum is irrelevant.
///                   If the spectrum is empty a sine wave is produced.
/// \param stereo : Stereo position ranging from 0 (left) to 1 (right).
/// \param volume : Overall volume of the sound (intensity of keypress)
///                 with typical values between 0 and 1.
/// \param stereo : Stereo position of the sound, ranging from 0 (left) to 1 (right).
/// \param envelope : Reference to the envelope structure (ADSR-data).
///////////////////////////////////////////////////////////////////////////////

void Synthesizer::playSound (const int id, const Sound &sound, const Envelope &env, const double sampletime, const int maxplaytime)
{
    Tone tone;
    tone.id=id;
    tone.sound = sound;
    tone.envelope = env;


    if (sound.mSpectrum.size()>0)
    {
        // Complex sound
        if (tone.waveform.size()==0)
        {
            SampledSound &sampledSound = mSoundCollection[id];
            if (sampledSound.isReady())tone.waveform = sampledSound.getWaveForm();
            else registerSound(id,sound,sampletime,0);
        }
        tone.frequency= 0;
    }
    else
    {
        // SineWave
        tone.frequency = static_cast<int_fast64_t>(100.0*sound.mFrequency*SineLength);
    }
    tone.clock=0;
    tone.clock_timeout = mAudioPlayer->getSamplingRate() * maxplaytime; // max duration 1 minute
    tone.stage=1;

    mPlayingMutex.lock();
    mPlayingTones.push_back(tone);
    mPlayingMutex.unlock();
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
        mPlayingMutex.lock();
        bool active = (mAudioPlayer and mPlayingTones.size()>0);
        mPlayingMutex.unlock();
        if (not active) msleep(10); // Synthesizer in idle state
        else
        {
            // first remove all sounds with a volume below cutoff:
            mPlayingMutex.lock();
            for (auto it = mPlayingTones.begin(); it != mPlayingTones.end(); /* no inc */)
                if (it->stage>=2 and it->amplitude<CutoffVolume)
                    it=mPlayingTones.erase(it);
                else ++it;
            size_t N = mPlayingTones.size();
            mPlayingMutex.unlock();

            // then generate the audio signal:
            if (N>0) generateAudioSignal();
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
    int_fast64_t SampleRate = mAudioPlayer->getSamplingRate();
    int_fast64_t cycle = 100L * SampleRate;
    int channels = mAudioPlayer->getChannelCount();
    if (channels<=0 or channels>2) return;

    int writtenSamples = 0;

    while (mAudioPlayer->getFreeSize()>=2 and writtenSamples < 10)
    {
        ++writtenSamples;
        double left=0, right=0;
        mPlayingMutex.lock();
        for (Tone &tone : mPlayingTones)
        {
            if (tone.frequency==0 and tone.waveform.size()==0)
                if (mSoundCollection[tone.id].isReady())
                        tone.waveform = mSoundCollection[tone.id].getWaveForm();
            if (tone.frequency>0 or tone.waveform.size()>0)
            {
                //============== MANAGE THE ENVELOPE =================
                double y = tone.amplitude;          // get last amplitude
                Envelope &envelope = tone.envelope; // get ADSR
                //Sound &sound = tone.sound;
                double volume = tone.sound.mVolume;
                double stereo = tone.sound.mStereo;
                switch (tone.stage)                 // Manage ADSR
                {
                    case 1: // ATTACK
                            y += envelope.attack*volume/SampleRate;
                            if (envelope.decay>0)
                            {
                                if (y >= volume) tone.stage++;
                            }
                            else
                            {
                                if (y >= envelope.sustain*volume) tone.stage+=2;
                            }
                            break;
                    case 2: // DECAY
                            y *= (1-envelope.decay/SampleRate); // DECAY
                            if (y <= envelope.sustain*volume) tone.stage++;
                            break;
                    case 3: // SUSTAIN
                            y += (envelope.sustain-y) * envelope.release/SampleRate;
                            if (tone.clock > tone.clock_timeout) tone.stage=4;
                            break;
                    case 4: // RELEASE
                            y *= (1-envelope.release/SampleRate);
                            break;
                }
                tone.amplitude = y;
                tone.clock ++;

                if (tone.frequency>0)
                {
                    double sinewave = y * volume * mSineWave[((tone.frequency*tone.clock)/cycle)%SineLength];
                    left += (1-stereo) * sinewave;
                    right += stereo * sinewave;
                }
                else
                {
                    if (envelope.hammer) if (tone.clock < static_cast<int>(mHammerWave.size()/2-1))
                    {
                        left += 0.2 * volume* (1-stereo) * mHammerWave[2*tone.clock];
                        right += 0.2 * volume *  stereo * mHammerWave[2*tone.clock+1];
                    }

                    int size = tone.waveform.size();
                    if (size>0)
                    {
                        left  += y*tone.waveform[(2*tone.clock)%size];
                        right += y*tone.waveform[(2*tone.clock+1)%size];
                    }
                }
            }
        }

        mPlayingMutex.unlock();
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

const Tone* Synthesizer::getSchedulerPointer (const int id) const
{
    const Tone *snd(nullptr);
    mPlayingMutex.lock();
    for (auto &ch : mPlayingTones) if (ch.id==id) { snd=&ch; break; }
    mPlayingMutex.unlock();
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

void Synthesizer::releaseSound (const int id)
{
    bool released=false;
    mPlayingMutex.lock();
    for (auto &ch : mPlayingTones) if (ch.id%100==id%100) { ch.stage=4; released=true; }
    mPlayingMutex.unlock();
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

bool Synthesizer::isPlaying (const int id) const
{ return (getSchedulerPointer(id) != nullptr); }


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

void Synthesizer::ModifySustainLevel (const int id, const double level)
{
    if (id) if (level) {};
//    auto snd = getSchedulerPointer(id);
//    mPlayingMutex.lock();
//    if (snd) snd->envelope.sustain = level;
//    else LogW ("id does not exist");
//    mPlayingMutex.unlock();
}




