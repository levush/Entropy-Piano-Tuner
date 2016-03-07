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

#ifndef RECORDERLEVEL_H
#define RECORDERLEVEL_H

#include "prerequisites.h"
#include "../messages/messagelistener.h"

class AudioRecorderAdapter;

////////////////////////////////////////////////////////////////////////////////
/// \brief Adapter for displaying the recording level (VU meter)
///
/// This adapter receives the messages to update the recording level (VU values)
/// and communicates them to the GUI
////////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN RecorderLevel : public MessageListener
{
public:
    RecorderLevel() {}
    ~RecorderLevel() {}

    virtual void handleMessage(MessagePtr m) override;

protected:
    /// \brief Change the shown level.
    /// \param value : Level value between 0 and 1.
    virtual void levelChanged(double value) = 0;
};

#endif // RECORDERLEVEL_H
