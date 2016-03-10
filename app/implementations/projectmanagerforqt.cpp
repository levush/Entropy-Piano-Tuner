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

#include "projectmanagerforqt.h"
#include "dialogs/editpianosheet/editpianosheetdialog.h"
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDate>
#ifdef __ANDROID__
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#endif

#include "core/system/log.h"
#include "core/config.h"

#include "dialogs/simplefiledialog.h"
#include "implementations/settingsforqt.h"
#include "mainwindow/mainwindow.h"
#include "piano/pianofileiocsv.h"
#include "piano/pianofileioxml.h"

#include "qtconfig.h"

ProjectManagerForQt::ProjectManagerForQt(MainWindow *mainwindow)
    : ProjectManagerAdapter(),
      mMainWindow(mainwindow) {

    mPianoFileWriters[piano::FT_CSV].reset(new PianoFileIOCsv());
    mPianoFileWriters[piano::FT_EPT].reset(new PianoFileIOXml());
}

ProjectManagerForQt::~ProjectManagerForQt()
{

}


ProjectManagerForQt::Results ProjectManagerForQt::editFile() {
    EditPianoSheetDialog d(mCore->getPianoManager()->getPiano(), mMainWindow);

    if (d.exec() == QDialog::Accepted) {
        d.applyData(&mCore->getPianoManager()->getPiano());
        return R_ACCEPTED;
    } else {
        return R_CANCELED;
    }
}

