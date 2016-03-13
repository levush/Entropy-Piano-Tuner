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

#ifndef ALGORITHMINFORMATION_H
#define ALGORITHMINFORMATION_H

#include "prerequisites.h"
#include "algorithmparameterdescription.h"

template class EPT_EXTERN std::vector<AlgorithmParameterDescription>;

class EPT_EXTERN AlgorithmInformation
{
public:
    using ParameterType = AlgorithmParameterDescription;
    using ParameterListType = std::vector<ParameterType>;

public:
    AlgorithmInformation(const std::string id,
                         const std::string version,
                         const std::string name,
                         const std::string description,
                         const int year,
                         const std::string author,
                         const ParameterListType &parameters);
    ~AlgorithmInformation();

    const std::string &getId() const {return mId;}
    const std::string &getName() const {return mName;}
    const std::string &getVersion() const {return mVersion;}
    const std::string &getDescription() const {return mDescription;}
    int getYear() const {return mYear;}
    const std::string &getAuthor() const {return mAuthor;}
    const ParameterListType &getParameters() const {return mParameters;}
    const ParameterType &getParameter(const std::string &id) const;

private:
    const std::string mId;
    const std::string mName;
    const std::string mVersion;
    const std::string mDescription;
    const int          mYear;
    const std::string mAuthor;
    const ParameterListType mParameters;
};


#endif // ALGORITHMINFORMATION_H
