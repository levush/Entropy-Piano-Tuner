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
//                               FFT Analyzer
//=============================================================================

#include "fftanalyzer.h"

#include <iostream>
#include <algorithm>
#include <utility>

#include "../config.h"
#include "../system/log.h"
#include "../system/eptexception.h"
#include "../piano/piano.h"
#include "../math/mathtools.h"


//-----------------------------------------------------------------------------
//                                Constructor
//-----------------------------------------------------------------------------

FFTAnalyzer::FFTAnalyzer() :
    mOptimalSuperposition(),            // Array for peak superposition
    mCurrentKernelKey(nullptr)          // Initially no kernel
{}

//-----------------------------------------------------------------------------
//		     Main worker function running in an independent thread
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Main analyzing function
/// \param piano Pointer to the piano
/// \param finalFFT Data of the final fourier transformation
/// \param sampling rate the sampling rate
/// \param finalKey the identified key
/// \returns Shared pointer of the detected key and idendified data
///
/// This function controls the main tasks of the FFTAnalyzer and is
/// used by the SignalAnalyzer to analyze the final fft.
///////////////////////////////////////////////////////////////////////////////

std::pair<FFTAnalyzerErrorTypes, std::shared_ptr<Key> > FFTAnalyzer::analyse (
        const Piano *piano,
        FFTDataPointer finalFFT,
        int finalKey)
{
    // consisty check
    EptAssert(piano, "Piano has to be set");
    EptAssert(finalFFT, "FFT has to exist");
    EptAssert(finalKey >= 0, "The final key has to be set.");

    std::shared_ptr<Key> key;  // the key output

    // dont handle an invalid fft's
    if (!finalFFT->isValid()) {
        LogD("Received invalid FFT to analyse. Cancel the analysis.");
        return std::make_pair(FFTAnalyzerErrorTypes::ERR_DATA, key);
    }


    // Start with the analysis
    LogV("FFTAnalyzer started");

    // Map the final FFT to a logarithmically binned spectrum:
    static SpectrumType spectrum(NumberOfBins);
    constructLogBinnedSpectrum(finalFFT, spectrum);
    Write("4-final-logspec.dat", spectrum);

    // In the first two octaves the ground frequency f1 is very weak
    // or even non-existing. Here we use instead the 2nd or 4th partial
    // for the frequency estimation. This function computes how many
    // octaves we intend to shift:

    auto octaves = [this] (int keynumber, int keyNumberOfA)
    {
        int distance = keyNumberOfA - keynumber;
        if (distance > 36) return 2;
        else if (distance > 24) return 1;
        else return 0;
    };

    // First frequency estimate without inharmonicity
    double factor = pow(2.0,octaves(finalKey, piano->getKeyboard().getKeyNumberOfA4()));
    double f1 = factor * estimateFrequency(finalKey, piano->getConcertPitch(), piano->getKeyboard().getKeyNumberOfA4());
    int m = Key::FrequencyToIndex(f1);
    double f = Key::IndexToFrequency(locatePeak(spectrum,m,40)) / factor;

    LogV("FFTAnalyzer: Estimated frequency f = %f, key = %d", f, finalKey);


    if (f>20 and f<6000)
    {
        // Inharmonicity estimation.
        double B = estimateInharmonicity(finalFFT, spectrum,f);
        LogV("FFTAnalyzer: Inharmonicity    B = %f", B);

        // Correct frequency, taking inharmonicty into account
        f = findAccuratePeakFrequency(finalFFT, factor*f)/factor/sqrt((1+B*factor*factor)/(1+B));
        LogV("FFTAnalyzer: Accurate frequency f = %f", f);

        // Quality check by collapsing higher partials (except of highest ocatave)
        double Q=0;
        if (f<2200)
        {
            Q = estimateQuality();
            LogV("FFTAnalyzer: Quality measure (cents)    Q = %f", Q);

            double cents = estimateFrequencyShift();
            LogV("FFTAnalyzer: frequency correction cents C = %f", cents);
        }

        // Create a new key:
        // This is done with a shared pointer, meaning that the instance
        // will be deleted automatically if it is no longer used
        key.reset(new Key());

        key->setRecordedFrequency(f);
        key->setMeasuredInharmonicity(B);
        key->setRecognitionQuality(Q);
        key->setSpectrum(spectrum);
        key->setRecorded(true);

        auto peaks = identifyPeaks(finalFFT, spectrum,f,B);
        // %zu is c++ standard, but windows doenst support this... workaround use int
        LogV("FFTAnalyzer: found %i peaks.", static_cast<int>(peaks.size()));
        key->setPeaks(peaks);
    }
    else
    {
        LogW("Frequence %f is out of bounds", f);
        return std::make_pair(FFTAnalyzerErrorTypes::ERR_FREQUENCY_OUT_OF_BOUNDS, key);
    }

    LogV("leaving FFTAnalyzer thread.");

    return std::make_pair(FFTAnalyzerErrorTypes::ERR_NONE, key);
}

