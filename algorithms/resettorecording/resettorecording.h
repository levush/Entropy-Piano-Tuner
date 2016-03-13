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
//                         Reset to recorded pitches
//=============================================================================

#ifndef RESETTORECORDING_H
#define RESETTORECORDING_H

#include "core/calculation/algorithmplugin.h"

// Algorithms are always protected by their own namespace:
ALGORITHM_H_START(resettorecording)

///////////////////////////////////////////////////////////////////////////////
/// \brief Algorithm to copy the recorded pitches into the tuning curve.
///
/// This algorith simply copies the recorded pitches. This works also
/// in the case where the keys were only partially recorded. If A4 has been
/// recorded and the ConcertPitch is different, the pitches will be shifted
/// to the wanted ConcertPitch. Therefore, to get an unshifted copy, the
/// ConcertPitch in the settings should be the same as the recorded A4.
/// If A4 was recorded it is assumed that A4=440Hz.
///
/// This algorithm shows the coder how algorithms can be implemented. It can
/// also be used by the users to copy or shift an existing tune.
///////////////////////////////////////////////////////////////////////////////

class ResetToRecording : public Algorithm
{
public:
    // constructor to copy the original piano
    ResetToRecording(const Piano &piano, const AlgorithmFactoryDescription &description);

protected:
    // the implementation of the worker function (sole required function)
    virtual void algorithmWorkerFunction() override final;
};

ALGORITHM_H_END(ResetToRecording)


#endif // RESETTORECORDING_H
