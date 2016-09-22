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
//                           Auditory preprocessing
//=============================================================================

#ifndef AUDITORYPREPROCESSING_H
#define AUDITORYPREPROCESSING_H

#include "core/system/simplethreadhandler.h"
#include "core/system/log.h"
#include "core/piano/piano.h"

namespace entropyminimizer
{

////////////////////////////////////////////////////////////////////////
/// \brief Class of functions for auditory preprocessing
///
/// This class includes functions needed for the preparation of the
/// spectra. It is controlled by the CalculationManager.
////////////////////////////////////////////////////////////////////////

class AuditoryPreprocessing
{
public:
    AuditoryPreprocessing(Piano &piano);
    ~AuditoryPreprocessing(){};

    using SpectrumType = Key::SpectrumType;
    using Keys = Keyboard::Keys;
    const size_t NumberOfBins = Key::NumberOfBins;

public:


    bool checkDataConsistency();        // check data consistency
    void normalizeSpectrum(Key &key);
    double getInharmonicPartial (double n,double f1,double B);
    double getInharmonicPartialIndex (double f, double f1, double B);
    void cleanSpectrum(Key &key);                // reduce noise in the spectra
    void cutLowFrequencies(Key &key);           // cut irrelevant low and high frequ

    void initializeSPLAFilter();    // define dBA filtering curve
    void convertToSPLA(SpectrumType &s);                 // compute A-weighted sound pressure
    void convertToLoudness (SpectrumType &s);

    void extrapolateInharmonicity();
    void improveHighFrequencyPeaks();
    void applyMollifier(Key &key);


private:
    static double ftom (double f) { return Key::FrequencyToRealIndex(f); }
    static double mtof (int m)    { return Key::IndexToFrequency(m); }

    Piano &mPiano;
    Keyboard &mKeyboard;
    Keys &mKeys;
    int mNumberOfKeys;
    int mKeyNumberOfA4;
    std::vector<double> mdBA;           // vector holding dBA curve

};

}  // namespace entropyminimizer

#endif // AUDITORYPREPROCESSING_H
