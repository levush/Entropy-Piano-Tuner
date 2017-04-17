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

//=============================================================================
//                     PCM Waveform generator using FFTW3
//=============================================================================

#include "waveformgenerator.h"

#include <random>
#include "../../math/mathtools.h"
#include "../../system/log.h"
#include "core/system/platformtoolscore.h"

//-----------------------------------------------------------------------------
//                               Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Constructor, resetting the member variables
///////////////////////////////////////////////////////////////////////////////

WaveformGenerator::WaveformGenerator () :
    mWaveformTime(convertAvailablePhysicalMemoryToWaveformTime()),
    mNumberOfKeys(),
    mLibrary(),
    mLibraryMutex(256),
    mComputing(),
    mIn(mWaveformSize/2+1),
    mOut(mWaveformSize),
    mFFT(),
    mQueue()
{
    LogI("Using waveforms of length %.2f seconds.", mWaveformTime);
}


//-----------------------------------------------------------------------------
//                                  Init
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Initialization procedure of the waveform generator.
///
/// This function initializes the waveform generator. It depends on the
/// number of keys and the actual output sampling rate. If these parameters
/// are changed during runtime, the waveform generator needs to be reinitialized.
/// The function basically allocates the memory needed for waveform generation.
///
/// \param numberOfKeys : Total number of keys
/// \param samplerate : Actual sampling rate of the ouput device
///////////////////////////////////////////////////////////////////////////////

void WaveformGenerator::init (int numberOfKeys, int samplerate)
{
    EptAssert(numberOfKeys > 0 and numberOfKeys < 256,
              "Number of keys out of range");
    EptAssert(samplerate > 0, "Range of sample rate invalid");

    mNumberOfKeys = numberOfKeys;
    mLibrary.resize(mNumberOfKeys);
    mSampleRate = samplerate;
    mWaveformSize = mWaveformTime * mSampleRate;
    mIn.resize(mWaveformSize/2+1);
    mOut.resize(mWaveformSize);

    for (auto &wave : mLibrary)
    {
        wave.resize(mWaveformSize);
        wave.assign(mWaveformSize,0);
    }
    mComputing.resize(mNumberOfKeys);
    mComputing.assign(mNumberOfKeys,false);
}


//-----------------------------------------------------------------------------
//                           pre-calculate waveform
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief pre-calculate a waveform
///
/// Whenever a new spectrum has been recorded or a file has been loaded, the
/// EPT asks the WaveformGenerator to pre-calculate the waveform. This
/// pre-calculation is first stored in a local queue in order to free the
/// calling thread as soon as possible.
///
/// \param keynumber : The number of the key to which the spectrum belongs
/// \param spectrum : Spectrum as a map from frequency to intensity
///////////////////////////////////////////////////////////////////////////////

void WaveformGenerator::preCalculate(int keynumber, const Spectrum &spectrum)
{
    if (spectrum.size()==0) return;
    mQueueMutex.lock();
    mQueue[keynumber] = spectrum;
    mComputing[keynumber] = true;
    invokeCallback(&WaveformGeneratorStatusCallback::queueSizeChanged, mQueue.size(), mComputing.size());
    mQueueMutex.unlock();
}


//-----------------------------------------------------------------------------
//                          get pre-calculated waveform
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Mutexed getter function to obtain the calculated waveform.
/// \param keynumber : Number of the key (registration id)
/// \return Waveform as a vector of floats
///////////////////////////////////////////////////////////////////////////////

WaveformGenerator::Waveform WaveformGenerator::getWaveForm (const int keynumber)
{
    if (keynumber < 0 or keynumber >= mNumberOfKeys) return Waveform();
    std::lock_guard<std::mutex> lock(mLibraryMutex[keynumber]);
    return mLibrary[keynumber];
}


//-----------------------------------------------------------------------------
//                              PCM interpolation
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get an a linearly interpolated value of the waveform
///
/// In this class the generated waveforms are stored in the pitch of the
/// original recording. After tuning different pitches are needed. This is
/// achieved in the Synthesizer by resampling the pre-calculated PCM signal
/// around the mean sampling rate. To this end one needs a function which
/// interpolates the waveform between neighboring indices.
///
/// \param W : The waveform as a vector of floats
/// \param t : Continuous time
/// \return Interpolated PCM value
///////////////////////////////////////////////////////////////////////////////

