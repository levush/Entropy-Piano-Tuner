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

#include "algorithmparameters.h"
#include "core/calculation/calculationmanager.h"
#include "core/system/eptexception.h"

SingleAlgorithmParameters::SingleAlgorithmParameters(const std::string &algorithmName) :
    mAlgorithmName(algorithmName)
{
    loadDefaultParams();
}

void SingleAlgorithmParameters::loadDefaultParams() {
    // load algorithm information from xml file
    // add all params with default value if not in algorithm description

    // If the algorithm is not present, we cannot load defaul parameters
    // This usully occurs if the user loads a file with present information of an
    // algorithm that is not implemented in the current distribution.
    // E.g. Shipping a ept file with a newly implemented algorithm, that the user
    // has not installed on his system.
    if (!CalculationManager::getSingleton().hasAlgorithm(getAlgorithmName())) {
        return;
    }

    auto info = CalculationManager::getSingleton().loadAlgorithmInformation(getAlgorithmName());

    for (const AlgorithmParameterDescription &param : info->getParameters()) {
        switch (param.getType()) {
        case AlgorithmParameterDescription::TYPE_DOUBLE:
            if (hasDoubleParameter(param.getID())) {continue;}
            setDoubleParameter(param.getID(), param.getDoubleDefaultValue());
            break;
        case AlgorithmParameterDescription::TYPE_INT:
            if (hasIntParameter(param.getID())) {continue;}
            setIntParameter(param.getID(), param.getIntDefaultValue());
            break;
        case AlgorithmParameterDescription::TYPE_LIST:
            if (hasStringParameter(param.getID())) {continue;}
            setStringParameter(param.getID(), param.getStringDefaultValue());
            break;
        case AlgorithmParameterDescription::TYPE_BOOL:
            if (hasBoolParameter(param.getID())) {continue;}
            setBoolParameter(param.getID(), param.getBoolDefaultValue());
            break;
        default:
            EPT_EXCEPT(EptException::ERR_NOT_IMPLEMENTED, "Parsing of default parameter not implemented");
            break;
        }
    }
}

double SingleAlgorithmParameters::getDoubleParameter(const std::string &s) const {
    EptAssert(hasDoubleParameter(s), "Parameter has to be in list before accessing.");
    return mDoubleParameters.at(s);
}

int SingleAlgorithmParameters::getIntParameter(const std::string &s) const {
    EptAssert(hasIntParameter(s), "Parameter has to be in list before accessing.");
    return mIntParamters.at(s);
}

const std::string &SingleAlgorithmParameters::getStringParameter(const std::string &id) const {
    EptAssert(hasStringParameter(id), "Parameter has to be in list before accessing.");
    return mStringParameters.at(id);
}

bool SingleAlgorithmParameters::getBoolParameter(const std::string &id) const {
    EptAssert(hasBoolParameter(id), "Parameter has to be in list befor accessing.");
    return mBoolParameters.at(id);
}

SingleAlgorithmParametersPtr AlgorithmParameters::getPreparedParameters(const std::string &id) const
{
    SingleAlgorithmParametersPtr a = getOrCreate(id);
    a->loadDefaultParams();

    return a;
}

SingleAlgorithmParametersPtr AlgorithmParameters::getOrCreate(const std::string &id) const
{
    for (SingleAlgorithmParametersPtr a : mParameters) {
        if (a->getAlgorithmName() == id) {
            return a;
        }
    }

    mParameters.push_back(std::make_shared<SingleAlgorithmParameters>(id));

    return mParameters.back();
}
