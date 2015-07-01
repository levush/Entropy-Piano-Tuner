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


#ifndef PROJECTMANAGERADAPTER_H
#define PROJECTMANAGERADAPTER_H

#include <string>
#include "../piano/pianofile.h"
#include "../messages/messagelistener.h"

class Core;
class Piano;


///////////////////////////////////////////////////////////////////////////////
/// \brief The ProjectManagerChangesInFileUpdatedCallback class
///////////////////////////////////////////////////////////////////////////////

class FileChangesCallback
{
public:
    virtual void changesInFileUpdated(bool changes) = 0;
};


///////////////////////////////////////////////////////////////////////////////
/// \brief Project manager adapter class.
///
/// This class provides the interface between the core and the GUI for project
/// management, i.e. loading and saving ept files, and keeping track of
/// possible changes.
///////////////////////////////////////////////////////////////////////////////

class ProjectManagerAdapter : public MessageListener
{
public:
    /// Enumeration of the possible outcomes of a typical mouse button
    enum Results
    {
        R_CANCELED,
        R_YES,
        R_NO,
        R_ACCEPTED,
    };



    struct FileDialogResult {
        FileDialogResult(const std::string path = "");

        const std::string path;
        const piano::FileType fileType;

        bool isValid() {return path.size() != 0 && fileType != piano::FT_NONE;}
    };

public:
    ProjectManagerAdapter();                    ///< Constructor
    virtual ~ProjectManagerAdapter() {}         ///< Empty destructor

    void init(Core *core);                      // Initialize the adapter
    void setCallback(FileChangesCallback *cb);  // Set callback function

    bool hasChangesInFile() const {return mChangesInFile;}

    const std::string &getCurrentFilePath() const {return mCurrentFilePath;}


    // signals to be called from the gui
    Results onNewFile();
    Results onSaveFile();
    Results onSaveFileAs();
    Results onOpenFile();
    bool    onQuit();
    Results onEditFile();
    Results onShare();
    Results onExport();


    // open or save file
    Results saveFile(const std::string &path, piano::FileType type);
    Results openFile(const std::string &path, bool cached = false);

protected:
    // signals to be reimplemented by the gui

    /// \brief edit a project file
    /// \return R_OK if successful fals if canceled
    virtual Results editFile() = 0;

    /// \brief asks the user whether to save the current project file
    /// \return true if save is wanted
    virtual Results askForSaving() = 0;

    /// \brief get a path were to save the file
    /// \return the absolute file path
    virtual FileDialogResult getSavePath(int fileType) = 0;

    /// \brief get a path to a file to open
    /// \return the absolute file path
    virtual FileDialogResult getOpenPath(int fileType) = 0;

    /// \brief shares the current file with the actual device
    /// \return R_ACCEPTED if successful
    virtual Results share() = 0;

    /// \brief function to fill the piano data with the current new settings
    virtual void fillNew(Piano &piano) = 0;

    /// Show a message box that an error occurred during the open process
    virtual void showOpenError() = 0;

    /// Show a message box that an error occurred during the save process
    virtual void showSaveError() = 0;

    /// message listener callback
    virtual void handleMessage(MessagePtr m) override;

    /// change the state of mChangesInFile and notify the listener
    void setChangesInFile(bool b);
private:
    Results checkForNoChanges();

protected:
    /// the core
    Core *mCore;

    /// the piano file
    PianoFile mPianoFile;

private:
    /// stores whether there are unsaved changes in the current file
    bool mChangesInFile;

    /// callback if mChangesInFileChanged
    FileChangesCallback *mCallback = nullptr;

    /// current file path
    std::string mCurrentFilePath;
};

#endif // PROJECTMANAGERADAPTER_H
