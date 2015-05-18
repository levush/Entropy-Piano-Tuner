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

#ifndef ALGORITHMFACTROYDESCRIPTION_H
#define ALGORITHMFACTROYDESCRIPTION_H

#include <string>
#include <map>
#include <algorithm>

class AlgorithmFactoryDescription
{
public:
    AlgorithmFactoryDescription(const std::string algorithmName);

    const std::string &getAlgorithmName() const {return mAlgorithmName;}

    void setDoubleParameter(const std::string &s, double p) {mDoubleParameters[s] = p;}
    bool hasDoubleParameter(const std::string &s) const {return mDoubleParameters.count(s) > 0;}
    double getDoubleParameter(const std::string &s) const;

    void setStringParameter(const std::string &id, const std::string &s) {mStringParameters[id] = s;}
    bool hasStringParameter(const std::string &id) const {return mStringParameters.count(id) > 0;}
    const std::string &getStringParameter(const std::string &id) const;

private:
    const std::string mAlgorithmName;

    std::map<std::string, double> mDoubleParameters;
    std::map<std::string, std::string> mStringParameters;
};

#endif // ALGORITHMFACTROYDESCRIPTION_H
