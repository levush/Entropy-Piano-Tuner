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
//                       Iterative entropy minimization
//=============================================================================

#ifndef ENTROPYMINIMIZER_H
#define ENTROPYMINIMIZER_H

#include "core/calculation/algorithm.h"
#include "core/messages/messagelistener.h"

/// Namespace for all entropy minimizer classes
namespace entropyminimizer
{

///////////////////////////////////////////////////////////////////////////////
/// \brief Iterative minimization of the entropy
///
/// This class is at the center of the entropy piano tuner, i.e. it carries
/// out the actual tuning procedure.
///
/// The most important member variable is the mAccumulator, where the spectra
/// are added up.
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

    static double ftom (double f) { return Key::FrequencyToRealIndex(f); }
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
    SpectrumType mAccumulator;
    std::vector<int> mPitch;
    std::vector<double>mInitialPitch;
    int mLowerCutoff;
    int mUpperCutoff;
    bool mRecalculateEntropy;
    int mRecalculateKey;
    double mRecalculateFrequency;

    double getRecordedPitchET440(int keynumber);               ///< Get recorded pitch
    int    getRecordedPitchET440AsInt(int keynumber);          ///< Get recorded pitch
    int getPitchET440(int keynumber, double f);                ///< Get pitch from frequ

protected:
    // only for development:
    void writeAccumulator(std::string filename);
    void writeSpectrum(int k, std::string filename, int pitch=0);
};

}  // namespace entropyminimizer

#endif // ENTROPYMINIMIZER_H
