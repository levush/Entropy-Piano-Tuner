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

#include "../system/log.h"
#include "../math/mathtools.h"

#include <iostream>


//=============================================================================
//                  Structure describing an envelope
//=============================================================================

//-----------------------------------------------------------------------------
//	                             Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Envelope::Envelope
/// \param attack : Rate of initial volume increase in units of 1/sec.
/// \param decay : Rate of the subsequent volume decrease in units of 1/sec.
///        If this rate is zero the decay phase is omitted and the volume
/// increases directly towards the sustain level controlled by the attack rate.
/// \param sustain : Level at which the volume saturates after decay in (0..1).
/// \param release : Rate at which the sound disappears after release in
/// units of 1/sec.
/// \param hammer : Volume of a hammer-like noise at the beginning
///////////////////////////////////////////////////////////////////////////////

Envelope::Envelope(double attack, double decay,
                   double sustain, double release, double hammer) :
    attack(attack),
    decay(decay),
    sustain(sustain),
    release(release),
    hammer(hammer)
{};


//=============================================================================
//                             CLASS SYNTHESIZER
//=============================================================================

//-----------------------------------------------------------------------------
//	                             Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor, intitializes the member variables.
///
/// \param audioadapter : Pointer to the implementation of the AudioPlayer
///////////////////////////////////////////////////////////////////////////////

Synthesizer::Synthesizer (AudioPlayerAdapter *audioadapter) :
    mNumberOfKeys(88),
    mPlayingTones(),
    mPlayingMutex(),
    mSineWave(),
    mHammerWave(),
    mAudioPlayer(audioadapter)
{}


//-----------------------------------------------------------------------------
//	                Initialize and start the main thread
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Initialize and start the synthesizer.
///
/// This function initializes the synthesizer in that it pre-calculates a sine
/// function as well as the hammer noise and then starts the main loop
/// of the synthesizer in an indpendent thread.
///////////////////////////////////////////////////////////////////////////////

void Synthesizer::init ()
{
    if (mNumberOfKeys < 0 or mNumberOfKeys > 256)
    { LogW("Called init with an invalid number of keys = %d",mNumberOfKeys); return; }
    else if (not mAudioPlayer)
    { LogW("Could not start synthesizer: AudioPlayer not connected."); return; }

    // Pre-calculate a sine wave for speedup
    mSineWave.resize(SineLength);
    for (int i=0; i<SineLength; ++i)
        mSineWave[i]=static_cast<float>(sin(MathTools::TWO_PI * i / SineLength));

    // Pre-calculate a piano-hammer-like noise (one second), could be improved
    mSampleRate = mAudioPlayer->getSamplingRate();
    mHammerWave.resize(mSampleRate);
    mHammerWave.assign(mSampleRate,0);
    for (int i=0; i<mSampleRate; i++)
        mHammerWave[i]=0.2*sin(2*3.141592655*i*18.0/mSampleRate)/pow(2,i*5.0/mSampleRate);
    for (int i=0; i<mSampleRate; i++)
        mHammerWave[i]+=0.2*sin(2*3.141592655*i*27.0/mSampleRate)/pow(2,i*5.0/mSampleRate);
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0,0.1);
    for (int i=0; i<mSampleRate; i++)
        mHammerWave[i]+=distribution(generator)/pow(2,i*10.0/mSampleRate);

    mWaveformGenerator.init(mNumberOfKeys,mSampleRate);
    mWaveformGenerator.start();
}


//-----------------------------------------------------------------------------
//                          Set number of keys
//-----------------------------------------------------------------------------

void Synthesizer::setNumberOfKeys (int numberOfKeys)
{
    if (numberOfKeys < 0 or numberOfKeys > 255)
    {
        LogW("Unreasonable number of keys = %d.",numberOfKeys);
        return;
    }
    else if (numberOfKeys != mNumberOfKeys)
    {
        mNumberOfKeys = numberOfKeys;
        init ();
    }
}

