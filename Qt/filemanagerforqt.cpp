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

#include "filemanagerforqt.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>

#include "core/system/eptexception.h"

FileManagerForQt::FileManagerForQt()
{
    // be sure that these directories exist
    QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation));
    QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
}

FileManagerForQt::~FileManagerForQt()
{

}


std::string FileManagerForQt::getLogFilePath(const std::string &logname) const {
    return QDir(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation)).absoluteFilePath(QString::fromStdString(logname)).toStdString();
}

std::string FileManagerForQt::getAlgorithmInformationFileContent(const std::string &algorithmId) const {
    QFile file(QString::fromStdString(":/algorithms/" + algorithmId + "/" + algorithmId + ".xml"));
    if (file.exists() == false) {
        EPT_EXCEPT(EptException::ERR_CANNOT_READ_FROM_FILE, "File '" + algorithmId + "' not found.");
    }
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        EPT_EXCEPT(EptException::ERR_CANNOT_READ_FROM_FILE, "File '" + algorithmId + "' could not be opened.");
    }

    QTextStream stream(&file);
    return stream.readAll().toStdString();
}
