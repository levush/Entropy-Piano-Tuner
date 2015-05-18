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

#include "keyrecognizer.h"

#include <iostream>
#include <fstream>
#include <functional>
#include <algorithm>

#include "../config.h"
#include "../math/mathtools.h"
#include "../piano/piano.h"
#include "../system/log.h"

const int    KeyRecognizer::M = 8192;
const double KeyRecognizer::fmin = 10;
const double KeyRecognizer::fmax = 20000;
const double KeyRecognizer::logfmin = log(fmin);
const double KeyRecognizer::logfmax = log(fmax);

//-----------------------------------------------------------------------------
//			                    Constructor
//-----------------------------------------------------------------------------

KeyRecognizer::KeyRecognizer(KeyRecognizerCallback *callback) :
    mCallback(callback),                // Pointer to the caller
    mFFTPtr(nullptr),                   // Pointer to the Fourier transform
    mFinal(false),                      // Flag for final FFT
    mPiano(nullptr),                    // Pointer to the current piano
    mConcertPitch(0),                   // Concert pitch in Hz (normally 440)
    mNumberOfKeys(0),                   // Number of piano keys (normally 88)
    mKeyNumberOfA(0),                   // Index of the A-key (normally 48)
    mFFT(),                             // Instance of FFT implementation
    mLogSpec(M),                        // Vector holding the log. spectrum
    mKernelFFT(M/2+1),                  // Vector holding the complex FFT of the kernel
    mLogSpecFFT(M/2+1),                 // Vector holding the complex FFT of the logspec
    mConvolution(M),                    // Convolution vector, real
    mSelectedKey(-1),                   // Selected key
    mKeyForced(false)                   // key selection forced
{}

//-----------------------------------------------------------------------------
//			               init
//-----------------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////////////
/// Function to define the kernel and optimize the fft. Optimazing will take
/// a while to be finished.
///
/// \param optimize Optimize the fft
///
///////////////////////////////////////////////////////////////////////////////
void KeyRecognizer::init(bool optimize)
{
    INFORMATION("KeyRecognizer: starting initialization");
    defineKernel();
    if (optimize)
    {
        mFFT.optimize(mLogSpec);
        mFFT.optimize(mLogSpecFFT);
    }
    INFORMATION("KeyRecognizer: initialization finished");
}

//-----------------------------------------------------------------------------
//			               init
//-----------------------------------------------------------------------------


///////////////////////////////////////////////////////////////////////////////
/// \brief Function to start the recognizing thread on a given fft
/// \param piano
/// \param fftPointer
/// \param samplingRate
///
/// When finished this will call the
///////////////////////////////////////////////////////////////////////////////
void KeyRecognizer::recognizeKey(bool forceRestart,
        const Piano *piano,
        FFTDataPointer fftPointer,
        int selectedKey,
        bool keyForced) {
    EptAssert(piano, "The piano has to be set.");
    EptAssert(fftPointer, "The fft data has to exist.");
    EptAssert(fftPointer->isValid(), "Invaild fft data");

    // stop the thread if we want to restart
    // or return if it is already running
    if (forceRestart) {stop();}
    else if (isThreadRunnding()) {return;}

    // The thread shall be started

    // copy data

    mPiano = piano;
    mConcertPitch = mPiano->getConcertPitch();                 // copy concert pitch (standard 440 Hz)
    mNumberOfKeys = mPiano->getKeyboard().getNumberOfKeys();   // copy number of piano keys (standard 88)
    mKeyNumberOfA = mPiano->getKeyboard().getKeyNumberOfA4();  // copy number of A440-key (standard 48)


    mFFTPtr = fftPointer;                                      // Get a pointer to the Fourier transform

    mSelectedKey = selectedKey;
    mKeyForced = keyForced;

    // start the thread

    start();
}


//-----------------------------------------------------------------------------
//			Worker function running in an independent thread
//-----------------------------------------------------------------------------

