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

#ifndef MESSAGECALUCLATIONPROGRESS_H
#define MESSAGECALUCLATIONPROGRESS_H

#include "message.h"

class MessageCaluclationProgress : public Message
{
public:
    enum MessageCalculationProgressTypes
    {
        CALCULATION_FAILED,
        CALCULATION_STARTED,
        CALCULATION_ENDED,
        CALCULATION_PROGRESSED,
        CALCULATION_ENTROPY_REDUCTION_STARTED,
    };

    enum MessageCalculationError
    {
        CALCULATION_ERROR_NONE,
        CALCULATION_ERROR_NO_DATA,
        CALCULATION_ERROR_NOT_ALL_KEYS_RECORDED,
        CALCULATION_ERROR_KEY_DATA_INCONSISTENT,
    };

public:
    MessageCaluclationProgress(MessageCalculationProgressTypes type, double value = 0);
    MessageCaluclationProgress(MessageCalculationProgressTypes type, MessageCalculationError errorcode);
    ~MessageCaluclationProgress();

    double getValue() const {return mValue;}
    MessageCalculationProgressTypes getCalculationType() const {return mCalculationType;}
    MessageCalculationError getErrorCode() const {return mErrorCode;}

private:
    const MessageCalculationProgressTypes mCalculationType;
    const double mValue;
    const MessageCalculationError mErrorCode;
};

#endif // MESSAGECALUCLATIONPROGRESS_H
