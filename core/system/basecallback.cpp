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
