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

#include "synthesizer.h"
#include "hammerknock.h"

#include "../../system/log.h"
#include "../../math/mathtools.h"

#include <random>
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
{}


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

Synthesizer::Synthesizer () :
    mNumberOfKeys(88),
    mPlayingTones(),
    mPlayingMutex(),
    mSineWave(),
    mHammerWaveLeft(),
    mHammerWaveRight(),
    mReverbSize(0),
    mReverbCounter(0),
    mReverbL(),
    mReverbR(),
    mIntensity(0)
{}


//-----------------------------------------------------------------------------
//	                Initialize and start the main thread
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Initialize and start the synthesizer.
///
/// This function initializes the synthesizer in that it pre-calculates a sine
/// function as well as the hammer noise and then starts the main loop
/// of the synthesizer in an independent thread.
///////////////////////////////////////////////////////////////////////////////

void Synthesizer::open (AudioInterface *audioInterface)
{
    PCMDevice::open(audioInterface);

    mSampleRate = getSampleRate();
    mChannels = getChannels();

    if (mNumberOfKeys < 0 or mNumberOfKeys > 256)
    { LogW("Called init with an invalid number of keys = %d",mNumberOfKeys); return; }

    // Pre-calculate a sine wave for speedup
    mSineWave.resize(SineLength);
    for (int i=0; i<SineLength; ++i)
        mSineWave[i]=static_cast<float>(sin(MathTools::TWO_PI * i / SineLength));

    // Pre-calculate a piano-hammer-like noise (one second), could be improved
    FFTComplexType amplitude (0.15/mSampleRate);
    int datasize = std::min(mSampleRate/2+1,static_cast<int>(mHammerKnockFFT[0].size()));
    FFTComplexVector fftl(mSampleRate/2+1,0),fftr(mSampleRate/2+1,0);
    for (int i=0; i<datasize; i++)
    {
        fftl[i] = FFTComplexType(mHammerKnockFFT[0][i],-mHammerKnockFFT[1][i])*amplitude;
        fftr[i] = FFTComplexType(mHammerKnockFFT[2][i],-mHammerKnockFFT[3][i])*amplitude;
    }
    FFT_Implementation transformer;
    mHammerWaveLeft.resize(mSampleRate);
    mHammerWaveRight.resize(mSampleRate);
    mHammerWaveLeft.assign(mSampleRate,0);
    mHammerWaveRight.assign(mSampleRate,0);
    transformer.calculateFFT(fftl,mHammerWaveLeft);
    transformer.calculateFFT(fftr,mHammerWaveRight);
    // Note that the wave size may change if the sample rate is odd
    // use data size instead
    for (size_t i=0; i<mHammerWaveLeft.size(); i++)
    {
        mHammerWaveRight[i] *= exp(-pow(i*3.0/mHammerWaveLeft.size(),1.5));
        mHammerWaveLeft[i] *= exp(-pow(i*3.0/mHammerWaveLeft.size(),1.5));
    }

    // Initialize reverb
    mReverbSize = mSampleRate/10;
    mReverbCounter = 0;
    mReverbL.resize(mReverbSize);
    mReverbR.resize(mReverbSize);
    mReverbL.assign(mReverbSize,0);
    mReverbR.assign(mReverbSize,0);
    mDelay1 = static_cast<int> (0.1128*mReverbSize);
    mDelay2 = static_cast<int> (0.3928*mReverbSize);
    mDelay3 = static_cast<int> (0.8762*mReverbSize);

    // Start the waveform generator
    mWaveformGenerator.init(mNumberOfKeys,mSampleRate);
    mWaveformGenerator.start();
}


//-----------------------------------------------------------------------------
//                          Set number of keys
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Tell the synthesizer to change the total number of keys
/// \param numberOfKeys : Number of keys
///////////////////////////////////////////////////////////////////////////////

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
        open (mAudioInterface);
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
    if (id>=0 and id<100) mWaveformGenerator.preCalculate(id, spectrum);
}


