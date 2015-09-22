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
//                               Stroboscope
//=============================================================================

// Test sine waves with: play -n synth sin 440 vol 0.1
// monitor system input with xoscope

#ifndef STROBOSCOPE_H
#define STROBOSCOPE_H

#include <complex>
#include <vector>
#include <mutex>

#include "../audiobase.h"

class AudioRecorderAdapter;


class Stroboscope
{
private:
    /// Damping factor of the normalizing amplitude level on a single frame (0...1)
    const double AMPLITUDE_DAMPING = 0.95;

    /// Damping of the complex phases from frame to frame (0...1)
    const double FRAME_DAMPING = 0.5;

public:
    using Complex = std::complex<double>;
    using ComplexVector = std::vector<Complex>;

    Stroboscope (AudioRecorderAdapter *recorder);
    void start () { mActive = true; }
    void stop  () { mActive = false; }

    void pushRawData (const AudioBase::PacketType &data);
    void setFrequencies (const std::vector<double> &frequencies);
    void setFramesPerSecond (double fps);

private:
    AudioRecorderAdapter *mRecorder;
    bool mActive;
    int mSamplesPerFrame;
    int mSampleCounter;
    double mMaxAmplitude;
    ComplexVector mComplexPhase;
    ComplexVector mComplexIncrement;
    ComplexVector mMeanComplexPhase;
    std::mutex mMutex;
};

#endif // STROBOSCOPE_H
