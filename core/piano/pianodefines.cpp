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

#include "pianodefines.h"
#include "core/system/eptexception.h"

namespace piano {

std::string toFileEnding(FileType ft) {
    switch (ft) {
    case FT_CSV: return "csv";
    case FT_EPT: return "ept";
    default: EPT_EXCEPT(EptException::ERR_NOT_IMPLEMENTED, "Invalid file type");
    }
}

std::wstring toFileEndingW(FileType ft) {
    switch (ft) {
    case FT_CSV: return L"csv";
    case FT_EPT: return L"ept";
    default: EPT_EXCEPT(EptException::ERR_NOT_IMPLEMENTED, "Invalid file type");
    }
}

FileType parseFileType(const std::string &fileEnding) {
    if (fileEnding == "csv") {return FT_CSV;}
    if (fileEnding == "ept") {return FT_EPT;}

    return FT_NONE;
}

FileType parseFileType(const std::wstring &fileEnding) {
    if (fileEnding == L"csv") {return FT_CSV;}
    if (fileEnding == L"ept") {return FT_EPT;}

    return FT_NONE;
}

FileType parseTypeOfFilePath(const std::wstring &filePath) {
    return parseFileType(filePath.substr(filePath.find_last_of(L".") + 1));
}

}  // namespace piano