//-----------------------------------------------------------------------------
//                 Determine the frequency of a known key
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief FFTAnalyzer::detectFrequencyOfKnownKey
/// \param finalFFT
/// \param piano
/// \param key
/// \param keyIndex
/// \return
///////////////////////////////////////////////////////////////////////////////

FrequencyDetectionResult FFTAnalyzer::detectFrequencyOfKnownKey (
        FFTDataPointer finalFFT,
        const Piano *piano,
        const Key &key,
        int keyIndex)
{
    // consisty check
    EptAssert(piano, "Piano has to be set");
    EptAssert(finalFFT, "FFT has to exist");
    EptAssert(finalFFT->isValid(), "The FFT data is not valid");
    EptAssert(keyIndex >= 0, "The final key has to be set.");

    // Create a shared pointer in which the result will be stored:
    FrequencyDetectionResult result = std::make_shared<FrequencyDetectionResultStruct>();

    // This is the frequency to which we would like to tune the string
    double targetFrequency = piano->getConcertPitch()/440.0 *
                             key.getComputedFrequency();

    if (targetFrequency <= 20 or targetFrequency > 10000)
    {
        result->error = FFTAnalyzerErrorTypes::ERR_NO_COMPUTED_FREQUENCY;
        return result;
    }

    // Define the frequency which corresponds to the middle of the array
    double centerFrequency = key.getRecordedFrequency();
    if (centerFrequency<=10) centerFrequency = targetFrequency;

    // Map the final FFT to a logarithmically binned spectrum:
    SpectrumType spectrum(NumberOfBins);
    constructLogBinnedSpectrum(finalFFT, spectrum);

    // Define the search size around the center frequency in cents
    const int searchSize = 200;
    TuningDeviationCurveType out(searchSize), zoomedPeak (searchSize);


    // First method: Simply magnify the lowest peak
    if (not key.isRecorded())
    {
        double maximum = 0;
        double middle = centerFrequency;
        if (piano->getKeyboard().getKeyNumberOfA4()-keyIndex > 24)
        {
            double B = piano->getExpectedInharmonicity(centerFrequency);
            middle *= 2 * sqrt((1+4*B)/(1+B));
        }

        for (int i=0; i<searchSize; ++i)
        {
            int m = Key::FrequencyToRealIndex(middle) - searchSize/2 + i;
            if (m>0 and m<static_cast<int>(spectrum.size()))
            {
                double value = spectrum[m] * spectrum[m];
                if (value > maximum) maximum = value;
                zoomedPeak[i] = value;
            }
        }
        if (maximum < 1E-15)
        {
            result->error = FFTAnalyzerErrorTypes::ERR_NO_PEAK_AMPLITUDE;
            return result;
        }
        for (auto &element : zoomedPeak) element /= maximum;
        out = std::move(zoomedPeak);
    }

    // else if the key has been recorded explicitely we use the method developed
    // by Christoph. It uses an inverse kernel convolution
    else
    {


        // create the kernel of the key, if the key changed
        if (&key != mCurrentKernelKey)
        {
            mCurrentKernel = constructKernel(key.getSpectrum());
            mCurrentKernelKey = &key;
        }

        // compute the deviation
        out = computeTuningDeviation(mCurrentKernel, spectrum, searchSize);
    }

    int maxIndex = MathTools::findMaximum(out);
    double index = MathTools::weightedArithmetricMean(out, std::max(maxIndex - 10, 0), maxIndex + 10);
    index -= searchSize / 2;

    double detectedFrequency = centerFrequency * std::pow(2.0, (index) / 1200.0);
    EptAssert(detectedFrequency > 1 && detectedFrequency < 20000, "Unallowed frequency range");

    int computedIndex = MathTools::roundToInteger(log(targetFrequency / centerFrequency) * 1200 / MathTools::LOG2);


    result->deviationInCents = static_cast<int>(index - computedIndex);
    result->detectedFrequency = detectedFrequency;
    result->positionOfMaximum = index;
    result->tuningDeviationCurve = std::move(out);

    LogV("Deviation %d, comp index %d", result->deviationInCents, computedIndex);

    return result;
}

