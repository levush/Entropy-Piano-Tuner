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

#ifndef FILEMANAGERFORQT_H
#define FILEMANAGERFORQT_H

#include "prerequisites.h"

#include "core/adapters/filemanager.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Implementation of the FileManager for Qt
///////////////////////////////////////////////////////////////////////////////

class FileManagerForQt : public FileManager
{
public:
    FileManagerForQt();                 ///< Constructor, creating directories
    ~FileManagerForQt() {}              ///< Empty destructor

    // Return the path of the log file with the given logname.
    virtual std::string getLogFilePath (const std::string &logname) const override final;

    // Read the content of the XML file of an algorithm with the given ID
    virtual std::wstring getAlgorithmInformationFileContent (const std::string &algorithmId) const override final;
};

#endif // FILEMANAGERFORQT_H
