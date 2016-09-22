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
//                             FFT Analyzer
//=============================================================================

#ifndef FFTANALYZER_H
#define FFTANALYZER_H

#include "prerequisites.h"
#include "fftanalyzererrorcodes.h"
#include "../piano/piano.h"
#include "../math/fftadapter.h"
#include "../math/fftimplementation.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Module performing the final analysis of the Fourier transform
///
/// After completion of the recording process, the SignalAnalyzer sends
/// the final Fourier transform via MessageNewFFTCalculated with a 'ture'
/// flag to the FFT analyzer.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN FFTAnalyzer
{
public:

    using SpectrumType = Key::SpectrumType;     ///< Type of a log spectrum
    using PeakListType = Key::PeakListType;     ///< Type for a peak map

    FFTAnalyzer();                              ///< Constructor
    ~FFTAnalyzer() {}                           ///< Empty destructor

    std::pair<FFTAnalyzerErrorTypes, std::shared_ptr<Key> > analyse(
            const Piano *piano,
            FFTDataPointer finalFFT,
            int finalKey);

    FrequencyDetectionResult detectFrequencyOfKnownKey(
            FFTDataPointer finalFFT,
            const Piano *piano,
            const Key &key,
            int keyIndex);

private:

    const int NumberOfBins=Key::NumberOfBins;

    SpectrumType mOptimalSuperposition;         ///< Superposition of the partials
    FFT_Implementation mFFT;                    ///< Instance of FFT implementation
    SpectrumType mCurrentKernel;                ///< The current kernel for the key detection
    const Key *mCurrentKernelKey;               ///< The key of which mCurrentKernel belongs to


private:    
    void   constructLogBinnedSpectrum(FFTDataPointer fftData, SpectrumType &spectrum);
    SpectrumType constructKernel(const SpectrumType &originalSpectrum);
    TuningDeviationCurveType computeTuningDeviation(const SpectrumType &kernel, const SpectrumType &signal, int searchSize);
    int    locatePeak (const SpectrumType &spectrum, int m, int width);
    double interpolatePeakPosition (const SpectrumType &spectrum, int m, int width);
    int    findNearestKey (double f, double conertPitch, int numberOfKeys, int keyNumberOfA);
    double estimateFrequency (int keynumber, double concertPitch, int keyNumberOfA);
    double findAccuratePeakFrequency (FFTDataPointer fftData, double f, int cents=5);

    double getExpectedInharmonicity (double f);
    double estimateInharmonicity (FFTDataPointer fftData, SpectrumType &spectrum, double f);
    double estimateQuality ();
    double estimateFrequencyShift();
    PeakListType identifyPeaks (FFTDataPointer fftData, const SpectrumType &spectrum, const double f, const double B);

    void Write(std::string filename, SpectrumType &v); // only for development
    void Write(std::string filename, FFTComplexVector &v);
    void WritePeaks(std::string filename, SpectrumType &v, PeakListType &peaks); // only for development
};

#endif // FFTANALYZER_H
