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
//                          File manager adapter
//=============================================================================

#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <fstream>

#include "prerequisites.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Abstract singleton adapter for opening files
///
/// This adapter provides an interface for opening files. The tuner produces
/// various files (e.g. log files) which have to be saved at certain standard
/// locations, depending on the platform of the implementation. For example,
/// Windows and Android applications provide different standard locations for
/// writing and reading files. These differences are accounted for in the
/// derived implemenations.
///
/// The FileManager is a singleton class, i.e., there is only a single instance.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN FileManager
{
public:
    /// \brief Constructor, setting the singleton pointer to its own instance.
    FileManager() { mSingleton.reset(this); }
    virtual ~FileManager() {}                       ///< Virtual destructor


    static FileManager &getSingleton();             // Get singleton reference

    virtual bool open(std::ifstream &stream,        // Open an input stream
                      const std::string &absolute,
                      std::ios_base::openmode mode = std::ios_base::in);

    virtual bool open(std::ofstream &stream,        // Open an output stream
                      const std::string &absolute,
                      std::ios_base::openmode mode = std::ios_base::out);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Abstract function: Get the standard path for logfiles
    ///
    /// Each platform provides a different standard directory for logfile.
    /// This abstract function has to be overridden accordingly in the
    /// respective implementation. The tuner software allows for different
    /// logs labelled by certain names passed here as an argument.
    /// \see FileManagerForQt
    /// \param logname : The name of the log
    /// \returns Absolute path to the log
    ///////////////////////////////////////////////////////////////////////////

    virtual std::string getLogFilePath(const std::string &logname) const = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Abstract function: Get the file content for an algorithm
    ///
    /// In the EPT each tuning algorithm comes with its own xml file. This
    /// function opens the xml file and returns its content in form of a
    /// single string. The only parameter is the ID of the algorithm.
    /// This function has to be implemented in the derived class.
    /// \see FileManagerForQt
    /// \param algorithID : Identification string of the algorithm
    /// \return String containing the information of the corresponding XML file
    ///////////////////////////////////////////////////////////////////////////

    virtual std::wstring getAlgorithmInformationFileContent (const std::string &algorithmId) const = 0;

private:
    static std::unique_ptr<FileManager> mSingleton; ///< Singleton unique pointer
};

#endif // FILEMANAGER_H
