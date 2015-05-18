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

#ifndef ALGORITHMPARAMETER_H
#define ALGORITHMPARAMETER_H

#include <string>
#include <vector>

class AlgorithmParameter
{
public:
    using StringParameterList = std::vector<std::pair<std::string, std::string> >;
public:
    enum Type {
        TYPE_DOUBLE,
        TYPE_LIST
    };

public:
    // constructor for a double paramter
    AlgorithmParameter(const std::string &id,
                       const std::string &label,
                       const std::string &description,
                       double defaultValue,
                       double minValue,
                       double maxValue,
                       int precision = -1);

    // constructor for a list
    AlgorithmParameter(const std::string &id,
                       const std::string &label,
                       const std::string &description,
                       const std::string &defaultValue,
                       const StringParameterList &list);

    ~AlgorithmParameter();

    const std::string &getID() const {return mID;}
    const std::string &getLabel() const {return mLabel;}
    const std::string &getDescription() const {return mDescription;}
    Type getType() const {return mParameterType;}

    double getDoubleDefaultValue() const {return mDoubleDefaultValue;}
    double getDoubleMinValue() const {return mDoubleMinValue;}
    double getDoubleMaxValue() const {return mDoubleMaxValue;}
    int    getDoublePrecision() const {return mDoublePrecision;}

    const std::string &getStringDefaultValue() const {return mStringDefaultValue;}
    const StringParameterList &getStringList() const {return mStringList;}

private:
    const Type mParameterType;
    const std::string mID;

    const std::string mLabel;
    const std::string mDescription;

    const double mDoubleDefaultValue = 0;
    const double mDoubleMinValue = 0;
    const double mDoubleMaxValue = 0;
    const int    mDoublePrecision = 1;

    const std::string mStringDefaultValue;
    const StringParameterList mStringList;
};

#endif // ALGORITHMPARAMETER_H
