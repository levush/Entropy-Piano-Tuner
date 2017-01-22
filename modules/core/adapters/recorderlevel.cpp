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
//             Adapter for displaying the recording level (VU meter)
//=============================================================================

#include "recorderlevel.h"
#include "../messages/messagerecorderenergychanged.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Message handler.
///
/// The message handler receives the incoming messages and listens for the
/// message that the energy (VU level) has changed. If so, it loads the message
/// and checks whether this concerns the input level. Subsequently, the virtual
/// function levelChanged is called, which in the GUI implementation takes the
/// appropriate action.
/// \param m : Incoming message.
///////////////////////////////////////////////////////////////////////////////

void RecorderLevel::handleMessage(MessagePtr m)
{
    if (m->getType() == Message::MSG_RECORDER_ENERGY_CHANGED)
    {
        auto mrec(std::static_pointer_cast<MessageRecorderEnergyChanged>(m));

        // If the received message is about a change of the input level, call
        // the virtual function levelChanged which in turn informs the GUI

        if (mrec->getLevelType() == MessageRecorderEnergyChanged::LevelType::LEVEL_INPUT)
        {
            this->levelChanged(mrec->getLevel());
        }
    }
}
