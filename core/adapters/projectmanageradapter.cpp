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
//                       Adapter for project management
//=============================================================================


#include "projectmanageradapter.h"

#include "filemanager.h"
#include "../core.h"
#include "../system/eptexception.h"
#include "../system/log.h"
#include "../messages/messagehandler.h"
#include "../messages/messageprojectfile.h"

//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

ProjectManagerAdapter::ProjectManagerAdapter()
    : mCore(nullptr),                                   // no pointer to core
      mChangesInFile(false)                             // no changes
{
}


//-----------------------------------------------------------------------------
//                  Initialize the project management adapter
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the project management adapter.
/// \param core : Pointer to the core instance.
///////////////////////////////////////////////////////////////////////////////

void ProjectManagerAdapter::init (Core *core)
{
    mCore = core;

    // use default file, but fill with current values
    fillNew(mCore->getPianoManager()->getPiano());
    MessageHandler::send<MessageProjectFile>(
                MessageProjectFile::FILE_CREATED,
                mCore->getPianoManager()->getPiano());
}


//-----------------------------------------------------------------------------
//                  Set a callback function for file changes
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Set a callback function for file changes
///
/// Like most applications, the entropy piano tuner goes into a state of
/// 'having unsaved changes' as soon as a data element of the current project
/// has been changed. Usually this is indicated in the GUI, e.g. by a little
/// star after the file name in the title bar of the window. In order to
/// inform the GUI about changes, we have to define a suitable callback function.
/// \param cb : Pointer to the callback function
///////////////////////////////////////////////////////////////////////////////

void ProjectManagerAdapter::setCallback (FileChangesCallback *cb)
{
    mCallback = cb;
}



//-----------------------------------------------------------------------------
//                          Menu command: New file
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief New-file menu response
///
///
/// \return
///////////////////////////////////////////////////////////////////////////////

ProjectManagerAdapter::Results ProjectManagerAdapter::onNewFile()
{

    if (checkForNoChanges() == R_CANCELED) {return R_CANCELED;}

    // create new file
    mCurrentFilePath.clear();
    mCore->getPianoManager()->getPiano() = Piano();
    fillNew(mCore->getPianoManager()->getPiano());

    setChangesInFile(true);
    MessageHandler::send<MessageProjectFile>(MessageProjectFile::FILE_CREATED,
                                             mCore->getPianoManager()->getPiano());

    // edit this new file
    if (editFile() == R_ACCEPTED)
    {
        MessageHandler::send<MessageProjectFile>(MessageProjectFile::FILE_EDITED,
                                                 mCore->getPianoManager()->getPiano());
    }
    return R_ACCEPTED;
}



//-----------------------------------------------------------------------------
//                          Menu command: Save
//-----------------------------------------------------------------------------

ProjectManagerAdapter::Results ProjectManagerAdapter::onSaveFile() {
    if (mCurrentFilePath.size() == 0)
    {
        // no path specified, use save as
        return onSaveFileAs();
    }

    // save at current location
    saveFile(mCurrentFilePath);
    return R_ACCEPTED;
}


//-----------------------------------------------------------------------------
//                          Menu command: Save as...
//-----------------------------------------------------------------------------

ProjectManagerAdapter::Results ProjectManagerAdapter::onSaveFileAs() {
    std::string path = getSavePath();
    if (path.size() == 0) {
        // user cancelled
        return R_CANCELED;
    }

    // file was stored    
    saveFile(path);
    return R_ACCEPTED;
}


//-----------------------------------------------------------------------------
//                           Menu command: Open
//-----------------------------------------------------------------------------

ProjectManagerAdapter::Results ProjectManagerAdapter::onOpenFile()
{
    if (checkForNoChanges() == R_CANCELED) {return R_CANCELED;}

    std::string path = getOpenPath();
    if (path.size() == 0)
    {
        // user cancelled
        return R_CANCELED;
    }


    // file was opened
    openFile(path);

    return R_ACCEPTED;
}