//-----------------------------------------------------------------------------
//                 Construct logarithmically binned spectrum
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Construct logarithmically binned spectrum from the mFinalFFT.
///
/// The FFT analysis yields a Fourier transform with an index which is linear
/// in the frequency. For the tuning process, however, we need a spectrum
/// which is logarithmic in the frequency. Only then it is possible to tune
/// the key by simply shifting the index of the spectrum. This function
/// transforms the incoming FFT into a logarithmically binned spectrum.
/// The dimension NumberOfBins is copied from the constant in class Key.
/// The actual map between the indices can be found in Key::IndexToFrequency.
///
/// \param fftData Data of the fourier transform
/// \param spectrum Vector holding the logarithmically binned spectrum.
///////////////////////////////////////////////////////////////////////////////

void FFTAnalyzer::constructLogBinnedSpectrum(FFTDataPointer fftData, SpectrumType &spectrum)
{
    const double b = 2.0 * fftData->fft.size() / fftData->samplingRate;
    std::function<double(double)> mtoq = [this,b] (double m)
             { return b * Key::IndexToFrequency(m); };
    MathTools::coarseGrainSpectrum (fftData->fft,spectrum,mtoq,0.25);
    MathTools::normalize(spectrum);
}

FFTAnalyzer::SpectrumType FFTAnalyzer::constructKernel(const SpectrumType &originalSpectrum)
{
    SpectrumType kernel(NumberOfBins);
    FFTComplexVector fftOfOriginal;
    mFFT.calculateFFT(originalSpectrum, fftOfOriginal);
    for (FFTComplexType &c : fftOfOriginal) {
        c = c / (c.real() * c.real() + c.imag() * c.imag());
    }
    mFFT.calculateFFT(fftOfOriginal, kernel);
    return kernel;
}

TuningDeviationCurveType FFTAnalyzer::computeTuningDeviation(
        const SpectrumType &kernel, const SpectrumType &signal, int searchSize)
{
    const int searchOffset = searchSize / 2;
    TuningDeviationCurveType out(searchSize);

    // in a range of 50 ct find the maximum when folding the kernel with the spectrum
    for (int j = -searchOffset; j < searchSize - searchOffset; j++) {
        out[j + searchOffset] = 0;
        for (int i = 0; i < NumberOfBins; i++) {
            out[j + searchOffset] += kernel[(i - j + NumberOfBins) % NumberOfBins] * signal[i];
        }
    }

    return out;
}

//-----------------------------------------------------------------------------
//       Locate a single peak in the logarithmically binned spectrum
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Search for a local peak in the logarithmically binned spectrum
/// \param spectrum : Logarithmically binned spectrum
/// \param m : Spectral index
/// \param width : Searching width in +/- cents
/// \return Integer index where the maximum is lodated
//////////////////////////////////////////////////////////////////////////////

