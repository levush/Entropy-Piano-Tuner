/*****************************************************************************
 * Copyright 2015 Haye Hinrichsen, Christoph Wick
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

#include "algorithmfactorydescription.h"
#include "core/system/eptexception.h"

AlgorithmFactoryDescription::AlgorithmFactoryDescription(
        const std::string algorithmName) :
    mAlgorithmName(algorithmName)
{

}

double AlgorithmFactoryDescription::getDoubleParameter(const std::string &s) const {
    EptAssert(hasDoubleParameter(s), "Parameter has to be in list before accessing.");
    return mDoubleParameters.at(s);
}

int AlgorithmFactoryDescription::getIntParameter(const std::__1::string &s) const {
    EptAssert(hasIntParameter(s), "Parameter has to be in list before accessing.");
    return mIntParamters.at(s);
}

const std::string &AlgorithmFactoryDescription::getStringParameter(const std::string &id) const {
    EptAssert(hasStringParameter(id), "Parameter has to be in list before accessing.");
    return mStringParameters.at(id);
}
