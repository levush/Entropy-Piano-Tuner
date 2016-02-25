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
//                      Core initialisation adapter
//=============================================================================

#include "coreinitialisationadapter.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief CoreInitialisationAdapter::mSingleton
///
/// This is a unique pointer belonging to the CoreInitializationAdapter and
/// pointing onto itself. It holds the only instance of the
/// CoreInitializationAdapter, which is defined as a singleton class.
////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<CoreInitialisationAdapter> CoreInitialisationAdapter::mSingleton;


//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Singleton constructor.
/// Resets the unique pointer to the only instance of the class.
///////////////////////////////////////////////////////////////////////////////

CoreInitialisationAdapter::CoreInitialisationAdapter()
{
    mSingleton.reset(this);
}