int FFTAnalyzer::locatePeak (const SpectrumType &spectrum, int m, int width)
{
    EptAssert(spectrum.size()==static_cast<size_t>(NumberOfBins)
              and spectrum.size()>0, "Inconsistent arguments");
    if (m<width or m>NumberOfBins-width) return 0;
    return MathTools::findMaximum (spectrum,m-width,m+width);
}


//-----------------------------------------------------------------------------
//       Interpolate the position of the maximum
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Search for a local peak in the logarithmically binned spectrum.
/// Optimize the index as a floating-point value
/// \param spectrum : Logarithmically binned spectrum
/// \param m : Spectral index
/// \param width : Searching width in +/- cents
/// \return Index where the maximum is located
//////////////////////////////////////////////////////////////////////////////

double FFTAnalyzer::interpolatePeakPosition (const SpectrumType &spectrum, int m, int width)
{
    EptAssert(spectrum.size()==static_cast<size_t>(NumberOfBins)
              and spectrum.size()>0, "Inconsistent arguments");
    if (m<width+1 or m>NumberOfBins-width-1) return 0;
    int mmax = MathTools::findMaximum (spectrum,m-width,m+width);
    double y1=spectrum[mmax-1], y2=spectrum[mmax], y3=spectrum[mmax+1];
    double N = y1 - 2*y2 + y3;
    if (N == 0) return mmax;
    else
    {
        double correction = (y1-y3)/2/N;
        if (fabs(correction)<1) return mmax+correction;
        else return mmax;
    }
}


//-----------------------------------------------------------------------------
//	Find the nearest key to a given frequency, assuming average stretch
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Compute the number of the nearest key.
///
/// This function uses a heuristic average tuning curve to estimate the
/// most likely number of the key for a given frequency.
/// \param f : Frequency in Hz.
/// \return Key number.
///////////////////////////////////////////////////////////////////////////////

int FFTAnalyzer::findNearestKey (double f, double conertPitch, int numberOfKeys, int keyNumberOfA)
{
    EptAssert(conertPitch>390 or conertPitch<500,"Concert pitch unreasonable.");
    // Approximate distance in keys from A-440:
    double d = 17.3123*log(f / conertPitch);
    // Average stretch polynomial, giving the expected deviation in cents
    double c =0.000019394+0.079694594*d-0.003718646*d*d+0.000450934*d*d*d + 0.000003724*d*d*d*d;
    int k=-1; k = static_cast<int>(keyNumberOfA + d-c/100+0.5);
    return (k>=0 and k<numberOfKeys ? k : -1);
}


//-----------------------------------------------------------------------------
//                 Roughly frequency estimate for a given key
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Estimate the frequency for a given keynumber.
///
/// This function uses a heuristic average tuning curve to roughly estimate
/// the frequency of a given key. This function can be seen as an inverse of
/// findNearestKey.
/// \param keynumber : Number of the key
/// \return Frequency in Hz with respect to the actual mConcertPitch.
///////////////////////////////////////////////////////////////////////////////

double FFTAnalyzer::estimateFrequency (int keynumber, double concertPitch, int keyNumberOfA)
{
    EptAssert(concertPitch>390 or concertPitch<500,"Concert pitch unreasonable.");
    // Distance in keys from A-440:
    double d = keynumber - keyNumberOfA;
    // Average stretch polynomial, giving the expected deviation in cents
    double c =0.000019394+0.079694594*d-0.003718646*d*d+
            0.000450934*d*d*d + 0.000003724*d*d*d*d;
    return pow(2.0, d/12+c/1200) * concertPitch;
}


//-----------------------------------------------------------------------------
//      Find accurate frequency of the peak in original spectrum
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Find the accurate frequency of a spectral peak in the original FFT.
///
/// The logarithmically binned spectrum has a resolution of one cent. However,
/// the spectral peaks are not always mapped faithfully but their location
/// may be slightly different (due to the integration involved in the mapping).
/// To eliminate this uncertainty, the present function looks for the maximum
/// of the spectral line in the original FFT and returns its frequency.
/// \param f : Estimate of the frequency where the peak is supposed to be.
/// \param cents : Width of the search window around f in cents.
/// \return : Frequency in Hz.
///////////////////////////////////////////////////////////////////////////////

