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

#include "algorithmfactory.h"
#include "calculationmanager.h"
#include "algorithminformation.h"
#include "core/system/eptexception.h"

AlgorithmFactoryBase::AlgorithmFactoryBase(const AlgorithmFactoryDescription &description) :
    mDescription(description)
{
    CalculationManager::getSingleton().registerFactory(description.getAlgorithmName(), this);
}

void AlgorithmFactoryBase::loadDefaultParams() {
    // load algorithm information from xml file
    // add all params with default value if not in algorithm description
    auto info = CalculationManager::getSingleton().loadAlgorithmInformation(mDescription.getAlgorithmName());

    for (const AlgorithmParameter &param : info->getParameters()) {
        switch (param.getType()) {
        case AlgorithmParameter::TYPE_DOUBLE:
            if (mDescription.hasDoubleParameter(param.getID())) {continue;}
            mDescription.setDoubleParameter(param.getID(), param.getDoubleDefaultValue());
            break;
        case AlgorithmParameter::TYPE_INT:
            if (mDescription.hasIntParameter(param.getID())) {continue;}
            mDescription.setIntParameter(param.getID(), param.getIntDefaultValue());
            break;
        case AlgorithmParameter::TYPE_LIST:
            if (mDescription.hasStringParameter(param.getID())) {continue;}
            mDescription.setStringParameter(param.getID(), param.getStringDefaultValue());
            break;
        default:
            EPT_EXCEPT(EptException::ERR_NOT_IMPLEMENTED, "Parsing of default parameter not implemented");
            break;
        }
    }

}
