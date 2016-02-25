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
//                       Adapter for project management
//=============================================================================


#include "projectmanageradapter.h"

#include "filemanager.h"
#include "../core.h"
#include "../system/eptexception.h"
#include "../system/log.h"
#include "../messages/messagehandler.h"
#include "../messages/messageprojectfile.h"

using piano::FT_CSV;
using piano::FT_EPT;
using piano::FT_NONE;
using piano::parseFileType;

ProjectManagerAdapter::FileDialogResult::FileDialogResult(const std::wstring p) :
    path(p),
    fileType(piano::parseTypeOfFilePath(p)) {
}

ProjectManagerAdapter::FileDialogResult::FileDialogResult(const std::wstring path, const piano::FileType fileType) :
    path(path),
    fileType(fileType) {
    assert(piano::parseTypeOfFilePath(path) == fileType);
}

//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor of the ProjectManagerAdapter
///////////////////////////////////////////////////////////////////////////////

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
/// \brief onNewFile: New-file menu response
///
/// This function is called when the button "New file" is touched in the GUI.
/// \return Enum of type ProjectManagerAdapter::Results
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

///////////////////////////////////////////////////////////////////////////////
/// \brief onSaveFile : Menu save-file response
///
/// This function is called when the button "Save file" is touched in the GUI.
/// \return Enum of type ProjectManagerAdapter::Results
///////////////////////////////////////////////////////////////////////////////

ProjectManagerAdapter::Results ProjectManagerAdapter::onSaveFile()
{
    if (mCurrentFilePath.size() == 0)
    {
        // no path specified, use save as
        return onSaveFileAs();
    }

    // save at current location
    saveFile(FileDialogResult(mCurrentFilePath, FT_EPT));
    return R_ACCEPTED;
}


//-----------------------------------------------------------------------------
//                          Menu command: Save as...
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief onSaveFileAs : Menu save-file-as response
///
/// This function is called when the button "Save file as..." is touched
/// in the GUI.
/// \return Enum of type ProjectManagerAdapter::Results
///////////////////////////////////////////////////////////////////////////////

ProjectManagerAdapter::Results ProjectManagerAdapter::onSaveFileAs() {
    FileDialogResult r = getSavePath(FT_EPT);
    if (!r.isValid()) {
        // user cancelled
        return R_CANCELED;
    }

    // file was stored    
    saveFile(FileDialogResult(r.path, r.fileType));
    return R_ACCEPTED;
}


//-----------------------------------------------------------------------------
//                           Menu command: Open
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief onOpen : Menu open-file response
///
/// This function is called when the button "Open file" is touched in the GUI.
/// \return Enum of type ProjectManagerAdapter::Results
///////////////////////////////////////////////////////////////////////////////

ProjectManagerAdapter::Results ProjectManagerAdapter::onOpenFile()
{
    if (checkForNoChanges() == R_CANCELED) {return R_CANCELED;}

    FileDialogResult r = getOpenPath(FT_EPT);
    if (!r.isValid()) {
        // user cancelled
        return R_CANCELED;
    }

    // file was opened
    openFile(r);

    return R_ACCEPTED;
}


//-----------------------------------------------------------------------------
//                          Menu command: Quit
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief onQuit : Menu quit response
///
/// This function is called when the button "Quit / Exit" is touched in the GUI.
/// \return Enum of type ProjectManagerAdapter::Results
///////////////////////////////////////////////////////////////////////////////

