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
//            Message reporting a frequency deviation during tuning
//=============================================================================

#ifndef MESSAGETUNINGDEVIATION_H
#define MESSAGETUNINGDEVIATION_H

#include "message.h"
#include "../analyzers/fftanalyzererrorcodes.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Message reporting a frequency deviation during tuning
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN MessageTuningDeviation : public Message
{
public:
    MessageTuningDeviation(FrequencyDetectionResult result);
    ~MessageTuningDeviation() {}

    FrequencyDetectionResult getResult() const {return mResult;}

private:
    FrequencyDetectionResult mResult;
};

#endif // MESSAGETUNINGDEVIATION_H
