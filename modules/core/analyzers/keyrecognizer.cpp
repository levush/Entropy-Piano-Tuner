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

#include "keyrecognizer.h"

#include <iostream>
#include <fstream>
#include <functional>
#include <algorithm>

#include "../config.h"
#include "../math/mathtools.h"
#include "../piano/piano.h"
#include "../system/log.h"

const int    KeyRecognizer::M = 1024;
const double KeyRecognizer::fmin = 20;
const double KeyRecognizer::fmax = 10000;
const double KeyRecognizer::logfmin = log(fmin);
const double KeyRecognizer::logfmax = log(fmax);


//-----------------------------------------------------------------------------
//			                    Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor of the KeyRecognizer
/// \param callback : Pointer to the instance of the callback.
///////////////////////////////////////////////////////////////////////////////

KeyRecognizer::KeyRecognizer(KeyRecognizerCallback *callback) :
    mCallback(callback),                // Pointer to the caller
    mFFTPtr(nullptr),                   // Pointer to the Fourier transform
    mConcertPitch(0),                   // Concert pitch in Hz (normally 440)
    mPiano(nullptr),                    // Pointer to the piano data
    mNumberOfKeys(0),                   // Number of piano keys (normally 88)
    mKeyNumberOfA(0),                   // Index of the A-key (normally 48)
    mFFT(),                             // Instance of FFT implementation
    mLogSpec(M),                        // Vector holding the log. spectrum
    mFlatSpectrum(M),                   // Vector holding the loglog. spectrum
    mKernelFFT(M/2+1),                  // Vector holding the complex FFT of the kernel
    mFlatFFT(M/2+1),                    // Vector holding the complex FFT of the loglogspec
    mConvolution(M),                    // Convolution vector, real
    mSelectedKey(-1),                   // Selected key
    mKeyForced(false)                   // selected key is forced
{}


//-----------------------------------------------------------------------------
//			                   Initialization
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Initialization of the KeyRecognizer.
///
/// This function defines the kernel and optimizes the plan for the FFT.
/// Optimization will take a while to be finished.
/// \param optimize : true if the FFT should be optimized
///////////////////////////////////////////////////////////////////////////////

void KeyRecognizer::init(bool optimize)
{
    LogV("KeyRecognizer: starting initialization");
    defineKernel();
    if (optimize)
    {
        mFFT.optimize(mLogSpec);
        mFFT.optimize(mFlatFFT);
    }
    LogV("KeyRecognizer: initialization finished");
}


//-----------------------------------------------------------------------------
//			            Start the key recognition thread
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Start key recognition.
///
/// Function to start the key recognition thread. This function
/// is called by the SignalAnalyzer.
/// \param forceRestart : true if restart of the thread is forced
/// \param piano : pointer to the piano data
/// \param fftPointer : pointer to the actual FFT
/// \param selectedKey : Number of the selected key (-1 if none)
/// \param keyForced : Boolean flag saying whether the selected key is forced
///////////////////////////////////////////////////////////////////////////////

void KeyRecognizer::recognizeKey (
        bool forceRestart,
        const Piano *piano,
        FFTDataPointer fftPointer,
        int selectedKey,
        bool keyForced)
{
    EptAssert(piano, "The piano has to be set.");
    EptAssert(fftPointer, "The fft data has to exist.");
    EptAssert(fftPointer->isValid(), "Invaild fft data");

    if (forceRestart) stop();                   // if restart forced stop thread
    else if (isThreadRunning()) return;        // if it is running do nothing

    // copy data from the piano
    mPiano = piano;
    mConcertPitch = piano->getConcertPitch();
    mNumberOfKeys = piano->getKeyboard().getNumberOfKeys();
    mKeyNumberOfA = piano->getKeyboard().getKeyNumberOfA4();
    mFFTPtr = fftPointer;           // save pointer to the Fourier transform
    mSelectedKey = selectedKey;     // copy selected key
    mKeyForced = keyForced;         // copy forcing flag

    start();                        // start the thread
}


//-----------------------------------------------------------------------------
//		  	 Worker function running in an independent thread
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Main worker function for executing the key recognition thread.
///////////////////////////////////////////////////////////////////////////////