//-----------------------------------------------------------------------------
//	                             Play a sound
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Function which plays a single note (sound)
///
/// This function generates a sound according to the sound structure passed
/// as a parameter. If the waveform for the sound was already computed
/// the sound is played with the envelope specified by the parameter env.
/// If a pre-calculated waveform does not yet exist, then by default no
/// sound is played. However, if the flag waitforcomputation is set, the
/// function waits until the computation of the sound is completed,
/// forcing the sound to be played. This is particularly important for the
/// echo sound after recording.
///
/// \param keynumber : Number of the key
/// \param frequency : Frequency of the sound
/// \param volume : Volume of the sound
/// \param env : Envelope structure describing dynamics (ADSR-curve)
/// \param waitforcomputation : Wait until the sound has been computed
///////////////////////////////////////////////////////////////////////////////

void Synthesizer::playSound (const int keynumber,
                             const double frequency,
                             const double volume,
                             const Envelope &env,
                             const bool waitforcomputation,
                             const bool stereo)
{
    if (frequency <= 0 or volume <= 0 or mNumberOfKeys == 0) return;
    Tone tone;
    tone.keynumber = keynumber;
    tone.frequency = frequency;
    double position = (20+(keynumber&0xff)) * 1.0 / (mNumberOfKeys+40);
    if (not stereo) position = 0.5;
    tone.leftamplitude = sqrt((1-position)*volume);
    tone.rightamplitude = sqrt(position*volume);
    tone.phaseshift = (position-0.5)/500;
    tone.envelope = env;
    tone.clock=0;
    tone.stage=1;
    tone.amplitude=0;

    int timeout = 0;
    if (frequency>0 and frequency<10)
    {
        while (waitforcomputation and mWaveformGenerator.isComputing(keynumber)
               and timeout++ < 1000) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        tone.waveform = mWaveformGenerator.getWaveForm(keynumber);
    }
    else
        tone.waveform.clear();

    mPlayingMutex.lock();
    mPlayingTones.push_back(tone);
    mPlayingMutex.unlock();
}


//-----------------------------------------------------------------------------
//	                    update of the intensity
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Update function to update intensity
///
/// This function looks whether there are sounds in the mPlayingTones-queue.
/// If there are notes to play, it checks their volume.
/// The tones below a certain cutoff volume are removed from the queue.
/// All other notes will be played.
///
/// This function will be called in generateAudioSignal at the beginning
/// to update the current states of the played notes.
///////////////////////////////////////////////////////////////////////////////

void Synthesizer::updateIntensity()
{
    mPlayingMutex.lock();
    if (mPlayingTones.size()>0) mIntensity = 1;
    mPlayingMutex.unlock();

    if (mIntensity < 0.0000001)
    {
        mIntensity = 0;
    }
    else
    {
        // first remove all sounds with a volume below cutoff:
        mPlayingMutex.lock();
        for (auto it = mPlayingTones.begin(); it != mPlayingTones.end(); /* no inc */)
            if (it->stage>=2 and it->amplitude<CutoffVolume)
                it=mPlayingTones.erase(it);
            else ++it;
        //size_t N = mPlayingTones.size();
        mPlayingMutex.unlock();
    }
}

//-----------------------------------------------------------------------------
//	                        Read audio data is requested
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Generate waveform and write it to data
/// \param data Pointer to the data
/// \param max_bytes Maximum number of bytes that can be written
/// \return Number of written bytes
///
///////////////////////////////////////////////////////////////////////////////

int64_t Synthesizer::read(char *data, int64_t max_bytes)
{
    const int64_t packet_size = (max_bytes / sizeof(DataType)) / 2;
    if (generateAudioSignal(reinterpret_cast<DataType*>(data), packet_size)) {
        return packet_size * sizeof(DataType);
    }
    std::fill(data, data + max_bytes, 0);
    return max_bytes;
}

//-----------------------------------------------------------------------------
//	                        Generate the waveform
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Generate waveform.
/// \param outputBuffer The buffer where to write
/// \return True on success, false if the player should pause (no notes)
///
/// This is the heart of the synthesizer which composes the pre-calculated
/// waveforms.
///////////////////////////////////////////////////////////////////////////////

