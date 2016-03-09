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

#ifndef WAVEFORMGENERATOR_H
#define WAVEFORMGENERATOR_H

#include "prerequisites.h"

#include "system/simplethreadhandler.h"
#include "system/basecallback.h"
#include "math/fftimplementation.h"


////////////////////////////////////////////////////////////////////////////////
/// \brief The WaveformGeneratorStatusCallback class
///
/// Implements callback functions called by the WaveformGenerator.
////////////////////////////////////////////////////////////////////////////////
class EPT_EXTERN WaveformGeneratorStatusCallback : public BaseCallbackInterface
{
public:
    ///
    /// \brief Called by WaveformGenerator if the size of the queue changed
    /// \param size The new size
    /// \param maxSize The maximum size, usually the number of keys
    ///
    virtual void queueSizeChanged(size_t size, size_t maxSize) = 0;
};

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

class EPT_EXTERN WaveformGenerator :
        public SimpleThreadHandler,
        public CallbackManager<WaveformGeneratorStatusCallback>
{
public:
    using Waveform = std::vector<float>;
    using Spectrum = std::map<double,double>;   // type of spectrum

    WaveformGenerator();
    virtual ~WaveformGenerator() {}

    void init (int numberOfKeys, int samplerate);
    void exit () { stop(); }
    void preCalculate (int keynumber, const Spectrum &spectrum);
    Waveform getWaveForm (const int keynumber);
    float getInterpolation(const Waveform &W, const double t);
    bool isComputing (const int keynumber);

private:
    const double mWaveformTime;             ///< Sampling time in seconds
    int mSampleRate;                        ///< Sample rate
    int mWaveformSize = 0;                  ///< Stored size of the waveform
    int mNumberOfKeys;                      ///< Local copy of the number of keys
    std::vector<Waveform> mLibrary;         ///< Collection (library) of sounds
    std::vector<std::mutex> mLibraryMutex;  ///< Access mutex for the library
    std::vector<bool> mComputing;           ///< Flag indicating that the sound is computed
    FFTComplexVector mIn;                   ///< FFT input array
    FFTRealVector mOut;                     ///< FFT output array
    FFT_Implementation mFFT;                ///< Instance of the FFT module
    std::map<int,Spectrum> mQueue;          ///< Queue of waveform generation requests
    std::mutex mQueueMutex;                 ///< Access mutex for waveform request queue

private:
    virtual void workerFunction() override;

    /// Helper function to compute the waveform time based on the installed memory
    double convertAvailablePhysicalMemoryToWaveformTime();
};

#endif // WAVEFORMGENERATOR_H
