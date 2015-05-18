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

#ifndef FFTANALYZERERRORCODES_H
#define FFTANALYZERERRORCODES_H

#include <memory>
#include <vector>

/// \brief Enumeration of possible errors during the fft analysis
enum class FFTAnalyzerErrorTypes {
    ERR_NONE,                           ///< No error occured
    ERR_FREQUENCY_OUT_OF_BOUNDS,        ///< The recorded frequency is out of the piano range
    ERR_KEY_NOT_RECORDED,               ///< The analyser needs access to a key that was not recorded
};

using TuningDeviationCurveType = std::vector<double>;

struct FrequencyDetectionResultStruct {
    FFTAnalyzerErrorTypes error = FFTAnalyzerErrorTypes::ERR_NONE;
    TuningDeviationCurveType tuningDeviationCurve;
    int deviationInCents = 0;
    double positionOfMaximum = 0;
    double detectedFrequency = -1;

    bool hasError() const {return error != FFTAnalyzerErrorTypes::ERR_NONE;}
};

using FrequencyDetectionResult = std::shared_ptr<FrequencyDetectionResultStruct>;

#endif // FFTANALYZERERRORCODES_H

