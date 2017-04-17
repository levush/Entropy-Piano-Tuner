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
//                         Simple thread handler
//=============================================================================

#ifndef SIMPLETHREADHANDLER_H
#define SIMPLETHREADHANDLER_H

#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>

#include "../prerequisites.h"
#include "../system/log.h"
#include "../system/eptexception.h"

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

class EPT_EXTERN SimpleThreadHandler
{
    // IMPORTANT:
    // Note, that virtual functions have to be defined in header
    // to be known by the algorithms that are in external shared
    // libraries
    // Convetion for this class: ALL IMPLEMENTATIONS IN HEADER

public:
    ///////////////////////////////////////////////////////////////////////////////
    /// The constructor sets the cancel-thread flag to false.
    /// It does not yet start the thread.
    ///////////////////////////////////////////////////////////////////////////////
    SimpleThreadHandler();

    ///////////////////////////////////////////////////////////////////////////////
    /// The destructor waits for the current thread to stop if it is running
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~SimpleThreadHandler();


    ///////////////////////////////////////////////////////////////////////////////
    /// If the thread is already running register it for termination and wait
    /// until it has terminated. Then restart a new thread.
    ///////////////////////////////////////////////////////////////////////////////
    virtual void start();

    ///////////////////////////////////////////////////////////////////////////////
    /// Mark the thread for cancellation. This function waits for termination of the
    /// thread, i.e. it blocks until the thread has been shut down. This waiting
    /// time will depend on the implementation of the workerFunction.
    ///////////////////////////////////////////////////////////////////////////////
    virtual void stop();

    ///////////////////////////////////////////////////////////////////////////////
    /// With this function it is possible to rename the thread. This is particularly
    /// useful for debugging since the Qt-creator shows the thread name in a list.
    /// However, there is no platform-independent implementation of thread name
    /// handling. Therefore, this function works only on Linux using the underlying
    /// pthread implementation. The function has to be called within the thread
    /// itself, i.e., in the worker function. It works only in debug mode.
    /// \param s : name of the thread
    ///////////////////////////////////////////////////////////////////////////////
    static void setThreadName(std::string s)
    {
#if defined(__linux__) and not defined(_DEBUG) and not defined(__ANDROID__)
        // Note: If you encounter a compiler error in the following line
        // please comment it out. It serves only for debugging purposes on Qt.
        pthread_setname_np (pthread_self(), s.c_str());
#else
        if (s.size()){}; // suppress warning if inactive
#endif
    }



protected:

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Virtual worker function, executed within the new thread.
    ///
    /// Overload this function with the code to be executed within the new thread.
    /// Make sure that it either returns by itself or when cancelThread() becomes true.
    /// This means that a typical implementation of the workerFunction() should contain
    /// a while(not cancelThread()) loop.
    ///
    /// If you want the thread to stay idle call msleep.
    ///////////////////////////////////////////////////////////////////////////////

    virtual void workerFunction() = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Reimplement this function if you want a custom exception handling,
    /// e.g. restart the thread. By default this function does nothing.
    /// \param e : The exception that was caught
    ///////////////////////////////////////////////////////////////////////////////

    virtual void exceptionCaught(const EptException &e) {(void) e;}

    ///////////////////////////////////////////////////////////////////////////////
    /// Register a thread for termination
    ///////////////////////////////////////////////////////////////////////////////
    void setCancelThread(bool b)
    {
        std::lock_guard<std::mutex> lock(mLockMutex); // protect remainder of block
        mCancelThread = b;
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// Find out whether a thread has been registered for termination
    ///////////////////////////////////////////////////////////////////////////////
    bool cancelThread() const
    {
        std::lock_guard<std::mutex> lock(mLockMutex);  // protect remainder of block
        return mCancelThread;
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// Call this function from the thread if you want to let it stay in an
    /// idle state without consuming CPU time.
    /// \param time : Waiting time (double) in milliseconds.
    ///////////////////////////////////////////////////////////////////////////////
    void msleep(double milliseconds)
    {
        std::this_thread::sleep_for(
                    std::chrono::microseconds(static_cast<int>(1000*milliseconds)));
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// \return Boolean telling whether the thread is running
    ///////////////////////////////////////////////////////////////////////////////
    bool isThreadRunning() const
    {
        return mRunning;
    }

private:
    ///////////////////////////////////////////////////////////////////////////////
    /// This function calls the abstract workerFunction of the implementation.
    /// It also catches its exceptions-
    ///////////////////////////////////////////////////////////////////////////////
    void simpleWorkerFunction();

private:
    bool mCancelThread;                     ///< Cancel flag
    std::atomic<bool> mRunning;             ///< Is the thread running
    std::thread mThread;                    ///< Local thread member variable
    mutable std::mutex mLockMutex;          ///< Mutex protecting the cancel flag
};

#endif // SIMPLETHREADHANDLER_H
