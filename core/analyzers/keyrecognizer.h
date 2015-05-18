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
//                              Key recognizer
//=============================================================================

#ifndef KEYRECOGNIZER_H
#define KEYRECOGNIZER_H

#include <cmath>

#include "../system/simplethreadhandler.h"
#include "../audio/audiorecorderadapter.h"
#include "../messages/messagelistener.h"
#include "../piano/piano.h"
#include "../math/fftimplementation.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Callback class for KeyRecognizer.
///
/// When KeyRecognizer::recognizeKey has
/// finished, he pure virtual function keyRecognized() will be called.
///////////////////////////////////////////////////////////////////////////////
class KeyRecognizerCallback {
public:
    virtual void keyRecognized(int keyIndex, double frequency) = 0;
};

///////////////////////////////////////////////////////////////////////////////
/// \brief Module for fast recognition of the pressed key
///
/// When a key is pressed, the SignalAnalyzer continuously Fourier-
/// transforms the signal in short time intervals.
/// Every time when a new FFT is ready,
/// the SignalAnalyzer sends a MessageNewFFTCalculated
/// with a 'false' flag indicating that the FFT is not yet final.
/// This message will start the present KeyRecognizer in a
/// new thread.
///
/// The KeyRecognizer works as a black box independent of all other
/// modules in the tuning program.
/// It produces a message with the estimated frequency and
/// the corresponding key number.
////////////////////////////////////////////////////////////////////////////////

class KeyRecognizer : public SimpleThreadHandler
{
private:
    static const int    M;                              ///< Number of bins (powers of 2,3,5)
    static const double fmin;                           ///< Frequency of bin 0
    static const double fmax;                           ///< Frequency of bin M-1

public:
    KeyRecognizer (KeyRecognizerCallback *callback);    ///< Constructor
    ~KeyRecognizer(){}                                  ///< Empty destructor

    void init(bool optimize);                           ///< Initialize (optimize fft)
    void recognizeKey(
            bool forceRestart,
            const Piano *piano,
            FFTDataPointer fftPointer,
            int selectedKey,
            bool keyForced);

private:
    void workerFunction() override final;               ///< Thread execution function

    void constructLogSpec();                            ///< Construct logarithmic spectrum
    void defineKernel();                                ///< Define convolution kernel
    void signalPreprocessing();                         ///< Preprocessing of the signal
    double estimateFrequency();                         ///< Frequency recognition
    int findNearestKey (double f);                      ///< Find nearest key, 0 if none
    int identifySelectedKey(double frequency);          ///< identify final key

private:
    KeyRecognizerCallback *mCallback;                   ///< Pointer to the caller
    FFTDataPointer mFFTPtr;                             ///< Pointer to Fourier transform
    bool mFinal;                                        ///< Flag for final FFT
    const Piano *mPiano;                                ///< Pointer to the piano
    double mConcertPitch;                               ///< Actual frequency of the A-key
    int mNumberOfKeys;                                  ///< Number of piano keys
    int mKeyNumberOfA;                                  ///< Index of the A-key
    FFT_Implementation mFFT;                            ///< Instance of FFT implementation
    std::vector<double> mLogSpec;                       ///< Logarithmic spectrum (LogSpec)
    FFTComplexVector mKernelFFT;                        ///< Fourier transform of the kernel
    FFTComplexVector mLogSpecFFT;                       ///< Fourier transform of LogSpec
    FFTRealVector mConvolution;                         ///< Convolution vector
    int mSelectedKey;                                   ///< The selected key by the user
    bool mKeyForced;                                    ///< Is the key selection forced


private:
    static const double logfmin;                        ///< Log of minimal frequency
    static const double logfmax;                        ///< Log of maximal frequency
    double mtof (int m);                                ///< Map bin index to frequency
    int ftom (double f);                                ///< Map frequency to bin index

    void Write(std::string filename, std::vector<double> &v); // only for development
};

#endif // KEYRECOGNIZER_H
