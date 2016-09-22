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
//                             Message listener
//=============================================================================

#ifndef MESSAGELISTENER_H
#define MESSAGELISTENER_H

#include "prerequisites.h"
#include "message.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Class of a message listener
///
/// All modules that are suppposed to respond to certain messages are
/// 'message listeners' and thus have to be derived from the present class.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN MessageListener
{
public:
    /// Constructor, registering the present class at the MessageHandler
    MessageListener(bool defaultActivation = true);

    /// Destructor
    virtual ~MessageListener();

    /// Handle message, to be overloaded in the derived class
    virtual void handleMessage(MessagePtr m) = 0;

    // Functions for activating and deactivating message listening
    bool isMessageListenerActive() const {return mMessageListenerActive;}
    void activateMessageListener() {mMessageListenerActive = true;}
    void deactivateMessageListener() {mMessageListenerActive = false;}

private:
    bool mMessageListenerActive;
};

#endif // MESSAGELISTENER_H