//-----------------------------------------------------------------------------
//	                Pre-calculate the PCM waveform of a sound
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Pre-calculate the PCM waveform of a sound
///
/// The purpose of this function is mainly to save time and to ensure a
/// resonable performance of the synthesizer even on small mobile devices.
/// It pre-calculates the wave forms to be played and stores them in
/// the map SoundLibrary. The sounds are identified by an
/// integer identifier tag (id). The function generates the texture
/// of the sound for the specified sampletime at constant volume.
/// Pre-calculation does not involve any aspects of envelopes and
/// sound dynamics.
///
/// The frequency accuracy is limited by the total sampletime. For example,
/// if we generate a sound with a sampletime of 1 second, then it is expected
/// to differ up to 1 Hz from the true sound, leading to potential beats of
/// 1 second. For a quick survey a sampletime of 1 second would be
/// sufficient, but for longer times 5-10 seconds would be desirable.
///
/// \param id : Identification tag (usually keynumber + offset)
/// \param sound : The sound to be produced (frequency and spectrum)
///////////////////////////////////////////////////////////////////////////////

void Synthesizer::preCalculateWaveform  (const int id,
                                         const Spectrum &spectrum)
{
    if (id>=0 and id<88) mWaveformGenerator.preCalculate(id, spectrum);
}


//-----------------------------------------------------------------------------
//	                             Play a sound
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Function which plays a single note (sound)
///
/// This function generates a sound according to the sound structure passed
/// as a parameter. If the sound happens to coincide with a previously
/// calculated sound marked by the identifier id, then the pre-calculated
/// wave form is loaded and played immediately with the envelope
/// specified by the parameter env. If a pre-calculated waveform does not
/// yet exist a short sample is played and a longer calculation is
/// scheduled for later.
///
/// If the sound contains an empty spectrum, a simple sine wave with the
/// fundamental frequency is played.
///
/// \param id : Identification tag or the sound (usually the keynumber).
/// \param sound : Sound structure describing statics (frequency and spectrum)
/// \param env : Envelope structure describing dynamics (ADSR-curve)
///////////////////////////////////////////////////////////////////////////////

void Synthesizer::playSound (const int keynumber,
                             const double frequency,
                             const double volume,
                             const Envelope &env,
                             const bool waitforcomputation)
{
    if (frequency <= 0 or volume <= 0 or mNumberOfKeys == 0) return;
    Tone tone;
    tone.keynumber = keynumber;
    tone.frequency = frequency;
    double stereo = (10+(keynumber&0xff)) * 1.0 / (mNumberOfKeys+20);
    tone.leftamplitude = sqrt((1-stereo)*volume);
    tone.rightamplitude = sqrt(stereo*volume);
    tone.phaseshift = (stereo-0.5)/500;
    tone.envelope = env;
    tone.clock=0;
    tone.stage=1;
    tone.amplitude=0;

    if (frequency>0 and frequency<10)
    {
        if (waitforcomputation and mWaveformGenerator.isComputing(keynumber)) msleep(1);
        tone.waveform = mWaveformGenerator.getWaveForm(keynumber);
    }
    else
        tone.waveform.clear();

    mPlayingMutex.lock();
    mPlayingTones.push_back(tone);
    mPlayingMutex.unlock();
}


//-----------------------------------------------------------------------------
//	                    Main thread (worker function)
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Main thread of the synthesizer
///
/// This workerFunction is the main thread of the synthesizer. It is running
/// in an endless loop until the synthesizer is stopped. It looks whether
/// there is an audio device and sounds in the mPlayingTones-queue. If not,
/// the synthesizer waits in an idle state. If there are notes to play,
/// it checks their volume. The tones below a certain cutoff volume are
/// removed from the queue. All other notes will be played. The playing
/// is done in a separate function called generateAudioSignal().
///////////////////////////////////////////////////////////////////////////////