bool Synthesizer::generateAudioSignal (DataType *outputBuffer, const int64_t packet_size)
{
    // update intensity and played tones
    updateIntensity();

    if (mIntensity == 0 ) {
        return false;
    }

    const int sampleRate = mSampleRate;
    const int channels = mChannels;

    int64_t clock_timeout = 40*sampleRate; // one minute timeout
    int hammerwavesize = static_cast<int>(mHammerWaveLeft.size());

    if (channels<=0 or channels>2) return false;


    for (int bufferIndex = channels - 1; bufferIndex < packet_size; bufferIndex += channels) {
        // pcm data
        double left = 0, right = 0;

        mPlayingMutex.lock();
        for (Tone &tone : mPlayingTones)
        {
            //============== MANAGE THE ENVELOPE =================

            double y = tone.amplitude;          // get last amplitude
            Envelope &envelope = tone.envelope; // get ADSR
            switch (tone.stage)                 // Manage ADSR
            {
                case 1: // ATTACK
                        y += envelope.attack/sampleRate;
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
                        y *= (1-(1+y)*envelope.decay/sampleRate); // DECAY
                        if (y <= envelope.sustain) tone.stage++;
                        break;
                case 3: // SUSTAIN
                        y += (envelope.sustain-y) * envelope.release / sampleRate;
                        if (tone.clock > clock_timeout) tone.stage=4;
                        break;
                case 4: // RELEASE
                        y *= (1-envelope.release/sampleRate);
                        break;
            }
            tone.amplitude = y;
            tone.clock ++;

            //================ CREATE THE PCM WAVEFORM ==============

            if (tone.frequency > 10) // if sine wave
            {
                double t = 1+tone.clock*1.0/sampleRate;
                int i = static_cast<int64_t>(SineLength*tone.frequency*t);
                int j = static_cast<int64_t>(SineLength*tone.frequency*(t+tone.phaseshift));
                left  += tone.leftamplitude  * y * 0.2 * mSineWave[i%SineLength];;
                right += tone.rightamplitude * y * 0.2 * mSineWave[j%SineLength];;
            }
            else // if complex sound
            {
                if (envelope.hammer) if (tone.clock < hammerwavesize)
                {
                    left += tone.leftamplitude * mHammerWaveLeft[tone.clock];
                    int phaseshifted = static_cast<int>(tone.clock + tone.phaseshift * sampleRate);
                    if (phaseshifted > 0 and phaseshifted < hammerwavesize)
                        right += tone.rightamplitude* mHammerWaveRight[phaseshifted];
                }

                double t = (1+tone.clock*1.0/sampleRate)*tone.frequency;
                left += tone.leftamplitude * y * 0.3 *
                        mWaveformGenerator.getInterpolation(tone.waveform,t);
                right += tone.rightamplitude * y * 0.3 *
                         mWaveformGenerator.getInterpolation(tone.waveform,t+tone.phaseshift);
            }
        }

        const double reverbamplitude = 0.2;
        double echo1 = mReverbR[(mReverbCounter+mDelay1) % mReverbSize];
        double echo2 = mReverbL[(mReverbCounter+mDelay2) % mReverbSize];
        double echo3 = mReverbR[(mReverbCounter+mDelay3) % mReverbSize];
        double echo4 = mReverbL[(mReverbCounter+1) % mReverbSize];
        left  += reverbamplitude * ( echo2 + echo3 );
        right += reverbamplitude * ( echo1 + echo4 );
        mReverbL[mReverbCounter] = left;
        mReverbR[mReverbCounter] = right;
        mReverbCounter = (mReverbCounter+1) % mReverbSize;
        mIntensity = 0.98 * mIntensity + left*left + right*right;

        mPlayingMutex.unlock();

        // write data to buffer
        if (channels==1)
        {
            outputBuffer[bufferIndex] = static_cast<DataType>((left+right)/2 * std::numeric_limits<DataType>::max());
        }
        else // if stereo
        {
            outputBuffer[bufferIndex - 1] = static_cast<DataType>(left * std::numeric_limits<DataType>::max());
            outputBuffer[bufferIndex] = static_cast<DataType>(right * std::numeric_limits<DataType>::max());
        }

    }
    return true;
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
