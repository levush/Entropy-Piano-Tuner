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

#include "runguard.h"

#include <QCryptographicHash>

#include "core/prerequisites.h"

//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor for a run guard.
/// \param key : String naming the run guard (e.g. entropypianotuner_runguard)
///////////////////////////////////////////////////////////////////////////////

RunGuard::RunGuard( const QString& key )
    : key (key)
    , memLockKey   (generateKeyHash( key, "_memLockKey"  ))
    , sharedmemKey (generateKeyHash( key, "_sharedmemKey"))
    , sharedMem (sharedmemKey)
    , memLock (memLockKey, 1)
{
    QSharedMemory fix (sharedmemKey);    // Fix for *nix: http://habrahabr.ru/post/173281/
    fix.attach();
}

//-----------------------------------------------------------------------------
//                         Generate hash string
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Generate hash string for a given keys
///
/// This function converts a key (entropypianotuner_runguard) and a second
/// key extenstion (e.g._memLockKey) into a hash string.
/// \param key : String holding the key
/// \param salt : String holding the key extension
/// \return : String containg the hash
////////////////////////////////////////////////////////////////////////////////

QString RunGuard::generateKeyHash( const QString& key, const QString& salt )
{
    QByteArray data;

    data.append( key.toUtf8() );
    data.append( salt.toUtf8() );
    data = QCryptographicHash::hash( data, QCryptographicHash::Sha1 ).toHex();

    return data;
}


//-----------------------------------------------------------------------------
//                 Check whether antoher instance is running
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Check whether another instance of the application is running
///
/// This function checks first whether the own instance is already running.
/// Then it checks whether another application has already created the
/// shared memory, indicating that another instance of the same
/// applicaiton is running.
/// \return Boolean indicating whether another instance of the same
/// application is running.
///////////////////////////////////////////////////////////////////////////////

bool RunGuard::isAnotherRunning()
{
    // If the shared memory is already attached to the own instance
    // there is certainly no other application running.
    if (sharedMem.isAttached()) return false;

    // Otherwise, we have to check whether another instance has
    // already created the shared memory. To find that out we
    // try to attach ourselves. If this is possible
    // it indicates that another application has already created the
    // shared memory. If so, we detach immediately afterwards.
    memLock.acquire();
    const bool sharedMemAlreadyExists = sharedMem.attach();
    if (sharedMemAlreadyExists) sharedMem.detach();
    memLock.release();
    return sharedMemAlreadyExists;
}


//-----------------------------------------------------------------------------
//                              Try to run
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Try to run the applicaiton, return false if another instance is running.
///
/// This function tries to create the shared memory and attach itself to it,
/// indicating that this is the running instance of the EPT-
/// \return True if successful, false if another application is running.
///////////////////////////////////////////////////////////////////////////////

bool RunGuard::tryToRun()
{
    if (isAnotherRunning()) return false; // Extra check

    memLock.acquire();
    const bool success = sharedMem.create( sizeof( quint64 ) );
    memLock.release();
    if (not success) release();
    return success;
}

//-----------------------------------------------------------------------------
//                        Release current application
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Release the running application.
///
/// This function detaches the application from the shared memery.
/// It has to be called when the application terminates.
///////////////////////////////////////////////////////////////////////////////

void RunGuard::release()
{
    memLock.acquire();
    if (sharedMem.isAttached()) sharedMem.detach();
    memLock.release();
}
