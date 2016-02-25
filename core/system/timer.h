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
//                                 Timer
//=============================================================================

#ifndef TIMER_H
#define TIMER_H

#include <chrono>

///////////////////////////////////////////////////////////////////////////////
/// \brief Versatile timer for the entropy tuner.
///
/// This timer works like a simple clock which is set to zero at the moment
/// of creation and whenever the function reset() is called. The class
/// provides a function waitUntil which waits for a certain minimum time
/// span since the last reset. It is used in the SignalAnalyzer in order
/// to compute FFTs with a certain mininum time interval in between.
///////////////////////////////////////////////////////////////////////////////

class Timer
{
public:
    Timer();
    ~Timer() {}

    void reset ();
    int64_t getMilliseconds();
    void wait (int milliseconds);
    bool timeout (int64_t milliseconds);
    void waitUntil (int64_t milliseconds, int interval_ms=5);

private:
    std::chrono::time_point<std::chrono::system_clock> mStart;
};

#endif // TIMER_H
