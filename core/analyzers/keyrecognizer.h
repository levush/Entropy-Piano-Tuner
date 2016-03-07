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
//                              Key recognizer
//=============================================================================

#ifndef KEYRECOGNIZER_H
#define KEYRECOGNIZER_H

#include "prerequisites.h"
#include "../system/simplethreadhandler.h"
#include "../messages/messagelistener.h"
#include "../piano/piano.h"
#include "../math/fftimplementation.h"


///////////////////////////////////////////////////////////////////////////////
/// \brief Callback class for KeyRecognizer.
///
/// When KeyRecognizer::recognizeKey has
/// finished, the pure virtual function keyRecognized() will be called.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN KeyRecognizerCallback
{
public:
    virtual void keyRecognized(int keyIndex, double frequency) = 0;
};



///////////////////////////////////////////////////////////////////////////////
/// \brief Module for fast recognition of the pressed key.
///
/// When a key is pressed, the SignalAnalyzer calls the function
/// recognizeKey to start an independent thread for key recognition on the
/// basis of the current Fourier transform. The key recognizer transmits the
/// estimated frequency and the corresponding key number via a callback function.
////////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN KeyRecognizer : public SimpleThreadHandler
{
private:
    static const int    M;                              ///< Number of bins (powers of 2,3,5)
    static const double fmin;                           ///< Frequency of bin 0
    static const double fmax;                           ///< Frequency of bin M-1

public:
    KeyRecognizer (KeyRecognizerCallback *callback);    // Constructor
    ~KeyRecognizer(){}                                  // Empty destructor

    void init(bool optimize);                           // Initialize (optimize FFT)
    void recognizeKey(bool forceRestart,                // Recognize a key:
                      const Piano *piano,               // This function is called by
                      FFTDataPointer fftPointer,        // the SignalAnalyzer
                      int selectedKey, bool keyForced);

private:
    void workerFunction() override final;               // Thread execution function

    double detectForcedFrequency();                     // Handle forced keys
    double detectFrequencyInTreble();                   // Handle keys in the treble
    void constructLogSpec();                            // Construct logarithmic spectrum
    void signalPreprocessing();                         // Preprocessing of the signal
    void defineKernel();                                // Define convolution kernel
    double estimateFrequency();                         // Frequency recognition
    int findNearestKey (double f);                      // Find nearest key, 0 if none

private:
    KeyRecognizerCallback *mCallback;                   ///< Pointer to the caller
    FFTDataPointer mFFTPtr;                             ///< Pointer to Fourier transform
    double mConcertPitch;                               ///< Actual frequency of the A-key
    const Piano *mPiano;                                ///< Pointer to the piano data
    int mNumberOfKeys;                                  ///< Number of piano keys
    int mKeyNumberOfA;                                  ///< Index of the A-key
    FFT_Implementation mFFT;                            ///< Instance of FFT implementation
    std::vector<double> mLogSpec;                       ///< Logarithmic spectrum (LogSpec)
    std::vector<double> mFlatSpectrum;                  ///< DoubleLogarithmic spectrum (LogLogSpec)
    FFTComplexVector mKernelFFT;                        ///< Fourier transform of the kernel
    FFTComplexVector mFlatFFT;                          ///< Fourier transform of LogLogSpec
    FFTRealVector mConvolution;                         ///< Convolution vector
    int mSelectedKey;                                   ///< Number of the actually selected key
    bool mKeyForced;                                    ///< Flag indicating that the key is forced

private:
    static const double logfmin;                        ///< Log of minimal frequency
    static const double logfmax;                        ///< Log of maximal frequency
    double mtof (int m);                                ///< Map bin index to frequency
    int ftom (double f);                                ///< Map frequency to bin index

    void Write(std::string filename, std::vector<double> &v, bool log=true); // only for development
};

#endif // KEYRECOGNIZER_H
