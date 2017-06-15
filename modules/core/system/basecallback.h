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

#ifndef BASECALLBACK_H
#define BASECALLBACK_H

#include <list>
#include <algorithm>
#include <functional>

#include "prerequisites.h"

class BaseCallbackManager;

///
/// \brief Base class for a callback implementation
///
/// Inherit this class if you want implement an abstract class that
/// provides callback funtions for a listener.
///
/// Upon destruction it will automatically remove itself from its manager.
///
class EPT_EXTERN BaseCallbackInterface {
public:
    virtual ~BaseCallbackInterface();

private:
    void addCallbackManager(BaseCallbackManager *manager);

    void removeCallbackManager(BaseCallbackManager *manager);

private:
    std::list<BaseCallbackManager *> mCallbackManager;

    // allow BaseCallbackManager to call addCallbackManager and removeCallbackManager
    friend class BaseCallbackManager;
};

///
/// \brief Base class for managing callback interfaces
///
/// Do not inherit from this class!
///
/// It implements methods for adding and removing listeners.
///
class EPT_EXTERN BaseCallbackManager
{
public:
    ///
    /// \brief The destructor
    ///
    /// It will automatically remove all listeners from its manager (this)
    ///
    virtual ~BaseCallbackManager() {
        while (mListeners.size() > 0) {
            auto l = mListeners.front();
            mListeners.pop_front();
            removeListener(l);
        }
    }

    ///
    /// \brief Adds a new listener to this manager
    /// \param listener The listener to add
    ///
    /// It will add this as callback manager to the given listener.
    ///
    void addListener(BaseCallbackInterface *listener) {
        auto it = std::find(mListeners.begin(), mListeners.end(), listener);
        if (it == mListeners.end()) {
            mListeners.push_back(listener);
            listener->addCallbackManager(this);
        }
    }

    ///
    /// \brief Removes an existing listener from this manager
    /// \param listener The listener to remove
    ///
    /// It will remove this as callback manager of the given listener.
    ///
    void removeListener(BaseCallbackInterface *listener) {
        auto it = std::find(mListeners.begin(), mListeners.end(), listener);
        if (it != mListeners.end()) {
            mListeners.erase(it);
            listener->removeCallbackManager(this);
        }
    }

    ///
    /// \brief Returns a constant list of all listeners
    /// \return mListeners
    ///
    const std::list<BaseCallbackInterface*> &listeners() const {return mListeners;}
private:

    /// The listeners
    std::list<BaseCallbackInterface*> mListeners;
};

///
/// \brief Callback manager that can invoke a method to all its listeners
///
/// Inherit this if you want to add callback listeners of a specific type to that class
///
template <class CallbackClass>
class CallbackManager : public BaseCallbackManager {
protected:
    ///
    /// \brief Invokes a method
    ///
    /// Calls the given method on all listeners using a std::function
    ///
    template <typename ...Args>
    void invokeCallback(std::function<void (CallbackClass&, Args...)> func, Args... args)
    {
        for (auto listener : listeners())
        {
            CallbackClass *cb = dynamic_cast<CallbackClass*>(listener);
            func(cb, args...);
        }
    }

    ///
    /// \brief Invokes a method
    ///
    /// Calls the given method on all listeners using funtion pointer
    ///
    template<typename ... Args>
    void invokeCallback(void (CallbackClass::*fptr)(Args...), Args... args) const {
        for (auto listener : listeners())
        {
            CallbackClass *cb = dynamic_cast<CallbackClass*>(listener);
            (cb->*fptr)(args...);
        }
    }
};



#endif // BASECALLBACK_H