double FFTAnalyzer::findAccuratePeakFrequency(FFTDataPointer fftData, double f, int cents)
{
    // Define the bounds in which the peak will be searched
    const double factor = 1.0 + 0.000577623 * cents;
    const double b = 2.0 * fftData->fft.size() / fftData->samplingRate;
    int q1 = MathTools::roundToInteger(b*f/factor);
    int q2 = MathTools::roundToInteger(b*f*factor);

    // search for the maximum
    if (q1 > 0 and q2 < static_cast<int>(fftData->fft.size()))
    {
        double fftmax=0;
        int qmax=q1;
        for (int q=q1; q<q2; ++q) if (fftData->fft[q]>fftmax)
        { fftmax = fftData->fft[q]; qmax=q; }
        return qmax/b;
    }
    else return f;
}


//-----------------------------------------------------------------------------
//			 Compute rough estimate for the expected inharmonicity
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// This function provides a very rough estimate of the expected
/// inharmonicity coefficient of an average upright. It is used
/// improve the expectation where the spectral lines should be located
/// in the spectrum during key recognition.
///
/// \param f : frequency in Hz
/// \return Inharmonicity coefficient B
///////////////////////////////////////////////////////////////////////////////

double FFTAnalyzer::getExpectedInharmonicity (double f)
{
    return (f > 100 ? exp(-15.45 + 1.354*log(f)) : 0.000099575);
}


//-----------------------------------------------------------------------------
//                      Estimate inharmonicity
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Estimate the inharmonicity B
///
/// This function estimates the inharmonicity coefficient B on the basis of
/// the recorded data. This estimate serves as a quality and consistency check.
///
/// The function uses the frequency estimate that has been obtained before
/// by determineFrequency. The frequency and inhamronicity is then optimized
/// iteratively. This is achieved by minimizing the Renyi entropy of the
/// collapsed peak according to the known inharmonicity formula.
///
/// \param spectrum : Logarithmically binned spectrum
/// \param f : Previous frequency estimate
/// \return Estimated inharmonicity coefficient B (dimensionless).
///////////////////////////////////////////////////////////////////////////////

double FFTAnalyzer::estimateInharmonicity (FFTDataPointer fftData, SpectrumType &spectrum, double f)
{
    // do not evaluate inharmonicity if parameters are invalid
    if (spectrum.size()==0 or f<20) return 0;

    // do not evaluate inharmonicity in the very high treble (top octave)
    // because this is usually very unreliable
    if (f > 2250) return 0;

    // for frequencies above 1kHz only f1 and f2 are taken into account
    // and the inharmonicity B is calculated directly from their ratio.
    if (f>1000)
    {
        double f2 = findAccuratePeakFrequency (fftData, 2.0174*f, 15);
        double z = f2*f2/f/f;
        if (z>4.4 or z<4) return 0;
        double B = (4-z)/(z-16);
        LogV("FFTAnalyzer: treble: B = %f", B);
        return B;
    }

    // In the frequency range from 26 Hz to 1kHz the inharmonicity is computed
    // by superposition of N higher partials and searching for the lowest
    // Renyi entropy of the superposition.

    // Define the number of partials taken into accout.
    int N=MathTools::roundToInteger(4*(8-log(f)));

    double B = 0;               // Inharmonicity to be determined
    double Hmin = 1E100;        // Initial Renyi entropy very high
    const int R = 80;           // width of the observation window in cents

    // Calling this function gives a first rough estimate of the inharmonicty
    double expected_B = getExpectedInharmonicity(f);
    LogV("FFTAnalyzer: expected B = %f", expected_B);

    mOptimalSuperposition.clear();
    for (double scan_B = expected_B/5; scan_B <= expected_B*5; scan_B*=1.03)
    {
        SpectrumType superposition(R,0);
        for (int n=1; n<=N; ++n)
        {
            double fn = n*f*sqrt((1+scan_B*n*n)/(1+scan_B));
            double mn = Key::FrequencyToRealIndex(fn);
            SpectrumType partialspectrum(R,0);
            if (mn-R/2>0 and mn+R/2<NumberOfBins)
            {
                for (int r=0; r<R; r++)
                {
                    int m = static_cast<int>(mn+r-R/2);
                    EptAssert (m>=0 and m<NumberOfBins,"m invalid");
                    partialspectrum[r]=pow(spectrum[m],2);
                }
                MathTools::normalize(partialspectrum);
                for (int r=0; r<R; r++) superposition[r]+=partialspectrum[r];
            }
        }

        MathTools::normalize(superposition);
        double H = MathTools::computeRenyiEntropy(superposition,0.1);
        if (fabs(H)<Hmin)
        {
            Hmin=H;
            B=scan_B;
            mOptimalSuperposition = superposition;
            Write("7-find-inharmonicity.dat",superposition);
        }
    }
    LogV("FFTAnalyzer: finished estimating inharmonicity: B = %f", B);

#if CONFIG_ENABLE_XMGRACE
    //system("killall -9 xmgrace; xmgrace -maxpath 200000  -fixed 570 440 -geometry 800x660 7-find-inharmonicity.dat &");
#endif  // CONFIG_ENABLE_XMGRACE

    return B;
}


