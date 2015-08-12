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

//======================================================================
//                            Piano data
//======================================================================

#include "piano.h"

#include <iostream>
#include <cmath>
#include <time.h>

#include "../system/eptexception.h"

const int Piano::DEFAULT_NUMBER_OF_KEYS = 88;
const int Piano::DEFAULT_KEY_NUMBER_OF_A = 48;
const double Piano::DEFAULT_CONCERT_PITCH = 440;

//----------------------------------------------------------------------
//			                   Constructor
//----------------------------------------------------------------------


Piano::Piano() :
    mName(""),
    mType(piano::PT_GRAND),
    mSerialNumber(""),
    mManufactureYear(""),
    mManufactureLocation(""),
    mTuningLocation(""),
    mTuningTime(""),
    mConcertPitch(DEFAULT_CONCERT_PITCH),
    mKeyboard(DEFAULT_NUMBER_OF_KEYS)
{
    setNow();
}



//----------------------------------------------------------------------
//			                   setNow
//----------------------------------------------------------------------


void Piano::setNow() {
    time_t rawtime = time(0);
    struct tm nowtime;
#ifdef _MSC_VER
    gmtime_s(&nowtime, &rawtime);
#else
    nowtime = *gmtime(&rawtime);
#endif
    char buffer[50];
    strftime(buffer, 50, "%Y-%m-%d %H:%M:%S", &nowtime);
    mTuningTime = buffer;
}


/// This function provides a rough estimate of the expected inharmonicity coefficient
/// for an average upright. It is used to improve the expectation where the spectral
/// lines should reside in the spectrum
///
/// \param f : frequency in Hz
/// \return Inharmonicity coefficient B

double Piano::getExpectedInharmonicity (double f) const
{
    return (f > 100 ? exp(-15.45 + 1.354*log(f)) : exp(-2.622 - 1.431*log(f)));
}


//-----------------------------------------------------------------------------
//			                   Equal temperament
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Function returning the equal temperament
///
/// This function computes the theoretical frequency of a given key according
/// to equal temperament (ET), optionally shifted by a given number of cents.
/// \param keynumber : the index of the key on the piano keyboard
/// \param cents : additional cents in cents
/// \param A4 : Frequency of A4 (replaced by mConcerPitch if zero by default)
/// \return Equal temperament frequency
///////////////////////////////////////////////////////////////////////////////

double Piano::getEqualTempFrequency (int keynumber, double cents, double A4) const
{
    EptAssert(keynumber>=0 and keynumber < mKeyboard.getNumberOfKeys(),"range of keynumber");
    return (A4>0 ? A4 : mConcertPitch) *
            pow(2.0, (100.0*(keynumber-mKeyboard.getKeyNumberOfA4())+cents)/1200.0);
}


//-----------------------------------------------------------------------------
//			               define the temperament
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Compute the defining temperatent
///
/// This function provides the defining temperament on which the tuning proces
/// takes place. At the moment non-standard temperaments are not yet
/// implemented, instead the function simply returns the equal temperamtne (ET).
/// By defining this function, we prepare ourselves for a possible extension.
///
/// \param keynumber : key number
/// \param cents : additional shift in cents
/// \return Frequency f1 without inharmonicity and stretch
///////////////////////////////////////////////////////////////////////////////

double Piano::getDefiningTempFrequency (int keynumber, double cents, double A4) const
{
    EptAssert(keynumber>=0 and keynumber < mKeyboard.getNumberOfKeys(),"range of keynumber");
    return getEqualTempFrequency(keynumber,cents,A4);
}
