#include "basecallback.h"

BaseCallbackInterface::~BaseCallbackInterface()
{
    while (mCallbackManager.size() > 0) {
        auto cm = mCallbackManager.front();
        mCallbackManager.pop_front();
        removeCallbackManager(cm);
    }
}

void BaseCallbackInterface::addCallbackManager(BaseCallbackManager *manager)
{
    auto it = std::find(mCallbackManager.begin(), mCallbackManager.end(), manager);
    if (it == mCallbackManager.end()) {
        mCallbackManager.push_back(manager);
        manager->addListener(this);
    }
}

void BaseCallbackInterface::removeCallbackManager(BaseCallbackManager *manager) {
    auto it = std::find(mCallbackManager.begin(), mCallbackManager.end(), manager);
    if (it != mCallbackManager.end()) {
        mCallbackManager.erase(it);
        manager->removeListener(this);
    }
}
