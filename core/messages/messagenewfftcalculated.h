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
//             Message informing that a new FFT has been calculated
//=============================================================================

#ifndef MESSAGENEWFFTCALCULATED_H
#define MESSAGENEWFFTCALCULATED_H

#include "message.h"
#include "../analyzers/fftanalyzererrorcodes.h"
#include "../math/fftadapter.h"

template class EPT_EXTERN std::shared_ptr<FFTPolygon>;

///////////////////////////////////////////////////////////////////////////////
/// \brief Class of a message informing that a new FFT has been calculated
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN MessageNewFFTCalculated : public Message
{
public:
    /// Type of the FFT Message
    enum class FFTMessageTypes {
        NewFFT,                             ///< A new fft was calculated
        FinalFFT,                           ///< The final fft was calculated
        Error,                              ///< An error occured during the calculation
    };

private:
    const FFTMessageTypes mFFTMessageType;
    FFTDataPointer mFFTData;
    const std::shared_ptr<FFTPolygon> mPolygon;

    const FFTAnalyzerErrorTypes mErrorType;

public:
    MessageNewFFTCalculated(FFTAnalyzerErrorTypes errorType);
    MessageNewFFTCalculated(FFTMessageTypes type,
                            FFTDataPointer fftdata,
                            std::shared_ptr<FFTPolygon> polygon);
    virtual ~MessageNewFFTCalculated() {}

    FFTDataPointer getData() const { return mFFTData; }
    std::shared_ptr<FFTPolygon> getPolygon() const { return mPolygon; }
    FFTMessageTypes getFFTMessageType() const { return mFFTMessageType; }
    FFTAnalyzerErrorTypes getErrorType() const { return mErrorType; }

    bool isFinal() const { return mFFTMessageType == FFTMessageTypes::FinalFFT; }
    bool hasError() const { return mFFTMessageType == FFTMessageTypes::Error; }
};

#endif // MESSAGENEWFFTCALCULATED_H
