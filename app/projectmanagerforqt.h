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

#ifndef PROJECTMANAGERFORQT_H
#define PROJECTMANAGERFORQT_H

#include <QObject>
#include <QWidget>
#include <map>
#include <memory>

#include "prerequisites.h"

#include "core/adapters/projectmanageradapter.h"

#include "piano/pianofileiointerface.h"

class MainWindow;

///////////////////////////////////////////////////////////////////////////////
/// \brief Implementation of the ProjectManagerAdapter in Qt.
///
/// The class will use a QFileDialog to open and close files.
/// It will open the EditPianoSheetDialog to edit the piano data sheet.
///////////////////////////////////////////////////////////////////////////////
class ProjectManagerForQt : public ProjectManagerAdapter
{
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Default constructor.
    /// \param mainwindow : Pointer to the MainWindow
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ProjectManagerForQt(MainWindow *mainwindow);

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Empty virtual destructor.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~ProjectManagerForQt();

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Opens the EditPianoSheetDialog to edit the piano data sheet.
    /// \return Returns whether the user accepted the changes he made.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual Results editFile() override final;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Displays a standard message box if the user wants to save the
    ///        changes made.
    /// \return Decision of the user
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual Results askForSaving() override final;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Opens a QFileDialog to get a path to save the file to.
    /// \return Absolute file path or an empty string if the user cancelled
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual FileDialogResult getSavePath(int fileType) override final;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Opens a QFileDialog to get a path to open a file.
    /// \return Absolute file path or an empty string if the user cancelled
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual FileDialogResult getOpenPath(int fileType) override final;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Handle the share button.
    /// \return Decision of the user.
    ///
    /// It will call a native java method on android for sharing.
    ///////////////////////////////////////////////////////////////////////////////
    virtual Results share() override final;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Shows an error message if an error during open occured.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void showOpenError() override final;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Shows an error message if an error during saving occured.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void showSaveError() override final;

protected:
    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Fills the given Piano with default values and texts.
    /// \param piano : The piano where to store the data to
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void fillNew(Piano &piano) override final;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to get the current path for the file dialog.
    /// \return The last used path or QStandardPaths::DocumentsLocation
    ///////////////////////////////////////////////////////////////////////////////
    QString getCurrentPath() const;

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Function to set the current path for the file dialog.
    /// \param path The absolute file path
    ///////////////////////////////////////////////////////////////////////////////
    void setCurrentPath(QString path);


    QString getFileFilters(int fileTypes, bool addAll) const;
    bool isVaildFileEndig(QString filename, int fileTypes) const;

    virtual void writePianoFile(const FileDialogResult &fileInfo, const Piano &piano) override final;
    virtual void readPianoFile(const FileDialogResult &fileInfo, Piano *piano) override final;

private:
    /// The main window.
    MainWindow *mMainWindow;

    /// The piano file writers
    std::map<piano::FileType, std::unique_ptr<PianoFileIOInterface>> mPianoFileWriters;
};

#endif // PROJECTMANAGERFORQT_H
