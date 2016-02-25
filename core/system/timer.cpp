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

#include "timer.h"

#include <thread>

#include "prerequisites.h"

//-----------------------------------------------------------------------------
//                              constructor
//-----------------------------------------------------------------------------

/// Constructor, resets the timer

Timer::Timer()
{
    reset();
}


//-----------------------------------------------------------------------------
//                             reset timer
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// Reset the timer and remember the current time locally.
///////////////////////////////////////////////////////////////////////////////

void Timer::reset ()
{
    mStart = std::chrono::system_clock::now();
}

//-----------------------------------------------------------------------------
//                          sleep in idle mode
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Wait in idle mode without consuming CPU time.
///
/// Call this function if you want to stay in an
/// idle state without consuming CPU time.
/// \param milliseconds : Waiting time in milliseconds
///////////////////////////////////////////////////////////////////////////////

void Timer::wait(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}


//-----------------------------------------------------------------------------
//                      Get elapsed time in milliseconds
//-----------------------------------------------------------------------------

int64_t Timer::getMilliseconds()
{
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now-mStart).count();
}


//-----------------------------------------------------------------------------
//                          Check for timeout
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Check for timeout.
///
/// \param milliseconds : Timeout time in milliseconds.
///////////////////////////////////////////////////////////////////////////////

bool Timer::timeout (int64_t milliseconds)
{
    int64_t delta_t = getMilliseconds();
    return (delta_t > milliseconds);
}


//-----------------------------------------------------------------------------
//             Wait a certain minimum time span since last reset
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Wait in idle mode for a certain minimum time span since last reset.
///
/// \param milliseconds : Mimimum waiting time in milliseconds.
/// \param interval_ms : Checking interval in milliseconds, default = 5ms
///////////////////////////////////////////////////////////////////////////////

void Timer::waitUntil (int64_t milliseconds, int interval_ms)
{
    while (not timeout(milliseconds)) wait(interval_ms);
}


