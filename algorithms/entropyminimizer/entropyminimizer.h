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
//                       Iterative entropy minimization
//=============================================================================

#ifndef ENTROPYMINIMIZER_H
#define ENTROPYMINIMIZER_H

#include "core/calculation/algorithmplugin.h"

/// Namespace for all entropy minimizer components
ALGORITHM_H_START(entropyminimizer)

///////////////////////////////////////////////////////////////////////////////
/// \brief Iterative entropy minimization algorithm
///
/// This class is at the center of the entropy piano tuner, i.e. it carries
/// out the actual tuning procedure.
///
/// The most important member variable is the mAccumulator, where the spectra
/// are added up. The purpose of the accumulator is to save time. Instead of
/// computing the sum of all spectra after each Monte Carlo step again, we
/// simply subtract the previous and add the new spectrum of the modified
/// key alone.
///////////////////////////////////////////////////////////////////////////////


class EntropyMinimizer : public Algorithm, public MessageListener
{
public:
    EntropyMinimizer(const Piano &piano, const AlgorithmFactoryDescription &desciption);
    ~EntropyMinimizer(){};

protected:
    virtual void handleMessage(MessagePtr m) override final;
    void algorithmWorkerFunction() override final;

private:
    bool performAuditoryPreprocessing();

    void ComputeInitialTuningCurve ();
    void minimizeEntropy ();

    using SpectrumType = Key::SpectrumType;
    using Keys = Keyboard::Keys;
    const int NumberOfBins = Key::NumberOfBins;


private:

    /// Convert frequency to array index in cent spacing
    static double ftom (double f) { return Key::FrequencyToRealIndex(f); }
    /// Convert array index in cent spacing to frequency in Hz
    static double mtof (int m)    { return Key::IndexToFrequency(m); }

    void updateTuningcurve (int keynumber);
    void updateTuningcurve ();
    void clear();
    double getElement(SpectrumType &spectrum, int m);
    void addToAccumulator (SpectrumType &spectrum, int shift, double intensity);
    void modifySpectralComponent (int key, int pitch);
    void setAllSpectralComponents();
    void addReferenceSpectrum (double intensity);
    int  getTolerance (int keynumber);

    double computeEntropy();

private:
    SpectrumType mAccumulator;          ///< Accumulator holding the sum of all spectra
    std::vector<int> mPitch;            ///< Vector of pitches (in cents)
    std::vector<double>mInitialPitch;   ///< Vector of initial pitches
    int mLowerCutoff;                   ///< Lower cutoff for fluctuations
    int mUpperCutoff;                   ///< Upper cutoff for fluctuations
    bool mRecalculateEntropy;           ///< Flag for entropy recalculation (after manual intervention by the user)
    int mRecalculateKey;                ///< Number of manually changed key
    double mRecalculateFrequency;       ///< Frequency of manually changed key

    double getRecordedPitchET440(int keynumber);               ///< Get recorded pitch
    int    getRecordedPitchET440AsInt(int keynumber);          ///< Get recorded pitch
    int getPitchET440(int keynumber, double f);                ///< Get pitch from frequ

protected:
    // only for development:
    void writeAccumulator(std::string filename);
    void writeSpectrum(int k, std::string filename, int pitch=0);
};

ALGORITHM_H_END(EntropyMinimizer)


#endif // ENTROPYMINIMIZER_H
