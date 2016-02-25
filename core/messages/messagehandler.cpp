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

#include "messagehandler.h"
#include <assert.h>
#include <algorithm>
#include <iostream>
#include "messagelistener.h"


//---------- Singleton variable holding the only instance of this class ---------

MessageHandler MessageHandler::mSingleton;

//------------------------- get Singleton reference -----------------------------

MessageHandler &MessageHandler::getSingleton() {
    return mSingleton;
}

//-------------------------- get Singleton pointer ------------------------------

MessageHandler *MessageHandler::getSingletonPtr() {
    return &mSingleton;
}

//----------- Main task: process the queue and handle the messages --------------

void MessageHandler::process()
{
    // update the listeners
    mListenersChangesMutex.lock();
    // first add then remove, since it can occur, that adding and removing is in the same frame
    std::for_each(mListenersToAdd.begin(), mListenersToAdd.end(), [this](MessageListener *l) {mListeners.push_back(l);});
    std::for_each(mListenersToRemove.begin(), mListenersToRemove.end(), [this](MessageListener *l) {mListeners.remove(l);});
    mListenersToAdd.clear();
    mListenersToRemove.clear();
    mListenersChangesMutex.unlock();

    // copy all messages to a local list, to prevent adding while processing
    std::list<MessagePtr> list;
    mMessageMutex.lock();
    mMessages.swap(list);
    mMessageMutex.unlock();

    // handle messages
    while (!list.empty())
    {
        MessagePtr nextmessage (list.front());
        list.pop_front();
        for (auto listener : mListeners) {
            // check if listener is in remove list, then skip, because this listener is destroyed
            {
                // lock mutex
                std::lock_guard<std::mutex> lock(mListenersChangesMutex);
                // check if existing
                if (std::find(mListenersToRemove.begin(), mListenersToRemove.end(), listener) != mListenersToRemove.end()) {
                    // skip
                    continue;
                }
            }

            // normal message handling
            if (listener->isMessageListenerActive()) {
                listener->handleMessage (nextmessage);
            }
        }
    }
}

//----------------- Add a new listener to the messaging system -----------------

void MessageHandler::addListener(MessageListener *listener) {
    std::lock_guard<std::mutex> lock(mListenersChangesMutex);

    assert (listener);
    assert (std::find(mListenersToAdd.begin(), mListenersToAdd.end(), listener) == mListenersToAdd.end());
    mListenersToAdd.push_back(listener);
}

//---------------- Remove a listener from the messaging system -----------------

void MessageHandler::removeListener(MessageListener *listener) {
    std::lock_guard<std::mutex> lock(mListenersChangesMutex);

    assert (listener);
    assert (std::find(mListenersToRemove.begin(), mListenersToRemove.end(), listener) == mListenersToRemove.end());
    mListenersToRemove.push_back(listener);
}

//--------------------------- Submit a message ---------------------------------

/// \param message the message to add
/// \param dropOlder if true it will remove all older messages of the same type
void MessageHandler::addMessage(MessagePtr message, bool dropOlder) {
    assert (message);
    mMessageMutex.lock();
    if (dropOlder) {
        for (auto it = mMessages.begin(); it != mMessages.end();) {
            if ((*it)->getType() == message->getType()) {
                it = mMessages.erase(it);
            } else {
                ++it;
            }
        }
    }
    mMessages.push_back(message);
    mMessageMutex.unlock();
}