void KeyRecognizer::workerFunction()
{
    setThreadName("KeyRecognizer");
    EptAssert(mFFTPtr, "FFT Data have to non zero");
    EptAssert(mFFTPtr->isValid(), "FFT Data have to exist");
    EptAssert(mCallback, "Callback class has to exist");

    double f=0;
    if (mKeyForced and mSelectedKey>=0) f = detectForcedFrequency();
    else f = detectFrequencyInTreble();
    CHECK_CANCEL_THREAD;

    if (f==0)
    {
        constructLogSpec();             // Compute log spectrum
        CHECK_CANCEL_THREAD;

        signalPreprocessing();          // Compute flattened loglog spectrum
        CHECK_CANCEL_THREAD;

        f = estimateFrequency();        // Estimate frequency
        CHECK_CANCEL_THREAD;
    }

    int keynumber = findNearestKey(f);      // determine keynumber

    mCallback->keyRecognized(keynumber, f); // notify callback
}


//-----------------------------------------------------------------------------
//			               Measure forced keys
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Detect forced key
///
/// If a key is forced we search for the maximum of the FFT in the vicinity
/// of the forced key with a width of a bit less than a half tone.
/// \return Frequency in Hz
///////////////////////////////////////////////////////////////////////////////

double KeyRecognizer::detectForcedFrequency()
{
    if (mSelectedKey<0 or not mKeyForced) return 0;
    std::vector<double> &fft = mFFTPtr->fft;
    const int n = static_cast<int>(mFFTPtr->fft.size());
    const int sr = mFFTPtr->samplingRate;
    auto ftoq = [n,sr] (double f) { return MathTools::roundToInteger(2*n*f/sr); };
    auto qtof = [n,sr] (double q) { return sr*q/(2*n); };
    double f = mPiano->getEqualTempFrequency(mSelectedKey);
    const int q1 = std::max(0,ftoq(f/1.04));
    const int q2 = std::min(ftoq(f*1.04),n);
    double max=0;
    for (int q=q1; q<=q2; ++q) if (fft[q]>max)
    {
        max=fft[q];
        f=qtof(q);
    }
    return f;
}


//-----------------------------------------------------------------------------
//			                 Detect treble keys
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Detect keys in the treble
///
/// For very high notes the kernel method is not suitable. This function
/// detects whether one of the very high keys has been hit. This is done
/// by comparing the second and the first moment of the FFT. If the ratio
/// is above the threshold, indicating a high key, the frequency is determined
/// simply by searching for the maximal peak in the range from 1..5 kHz.
/// \return Frequency in Hz
///////////////////////////////////////////////////////////////////////////////

double KeyRecognizer::detectFrequencyInTreble()
{
    const double threshold = 0.09;
    std::vector<double> &fft = mFFTPtr->fft;
    const int n = static_cast<int>(mFFTPtr->fft.size());
    const int sr = mFFTPtr->samplingRate;
    auto ftoq = [n,sr] (double f) { return MathTools::roundToInteger(2*n*f/sr); };
    auto qtof = [n,sr] (double q) { return sr*q/(2*n); };
    const int q1=ftoq(20), q2=ftoq(1000), q3=ftoq(4500);
    double m1=0, m2=0;
    if (n<=0 or q1<0 or q3>=n) return 0;
    for (int q=q1; q<=q3; ++q)
    {
        m1 += fft[q]*q;
        m2 += fft[q]*q*q;
    }
    if (m1<=0) return 0;
    double f=0;
    if (m2/m1/n > threshold)
    {
        double max=0;
        for (int q=q2; q<q3; ++q) if (fft[q]>max)
        {
            max=fft[q];
            f=qtof(q);
        }
    }
    return f;
}


//-----------------------------------------------------------------------------
//			               Index mapping function
//-----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////
/// \brief Convert frequency to logspec index
///
/// The key recognizer performs a logarithmic binning independent of
/// the one carried out in the FFTAnalyzer. To this end the frequency f
/// is mapped to an integer index m. The formula reads:
/// \f[ m(f) = \left[M \frac{\ln(f-f_{min})}{\ln(f_{max}-f_{min})}\right], \f]
/// where M is the total number of bins. The expression is rounded to an integer.
/// \param f : Frequency in Hz
/// \return Integer index
////////////////////////////////////////////////////////////////////////

int KeyRecognizer::ftom (double f) {
    return static_cast<int>(0.5+M*(log(f)-logfmin)/(logfmax-logfmin));
}

