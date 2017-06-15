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

#include "auditorypreprocessing.h"

#include <iostream>

#include "core/system/eptexception.h"
#include "core/messages/messagehandler.h"
#include "core/messages/messagecaluclationprogress.h"
#include "core/piano/keyboard.h"
#include "core/piano/key.h"
#include "core/math/mathtools.h"
#include "core/config.h"

namespace entropyminimizer
{

//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor
///
/// Initializes the member variables.
///////////////////////////////////////////////////////////////////////////////

AuditoryPreprocessing::AuditoryPreprocessing (Piano &piano) :
    mPiano(piano),
    mKeyboard(mPiano.getKeyboard()),
    mKeys(mKeyboard.getKeys()),
    mNumberOfKeys(mKeyboard.getNumberOfKeys()),
    mKeyNumberOfA4(mKeyboard.getKeyNumberOfA4()),
    mdBA()
{
}


//-----------------------------------------------------------------------------
//			              Check consistency of dataset
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Check consistency of the piano dataset
///
/// This function tests the consistency of the incoming Piano dataset.
/// \return true if consistent.
///////////////////////////////////////////////////////////////////////////////
///
bool AuditoryPreprocessing::checkDataConsistency()
{
    // First check whether the number of keys is consistent.
    EptAssert(mKeys.size() > 0, "Piano should have at least one key");
    EptAssert(mKeys.size() == static_cast<size_t>(mNumberOfKeys),
              "Key vector length mismatch");

    // Next let us see whether all keys have been recorded.
    bool allkeysrecorded = true;
    bool somekeysrecorded = false;
    for (Key key : mKeys)
    {
        if (key.isRecorded()) somekeysrecorded = true;
        else allkeysrecorded = false;
    }
    if (not somekeysrecorded)
    {
        MessageHandler::send<MessageCaluclationProgress>
                (MessageCaluclationProgress::CALCULATION_FAILED,
                 MessageCaluclationProgress::CALCULATION_ERROR_NO_DATA);
        LogW("Calculation started without data");
        return false;
    }
    if (not allkeysrecorded)
    {
        MessageHandler::send<MessageCaluclationProgress>
                (MessageCaluclationProgress::CALCULATION_FAILED,
                 MessageCaluclationProgress::CALCULATION_ERROR_NOT_ALL_KEYS_RECORDED);
        LogW("Not all keys have been recorded");
        return false;
    }

    // Thirdly, let us see whether the frequencies and inharmonicities
    // are within reasonable bounds
    bool consistent = true;
    for (int keynumber=0; keynumber<mNumberOfKeys; keynumber++)
    {
        Key &key = mKeys[keynumber];
        double f = key.getRecordedFrequency();
        if (f<20 or f>20000)
        {
            LogW("Key %d: Frequency f=%f out of range.",keynumber,f);
            consistent = false;
        }
        double B = key.getMeasuredInharmonicity();
        if (B<0 or B>1)
        {
            LogW("Key %d: Inharmonicity B=%f out of range.",keynumber,B);
            consistent=false;
        }
        Key::SpectrumType &spectrum = key.getSpectrum();
        if (spectrum.size() != static_cast<size_t>(Key::NumberOfBins))
        {
            LogW("Key %d: Logspec size is %d, expected %d.",
                    keynumber,
                    static_cast<int>(spectrum.size()),
                    static_cast<int>(Key::NumberOfBins));
            consistent=false;
        }
        else if (MathTools::computeNorm(spectrum)==0)
        {
            LogW("Key %d: Logspec norm = %f.",keynumber,
                    MathTools::computeNorm(spectrum));
            consistent=false;
        }
    }

    // If data is found to be inconsistent send a corresponding message.
    if (not consistent)
    {
        MessageHandler::send<MessageCaluclationProgress>
                (MessageCaluclationProgress::CALCULATION_FAILED,
                 MessageCaluclationProgress::CALCULATION_ERROR_KEY_DATA_INCONSISTENT);
    }
    return consistent;
}


//-----------------------------------------------------------------------------
//                          Normalize all spectra
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Normalize spectrum
///
/// This function normalizes the logarithmic power spectra in such a way that
/// the sum over all components is equal to 1.
///////////////////////////////////////////////////////////////////////////////

void AuditoryPreprocessing::normalizeSpectrum(Key &key)
{
    Key::SpectrumType &spectrum = key.getSpectrum();
    MathTools::normalize(spectrum);
}


//-----------------------------------------------------------------------------
//            Compute frequency of inharmonic partial
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Compute frequency of inharmonic partial
///
/// This is the well-known function \f$f_n=f_1 \sqrt{\frac{1+B n^2}{1+B} } \f$.
///
/// \param n : Number of the partial
/// \param f1 : Frequency of the first partial
/// \param B : Inharmonicity coefficient
/// \return Frequency fn
///////////////////////////////////////////////////////////////////////////////

double AuditoryPreprocessing::getInharmonicPartial (double n, double f1, double B)
{
    return f1 * n * sqrt((1+B*n*n)/(1+B));
}


//-----------------------------------------------------------------------------
//            Compute the inharmonic index for a given frequency
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// This function is the inverse of the function InharmonicPartialFrequency.
/// It computes the number of the actual partial (as a floating point)
/// for given f, f1, and B. This is an exact inverse function.
///
/// \param f : Frequency of the n+th partial
/// \param f1 : Frequency of the first partial
/// \param B : Inharmonicity coefficient
/// \return Number of the actual partial as a floating point
///////////////////////////////////////////////////////////////////////////////

double AuditoryPreprocessing::getInharmonicPartialIndex (double f, double f1, double B)
{
    double x=f/f1;
    if (B==0) return x;
    else return 0.7071067811865475*sqrt((-1 + sqrt(1 + 4*B*(1+B)*x*x))/B);
}


//-----------------------------------------------------------------------------
//                              Clean spectrum
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Clean logarithmically binned spectrum
///
/// The purpose of this function is to cut away all contributions in the
/// logarithmically binned spectrum which are not close to the location of
/// expected partials (for given ground frequency f1 and inharmonicity B).
/// To this end we define an envelope function in the range [0,1] which
/// exhibits sharp peaks around the low-lying partials and broader peaks
/// for the higher partials. This ensures that possible inaccuracies in the
/// previous determination of the inharmonicity do not load to an
/// errorneous cancellation of higher partials.
///
/// \param key : Reference to the key
///////////////////////////////////////////////////////////////////////////////

void AuditoryPreprocessing::cleanSpectrum (Key &key)
{
    SpectrumType &spectrum = key.getSpectrum();
    const int M = static_cast<int>(spectrum.size());
    const double f = key.getRecordedFrequency();
    const double B = key.getMeasuredInharmonicity();

    auto wave = [f,B,this] (int m)
        { return cos(MathTools::PI*getInharmonicPartialIndex(mtof(m),f,B)); };
    auto envelope = [wave,f,this] (int m)
        { return  pow(fabs(wave(m)),200.0/pow(mtof(m)/f,1.5)); };

    for (int m=0; m<M; m++) spectrum[m] *= envelope(m);
}



//-----------------------------------------------------------------------------
//                         Cut frequencies below f1
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Cut all frequencies below 5/6*f1 in order to reduce noise
/// \param key : Reference to the key
///////////////////////////////////////////////////////////////////////////////

void AuditoryPreprocessing::cutLowFrequencies(Key &key)
{
    SpectrumType &spectrum = key.getSpectrum();
    const double f = key.getRecordedFrequency();
    const int lowcutindex = std::min<int>(static_cast<int>((5*ftom(f)))/6,static_cast<int>(NumberOfBins));
    for (int m=0; m<lowcutindex; m++) spectrum[m]=0;
}



//-----------------------------------------------------------------------------
//			          Initialize the filter function
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the filter function in the vector mdBA
///
/// Here we use ordinary dBA filtering. The empirical function is
/// given by \f[ _A(f)= {12200^2\cdot f^4\over (f^2+20.6^2)
/// \quad\sqrt{(f^2+107.7^2)\,(f^2+737.9^2)} \quad (f^2+12200^2)}\f]
/// For the sake of efficiency the values according to the logarithmically binned
/// spectra are stored in a vector mdBA.
///////////////////////////////////////////////////////////////////////////////

void AuditoryPreprocessing::initializeSPLAFilter()
{
    mdBA.clear();
    mdBA.resize(NumberOfBins);
    for (uint m=0; m<NumberOfBins; ++m)
    {
        double f = mtof(m);
        double Ra = 12200.0*12200.0*f*f*f*f / (f*f+20.6*20.6) /
               sqrt((f*f+107.7*107.7)*(f*f+737.9*737.9)) / (f*f+12200.0*12200.0);
        mdBA[m] = 2.0+20*log10(Ra);
        //std::cout << f << "\t" << mdBA[m] << std::endl;
    }
}


//-----------------------------------------------------------------------------
//              Compute A-weighted sound pressure level SPLA
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Compute A-weighted sound pressure level SPLA
///
/// This function computes the dBA-weighted sound pressure level (SPLA).
/// The SPLA is basically the logarithm of the power of each spectral line
/// with respect to a suitable lower cutoff. For simplicity the SPLA is
/// stored in the same spectrum vector of the local copy, destroying the
/// existing data which is not used for tuning.
///
/// \param spectrum : Reference to the spectrum
///////////////////////////////////////////////////////////////////////////////

void AuditoryPreprocessing::convertToSPLA (SpectrumType &spectrum)
{
    if (mdBA.size()==0) initializeSPLAFilter();
    EptAssert(mdBA.size()==NumberOfBins,"mdBA should be initialized.");
    const double I0 = 1E-7;	// auditory threshold intensity
    for (uint m=0; m<NumberOfBins; ++m)
    {
        double SPLA = 10 * log10(spectrum[m]/I0)  + mdBA[m];
        if (SPLA<0) spectrum[m]=0;
        else spectrum[m] = I0 * pow(10.0, SPLA/10.0);
    }
}


//-----------------------------------------------------------------------------
//               extrapolate missing inharmonicity estimates
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Extrapolate inharmonicity and estimate missing values
///
/// In the upper half of the manual, the inharmonicity of the strings of
/// a piano grows typically exponentially with the key index. During
/// recording, however, some of the inharmonicity values are usually not
/// correctly measured. This function identifies the outlyers and replaces
/// them by a suitable interpolated value. Note that this works only with
/// the standard design of pianos (parallel strings of the same kind in
/// the upper half).
///////////////////////////////////////////////////////////////////////////////

void AuditoryPreprocessing::extrapolateInharmonicity()
{
    const int firstkey = mKeyNumberOfA4-8;
    double K=0, Y=0, KK=0, KY=0, N=0, BE=0;
    for (int k = firstkey; k < mNumberOfKeys; ++k)
    {
        if (N>1)
        {
            double a = (N*KY-K*Y)/(N*KK-K*K);
            double b = (KK*Y-K*KY)/(N*KK-K*K);
            BE = exp(a*k+b);
        }
        double B = mKeys[k].getMeasuredInharmonicity();
        bool B_is_valid = (B>0);
        if (B>0 and BE>0 and N>5) if (fabs(log(B/BE))>0.2) B_is_valid=false;
        if (B_is_valid)
        {
            double y = log(B);
            K+=k; Y+=y; KK+=k*k; KY+=k*y; N++;
        }
        else
        {
            if (BE==0)
            {
                double f1 = mPiano.getEqualTempFrequency(k,0,440);
                BE = mPiano.getExpectedInharmonicity(f1);
            }
            mKeys[k].setMeasuredInharmonicity(BE);
        }
    }
}


//-----------------------------------------------------------------------------
//                  Improve high-frequency spectral lines
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Improve high-frequency spectral lines
///
/// Depending on the microphone the high-frequency spectral lines above 5kHz
/// are sometimes very weak.
///////////////////////////////////////////////////////////////////////////////

void AuditoryPreprocessing::improveHighFrequencyPeaks ()
{
    int start = mKeyNumberOfA4;
    for (int k = start; k < mNumberOfKeys; k++)
    {
        Key &key = mKeys[k];
        SpectrumType &spectrum = key.getSpectrum();
        double f = key.getRecordedFrequency();
        double B = key.getMeasuredInharmonicity();
        if (f<=0 or B<=0) continue;
        int m = MathTools::roundToInteger(ftom(f));
        if (m<0 or m>static_cast<int>(NumberOfBins)) continue;
        double factor = (1.0*(k-start))/(mNumberOfKeys-start);
        double intensity = spectrum[m] * factor;
        for (int n=2; n<=6; ++n)
        {
            double fn = getInharmonicPartial(n,f,B);
            if (fn<20 or fn>10000) continue;
            int mn = MathTools::roundToInteger(ftom(fn));
            for (int i=mn-10; i<=mn+10; ++i)
                if (i>=0 and i < static_cast<int>(NumberOfBins))
                    spectrum[i] = intensity * pow(4,-n) * exp(-0.1*(i-mn)*(i-mn));
        }
    }
}



//-----------------------------------------------------------------------------
//                  Mollify
//-----------------------------------------------------------------------------

void AuditoryPreprocessing::applyMollifier (Key &key)
{
    SpectrumType &spectrum = key.getSpectrum();
    SpectrumType copy = spectrum;
    //double f1 = key.getRecordedFrequency();


    //SpectrumType copy = spectrum;
    int M = static_cast<int>(NumberOfBins);
    for (int m=0; m<M; ++m)
    {
        double f = mtof(m);
        const double df=55.0/f+f/2000.0;
        int dm = MathTools::roundToInteger(ftom(f+df)) - m;
        double sum=0,norm=0;
        for (int ms = std::max(1,m-3*dm); ms<=std::min(m+3*dm,M-1); ms++)
        {
            double weight = exp(-1.0*(ms-m)*(ms-m)/dm/dm);
            norm+=weight;
            sum+=copy[ms]*weight;
        }
        if (norm>0) spectrum[m]=sum/norm;
    }

}




//-----------------------------------------------------------------------------
}  // namespace entropyminimizer
