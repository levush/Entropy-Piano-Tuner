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
//            Message indicating a local change in the tuning curve
//=============================================================================

#ifndef MESSAGECHANGETUNINGCURVE_H
#define MESSAGECHANGETUNINGCURVE_H

#include "message.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Class for a message indicating a change in the tuning curve.
///
/// During the calculation and upon manual manipulation the tuned frequency
/// may change. These changes are emitted as messages in order to inform the
/// GUI to redraw the corresponding green markers.
///////////////////////////////////////////////////////////////////////////////

class MessageChangeTuningCurve : public Message
{
public:
    MessageChangeTuningCurve (int keynumber, double frequency);
    ~MessageChangeTuningCurve() {};

    int getKeyNumber() { return mKeynumber; }
    double getFrequency() { return mFrequency; }

private:
    const int mKeynumber;
    const double mFrequency;
};

#endif // MESSAGECHANGETUNINGCURVE_H