//-----------------------------------------------------------------------------
//			            Inverse index mapping function
//-----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////
/// \brief Convert logspec index to frequency
///
/// This function maps the logarithmic binning index back to the
/// frequency. It is the inverse function of ftom:
/// \f[ f(m) = f_{min} + (f_{max}-f_{min})^{m/M} \,. \f]
/// \param m : the index
/// \return frequency in Hz
////////////////////////////////////////////////////////////////////////

double KeyRecognizer::mtof (int m) {
    return fmin * pow(fmax/fmin,static_cast<double>(m)/M);
}


//-----------------------------------------------------------------------------
//			         Construct logarithmic spectrum
//-----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////
/// \brief Construct logarithmic spectrum
///
/// This function maps the FFT spectrum, which is linear in the frequency,
/// to a logarithmically binned spectrum. To this end the amplitudes
/// of the FFT power spectrum are evently distributed into
/// bins of the logarithmic spectrum (mLogSpec).
////////////////////////////////////////////////////////////////////////

void KeyRecognizer::constructLogSpec()
{
    const int Q = static_cast<int>(mFFTPtr->fft.size());
    std::function<double(double)> mtoq = [this,Q] (double m)
        { return 2*fmin*Q/mFFTPtr->samplingRate*pow(fmax/fmin,m/M); };
    MathTools::coarseGrainSpectrum (mFFTPtr->fft,mLogSpec,mtoq);
}


//-----------------------------------------------------------------------------
//                        Preprocessing of the singal
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Preprocessing of the signal
///
/// The purpose of this function is to convert the logarithmically binned
/// spectrum (called logspec), which is logarithmic in the frequency, also
/// logarithmically in the amplitude. The result is a double-logarithmic
/// spectrum, i.e., logarithmic in its binning and also in its value.
/// Values <= 0 are mapped to zero.
///
/// Some spectra, in particular those of the high keys, show a broad noise
/// background which varies only smoothly with the frequency. The second part
/// of the function substracts a gliding average over a certain window,
/// centereing the loglog spectrum around the zero line.
///////////////////////////////////////////////////////////////////////////////

void KeyRecognizer::signalPreprocessing()
{
    Write("01-logspec.dat",mLogSpec);
    double norm = MathTools::computeNorm(mLogSpec);
    if (norm<=0) return;
    auto decibel = [norm](double x) {return 10*log10(x/norm);};
    auto dB= MathTools::transformVector<double>(mLogSpec,decibel);
    Write("02-dB.dat",dB);


    // Flatten noise by subtracting a gliding average
    for (int i=0; i<M; ++i)
    {
        //const int w=MathTools::roundToInteger(1+5000/mtof(i)); // width of the average
        const int w=30;
        int a = std::max(0,i-w);
        int b = std::min(M,i+1);
        double sum=0;
        for (int k=a; k<b; ++k) sum+=dB[k]*dB[k];
        mFlatSpectrum[i]=std::max(0.0,dB[i]+sqrt(sum/(b-a))-5);
    }
    Write("03-dBflat.dat",mFlatSpectrum);


    // If key is selected decrease all spectral lines below
    // and slightly increase the main spectral line.
    if (mSelectedKey>=0 and not mKeyForced)
    {
        int m1=ftom(mPiano->getEqualTempFrequency(mSelectedKey)*0.93);
        int m2=ftom(mPiano->getEqualTempFrequency(mSelectedKey)*1.07);
        if (m1>=0 and m1<=m2 and m2<=M)
        {
            for (int k=0; k<m1; ++k)  mFlatSpectrum[k] *= 0.75;
            for (int k=m1; k<m2; ++k) mFlatSpectrum[k] *= 1.2;
        }
    }
    Write("04-FlatSpectrum.dat",mFlatSpectrum);

}


//-----------------------------------------------------------------------------
//			                Define the kernel
//-----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////
/// \brief Define the kernel vector for key recognition.
///
/// The tuner needs to recognize the key that is pressed. The essential
/// part in the recognition procedure is a kernel function which is
/// convolved linearly with the actually measured spectrum. It has
/// positive peaks in those positions where partials are expected and
/// negative peaks in the place of wrong partials. It is stored in a
/// in a STL map.
////////////////////////////////////////////////////////////////////////

