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
//                  Adapter for fast Fourier transformations
//=============================================================================

#ifndef FFTADAPTER
#define FFTADAPTER

#include "prerequisites.h"

// Data types to be processed by the FFT software
using FFTRealType      = double;
using FFTComplexType   = std::complex<double>;
using FFTRealVector    = std::vector<FFTRealType>;
using FFTComplexVector = std::vector<FFTComplexType>;

// external used datatypes

/// data type
using FFTWType      = FFTRealType;
/// fftw array
using FFTWVector = std::vector<FFTWType>;
/// Type for a frequency-to-intensity map for graphics
typedef std::map<double,double> FFTPolygon;

///////////////////////////////////////////////////////////////////////////////
/// \brief Data struct for a FFT
///
/// A fourier transform is always defined by its data and the sampling rate.
/// The first argument its the FFTPointer the second one the sampling rate.
/// Invalid or unset states have a invalid FFTPointer or a negative sampling
/// rate.
///////////////////////////////////////////////////////////////////////////////
struct EPT_EXTERN FFTData {
    FFTWVector fft;         ///< The actual fft
    int samplingRate = -1;  ///< The sampling rate of the fft

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to validate the fft data.
    /// \return It the FFTData valide.
    ///
    /// This will check if the fft size is greater zero and if the sampling rate
    /// is greater zero.
    ///////////////////////////////////////////////////////////////////////////////
    bool isValid() {
        return fft.size() > 0 && samplingRate > 0;
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to get the time of the signal in seconds
    /// \return 2 * fft.size() / samplingRate
    ///
    ///////////////////////////////////////////////////////////////////////////////
    double getTime() {
        return fft.size() * 2.0 / samplingRate;
    }
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Shared pointer of FFTData
///
/// Type for a shared pointer with the FFT and its sampling rate delivered via
/// finall-FFT message by the SignalAnalyzer.
///////////////////////////////////////////////////////////////////////////////
using FFTDataPointer = std::shared_ptr<FFTData>;
template class EPT_EXTERN std::shared_ptr<FFTData>;

///////////////////////////////////////////////////////////////////////////////
/// \brief Abstract base class for handling fast Fourier transforms
///
/// The tuner makes frequent use of fast Fourier transforms. However,
/// existing packages such as fftw3 are not completely thread-safe.
/// This adapter class establishes an interface between the tuner
/// and a specific implementation.
///
/// The adapter is designed for two types of FFTs, namely, real to complex
/// and comples to real. The second one is the inverse of the first one.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN FFTAdapter
{
public:

    FFTAdapter() {}
    virtual ~FFTAdapter() {}

    virtual void calculateFFT  (const FFTRealVector &in, FFTComplexVector &out) = 0;
    virtual void calculateFFT  (const FFTComplexVector &in, FFTRealVector &out) = 0;

    // Call these functions to speed up FFT with the same size
    virtual void optimize (FFTRealVector &in) = 0;
    virtual void optimize (FFTComplexVector &in) = 0;

};

#endif // FFTADAPTER