void KeyRecognizer::workerFunction()
{
    EptAssert(mPiano, "The piano has to be set");
    EptAssert(mFFTPtr, "FFT Data have to non zero");
    EptAssert(mFFTPtr->isValid(), "FFT Data have to exist");
    EptAssert(mCallback, "Callback class has to exist");

    constructLogSpec();
    signalPreprocessing();

    CHECK_CANCEL_THREAD;

    double f = estimateFrequency();

    CHECK_CANCEL_THREAD;

    int keynumber = identifySelectedKey(f);

    std::cout << "KeyRecognizer: f=" << f << ", key=" << keynumber << std::endl;

    // notify callback

    mCallback->keyRecognized(keynumber, f);

    //Write("3-keyrecog-convolution.dat",mConvolution);
}

//-----------------------------------------------------------------------------
//			            index mapping functions
//-----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////
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
/// This function maps the FFT spectrum, which is linear in the frequency,
/// to a logarithmically binned spectrum. To this end the amplitudes
/// of the FFT power spectrum are evently distributed into
/// bins of the logarithmic spectrum (mLogSpec).
////////////////////////////////////////////////////////////////////////

double f(double x) { return x*x; }

void KeyRecognizer::constructLogSpec()
{
    const int Q = mFFTPtr->fft.size();
    std::function<double(double)> mtoq = [this,Q] (double m)
        { return 2*fmin*Q/mFFTPtr->samplingRate*pow(fmax/fmin,m/M); };
    MathTools::coarseGrainSpectrum (mFFTPtr->fft,mLogSpec,mtoq);

    //Write("3-keyrecog-logspec.dat",mLogSpec);
}


//-----------------------------------------------------------------------------
//			                Define kernel
//-----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////
/// The tuner needs to recognize the key that is pressed. The essential
/// part in the recognition procedure is a kernel function which is
/// convolved linearly with the actually measured spectrum. It has
/// positive peaks in those positions where partials are expected and
/// negative peaks in the place of wrong partials. It is stored in a
/// in a STL map.
////////////////////////////////////////////////////////////////////////

void KeyRecognizer::defineKernel ()
{

    static const int width=15;     // width of the peaks in cents
    static const int partials=20;  // number of partials to be detected (20)
    static const double B=0.0002;   // here still with a constant inharmonicity

    static std::vector<double> kernel(M); //must be static for using fftw3

    // lambda function for setting a peak
    auto setpeak = [] (int m, double amplitude)
    { for (int n=m-width; n<=m+width; n++) kernel[(n+M)%M]+=amplitude*(width-std::abs(n-m)); };

    // lambda function for computing the frequency of the nth partial
    auto partial = [] (double f1, int n, double B)
    { return f1 * n * sqrt((1+B*n*n)/(1+B)); };

    // lambda function for computing the index of the nth partial
    auto partialindex = [this,partial] (int m, int n, double B, int div)
    { double f=mtof(m); return ftom(f*partial(f/div,n,B)/partial(f/div,div,B)); };

    // lambda function for the intensity decay of the peaks
    auto intensity = [] (int n) { return pow(static_cast<double>(n),-0.1); };

    // Define the kernel function
    for (int n=1; n<=partials; ++n) setpeak(partialindex(0, n, B, 1),intensity(n));
    for (int div=2; div<=15; div++) for (int n=1; n<=30; ++n) if (n%div>0)
        setpeak(partialindex(0,n,B,div),-0.05*intensity(n));

    mFFT.calculateFFT(kernel,mKernelFFT); // calculate FFT

    //Write("3-keyrecog-kernel.dat",kernel);
}


//-----------------------------------------------------------------------------
//                        Preprocessing of the singal
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
///  \brief Preprocessing of the signal
///
/// This is so to say a high pass filter. It will cut (set to 0) all
/// frequencies below the selected key.
/// The signal processing is active if the key selection is forced
///////////////////////////////////////////////////////////////////////////////

void KeyRecognizer::signalPreprocessing() {
    EptAssert(mPiano, "The piano has to be set");

    // if the selected key is forced we do a signal preprocessing
    if (mSelectedKey < 0 || !mKeyForced) {
        return;
    }

    // get the esitimated frequency of the key
    double f = mPiano->getEqualTempFrequency(mSelectedKey);

    // all frequencies lower than 0.9 * f are set to 0
    f *= 0.9;
    for (int n = 0; n < std::min<int>(ftom(f), mLogSpec.size()); n++) {
        mLogSpec[n] = 0;
    }
}

