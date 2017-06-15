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
//                              Message handler
//=============================================================================

#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <list>
#include <queue>
#include <memory>
#include <mutex>

#include "prerequisites.h"
#include "messagelistener.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief Class for handling and sending messages
///
/// This class adds messages of the different threads, places them
/// in a queue and sends them to the connected listeners.
///
/// This class is a singleton.
/// Note that "process" has to be called in the thread of the GUI.
//////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN MessageHandler
{
public:
    /// short function for creating and sending a message
    template <class msgclass, class... Args>
    static void send(Args&&... args) {
        // this function has to be implemented in the header, since it is static template (linker errors elswise!)
        getSingleton().addMessage(std::make_shared<msgclass>(args...), false);
    }
    /// short function for creating and sending a simple message
    static void send(Message::MessageTypes type) {send<Message>(type);}

    /// short function for creating and sending a message and drop older messages of the same type so that this message is unique
    template <class msgclass, class... Args>
    static void sendUnique(Args&&... args) {
        // this function has to be implemented in the header, since it is static template (linker errors elswise!)
        getSingleton().addMessage(std::make_shared<msgclass>(args...), true);
    }
    /// short function for creating and sending a simple message
    static void sendUnique(Message::MessageTypes type) {sendUnique<Message>(type);}
private:
    /// \brief private constructor since this class is a singleton
    MessageHandler() {}

public:
    ~MessageHandler(){}                                     ///< Empty desctructor

    static MessageHandler &getSingleton();                  ///< get a reference to the singleton class
    static MessageHandler *getSingletonPtr();               ///< get a pointer to the singleton class

    void process();                                         ///< Main task, processing the events in the queue

    void addListener(MessageListener *listener);            ///< Connect a new message listener
    void removeListener(MessageListener *listener);         ///< Disconnect a message listener
    void addMessage(MessagePtr message, bool dropOlder = false);  ///< Submit a message

private:

    static MessageHandler mSingleton;                       ///< Singleton instance
    std::list<MessageListener*> mListeners;                 ///< List of all listeners
    std::list<MessageListener*> mListenersToAdd;            ///< List of listeners to add in the next frame
    std::list<MessageListener*> mListenersToRemove;         ///< List of listeners to remove in the next frame
    mutable std::mutex mListenersChangesMutex;              ///< Mutex for accessing the listeners list
    std::list<MessagePtr> mMessages;                        ///< Queue of messages to be submitted
    mutable std::mutex mMessageMutex;                       ///< Mutex for accessing the queue
};


#endif // MESSAGEHANDLER_H
