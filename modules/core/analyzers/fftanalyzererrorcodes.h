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

#ifndef FFTANALYZERERRORCODES_H
#define FFTANALYZERERRORCODES_H

#include <memory>
#include <vector>

#include "prerequisites.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Enumeration of possible errors during the fft analysis
///////////////////////////////////////////////////////////////////////////////

enum class EPT_EXTERN FFTAnalyzerErrorTypes
{
    ERR_NONE,                           ///< No error occured
    ERR_FREQUENCY_OUT_OF_BOUNDS,        ///< The recorded frequency is out of the piano range
    ERR_NO_COMPUTED_FREQUENCY,          ///< The analyzer needs a computed frequency
    ERR_NO_PEAK_AMPLITUDE,              ///< No intensity in the signal near expected peak
    ERR_DATA,                           ///< The data contains error and cannot be processed
};


///////////////////////////////////////////////////////////////////////////////
/// \brief Structure holding the data of a successful frequency detection
///////////////////////////////////////////////////////////////////////////////

using TuningDeviationCurveType = std::vector<double>;

struct EPT_EXTERN FrequencyDetectionResultStruct
{
    FFTAnalyzerErrorTypes error = FFTAnalyzerErrorTypes::ERR_NONE;
    TuningDeviationCurveType tuningDeviationCurve;
    int deviationInCents = 0;
    double overpullInCents = 0;
    double positionOfMaximum = 0;
    double detectedFrequency = -1;

    bool hasError() const {return error != FFTAnalyzerErrorTypes::ERR_NONE;}
};

using FrequencyDetectionResult = std::shared_ptr<FrequencyDetectionResultStruct>;
template class EPT_EXTERN std::shared_ptr<FrequencyDetectionResultStruct>;

#endif // FFTANALYZERERRORCODES_H