ProjectManagerForQt::Results ProjectManagerForQt::askForSaving() {
    QMessageBox msgBox(mMainWindow);
    msgBox.setWindowTitle(mMainWindow->windowTitle());
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(MainWindow::tr("The document has been modified."));
    msgBox.setInformativeText(MainWindow::tr("Do you want to save your changes?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    switch (msgBox.exec()) {
    case QMessageBox::Yes:
        return R_YES;
    case QMessageBox::No:
        return R_NO;
    default:
        return R_CANCELED;
    }
}

ProjectManagerForQt::FileDialogResult ProjectManagerForQt::getSavePath(int fileType) {
#if CONFIG_USE_SIMPLE_FILE_DIALOG
    Q_UNUSED(fileType);
    return SimpleFileDialog::getSaveFile(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toStdWString();
#else
    QString path(getCurrentPath());
    QDir().mkdir(path);
    QString defaultSuffix;
    if (fileType & piano::FT_EPT) {defaultSuffix = "ept";}
    else if (fileType & piano::FT_CSV) {defaultSuffix = "csv";}
    // static method call is required to get native file dialog
    QString file = QFileDialog::getSaveFileName(mMainWindow, MainWindow::tr("Save"), path, getFileFilters(fileType, true), 0);
    if (file.isEmpty()) {
        // canceled
        return std::wstring();
    }
    if (isVaildFileEndig(file, fileType) == false) {
        // add default ending
        file.append("." + defaultSuffix);
    }
    // update/store path
    setCurrentPath(QFileInfo(file).absoluteDir().absolutePath());
    // return selected file
    return file.toStdWString();
#endif
}

ProjectManagerForQt::FileDialogResult ProjectManagerForQt::getOpenPath(int fileType)  {
#if CONFIG_USE_SIMPLE_FILE_DIALOG
    Q_UNUSED(fileType);
    return SimpleFileDialog::getOpenFile(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toStdWString();
#else
    QString path(getCurrentPath());
    QDir().mkdir(path);
    // static method call is required to get native file dialog
    QString file = QFileDialog::getOpenFileName(mMainWindow, MainWindow::tr("Open"), path, getFileFilters(fileType, true), 0);
    if (file.isEmpty()) {
        // canceled
        return std::wstring();
    }
    // update/store path
    setCurrentPath(QFileInfo(file).absoluteDir().absolutePath());
    // return selected file
    return file.toStdWString();
#endif
}

ProjectManagerForQt::Results ProjectManagerForQt::share() {
#ifdef __ANDROID__
    // on android we have to call a java method, that does the sharing

    // get the application instance
    QAndroidJniObject instance = QAndroidJniObject::callStaticObjectMethod("org/uniwue/tp3/TunerApplication", "getInstance", "()Lorg/uniwue/tp3/TunerApplication;");
    QAndroidJniObject jTitle = QAndroidJniObject::fromString(MainWindow::tr("Share tuning data"));
    QAndroidJniObject jPath = QAndroidJniObject::fromString(QString::fromStdWString(getCurrentFilePath()));
    // get the path to the file to open, zero length if there is none
    instance.callMethod<void>("shareFile", "(Ljava/lang/String;Ljava/lang/String;)V", jTitle.object<jstring>(), jPath.object<jstring>());
    // check for errors
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
#else
    LogI("Sharing for this platform is not supported yet.");
#endif
    return R_CANCELED; // function has to return something, therefore I put this here
}

void ProjectManagerForQt::fillNew(Piano &piano) {
    piano.setName(MainWindow::tr("New piano").toStdWString());
    piano.setSerialNumber(QString("0000-0000").toStdWString());
    piano.setManufactureYear(QString("%1").arg(QDate::currentDate().year()).toStdWString());
    piano.setManufactureLocation(MainWindow::tr("Unknown").toStdWString());

    piano.setTuningLocation(MainWindow::tr("Unknown").toStdWString());
    piano.setTuningTimeToCurrentTime();
    piano.setConcertPitch(Piano::DEFAULT_CONCERT_PITCH);
    piano.getKeyboard().changeKeyboardConfiguration(Piano::DEFAULT_NUMBER_OF_KEYS,
                                        Piano::DEFAULT_KEY_NUMBER_OF_A);
}

void ProjectManagerForQt::showOpenError() {
    QMessageBox::critical(mMainWindow, MainWindow::tr("Error"), MainWindow::tr("File could not be opened."));
}

void ProjectManagerForQt::showSaveError() {
    QMessageBox::critical(mMainWindow, MainWindow::tr("Error"), MainWindow::tr("File could not be saved."));
}

QString ProjectManagerForQt::getCurrentPath() const {
    QSettings s;
    return s.value(SettingsForQt::KEY_CURRENT_FILE_DIALOG_PATH, QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();
}

void ProjectManagerForQt::setCurrentPath(QString path) {
    QSettings s;
    s.setValue(SettingsForQt::KEY_CURRENT_FILE_DIALOG_PATH, path);
    EptAssert(QDir(path).exists(), "Default path should exist.");
}

QString ProjectManagerForQt::getFileFilters(int fileTypes, bool addAll) const {
    QString files;
    if (fileTypes & piano::FT_EPT) {
        files += MainWindow::tr("Entropy piano tuner") + " (*.ept);;";
    }
    if (fileTypes & piano::FT_CSV) {
        files += MainWindow::tr("Comma-separated values") + " (*.csv);;";
    }
    if (addAll) {
        files += MainWindow::tr("All files") + " (*)";
    }
    return files;
}

bool ProjectManagerForQt::isVaildFileEndig(QString filename, int fileTypes) const {
    if (fileTypes & piano::FT_EPT) {
        if (filename.endsWith(".ept")) {return true;}
    }
    if (fileTypes & piano::FT_CSV) {
        if (filename.endsWith(".csv")) {return true;}
    }

    return false;
}

void ProjectManagerForQt::writePianoFile(const FileDialogResult &fileInfo, const Piano &piano) {
    QFile file(QString::fromStdWString(fileInfo.path));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        EPT_EXCEPT(EptException::ERR_CANNOT_WRITE_TO_FILE, QString("Could not open file '%1'").arg(QString::fromStdWString(fileInfo.path)).toStdString().c_str());
    }

    mPianoFileWriters[fileInfo.fileType]->write(&file, piano);
}

void ProjectManagerForQt::readPianoFile(const FileDialogResult &fileInfo, Piano *piano) {
    QFile file(QString::fromStdWString(fileInfo.path));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        EPT_EXCEPT(EptException::ERR_CANNOT_READ_FROM_FILE, QString("Could not open file '%1'").arg(QString::fromStdWString(fileInfo.path)).toStdString().c_str());
    }

    mPianoFileWriters[fileInfo.fileType]->read(&file, *piano);
}
