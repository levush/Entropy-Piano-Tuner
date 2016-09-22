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

#include "algorithminformation.h"
#include "../system/eptexception.h"

AlgorithmInformation::AlgorithmInformation(const std::string id,
                                           const std::string version,
                                           const std::wstring name,
                                           const std::wstring description,
                                           const std::wstring year,
                                           const std::wstring author,
                                           const ParameterListType &parameters) :
    mId(id),
    mVersion(version),
    mName(name),
    mDescription(description),
    mYear(year),
    mAuthor(author),
    mParameters(parameters)
{

}

AlgorithmInformation::~AlgorithmInformation()
{

}

const AlgorithmInformation::ParameterType &AlgorithmInformation::getParameter(const std::string &id) const {
    for (const ParameterType &param: mParameters) {
        if (param.getID() == id) {
            return param;
        }
    }

    EPT_EXCEPT(EptException::ERR_NOT_IMPLEMENTED, "A parameter with the id '" + id + "' was not found");
}