//-----------------------------------------------------------------------------
//			              Estimate frequency
//-----------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////
/// This function estimates the frequency of a pressed piano key.
/// To this end the logarithmically binned spectrum is convolved with
/// the recognition kernel for all frequencies. For speedup this
/// convolution product is carried out by two Fourier transformations
/// (in log space instead of real space) with an ordinary multiplication
/// in between. The m-value (slot) where
/// the response of the kernel is maximal is then used as a first guess.
/// \return Estimated frequency in Hz
////////////////////////////////////////////////////////////////////////

double KeyRecognizer::estimateFrequency ()
{
    mFFT.calculateFFT(mLogSpec,mLogSpecFFT);
    for (size_t n = 0; n < mLogSpecFFT.size(); ++n) mLogSpecFFT[n] *= std::conj(mKernelFFT[n]);
    mFFT.calculateFFT(mLogSpecFFT,mConvolution);
    int m  = MathTools::findMaximum(mConvolution);
    double f = mtof(m);
    //Write("3-keyrecog-convolution.dat",mConvolution);

    // Above 1kHz the kernel is no longer reliable. Here we search for the absolute maximum
    if (f > 1000 and m < M - 1000)
    {
        int m2 = MathTools::findMaximum(mLogSpec, m + 1000, M);
        if (mLogSpec[m2] > 1.2 * mLogSpec[m]) f = mtof(m2);
    }
    return f;
}


//-----------------------------------------------------------------------------
//	Find the nearest key to a given frequency, assuming average stretch
//-----------------------------------------------------------------------------

int KeyRecognizer::findNearestKey (double f)
{
    if (mConcertPitch<=390 or mConcertPitch>500) return -1;
    // Approximate distance in keys from A-440:
    double d = 17.3123*log(f/mConcertPitch);
    // Average stretch polynomial, giving the expected deviation in cents
    double c =0.000019394+0.079694594*d-0.003718646*d*d+0.000450934*d*d*d + 0.000003724*d*d*d*d;
    int k=-1; k = static_cast<int>(mKeyNumberOfA+d-c/100+0.5);
    return (k>=0 and k<mNumberOfKeys ? k : -1);
}


//-----------------------------------------------------------------------------
//                        Identify the selected key
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Identify the selected key.
///
/// \param frequency The detected frequency
/// The KeyRecognizer has continuously sent messages containing the number
/// of the recognized key. These results have beeen registered by the
/// in the histogram mKeyCount. This function declares a
/// key as selected if more it was recognized for more than 50%.
/// \return Number of the key, -1 if none.
///////////////////////////////////////////////////////////////////////////////

int KeyRecognizer::identifySelectedKey(double frequency)
{
    double time = mFFTPtr->getTime();
    double meanindex = MathTools::computeMoment(mLogSpec,1);
    double frequencyestimator = Key::IndexToFrequency(meanindex);

    if (time < 1.5 and frequencyestimator > 1100) // if its a key in the diskant
    {
        // Locate the most dominant high-frequency peak
        double maxpeak = 0;
        int pos = 0, start = Key::FrequencyToIndex(1800), end = mLogSpec.size();
        for (int m=start; m<end; ++m) if (mLogSpec[m]>maxpeak) { maxpeak=mLogSpec[m]; pos=m; }
        double frequency = Key::IndexToFrequency(pos);
        return findNearestKey(frequency);
    }

    return findNearestKey(frequency);
}


//-----------------------------------------------------------------------------
//			Write function for development purposes
//-----------------------------------------------------------------------------

void KeyRecognizer::Write(std::string filename, std::vector<double> &v)
{
#if CONFIG_ENABLE_XMGRACE
    std::ofstream os(filename);
    for (uint m=0; m<v.size(); ++m)
    {
        //os << m << "\t" << v[m] << std::endl;
        os << mtof(m) << "\t" << v[m] << std::endl;
    }
    os.close();
#else
    (void)filename; (void)v; // suppress warnings
#endif // CONFIG_ENABLE_XMGRACE
}