//-----------------------------------------------------------------------------
//                  estimate quality of the recorded sound
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Estimate the quality of the recorded signal
///
/// This function estimates the quality of the recorded sound. As a criterion
/// for the quality we consider the coincidence of the partial series with
/// the theoretical formula fn=f1*n*sqrt((1+B*n*n)/(1+B)). To this end we
/// analyze the optimal superposition of all partials according to this
/// formula. This is essentially what the tuning indicator shows in its
/// window.
/// \return Quality between 0 and 1
///////////////////////////////////////////////////////////////////////////////

double FFTAnalyzer::estimateQuality ()
{
    // if there is no such data return quality zero
    if (mOptimalSuperposition.size()==0) return 0;

    // cut the superposition at the edges
    const int cut = static_cast<int>(mOptimalSuperposition.size()/2)-10;
    SpectrumType vec = SpectrumType(mOptimalSuperposition.begin()+cut,
                                    mOptimalSuperposition.end()-cut);

    // Compute the zeroth, first, and second moment of the distribution
    double M0 = MathTools::computeNorm(vec);
    if (M0==0) return 0; // if not normalizable return zero quality
    MathTools::normalize(vec);
    double M1 = MathTools::computeMoment(vec,1);
    double M2 = MathTools::computeMoment(vec,2);

    // Determine the variance from the moments
    double variance = M2-M1*M1;
    // translate heuristicaly in a measure of quality to be displayed.
    // M0 is the fraction of the data that was not cut away.
    // Therefore, the result is expected to be between 0 and 1
    return M0/(1+0.1*pow(variance,1.5));
}


//-----------------------------------------------------------------------------
//              estimate frequency shift of the recorded sound
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Estimate the frequency shift
///
/// This function estimates how much the frequency determined by the
/// collapse of the partials deviates from the center. In other words, it
/// returns the deviation (in cent) of the center of mass of the peak shown
/// in the tuning indicator.
/// \return Deviation in cent
///////////////////////////////////////////////////////////////////////////////

double FFTAnalyzer::estimateFrequencyShift()
{
    // cut out a section of 20 cents
    size_t start = mOptimalSuperposition.size()/2-10;
    size_t stop  = mOptimalSuperposition.size()/2+10;
    if (start>=stop or stop>=mOptimalSuperposition.size()) return 0;
    SpectrumType vec = SpectrumType(mOptimalSuperposition.begin()+start,
                                    mOptimalSuperposition.begin()+stop);

    // The deviation is the first moment of the distribution in this section
    return MathTools::computeMoment(vec,1)-10;
}


//-----------------------------------------------------------------------------
//      Identify peaks of the spectrum for given inharmonicity
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Identify the peaks in the spectrum
/// \param fftData : Original (non-logarithmic) FFT data
/// \param spectrum : Converted (logarithmically organized) spectrum
/// \param f : Supposed fundamental frequency
/// \param B : Supposed inharmonicity parameter
/// \return List of peaks
///////////////////////////////////////////////////////////////////////////////

