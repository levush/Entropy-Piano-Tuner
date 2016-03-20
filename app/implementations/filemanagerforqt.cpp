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
//           Qt file manager implemenation: Open and close files
//=============================================================================

#include "filemanagerforqt.h"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTextStream>

#include "core/system/log.h"
#include "core/system/eptexception.h"

//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// This constructor checks wether the Qt standard locations for cache,
/// generic cache and documentation are writable. If the directories do
/// not yet exist they are created. In case of problems there will be
/// corresponding warnings.
///////////////////////////////////////////////////////////////////////////////

FileManagerForQt::FileManagerForQt()
{
    // Create writable directories if they do not yet exist
    if (not QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)))
        LogW("Could not create or find a writeable location for the cache");
    if (not QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation)))
        LogW("Could not create or find a writeable location for the generic cache");
    if (not QDir().mkpath(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)))
        LogW("Could not create or find a writeable location for the documentation");
}


//-----------------------------------------------------------------------------
//                        Return path of the log file
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Returns the path of the log file with the given logname.
///
/// Returns the full path of the log file. The function does not check
/// whether the log file actually exists
/// \param logname : Name of the log
/// \return : String containing the path to the log file
///////////////////////////////////////////////////////////////////////////////

std::string FileManagerForQt::getLogFilePath(const std::string &logname) const
{
    QDir directory(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation));
    return directory.absoluteFilePath(QString::fromStdString(logname)).toStdString();
}


//-----------------------------------------------------------------------------
//    Read the content of the XML file of an algorithm with the given ID
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Read the content of the XML file of an algorithm with the given ID
/// \param algorithmId : String identifying the algorithm
/// \return The whole content of the corresponding XML file in a single string.
///////////////////////////////////////////////////////////////////////////////

std::wstring FileManagerForQt::getAlgorithmInformationFileContent (const std::string &algorithmId) const
{
    QFile file(QString::fromStdString(":/algorithms/" + algorithmId + ".xml"));

    // If the file does not exist throw an exception:
    if (file.exists() == false)
        EPT_EXCEPT(EptException::ERR_CANNOT_READ_FROM_FILE, "File '" + algorithmId + "' not found.");

    // If the existing file cannot be opened for reading throw an exception:
    if (not file.open(QFile::ReadOnly | QFile::Text))
        EPT_EXCEPT(EptException::ERR_CANNOT_READ_FROM_FILE, "File '" + algorithmId + "' could not be opened.");

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream.setAutoDetectUnicode(true);
    QString content = stream.readAll();
    return content.toStdWString();
}