void Synthesizer::workerFunction ()
{
    setThreadName("Synthesizer");
    while (not cancelThread())
    {
        mPlayingMutex.lock();
        bool active = (mAudioPlayer and mPlayingTones.size()>0);
        mPlayingMutex.unlock();
        if (not active) msleep(5); // Synthesizer in idle state
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
/// This is the heart of the synthesizer which composes the pre-calculated
/// waveforms.
///////////////////////////////////////////////////////////////////////////////

void Synthesizer::generateAudioSignal ()
{
    int64_t mSampleRate = mAudioPlayer->getSamplingRate();
    int64_t clock_timeout = 60*mSampleRate; // one minute timeout

    int channels = mAudioPlayer->getChannelCount();
    if (channels<=0 or channels>2) return;

    int writtenSamples = 0;

    while (mAudioPlayer->getFreeSize()>=2 and writtenSamples < 10)
    {
        ++writtenSamples;
        double left=0, right=0; // PCM
        mPlayingMutex.lock();
        for (Tone &tone : mPlayingTones)
        {
            //============== MANAGE THE ENVELOPE =================

            double y = tone.amplitude;          // get last amplitude
            Envelope &envelope = tone.envelope; // get ADSR
            switch (tone.stage)                 // Manage ADSR
            {
                case 1: // ATTACK
                        y += envelope.attack/mSampleRate;
                        if (envelope.decay>0)
                        {
                            if (y >= 1) tone.stage++;
                        }
                        else
                        {
                            if (y >= envelope.sustain) tone.stage+=2;
                        }
                        break;
                case 2: // DECAY
                        y *= (1-(1+y)*envelope.decay/mSampleRate); // DECAY
                        if (y <= envelope.sustain) tone.stage++;
                        break;
                case 3: // SUSTAIN
                        y += (envelope.sustain-y) * envelope.release / mSampleRate;
                        if (tone.clock > clock_timeout) tone.stage=4;
                        break;
                case 4: // RELEASE
                        y *= (1-envelope.release/mSampleRate);
                        break;
            }
            tone.amplitude = y;
            tone.clock ++;

            //================ CREATE THE PCM WAVEFORM ==============

            if (tone.frequency > 10) // if sine wave
            {
                double t = 1+tone.clock*1.0/mSampleRate;
                int i = static_cast<int64_t>(SineLength*tone.frequency*t);
                int j = static_cast<int64_t>(SineLength*tone.frequency*(t+tone.phaseshift));
                left  += tone.leftamplitude  * y * 0.2 * mSineWave[i%SineLength];;
                right += tone.rightamplitude * y * 0.2 * mSineWave[j%SineLength];;
            }
            else // if complex sound
            {
//                if (envelope.hammer) if (tone.clock < static_cast<int>(mHammerWave.size()/2-1))
//                {
//                    left += 0.2 * volume* (1-stereo) * mHammerWave[2*tone.clock];
//                    right += 0.2 * volume *  stereo * mHammerWave[2*tone.clock+1];
//                }

                double t = (1+tone.clock*1.0/mSampleRate)*tone.frequency;
                left += tone.leftamplitude * y * 0.3 *
                        mWaveformGenerator.getInterpolation(tone.waveform,t);
                right += tone.rightamplitude * y * 0.3 *
                        mWaveformGenerator.getInterpolation(tone.waveform,t+tone.phaseshift);
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
// 	                      Get tone pointer (private)
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get a pointer to the sound according to the given ID.
///
/// \param id : Identifier of the sound, nullptr if not found
///////////////////////////////////////////////////////////////////////////////

const Tone* Synthesizer::getSoundPointer (const int id) const
{
    const Tone *snd(nullptr);
    mPlayingMutex.lock();
    for (auto &ch : mPlayingTones) if (ch.keynumber==id) { snd=&ch; break; }
    mPlayingMutex.unlock();
    return snd;
}

Tone* Synthesizer::getSoundPointer (const int id)
{
    Tone *snd(nullptr);
    mPlayingMutex.lock();
    for (auto &ch : mPlayingTones) if (ch.keynumber==id) { snd=&ch; break; }
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
    for (auto &ch : mPlayingTones) if ((ch.keynumber & 0xff)==id) { ch.stage=4; released=true; }
    mPlayingMutex.unlock();
    if (not released) LogW("Release: Sound with id=%d does not exist.",id);
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
{ return (getSoundPointer(id) != nullptr); }


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
    Tone* snd = getSoundPointer(id);
    if (snd)
    {
        mPlayingMutex.lock();
        snd->envelope.sustain = level;
        mPlayingMutex.unlock();
    }
    else LogW ("Cannot modify sustain level: id %d does not exist",id);
}
