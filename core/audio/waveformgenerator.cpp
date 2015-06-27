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

#include "waveformgenerator.h"

#include "../math/mathtools.h"

#include <iostream>

//-----------------------------------------------------------------------------
//                               Constructor
//-----------------------------------------------------------------------------

WaveformGenerator::WaveformGenerator () :
    mNumberOfKeys(),
    mLibrary(),
    mLibraryMutex(256),
    mComputing(),
    mIn(size/2+1),
    mOut(size),
    mFFT(),
    mQueue()
{
}


//-----------------------------------------------------------------------------
//                                  Init
//-----------------------------------------------------------------------------

void WaveformGenerator::init (int numberOfKeys)
{
    EptAssert(numberOfKeys > 0 and numberOfKeys < 256,
              "Number of keys out of range");
    mNumberOfKeys = numberOfKeys;
    mLibrary.resize(mNumberOfKeys);
    for (auto &wave : mLibrary)
    {
        wave.resize(size);
        wave.assign(size,0);
    }
    mComputing.resize(mNumberOfKeys);
    mComputing.assign(mNumberOfKeys,false);
}


//-----------------------------------------------------------------------------
//                           pre-calculate waveform
//-----------------------------------------------------------------------------

void WaveformGenerator::preCalculate(int keynumber, const Spectrum &spectrum)
{
    mQueueMutex.lock();
    mQueue[keynumber] = spectrum;
    mComputing[keynumber] = true;
    mQueueMutex.unlock();
}


//-----------------------------------------------------------------------------
//                          get pre-calculated waveform
//-----------------------------------------------------------------------------

WaveformGenerator::Waveform WaveformGenerator::getWaveForm (const int keynumber)
{
    if (keynumber < 0 or keynumber >= mNumberOfKeys) return Waveform();
    std::lock_guard<std::mutex> lock(mLibraryMutex[keynumber]);
    return mLibrary[keynumber];
}


//-----------------------------------------------------------------------------
//                              PCM interpolation
//-----------------------------------------------------------------------------

double WaveformGenerator::getInterpolation (const Waveform &W, const double t)
{
    double realindex = t*size/time;
    int index = static_cast<int> (realindex);
    return W[index%size] + (realindex-index)*(W[(index+1)%size]-W[index%size]);
}


//-----------------------------------------------------------------------------
//                        Check if computation is ready
//-----------------------------------------------------------------------------

bool WaveformGenerator::isComputing (const int keynumber)
{
    std::lock_guard<std::mutex> lock(mQueueMutex);
    return mComputing[keynumber];
}

//-----------------------------------------------------------------------------
//                             Main thread function
//-----------------------------------------------------------------------------

void WaveformGenerator::workerFunction()
{
    std::cout << "Now we optimize the plan" << std::endl;
    //mFFT.optimize(mIn);
    std::cout << "Plan optimization finished" << std::endl;
    Spectrum spectrum;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0,MathTools::PI*2);
    while (not cancelThread())
    {
        int keynumber = -1;
        mQueueMutex.lock();
        if(not mQueue.empty())
        {
            auto element = mQueue.begin();
            keynumber = element->first;
            spectrum = element->second;
            mQueue.erase(element);
        }
        mQueueMutex.unlock();

        if (keynumber >= 0 and keynumber<=mNumberOfKeys)
        {
            double norm=0;
            for (auto &partial : spectrum) norm += partial.second;
            mIn.assign(size/2+1,0);
            if (norm>0)
            {
                for (auto &partial : spectrum)
                {
                    const double frequency = partial.first;
                    const double intensity = sqrt(partial.second / norm);
                    int k = MathTools::roundToInteger(frequency*time);
                    if (k>0 and k<size)
                    {
                        std::complex<double> phase(0,distribution(generator));
                        mIn[k] = exp(phase) * intensity;
                    }
                }
                std::cout << "doing " << keynumber << std::endl;
                mFFT.calculateFFT(mIn,mOut);
                mLibraryMutex[keynumber].lock();
                for (int i=0; i<size; i++) mLibrary[keynumber][i]=mOut[i];
                mLibraryMutex[keynumber].unlock();
                mQueueMutex.lock();
                mComputing[keynumber] = false;
                mQueueMutex.unlock();
                std::cout << "finished " << std::endl;
            }
        }
        else msleep(20);
    }
}


