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

//============================================================================
//        RunGuard, preventing multiple instances of the application
//============================================================================

#ifndef RUNGUARD_H
#define RUNGUARD_H

#include <QObject>
#include <QSharedMemory>
#include <QSystemSemaphore>

#include "prerequisites.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Class for checking whether the application is already running
///
/// An object of this class can be used to prevent the system from starting
/// serveral instances of the application. There should be no more than
/// one open window of the EPT at a given time.
///
/// Internally the instances communicate via a QSharedMemory labelled by a
/// given key. The access to this memory is mutexted by a QSystemSemaphore.
///////////////////////////////////////////////////////////////////////////////

class RunGuard
{
public:
    RunGuard (const QString& key);
    ~RunGuard() {release();}

    bool isAnotherRunning();
    bool tryToRun();
    void release();

private:
    QString generateKeyHash (const QString& key, const QString& salt);

    const QString key;          ///< Name assigned to the instance
    const QString memLockKey;   ///< Key labelling the semaphore lock
    const QString sharedmemKey; ///< Key labelling the shared memory

    QSharedMemory sharedMem;    ///< Shared memory labelled by a given key
    QSystemSemaphore memLock;   ///< Lock used when shared memory is accessed
};

#endif  // RUN_GUARD