float WaveformGenerator::getInterpolation (const Waveform &W, const double t)
{
    float realindex = t*mWaveformSize/mWaveformTime;
    int index = static_cast<int> (realindex);
    float leftvalue =  W[index%mWaveformSize];
    return leftvalue + (realindex-index)*(W[(index+1)%mWaveformSize]-leftvalue);
}


//-----------------------------------------------------------------------------
//                        Check if computation is ready
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Find out whether the WaveformGenerator is still computing
///
/// In some cases, for example when playing the echo sound after recording,
/// it is necessary to re-calculate the waveform and to make sure that the
/// new waveform (and not the previous one) is played. This means that one
/// has to wait for the computation to finish -- a task accomplished by ths
/// function.
/// \param keynumber : Number of the key
/// \return : True if the WaveformGenerator is still computing this key.
///////////////////////////////////////////////////////////////////////////////

bool WaveformGenerator::isComputing (const int keynumber)
{
    std::lock_guard<std::mutex> lock(mQueueMutex);
    return mComputing[keynumber];
}

//-----------------------------------------------------------------------------
//                             Main thread function
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Main thread function for wavefrom generation
///
/// This thread checks the queue of pre-calculation requests. If there is such
/// a request it is removed from the queue and a new wave form is computed.
/// The generation is done by using FFTW3. Each peak gets a complex random
/// phase in order to avoid a synchronous "click" at the beginning.
///////////////////////////////////////////////////////////////////////////////

void WaveformGenerator::workerFunction()
{
    setThreadName("Waveformer");
    LogI("Waveform generator thread started");
    Spectrum spectrum;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0,MathTools::PI*2);
    int counter = 0;
    while (not cancelThread())
    {
        int keynumber = -1;

        // Check the queue for new keys to be computed
        mQueueMutex.lock();
        if(not mQueue.empty())
        {
            auto element = mQueue.begin();
            keynumber = element->first;
            spectrum = element->second;
            mQueue.erase(element);
        }
        mQueueMutex.unlock();

        // If so, compute the waveform
        if (keynumber >= 0 and keynumber<=mNumberOfKeys and spectrum.size()>0)
        {
            if (counter == 0) LogI("Waveform generator starting in background")
            counter ++;
            double norm=0;
            for (auto &partial : spectrum) norm += partial.second;
            mIn.assign(mWaveformSize/2+1,0);
            if (norm>0)
            {
                for (auto &partial : spectrum)
                {
                    const double frequency = partial.first;
                    const double intensity = sqrt(partial.second / norm);
                    int k = MathTools::roundToInteger(frequency*mWaveformTime);
                    if (k>0 and k<mWaveformSize/2+1)
                    {
                        std::complex<double> phase(0,distribution(generator));
                        mIn[k] = exp(phase) * intensity;
                    }
                }
                mFFT.calculateFFT(mIn,mOut);
                mLibraryMutex[keynumber].lock();
                for (size_t i=0; i<mOut.size(); i++) mLibrary[keynumber][i]=mOut[i];
                mLibraryMutex[keynumber].unlock();
            }
        }
        else
        {
            if (counter > 0) LogI("Waveform generator stops after %d waveforms.",counter);
            counter = 0;
            msleep(20);
        }

        // Finally register the job as being done
        if (keynumber >= 0  and keynumber<=mNumberOfKeys)
        {
            mQueueMutex.lock();
            mComputing[keynumber] = false;
            invokeCallback(&WaveformGeneratorStatusCallback::queueSizeChanged, mQueue.size(), mComputing.size());
            mQueueMutex.unlock();
        }
    }
}

double WaveformGenerator::convertAvailablePhysicalMemoryToWaveformTime()
{
    const long long availableInB = PlatformToolsCore::getSingleton()->getInstalledPhysicalMemoryInB();
    const double availableInMiB = availableInB / 1024.0 / 1024.0;

    // the required memory for the complete program for 15s waveform is approx 344 MiB
    // the required memory for the complete program for  5s waveform is approx 188 MiB
    // the required memory for the complete program for  2s waveform is approx 140 MiB

    // If the device has more than 500 MiB memory, use full 15 secs
    // If the device has less than 256 MiB memory, use 2 secs
    // Interpolate inbetween

    // devices with less than 256 MiB memory should not be supported!

    if (availableInMiB > 500) {return 15;}
    if (availableInMiB < 256) {return 2;}
    return (15.0 - 2.0) / (500.0 - 256.0) * (availableInMiB - 256.0) + 2.0;
}