FFTAnalyzer::PeakListType FFTAnalyzer::identifyPeaks (FFTDataPointer fftData,
                                                      const SpectrumType &spectrum,
                                                      const double f, const double B)
{
    // Define the number of peaks to be analyzed depending on the frequency
    const int MaxNumberOfPeaks = 50;
    int N = std::min(MaxNumberOfPeaks, static_cast<int>(10000.0/f));   // number of peaks

    // Define the usual inhamonicity formula
    auto InharmonicPartial = [] (double f, int n, double B) { return f*n*sqrt((1+B*n*n)/(1+B)); };

    PeakListType peaks;
    for (int n=1; n<=N; ++n)
    {
        // Compute the frequency where the peak should be
        double fn = InharmonicPartial(f,n,B);

        // Locate it in the logarithmically organized spectrum in a window of +/- 20 cents
        int m = locatePeak (spectrum, Key::FrequencyToIndex(fn), 20);

        // If succesful refine the search by looking for the maximum in the original FFT
        if (m>0)
        {
            double f = Key::IndexToFrequency(m);
            double fc = findAccuratePeakFrequency(fftData, f);
            // Append the refined value to the list of peaks
            peaks[fc]=spectrum[m];
        }
    }
    return peaks;
}


//-----------------------------------------------------------------------------
//	   Write function for development purposes, not active in the release
//-----------------------------------------------------------------------------

void FFTAnalyzer::Write(std::string filename, SpectrumType &v)
{
#if CONFIG_ENABLE_XMGRACE
    std::ofstream os(filename);
    for (uint m=0; m<v.size(); ++m)
    {
        if (v.size() != static_cast<size_t>(NumberOfBins)) os << m << "\t" << v[m] << std::endl;
        else os << Key::IndexToFrequency(m) << "\t" << v[m] << std::endl;
    }
    os.close();
#else
    (void)filename; (void)v; // suppress warnings
#endif // CONFIG_ENABLE_XMGRACE
}



void FFTAnalyzer::Write(std::string filename, FFTComplexVector &v)
{
#if CONFIG_ENABLE_XMGRACE
    std::ofstream os(filename);
    for (uint m=0; m<v.size(); ++m)
    {
        os << m << "\t" << v[m].real() << std::endl;
    }
    os << "&" << std::endl;
    for (uint m=0; m<v.size(); ++m)
    {
        os << m << "\t" << v[m].imag() << std::endl;
    }
    os.close();
#else
    (void)filename; (void)v; // suppress warnings
#endif // CONFIG_ENABLE_XMGRACE
}



void FFTAnalyzer::WritePeaks(std::string filename, SpectrumType &v,PeakListType &peaks)
{
#if CONFIG_ENABLE_XMGRACE
    std::ofstream os(filename);
    os << "@g0 type logxy" << std::endl;
    os << "# logspec / peaks according to F&B / peaks in the list" << std::endl;
    for (int m=0; m<Key::NumberOfBins; ++m) if (v[m]>1E-100) os << Key::IndexToFrequency(m) << "\t" << v[m] << std::endl;
    os << "&" << std::endl;
//    for (int n=1; n<=N; ++n)
//    {
//        double fn = InharmonicPartial(F,n,B);
//        os << fn-0.00001 << tab << 1E-10 << std::endl;
//        os << fn << tab << 1 << std::endl;
//        os << fn+0.00001 << tab << 1E-10 << std::endl;
//    }
//    os << "&" << std::endl;
    for (auto p:peaks)
    {
        double fn = Key::IndexToFrequency(p.first);
        os << fn-0.00001 << '\t' << 1E-9 << std::endl;
        os << fn << '\t' << p.second << std::endl;
        os << fn+0.00001 << '\t' << 1E-9 << std::endl;
    }
#else
    (void)filename; (void)peaks; (void)v;  // suppress warnings
#endif  // CONFIG_ENABLE_XMGRACE
}
