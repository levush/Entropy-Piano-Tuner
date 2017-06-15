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

#ifndef ALGORITHMPARAMETERS_H
#define ALGORITHMPARAMETERS_H

#include <vector>
#include <string>
#include <map>
#include <memory>

#include "prerequisites.h"

class EPT_EXTERN SingleAlgorithmParameters
{
public:
    SingleAlgorithmParameters(const std::string &algorithmName);

    const std::string &getAlgorithmName() const {return mAlgorithmName;}

    void loadDefaultParams();

    void setDoubleParameter(const std::string &s, double p) {mDoubleParameters[s] = p;}
    bool hasDoubleParameter(const std::string &s) const {return mDoubleParameters.count(s) > 0;}
    double getDoubleParameter(const std::string &s) const;
    const std::map<std::string, double> &getDoubleParameters() const {return mDoubleParameters;}

    void setIntParameter(const std::string &s, int i) {mIntParamters[s] = i;}
    bool hasIntParameter(const std::string &s) const {return mIntParamters.count(s) > 0;}
    int getIntParameter(const std::string &s) const;
    const std::map<std::string, int> &getIntParameters() const {return mIntParamters;}

    void setStringParameter(const std::string &id, const std::string &s) {mStringParameters[id] = s;}
    bool hasStringParameter(const std::string &id) const {return mStringParameters.count(id) > 0;}
    const std::string &getStringParameter(const std::string &id) const;
    const std::map<std::string, std::string> &getStringParameters() const {return mStringParameters;}

    void setBoolParameter(const std::string &id, bool b) {mBoolParameters[id] = b;}
    bool hasBoolParameter(const std::string &id) const {return mBoolParameters.count(id) > 0;}
    bool getBoolParameter(const std::string &id) const;
    const std::map<std::string, bool> &getBoolParameters() const {return mBoolParameters;}

private:
    const std::string mAlgorithmName;

    std::map<std::string, double> mDoubleParameters;
    std::map<std::string, int> mIntParamters;
    std::map<std::string, std::string> mStringParameters;
    std::map<std::string, bool> mBoolParameters;
};

typedef std::shared_ptr<SingleAlgorithmParameters> SingleAlgorithmParametersPtr;
typedef std::shared_ptr<const SingleAlgorithmParameters> ConstSingleAlgorithmParametersPtr;

class EPT_EXTERN AlgorithmParameters
{
public:
    SingleAlgorithmParametersPtr getPreparedParameters(const std::string &id) const;
    SingleAlgorithmParametersPtr getOrCreate(const std::string &id) const;

    std::vector<SingleAlgorithmParametersPtr> &getParameters() {return mParameters;}
    const std::vector<SingleAlgorithmParametersPtr> &getParameters() const {return mParameters;}
private:
    // mutable, because getOrCreate may add parameters
    mutable std::vector<SingleAlgorithmParametersPtr> mParameters;
};


#endif // ALGORITHMPARAMETERS_H
