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

///////////////////////////////////////////////////////////////////////////////
/// The constructor sets the cancel-thread flag to false.
/// It does not yet start the thread.
///////////////////////////////////////////////////////////////////////////////

SimpleThreadHandler::SimpleThreadHandler()
    : mCancelThread(false),
      mRunning(false)
{}


//------------------ Destructor ------------------- ---------------------------

///////////////////////////////////////////////////////////////////////////////
/// The destructor waits for the current thread to stop if it is running
///////////////////////////////////////////////////////////////////////////////

SimpleThreadHandler::~SimpleThreadHandler()
{
    stop();
}


//--------------- threadsafe cancelThread setter function----------------------

///////////////////////////////////////////////////////////////////////////////
/// Register a thread for termination
///////////////////////////////////////////////////////////////////////////////

void  SimpleThreadHandler::setCancelThread(bool b)
{
    std::lock_guard<std::mutex> lock(mLockMutex); // protect remainder of block
    mCancelThread = b;
}


//--------------- threadsafe cancelThread getter function----------------------

///////////////////////////////////////////////////////////////////////////////
/// Find out whether a thread has been registered for termination
///////////////////////////////////////////////////////////////////////////////

bool  SimpleThreadHandler::cancelThread() const
{
    std::lock_guard<std::mutex> lock(mLockMutex);  // protect remainder of block
    return mCancelThread;
}


//-------------------------- Start the thread ---------------------------------

///////////////////////////////////////////////////////////////////////////////
/// If the thread is already running register it for termination and wait
/// until it has terminated. Then restart a new thread.
///////////////////////////////////////////////////////////////////////////////

void SimpleThreadHandler::start()
{
    stop();
    setCancelThread(false);
    mThread = std::thread(&SimpleThreadHandler::simpleWorkerFunction, this);
}


//------------------- Mark the thread as to be cancelled ---------------------

///////////////////////////////////////////////////////////////////////////////
/// Mark the thread for cancellation. This function waits for termination of the
/// thread, i.e. it blocks until the thread has been shut down. This waiting
/// time will depend on the implementation of the workerFunction.
///////////////////////////////////////////////////////////////////////////////

void SimpleThreadHandler::stop()
{
    setCancelThread(true);                  // Set cancel flag to true
    if (mThread.joinable()) mThread.join(); // Wait for thread to terminate
}


//--------------- Let the thread sleep in an idle state ----------------------

///////////////////////////////////////////////////////////////////////////////
/// Call this function from the thread if you want to let it stay in an
/// idle state without consuming CPU time.
/// \param time : Waiting time (double) in milliseconds.
///////////////////////////////////////////////////////////////////////////////

void SimpleThreadHandler::msleep(double milliseconds)
{
    std::this_thread::sleep_for(
                std::chrono::microseconds(static_cast<int>(1000*milliseconds)));
}


//--------------- Returns whether the thread is running ----------------------

///////////////////////////////////////////////////////////////////////////////
/// \return Boolean telling whether the thread is running
///////////////////////////////////////////////////////////////////////////////

bool SimpleThreadHandler::isThreadRunnding() const
{
    return mRunning;
}


//--- called when an exception was caught during the worker function ---------

///////////////////////////////////////////////////////////////////////////////
/// Reimplement this function if you want a custom exception handling,
/// e.g. restart the thread. By default this function does nothing.
/// \param e : The exception that was caught
///////////////////////////////////////////////////////////////////////////////

void SimpleThreadHandler::exceptionCaught(const EptException &e)
{
    // empty
    (void)e;
}


//------------------------------ Set thread name ------------------------------

///////////////////////////////////////////////////////////////////////////////
/// With this function it is possible to rename the thread. This is particularly
/// useful for debugging since the Qt-creator shows the thread name in a list.
/// However, there is no platform-independent implementation of thread name
/// handling. Therefore, this function works only on Linux using the underlying
/// pthread implementation. The function has to be called within the thread
/// itself, i.e., in the worker function. It works only in debug mode.
/// \param s : name of the thread
///////////////////////////////////////////////////////////////////////////////

void SimpleThreadHandler::setThreadName(std::string s)
{
#if defined(__linux__) and not defined(QT_NO_DEBUG) and not defined(__ANDROID__)
    // Note: If you encounter a compiler error in the following line
    // please comment it out. It serves only for debugging purposes on Qt.
    pthread_setname_np (pthread_self(), s.c_str());
#else
    if (s.size()){}; // suppress warning if inactive
#endif
}


// -------------------------- Private helper function -------------------------

///////////////////////////////////////////////////////////////////////////////
/// This function calls the abstract workerFunction of the implementation.
/// It also catches its exceptions-
///////////////////////////////////////////////////////////////////////////////

void SimpleThreadHandler::simpleWorkerFunction()
{
    mRunning = true;

    try
    {
        workerFunction();
    }
    catch (const EptException &e)
    {
        LogE("Worker thread stopped with EptException: %s", e.what());
        exceptionCaught(e);
    }
    catch (const std::exception &e)
    {
        LogE("Worker thread stopped with std::exception: %s", e.what());
    }
    catch (...) {
        LogE("Worker thread stopped with an unknown exception");
    }

    mRunning = false;
}
