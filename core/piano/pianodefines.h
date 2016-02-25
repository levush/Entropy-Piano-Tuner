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

#ifndef PIANODEFINES_H
#define PIANODEFINES_H

#include <string>

namespace piano {

    /// Enumeration of piano types
    enum PianoType
    {
        PT_GRAND = 0,                          ///< The piano is a grand piano
        PT_UPRIGHT,                           ///< The piano is a upgright piano

        PT_COUNT
    };

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief The KeyState enum
    ///
    ///////////////////////////////////////////////////////////////////////////////
    enum KeyState {
        STATE_NORMAL = 0,            ///< Normal state
        STATE_FORCED,                ///< Key is forced

        STATE_COUNT
    };

    //////////////////////////////////////////////////////////////////////////////
    /// \brief The KeyColor enum
    ///
    //////////////////////////////////////////////////////////////////////////////
    enum KeyColor {
        KC_BLACK = 0,             ///< Black key
        KC_WHITE,                 ///< White key

        KC_COUNT
    };

    /// \brief supported piano file types
    enum FileType
    {
        FT_NONE   = 0,
        FT_EPT    = 1,
        FT_CSV    = 2,
    };

    std::string toFileEnding(FileType ft);
    std::wstring toFileEndingW(FileType ft);

    FileType parseFileType(const std::string &fileEnding);
    FileType parseFileType(const std::wstring &fileEnding);
    FileType parseTypeOfFilePath(const std::wstring &filePath);

}  // namespace piano

#endif // PIANODEFINES_H

