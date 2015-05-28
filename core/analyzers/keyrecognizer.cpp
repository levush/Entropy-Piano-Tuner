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

const int    KeyRecognizer::M = 1024;
const double KeyRecognizer::fmin = 10;
const double KeyRecognizer::fmax = 20000;
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
    mNumberOfKeys(0),                   // Number of piano keys (normally 88)
    mKeyNumberOfA(0),                   // Index of the A-key (normally 48)
    mFFT(),                             // Instance of FFT implementation
    mLogSpec(M),                        // Vector holding the log. spectrum
    mLogLogSpec(M),                     // Vector holding the loglog. spectrum
    mKernelFFT(M/2+1),                  // Vector holding the complex FFT of the kernel
    mLogLogSpecFFT(M/2+1),              // Vector holding the complex FFT of the loglogspec
    mConvolution(M)                    // Convolution vector, real
{}


//-----------------------------------------------------------------------------
//			                   Initialization
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Initialization of the KeyRecognizer.
///
/// This function defines the kernel and optimizes the plan for the FFT.
/// Optimization will take a while to be finished.
///
/// \param optimize : true if the FFT should be optimized
///
///////////////////////////////////////////////////////////////////////////////

void KeyRecognizer::init(bool optimize)
{
    VERBOSE("KeyRecognizer: starting initialization");
    defineKernel();
    if (optimize)
    {
        mFFT.optimize(mLogSpec);
        mFFT.optimize(mLogLogSpecFFT);
    }
    VERBOSE("KeyRecognizer: initialization finished");
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
///////////////////////////////////////////////////////////////////////////////

void KeyRecognizer::recognizeKey (
        bool forceRestart,
        const Piano *piano,
        FFTDataPointer fftPointer)
{
    EptAssert(piano, "The piano has to be set.");
    EptAssert(fftPointer, "The fft data has to exist.");
    EptAssert(fftPointer->isValid(), "Invaild fft data");

    if (forceRestart) stop();                   // if restart forced stop thread
    else if (isThreadRunnding()) return;        // if it is running do nothing

    // copy data from the piano
    mConcertPitch = piano->getConcertPitch();
    mNumberOfKeys = piano->getKeyboard().getNumberOfKeys();
    mKeyNumberOfA = piano->getKeyboard().getKeyNumberOfA4();

    mFFTPtr = fftPointer;           // save pointer to the Fourier transform

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
    EptAssert(mFFTPtr, "FFT Data have to non zero");
    EptAssert(mFFTPtr->isValid(), "FFT Data have to exist");
    EptAssert(mCallback, "Callback class has to exist");

    constructLogSpec();             // Compute log spectrum
    CHECK_CANCEL_THREAD;

    signalPreprocessing();          // Compute flattened loglog spectrum
    CHECK_CANCEL_THREAD;

    double f = estimateFrequency(); // Estimate frequency
    CHECK_CANCEL_THREAD;

    int keynumber = findNearestKey(f);      // determine keynumber

    std::cout << "KeyRecognizer: f=" << f << ", key=" << keynumber << std::endl;

    mCallback->keyRecognized(keynumber, f); // notify callback
}


//-----------------------------------------------------------------------------
//			             Index mapping functions
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

void KeyRecognizer::constructLogSpec()
{
    const int Q = mFFTPtr->fft.size();
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
    std::vector<double> copy(M);
    for (int i=0; i<M; ++i) copy[i]=(mLogSpec[i]>0 ? log(mLogSpec[i]) : 0);

    // Flatten noise by subtracting a gliding average
    const int w=30;             // width of the average
    for (int i=0; i<M; ++i)
    {
        int a = std::max(0,i-w);
        int b = std::min(M,i+w+1);
        double sum=0;
        for (int k=a; k<b; ++k) sum+=copy[k];
        mLogLogSpec[i]=copy[i]-sum/(b-a);
    }
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
    static const double B=0.0005;   // here still with a constant inharmonicity

    static std::vector<double> kernel(M); //must be static for using fftw3
    kernel.assign(M,0);

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

    //Write("keyrecog-kernel.dat",kernel,false);
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
    mFFT.calculateFFT(mLogLogSpec,mLogLogSpecFFT);
    for (size_t n = 0; n < mLogLogSpecFFT.size(); ++n)
        mLogLogSpecFFT[n] *= std::conj(mKernelFFT[n]);
    mFFT.calculateFFT(mLogLogSpecFFT,mConvolution);
    int m  = MathTools::findMaximum(mConvolution);
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


////-----------------------------------------------------------------------------
////			Write function for development purposes
////-----------------------------------------------------------------------------

//void KeyRecognizer::Write(std::string filename, std::vector<double> &v, bool log)
//{
//#if CONFIG_ENABLE_XMGRACE
//    std::ofstream os(filename);
//    for (uint m=0; m<v.size(); ++m)
//    {
//        //os << m << "\t" << v[m] << std::endl;
//        if (log) os << mtof(m) << "\t" << v[m] << std::endl;
//        else os << m << "\t" << v[m] << std::endl;
//    }
//    os.close();
//#else
//    (void)filename; (void)v; // suppress warnings
//#endif // CONFIG_ENABLE_XMGRACE
//}

