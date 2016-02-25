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
//                        Adapter for mode selection
//=============================================================================


#include "modeselectoradapter.h"

#include "../messages/messagemodechanged.h"
#include "../messages/messagehandler.h"


//-----------------------------------------------------------------------------
//                              Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief ModeSelectorAdapter::ModeSelectorAdapter
///
/// The constructor initializes the mCurrentMode variable with MODE_COUNT,
/// indicating that the operation mode has not yet been set.
///////////////////////////////////////////////////////////////////////////////

ModeSelectorAdapter::ModeSelectorAdapter()
    : mCurrentMode(MODE_COUNT)
{
}

//-----------------------------------------------------------------------------
//                          Set operation mode
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Set operation mode.
///
/// This function has to be called by the GUI to change the operation mode. If
/// the operation mode is different from the previous one, the function sends
/// a corresponding message.
/// \param m : New operation mode
///////////////////////////////////////////////////////////////////////////////

void ModeSelectorAdapter::setMode(OperationMode m)
{
    if (mCurrentMode != m)
    {
        auto oldMode = mCurrentMode;
        mCurrentMode = m;
        MessageHandler::sendUnique<MessageModeChanged>(m, oldMode);
    }
}
