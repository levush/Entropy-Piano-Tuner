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

class EPT_EXTERN AlgorithmInformation
{
public:
    using ParameterType = AlgorithmParameterDescription;
    using ParameterListType = std::vector<ParameterType>;

public:
    AlgorithmInformation(const std::string id,
                         const std::string version,
                         const std::wstring name,
                         const std::wstring description,
                         const std::wstring year,
                         const std::wstring author,
                         const ParameterListType &parameters);
    ~AlgorithmInformation();

    const std::string &getId() const {return mId;}
    const std::string &getVersion() const {return mVersion;}
    const std::wstring &getName() const {return mName;}
    const std::wstring &getDescription() const {return mDescription;}
    const std::wstring &getYear() const {return mYear;}
    const std::wstring &getAuthor() const {return mAuthor;}
    const ParameterListType &getParameters() const {return mParameters;}
    const ParameterType &getParameter(const std::string &id) const;

private:
    const std::string mId;
    const std::string mVersion;
    const std::wstring mName;
    const std::wstring mDescription;
    const std::wstring mYear;
    const std::wstring mAuthor;
    const ParameterListType mParameters;
};


#endif // ALGORITHMINFORMATION_H
