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

#ifndef PIANOFILE_H
#define PIANOFILE_H

#include <fstream>
#include "../../thirdparty/tinyxml2/tinyxml2.h"
#include "piano.h"

class PianoFile
{
public:
    using FileVersionType = int;
    static const std::string FILE_TYPE_NAME;
    static const FileVersionType UNSET_FILE_VERSION;
    static const FileVersionType CURRENT_FILE_VERSION;
    static const FileVersionType MIN_SUPPORTED_FILE_VERSION;
public:
    PianoFile();
    ~PianoFile();

    bool read(const std::string &absolutePath, Piano &piano);
    bool write(const std::string &absolutePath, const Piano &piano) const;

    void readXmlFile(const std::string &absolutePath, Piano &piano);
    void writeXmlFile(const std::string &absolutePath, const Piano &piano) const;

protected:
    std::string getText(const tinyxml2::XMLElement *element);
    void createTextXMLElement(tinyxml2::XMLElement *parent, const char *label, const char *text) const;

    // Piano data
    void read(const tinyxml2::XMLElement *e, Piano &piano);
    void write(tinyxml2::XMLElement *e, const Piano &piano) const;


    // Key data
    void read(const tinyxml2::XMLElement *e, Keyboard &keyboard);
    void write(tinyxml2::XMLElement *e, const Keyboard &keyboard) const;

private:
    /// file version as integer
    FileVersionType mFileVersion;
};

#endif // PIANOFILE_H
