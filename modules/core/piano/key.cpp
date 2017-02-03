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
//                    Class describing a single piano key
//=============================================================================

#include "key.h"

#include <cmath>
#include "../system/eptexception.h"
#include "../system/log.h"
#include "../math/mathtools.h"


// Constants characterizing the logarighmically bins, do not change!

const int Key::NumberOfBins  = 10800;       // Number of log. bins
const int Key::BinsPerOctave = 1200;        // bins per octave
const double Key::fmin       = 20.601722;   // frequ. of lowest bin


//-----------------------------------------------------------------------------
//                          clear data
//-----------------------------------------------------------------------------

void Key::clear()
{
    mSpectrum = SpectrumType(NumberOfBins,0);
    mPeaks.clear();
    mRecordedFrequency = 0;
    mMeasuredInharmonicity = 0;
    mRecognitionQuality = 0;
    mComputedFrequency = 0;
    mTunedFrequency = 0;
    mOverpull = 0;
    mRecorded = false;
}


//-----------------------------------------------------------------------------
//                map frequency to real-valued logspec index
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Converts a frequency in Hz to the corresponding logbin index. For reasons
/// of invertibility this function returns a floating-point value. In order
/// to get the actual logbin index the result of this function has to be
/// rounded (not truncated) to an integer.
/// \param f : Frequency in Hz (usually in the range 0 < f < 20 kHz).
/// \return Floating point value representing the logbin index. Note that this
/// value may lie outside of the allowed index range of the logbin spectrum.
/// This function will return a negative number if the frequency is <= 0.
///////////////////////////////////////////////////////////////////////////////

double Key::FrequencyToRealIndex (double f)
{
    if (f < 0) {
        LogD("Frequency <= 0Hz detected.");
        return -1;
    } else if (f > 20000) {
        LogD("Frequency higher than 20KHz detected.");
    }

    static const double lnfmin = log(fmin);
    return BinsPerOctave * (log(f) - lnfmin) / MathTools::LOG2;
}

//-----------------------------------------------------------------------------
//                map frequency to integer logspec index
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Converts a frequency in Hz to the corresponding logbin index.
/// \param f : Frequency in Hz (usually in the range 0 < f < 20 kHz).
/// \return Logbin index. Note that this
/// value may lie outside of the allowed index range of the logbin spectrum.
/// This function will return a negative number if the frequency is <= 0.
///////////////////////////////////////////////////////////////////////////////

int Key::FrequencyToIndex (double f)
{ return MathTools::roundToInteger(FrequencyToRealIndex(f)); }


//-----------------------------------------------------------------------------
//                  map logspec index to frequency
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Converts a logbin index to the corresponding frequency in Hz.
/// \param m : Index of the logbin spectrum in the range [0,NumberOfBins]
/// \return Frequency in Hz
///////////////////////////////////////////////////////////////////////////////

double Key::IndexToFrequency (double m)
{
    // m may be negative aswell here
    return fmin * pow(2, m / BinsPerOctave);
}


//-----------------------------------------------------------------------------
//                   set and get recorded frequency
//-----------------------------------------------------------------------------

void Key::setRecordedFrequency (const double f)
{
    EptAssert(f > 0 && f < 20000, "Frequency must in the range (0,20000)");
    mRecordedFrequency=f;
}

double Key::getRecordedFrequency () const
{ return mRecordedFrequency; }

double &Key::getRecordedFrequency ()
{ return mRecordedFrequency; }


//-----------------------------------------------------------------------------
//                  set and get measured inharmonicity
//-----------------------------------------------------------------------------

void Key::setMeasuredInharmonicity (double B)
{
    EptAssert (B >= 0, "The inharmonicity must be positive");
    if (B>1)
    {
        LogW("Inharmonicity larger than 1 ignored");
        return;
    }
    mMeasuredInharmonicity = B;
}

double Key::getMeasuredInharmonicity () const
{ return mMeasuredInharmonicity; }

double &Key::getMeasuredInharmonicity ()
{ return mMeasuredInharmonicity; }


//-----------------------------------------------------------------------------
//                  set and get recognition quality
//-----------------------------------------------------------------------------

void Key::setRecognitionQuality (double Q)
{ mRecognitionQuality = Q; }

double Key::getRecognitionQuality () const
{ return mRecognitionQuality; }

double &Key::getRecognitionQuality ()
{ return mRecognitionQuality; }


//-----------------------------------------------------------------------------
//                   set and get computed frequency
//-----------------------------------------------------------------------------

// Note: The computed frequency is always stored in a pitch with A4 = 440Hz

void Key::setComputedFrequency (double f)
{
    EptAssert(f >= 0 && f < 20000, "Freqency must be below 20000 and greater than 0");
    mComputedFrequency=f;
}

double Key::getComputedFrequency () const
{ return mComputedFrequency; }

double &Key::getComputedFrequency ()
{ return mComputedFrequency; }


//-----------------------------------------------------------------------------
//                   set and get tuned frequency
//-----------------------------------------------------------------------------

// Note: The computed frequency is stored in the actually tuned pitch

void Key::setTunedFrequency (double f)
{
    EptAssert(f >= 0 && f < 20000, "Freqency must be below 20000 and greater than 0");
    mTunedFrequency=f;
}

double Key::getTunedFrequency () const
{ return mTunedFrequency; }

double &Key::getTunedFrequency ()
{ return mTunedFrequency; }


//-----------------------------------------------------------------------------
//                      set and get overpull in cents
//-----------------------------------------------------------------------------

void Key::setOverpull (double cents)
{
    if (cents>25) mOverpull=25;
    else if (cents<-25) mOverpull=-25;
    else mOverpull = cents;
}

double Key::getOverpull () const
{ return mOverpull; }

double &Key::getOverpull ()
{ return mOverpull; }


//-----------------------------------------------------------------------------
//                          copy vector to mSpectrum
//-----------------------------------------------------------------------------

void Key::setSpectrum(const SpectrumType &s)
{
    EptAssert(s.size() == static_cast<size_t>(NumberOfBins),
              "Spectrum size must match the total number of bins");
    mSpectrum = s;
}


//-----------------------------------------------------------------------------
//                              get spectrum
//-----------------------------------------------------------------------------

Key::SpectrumType &Key::getSpectrum ()
{ return mSpectrum; }

const Key::SpectrumType & Key::getSpectrum () const
{ return mSpectrum; }


//-----------------------------------------------------------------------------
//                           copy list to mPeaks
//-----------------------------------------------------------------------------

void Key::setPeaks(const PeakListType &s)
{
    EptAssert(s.size() < 200, "Peak list should not be unreasonably large");
    mPeaks = s;
}


//-----------------------------------------------------------------------------
//                                  get peaks
//-----------------------------------------------------------------------------

Key::PeakListType &Key::getPeaks ()
{ return mPeaks; }

const Key::PeakListType & Key::getPeaks () const
{ return mPeaks; }
