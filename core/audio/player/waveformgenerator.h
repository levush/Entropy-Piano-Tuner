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
//                     PCM Waveform generator using FFTW3
//=============================================================================

#ifndef WAVEFORMGENERATOR_H
#define WAVEFORMGENERATOR_H

#include "../../system/simplethreadhandler.h"
#include "../../math/fftimplementation.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief The WaveformGenerator class
///
/// The purpose of this class is mainly to save time. In early versions of the
/// EPT we generated the waveforms in real time. This led on small mobile
/// devices to problems if more the one key was pressed. The present
/// wave form generator creates the PCM waveforms in advance and stores them
/// in a vector. This costs some memory but it save a lot of time. The
/// waveform is generated in the recording pitch. The synthesizer changes
/// the pitch if required by resampling.
///
/// The WaveformGenerator runs in an independent thread with normal priority.
////////////////////////////////////////////////////////////////////////////////

class WaveformGenerator : public SimpleThreadHandler
{
private:
#ifdef __ANDROID__
    const double mWaveformTime = 5;
#else
    const double mWaveformTime = 15;
#endif

public:
    using Waveform = std::vector<float>;
    using Spectrum = std::map<double,double>;   // type of spectrum

    WaveformGenerator();
    void init (int numberOfKeys, int samplerate);
    void exit () { stop(); }
    void preCalculate (int keynumber, const Spectrum &spectrum);
    Waveform getWaveForm (const int keynumber);
    float getInterpolation(const Waveform &W, const double t);
    bool isComputing (const int keynumber);

private:
    int mSampleRate;
    int mWaveformSize = 0;
    int mNumberOfKeys;
    std::vector<Waveform> mLibrary;
    std::vector<std::mutex> mLibraryMutex;
    std::vector<bool> mComputing;
    FFTComplexVector mIn;
    FFTRealVector mOut;
    FFT_Implementation mFFT;
    std::map<int,Spectrum> mQueue;
    std::mutex mQueueMutex;

private:
    void workerFunction();
};

#endif // WAVEFORMGENERATOR_H