bool ProjectManagerAdapter::onQuit()
{
    if (mChangesInFile)
    {
        switch (askForSaving())
        {
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
    LogI("Quitting accepted");
    return true;
}


//-----------------------------------------------------------------------------
//                    Menu command: Edit piano data sheet
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief onEditFile : Menu edit-file response
///
/// This function is called when the button "Edit file" (piano data sheet)
/// is touched in the GUI.
/// \return Enum of type ProjectManagerAdapter::Results
///////////////////////////////////////////////////////////////////////////////

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
//                    Menu command: Share file
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief onShareFile : Menu share-file response
///
/// This function is called when the button "Save file" is touched in the GUI.
/// \return Enum of type ProjectManagerAdapter::Results
///////////////////////////////////////////////////////////////////////////////

ProjectManagerAdapter::Results ProjectManagerAdapter::onShare()
{
    if (checkForNoChanges() != R_ACCEPTED) {return R_CANCELED;}
    // share file
    return share();
}


//-----------------------------------------------------------------------------
//                    Menu command: Export data
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief onExport : Menu Export-file response
///
/// This function is called when the button "Export file" is touched in the GUI.
/// \return Enum of type ProjectManagerAdapter::Results
///////////////////////////////////////////////////////////////////////////////

ProjectManagerAdapter::Results ProjectManagerAdapter::onExport()
{
    if (checkForNoChanges() != R_ACCEPTED) {return R_CANCELED;}

    FileDialogResult r = getSavePath(FT_CSV);
    if (!r.isValid())
    {
        // user cancelled
        return R_CANCELED;
    }

    // file was stored
    saveFile(FileDialogResult(r.path, r.fileType));
    return R_ACCEPTED;
}


//-----------------------------------------------------------------------------
//                           Check for no changes
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Check whether no changes have been made. If not ask whether
/// the user wants to save the file
/// \return  Enum of type ProjectManagerAdapter::Results
///////////////////////////////////////////////////////////////////////////////

ProjectManagerAdapter::Results ProjectManagerAdapter::checkForNoChanges()
{
    if (mChangesInFile)
    {
        switch (askForSaving())
        {
        case R_NO:
            // dont save file, but resume
            break;
        case R_CANCELED:
            return R_CANCELED;
        default:
            // save the file first
            if (onSaveFile() == R_CANCELED)
            {
                // canceling the save progress will also cancel the next desired process
                return R_CANCELED;
            }
            break;
        }
    }
    return R_ACCEPTED;
}


//-----------------------------------------------------------------------------
//                      Handle and dispatch messages
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Message handler and dispatcher for the ProjectManagerAdapter
/// \param m : Pointer to the incoming message
///////////////////////////////////////////////////////////////////////////////

void ProjectManagerAdapter::handleMessage(MessagePtr m)
{
    switch (m->getType())
    {
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


//-----------------------------------------------------------------------------
//                      Mark a file as being changed
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Mark a file as being changed
///
/// This function calls the callback function changesInFileUpdated.
/// \param b : True if changes have been made, otherwise False
///////////////////////////////////////////////////////////////////////////////

void ProjectManagerAdapter::setChangesInFile(bool b)
{
    mChangesInFile = b;
    if (mCallback)
    {
        mCallback->changesInFileUpdated(mChangesInFile);
    }

}


//-----------------------------------------------------------------------------
//                              Save file
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief ProjectManagerAdapter::saveFile
/// \param path : Path where the file shall be stored
/// \param type : Type of the file
///
/// This function calls the function mPianoFile.write in order to write
/// the entire piano. Possible exceptions are caught and handled. If the
/// file was save successfully the flag for changes is reset to false.
/// \return Enum of type ProjectManagerAdapter::Results
///////////////////////////////////////////////////////////////////////////////

ProjectManagerAdapter::Results ProjectManagerAdapter::saveFile(const FileDialogResult &fileInfo)
{
    EptAssert(fileInfo.isValid(), "File type not valid.");

    try // catching possible exception errors
    {
        // Try to save the piano
        writePianoFile(fileInfo, mCore->getPianoManager()->getPiano());

        LogI("File saved!");
        mCurrentFilePath = fileInfo.path;    // remember current path
        setChangesInFile(false);             // after saving the status is that there are no changes

        // Tell the other modules that the file has been saved
        MessageHandler::send<MessageProjectFile>(MessageProjectFile::FILE_SAVED,
                                                 mCore->getPianoManager()->getPiano());
    }
    catch (const EptException &e)
    {
        LogW("Exception during saving a file: %s", e.what());
        // show message box, that there is an error
        showSaveError();
        return R_CANCELED;
    }
    return R_ACCEPTED;
}


//-----------------------------------------------------------------------------
//                          Read (open) piano file
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Open (read) a file
/// \param path : Path from where the file is read
/// \param cached : flag whether reading is cached
/// (retrieved from another source)
/// \return Enum of type ProjectManagerAdapter::Results
///////////////////////////////////////////////////////////////////////////////

ProjectManagerAdapter::Results ProjectManagerAdapter::openFile(const FileDialogResult &fileInfo, bool cached)
{
    try
    {
        readPianoFile(fileInfo, &mCore->getPianoManager()->getPiano());

        LogI("File opened!");
        if (cached)
        {
            // file is in cache, we need to save it at a new position
            mCurrentFilePath.clear();
            setChangesInFile(true);
        }
        else
        {
            mCurrentFilePath = fileInfo.path;
            setChangesInFile(false);
        }
        MessageHandler::send<MessageProjectFile>(MessageProjectFile::FILE_OPENED, mCore->getPianoManager()->getPiano());
    }
    catch (const EptException &e)
    {
        LogW("Exception during opening a file: %s", e.what());
        // show message box, that there is an error
        showOpenError();
        return R_CANCELED;
    }
    return R_ACCEPTED;
}
