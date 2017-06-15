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
//                           Overpull estimator
//=============================================================================

#ifndef OVERPULL_H
#define OVERPULL_H

#include "prerequisites.h"
#include "../piano/piano.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Class for estimating the overpull needed in a pitch raise.
///
/// If a piano is heavily out of tune, one has to perform a pitch raise.
/// The rising string tension increases the load on the soundboard, leading to
/// an elastic deformation of the soundboard. This deformation in turn lowers
/// the pitches. Overpulling means to tune the strings a little bit sharper
/// than intended so that the expected loss during the tuning process is
/// compensated.
///
/// The overpull algorithm is implemented in a fully automatic manner. If in
/// the recording mode enough keys are measured (all with a distance less or
/// equal than a fifth), and if the piano is flat (or sharp) by more than
/// 5%, the overpull curve is automatically displayed in magenta color in
/// the tuning window.
///
/// The overpull curve decreases during tuning and finally becomes flat and
/// vanishes entirely. That is, for each key the overpull of all other keys
/// is newly calculated.
///
/// The instance of the overpull estimator is held by the SignalAnalyzer.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN OverpullEstimator
{
public:
    OverpullEstimator();
    ~OverpullEstimator() {}

    void init (const Piano *piano);
    double getOverpull (int keynumber, const Piano *piano);

private:
    piano::PianoType mPianoType;        ///< Piano type (upright/grand)
    int mNumberOfKeys;                  ///< Total number of keys
    int mNumberOfBassKeys;              ///< Keys on the bass bridge
    double mConcertPitch;               ///< Concert pitch (A4)
    std::vector<std::vector<float>> R;  ///< Response matrix

    void computeInteractionMatrix (double averagePull = 0.22);
};

#endif // OVERPULL_H
