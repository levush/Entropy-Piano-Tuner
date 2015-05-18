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
//                         Simple thread handler
//=============================================================================

#ifndef SIMPLETHREADHANDLER_H
#define SIMPLETHREADHANDLER_H

#include <mutex>
#include <thread>
#include <atomic>
#include "eptexception.h"

#define CHECK_CANCEL_THREAD { if (cancelThread()) {return;} }
#define CANCEL_THREAD       { setCancelThread(true); return; }

///////////////////////////////////////////////////////////////////////////////
/// \brief Simple thread handler
///
/// This class simplifies the handling of C11 threads.
///
/// <B>Usage:</B> The class creating the thread has to be derived from
/// SimpleThreadHandler and to overload the pure virtual workerFunction()
/// with the code to be executed within the thread. The worker
/// function should either return by itself or when cancelThread() becomes
/// true. This means that the workerFunction() usually contains
/// a while(not cancelThread()) loop.
///
/// The thread is started by calling start(). By calling stop() the cancel
/// flag is set and the thread will terminate after some time depending
/// on the implementation of the workerFunction(). For keeping the thread
/// idle in the workerFunction() call the member function msleep().
///////////////////////////////////////////////////////////////////////////////

class SimpleThreadHandler
{
public:
    SimpleThreadHandler();                  ///< Constructor
    virtual ~SimpleThreadHandler();         ///< virtual destructor calls stop

    virtual void start();                   ///< Start the thread
    virtual void stop();                    ///< Stop the thread

protected:
    /// \brief Virtual worker function, executed within the new thread.
    ///
    /// Overload this function with the code to be executed within the new thread.
    /// Make sure that it either returns by itself or when cancelThread() becomes true.
    /// This means that a typical implementation of the workerFunction() should contain
    /// a while(not cancelThread()) loop.
    ///
    /// If you want the thread to stay idle call msleep.
    virtual void workerFunction() = 0;

    /// called when an exception was caught during the worker function
    virtual void exceptionCaught(const EptException &e);

    bool cancelThread() const;              ///< Cancel-flag getter method, thread-safe
    void setCancelThread(bool b);           ///< Cancel-flag setter method, thread-safe
    void msleep(int milliseconds);          ///< Sleep function for staying idle
    bool isThreadRunnding() const;          ///< Flag to check if the thread is running

private:
    void simpleWorkerFunction();            ///< Private helper function

    bool mCancelThread;                     ///< Cancel flag
    std::atomic<bool> mRunning;             ///< Is the thread running
    std::thread mThread;                    ///< Local thread member variable
    mutable std::mutex mLockMutex;          ///< Mutex protecting the cancel flag
};

#endif // SIMPLETHREADHANDLER_H
