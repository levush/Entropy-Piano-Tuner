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
//                      A simple sine wave synthesizer
//=============================================================================

#ifndef WAVEFORMGENERATOR_H
#define WAVEFORMGENERATOR_H

#include "sound.h"

#include "../system/simplethreadhandler.h"
#include "../math/fftimplementation.h"

class WaveformGenerator : public SimpleThreadHandler
{
private:
    const int size = 65536*2;
    const double time = 2*2.972;

public:

    using Waveform = std::vector<float>;

    WaveformGenerator();
    void init (int numberOfKeys);
    void preCalculate (int keynumber, const Sound &sound);
    Waveform getWaveForm (const int keynumber);
    double getInterpolation (const Waveform &W, const double t);

private:
    int mNumberOfKeys;
    std::vector<Waveform> mLibrary;
    std::vector<std::mutex> mLibraryMutex;
    FFTComplexVector mIn;
    FFTRealVector mOut;
    FFT_Implementation mFFT;
    std::map<int,Sound> mQueue;
    std::mutex mQueueMutex;

private:
    void workerFunction();
};

#endif // WAVEFORMGENERATOR_H
