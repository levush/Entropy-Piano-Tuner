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
//                               Stroboscope
//=============================================================================

// Test sine waves with: play -n synth sin 440 vol 0.1
// monitor system input with xoscope

#ifndef STROBOSCOPE_H
#define STROBOSCOPE_H

#include <complex>
#include <vector>
#include <mutex>

class AudioRecorder;

///////////////////////////////////////////////////////////////////////////////
/// \brief Class for a stroboscopic tuning indicator
///
/// Ordinary stroboscopic tuners show the analog audio signal amplitude in
/// terms of light intensity masked by a rotating disk. See e.g.
/// https://en.wikipedia.org/wiki/Electronic_tuner#How_a_strobe_tuner_works
///
/// Doing the same in our software would require to send the complete PCM data
/// to a stroboscopic drawer which would display it in real time pixel by pixel.
/// This would be computationally expensive. Moreover, the messaging sytem
/// would not be suitable for such a data stream.
///
/// We therefore choose a different method. The AudioRecorderAdapter holds an
/// instance of this class and calls the function pushRawData (see below),
/// transmitting all PCM data. This data is reorganized in packages of the
/// size mSamplesPerFrame. Each frame is convolved with a complex number
/// rotating on its unit circle. The speed at which the complex number
/// rotates corresponds to the expected frequencies of the partials, as
/// specified by the function set Frequencies. The result is a set of
/// complex numbers (one for each partial), encoding the intensity of the
/// partial and its complex phase shift. This data is sent (only once for
/// each frame) via the messaging system.
///
/// The TuningIndicatorDrawer listens to these messages and draws horizontal
/// bars with phase-shifted rainbow colors.
/// \see AudioRecorderAdapter
/// \see TuningIndicatorDrawer
///////////////////////////////////////////////////////////////////////////////

class Stroboscope
{
private:
    typedef double PCMDataType;
    typedef std::vector<PCMDataType> PacketType;

    /// Damping factor of the normalizing amplitude level on a single frame (0...1)
    const double AMPLITUDE_DAMPING = 0.95;

    /// Damping of the complex phases from frame to frame (0...1)
    const double FRAME_DAMPING = 0.5;

public:
    using Complex = std::complex<double>;           ///< Type for a complex number
    using ComplexVector = std::vector<Complex>;     ///< Type for a vector of complex numbers

    Stroboscope (AudioRecorder*recorder);   ///< Constructor
    void start () { mActive = true; }               ///< Start the stroboscope
    void stop  () { mActive = false; }              ///< Stop the stroboscope

    void setFramesPerSecond (double fps);
    void setFrequencies (const std::vector<double> &frequencies);
    void pushRawData (const PacketType &data);

private:
    AudioRecorder*mRecorder;                ///< Pointer to the audio recorder
    bool mActive;                           ///< Flag indicating activity (start/stop)
    int mSamplesPerFrame;                   ///< Number of PCM samples per frame
    int mSampleCounter;                     ///< Actual number of PCM samples read
    double mMaxAmplitude;                   ///< Sliding amplitude to normalize the data
    ComplexVector mComplexPhase;            ///< Rotating complex number
    ComplexVector mComplexIncrement;        ///< Factor by which the complex number rotates
    ComplexVector mMeanComplexPhase;        ///< Phase average over the actual frame
    std::mutex mMutex;                      ///< Mutex protecting access from different threads
};

#endif // STROBOSCOPE_H
