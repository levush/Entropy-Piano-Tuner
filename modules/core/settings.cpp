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

#include "settings.h"
#include <assert.h>
#include <locale>

std::unique_ptr<Settings> Settings::mSingleton;

//----------------------------------------------------------------------------
//                               Constructor
//----------------------------------------------------------------------------

Settings::Settings()
{
    mSingleton.reset(this);
}


//----------------------------------------------------------------------------
//                 Get a pointer to the singleton instance
//----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Get a pointer to the singleton instance
/// \return Pointer to singleton
///////////////////////////////////////////////////////////////////////////////

Settings &Settings::getSingleton()
{
    assert(mSingleton);
    return *mSingleton;
}


//----------------------------------------------------------------------------
//                  Get language ID, system language if none
//----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Settings::getUserLanguageId
/// \return String containing the language id
///////////////////////////////////////////////////////////////////////////////

std::string Settings::getUserLanguageId() const
{
    if (not mLanguageId.empty()) return mLanguageId;
    else return std::locale("").name().substr(0, 2);
}
