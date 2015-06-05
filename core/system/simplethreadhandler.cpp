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

//=============================================================================
//                            Thread handler
//=============================================================================

#include "simplethreadhandler.h"
#include <chrono>
#include "../system/log.h"


//------------------ Constructor, clear cancel flag ---------------------------

/// The constructor sets the cancel-thread flag to false.
/// It does not yet start the thread.

SimpleThreadHandler::SimpleThreadHandler()
    : mCancelThread(false),
      mRunning(false)
{}

//------------------ Destructor ------------------- ---------------------------

/// The destructor waits for the current thread to stop if it is running
SimpleThreadHandler::~SimpleThreadHandler() {
    stop();
}

//--------------- threadsafe cancelThread getter function----------------------

bool  SimpleThreadHandler::cancelThread() const
{
    std::lock_guard<std::mutex> lock(mLockMutex);  // protect remainder of block
    return mCancelThread;
}


//--------------- threadsafe cancelThread setter function----------------------

void  SimpleThreadHandler::setCancelThread(bool b)
{
    std::lock_guard<std::mutex> lock(mLockMutex); // protect remainder of block
    mCancelThread = b;
}


//-------------------------- Start the thread ---------------------------------

void SimpleThreadHandler::start()
{
    stop();
    setCancelThread(false);
    mThread = std::thread(&SimpleThreadHandler::simpleWorkerFunction, this);
}


//------------------- Mark the thread as to be cancelled ---------------------

/// Mark the thread for cancellation. This function waits for termination of the
/// thread, i.e. it blocks until the thread has been shut down. This waiting
/// time will depend on the implementation of the workerFunction.

void SimpleThreadHandler::stop()
{
    setCancelThread(true);                  // Set cancel flag to true
    if (mThread.joinable()) mThread.join(); // Wait for thread to terminate
}

//--------------- Let the thread sleep in an idle state ----------------------

/// Call this function from the thread if you want to let it stay in an
/// idle state without consuming CPU time

void SimpleThreadHandler::msleep(double milliseconds)
{
    std::this_thread::sleep_for(
                std::chrono::microseconds(static_cast<int>(1000*milliseconds)));
}

//--------------- Returns whether the thread is running ----------------------

/// \return Is the thread running

bool SimpleThreadHandler::isThreadRunnding() const {
    return mRunning;
}

//--- called when an exception was caught during the worker function ---------

/// reimplement this if you want a custom exception handling, e.g. restart the thread.
/// By default this function does nothing.
/// \param e the exception that was caught
void SimpleThreadHandler::exceptionCaught(const EptException &e)
{
    // empty
    (void)e;
}

// -------------------------- Private helper function ------------------------

/// This function calls the abstract workerFunction of the implementation.
/// It also catches its exceptions
void SimpleThreadHandler::simpleWorkerFunction()
{
    mRunning = true;

    try
    {
        workerFunction();
    }
    catch (const EptException &e)
    {
        ERROR("Worker thread stopped with EptException.");
        exceptionCaught(e);
    }
    catch (const std::exception &e)
    {
        ERROR("Worker thread stopped with std::exception.");
    }
    catch (...) {
        ERROR("Worker thread stopped with an unknown exception");
    }

    mRunning = false;
}