//-----------------------------------------------------------------------------
//                          Menu command: Quit
//-----------------------------------------------------------------------------

bool ProjectManagerAdapter::onQuit() {
    if (mChangesInFile) {
        switch (askForSaving()) {
        case R_NO:
            // dont save file, but resume
            break;
        case R_CANCELED:
            return false;
        default:
            // save the file
            if (onSaveFile() == R_CANCELED) {
                // cancel aswell
                return R_CANCELED;
            }
            break;
        }
    }

    INFORMATION("Quitting accepted");

    return true;
}


//-----------------------------------------------------------------------------
//                    Menu command: Edit piano data sheet
//-----------------------------------------------------------------------------

ProjectManagerAdapter::Results ProjectManagerAdapter::onEditFile()
{
    Results r = editFile();
    // edit current file
    if (r == R_ACCEPTED)
    {
        // there are changes
        setChangesInFile(true);
        // notify system that we edit the file
        MessageHandler::send<MessageProjectFile>(MessageProjectFile::FILE_EDITED, mCore->getPianoManager()->getPiano());
    }

    return r;
}


//-----------------------------------------------------------------------------
//                    Menu command: Edit piano data sheet
//-----------------------------------------------------------------------------



ProjectManagerAdapter::Results ProjectManagerAdapter::onShare() {
    if (checkForNoChanges() != R_ACCEPTED) {return R_CANCELED;}

    // share file
    return share();
}


ProjectManagerAdapter::Results ProjectManagerAdapter::checkForNoChanges() {
    if (mChangesInFile) {
        switch (askForSaving()) {
        case R_NO:
            // dont save file, but resume
            break;
        case R_CANCELED:
            return R_CANCELED;
        default:
            // save the file first
            if (onSaveFile() == R_CANCELED) {
                // canceling the save progress will also cancel the next desired process
                return R_CANCELED;
            }
            break;
        }
    }
    return R_ACCEPTED;
}

void ProjectManagerAdapter::handleMessage(MessagePtr m) {
    switch (m->getType()) {
    case Message::MSG_CHANGE_TUNING_CURVE:
    case Message::MSG_NEW_FFT_CALCULATED:
    case Message::MSG_KEY_DATA_CHANGED:
    case Message::MSG_CLEAR_RECORDING:
        setChangesInFile(true);
        break;
    default:
        break;
    }
}

void ProjectManagerAdapter::setChangesInFile(bool b) {
    mChangesInFile = b;
    if (mCallback) {
        mCallback->changesInFileUpdated(mChangesInFile);
    }

}

ProjectManagerAdapter::Results ProjectManagerAdapter::saveFile(const std::string &path) {
    try {
        if (!mPianoFile.write(path, mCore->getPianoManager()->getPiano())) {
            WARNING("File could not be saved");
            return R_CANCELED;
        }


        INFORMATION("File saved!");
        mCurrentFilePath = path;
        setChangesInFile(false);

        MessageHandler::send<MessageProjectFile>(MessageProjectFile::FILE_SAVED, mCore->getPianoManager()->getPiano());
    }
    catch (const EptException &e) {
        // show message box, that there is an error
        showSaveError();
        return R_CANCELED;
    }
    return R_ACCEPTED;
}

ProjectManagerAdapter::Results ProjectManagerAdapter::openFile(const std::string &path, bool cached) {
    try {
        if (!mPianoFile.read(path, mCore->getPianoManager()->getPiano())) {
            WARNING("File could not be opened");
            return R_CANCELED;
        }


        INFORMATION("File opened!");
        if (cached) {
            // file is in cache, we need to save it at a new position
            mCurrentFilePath.clear();
            setChangesInFile(true);
        } else {
            mCurrentFilePath = path;
            setChangesInFile(false);
        }

        MessageHandler::send<MessageProjectFile>(MessageProjectFile::FILE_OPENED, mCore->getPianoManager()->getPiano());
    }
    catch (const EptException &e) {
        // show message box, that there is an error
        showOpenError();
        return R_CANCELED;
    }
    return R_ACCEPTED;
}