void KeyRecognizer::defineKernel ()
{

    static const int width=M/300;     // width of the peaks in bins
    static const int partials=20;  // number of partials to be detected (20)
    static const double B=0.000;   // here still with a constant inharmonicity

    static std::vector<double> kernel(M); //must be static for using fftw3
    kernel.assign(M,0);

    // lambda function for setting a peak
    auto setpeak = [] (int m, double amplitude)
    { for (int n=m-width; n<=m+width; n++) kernel[(n+M)%M]=amplitude*(width-std::abs(n-m)); };

    // lambda function for computing the frequency ratio of the nth partial
    auto partial = [] (int n, double B)
    { return n * sqrt((1+B*n*n)/(1+B)); };

    // lambda function for computing the index of the nth partial
    auto partialindex = [this,partial] (int n, double B, int div)
    { return ftom(fmin*partial(n,B)/partial(div,B)); };

    // lambda function for the intensity decay of the peaks
    auto intensity = [] (int n) { return pow(static_cast<double>(n),-0.3); };

    // Define the kernel function
    for (int div=2; div<=4; div++) for (int n=1; n<=30; ++n) if (n%div>0) if (n>div-2)
            setpeak(partialindex(n,B,div),-0.3*intensity(n));

    for (int n=1; n<=partials; ++n) setpeak(partialindex(n, B, 1),intensity(n));


    mFFT.calculateFFT(kernel,mKernelFFT); // calculate FFT

    Write("05-keyrecog-kernel.dat",kernel,true);
}


//-----------------------------------------------------------------------------
//			              Estimate frequency
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Estimate frequency for a given log-log spectrum.
///
/// This function estimates the frequency of a pressed piano key.
/// To this end the logarithmically binned log spectrum is convolved with
/// the recognition kernel for all frequencies. For speedup this
/// convolution product is carried out by two Fourier transformations
/// (in log space instead of real space) with an ordinary multiplication
/// in between. The m-value (slot) where the response of the kernel is
/// maximal is then used as a first estimate of the frequency.
/// \return Estimated frequency in Hz
///////////////////////////////////////////////////////////////////////////////

double KeyRecognizer::estimateFrequency ()
{
    mFFT.calculateFFT(mFlatSpectrum,mFlatFFT);
    for (size_t n = 0; n < mFlatFFT.size(); ++n)
        mFlatFFT[n] *= std::conj(mKernelFFT[n]);
    mFFT.calculateFFT(mFlatFFT,mConvolution);
    int m  = MathTools::findMaximum(mConvolution);
    Write("06-convolution.dat",mConvolution,false);
    return mtof(m);
}


//-----------------------------------------------------------------------------
//	Find the nearest key to a given frequency, assuming average stretch
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Find the nearest key for a given frequency.
///
/// This function determines the most likely index of a key for a given
/// frequency, referring to the actual concert pitch.
/// \param f : Frequency in Hz
/// \return Index of the key in the range 0..mNumberOfKeys
///////////////////////////////////////////////////////////////////////////////

int KeyRecognizer::findNearestKey (double f)
{
    if (mConcertPitch<=390 or mConcertPitch>500) return -1;
    // Approximate distance in keys from A-440:
    double d = 17.3123*log(f/mConcertPitch);
    // Average stretch polynomial, giving the expected deviation in cents
    double c = 0.000019394+0.079694594*d-0.003718646*d*d
             + 0.000450934*d*d*d + 0.000003724*d*d*d*d;
    int k=-1; k = static_cast<int>(mKeyNumberOfA+d-c/100+0.5);
    return (k>=0 and k<mNumberOfKeys ? k : -1);
}


//-----------------------------------------------------------------------------
//			Write function for development purposes
//-----------------------------------------------------------------------------

void KeyRecognizer::Write(std::string filename, std::vector<double> &v, bool log)
{
#if CONFIG_ENABLE_XMGRACE
    std::ofstream os(filename);
    for (uint m=0; m<v.size(); ++m)
    {
        //os << m << "\t" << v[m] << std::endl;
        if (log) os << mtof(m) << "\t" << v[m] << std::endl;
        else os << m << "\t" << v[m] << std::endl;
    }
    os.close();
#else
    (void)filename; (void)v; (void)log; // suppress warnings
#endif // CONFIG_ENABLE_XMGRACE
}

