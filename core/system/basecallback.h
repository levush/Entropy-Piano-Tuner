#ifndef BASECALLBACK_H
#define BASECALLBACK_H

#include <list>
#include <algorithm>
#include <functional>

class BaseCallbackManager;

// Base class for a callback interface
class BaseCallbackInterface {
public:
    virtual ~BaseCallbackInterface();

    void addCallbackManager(BaseCallbackManager *manager);

    void removeCallbackManager(BaseCallbackManager *manager);

private:
    std::list<BaseCallbackManager *> mCallbackManager;

    friend class BaseCallbackManager;
};

// Base class managing listeners
class BaseCallbackManager
{
public:
    virtual ~BaseCallbackManager() {
        while (mListeners.size() > 0) {
            auto l = mListeners.front();
            mListeners.pop_front();
            removeListener(l);
        }
    }

    void addListener(BaseCallbackInterface *listener) {
        auto it = std::find(mListeners.begin(), mListeners.end(), listener);
        if (it == mListeners.end()) {
            mListeners.push_back(listener);
            listener->addCallbackManager(this);
        }
    }
    void removeListener(BaseCallbackInterface *listener) {
        auto it = std::find(mListeners.begin(), mListeners.end(), listener);
        if (it != mListeners.end()) {
            mListeners.erase(it);
            listener->removeCallbackManager(this);
        }
    }

    const std::list<BaseCallbackInterface*> &listeners() const {return mListeners;}
private:
    std::list<BaseCallbackInterface*> mListeners;
};

// Inherit this if you want to add callback listeners of a specific type to that class
template <class CallbackClass>
class CallbackManager : public BaseCallbackManager {
protected:
    template <typename ...Args>
    void invokeCallback(std::function<void (CallbackClass&, Args...)> func, Args... args)
    {
        for (auto listener : listeners())
        {
            CallbackClass *cb = dynamic_cast<CallbackClass*>(listener);
            func(cb, args...);
        }
    }

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
