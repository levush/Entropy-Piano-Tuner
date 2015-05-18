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

#ifndef ALGORITHMFACTORY_H
#define ALGORITHMFACTORY_H

#include <string>
#include <memory>

#include "algorithmfactorydescription.h"

class Algorithm;
class Piano;

////////////////////////////////////////////////////////////////////////
/// \brief The AlgorithmFactoryBase class create the desired Algorithm
///
/// It provides a pure virtual fuction for creating an Algorithm.
/// Also it stores the name of the Algorithm
////////////////////////////////////////////////////////////////////////

class AlgorithmFactoryBase
{
protected:
    AlgorithmFactoryBase(const AlgorithmFactoryDescription &description);

public:
    virtual std::unique_ptr<Algorithm> createAlgorithm(const Piano &piano) = 0;

    AlgorithmFactoryDescription &getDescription() {return mDescription;}

protected:
    void loadDefaultParams();

protected:
    AlgorithmFactoryDescription mDescription;
};


////////////////////////////////////////////////////////////////////////
/// \brief The AlgorithmFactory class is a template class to be created
///        with the actual Algorithm
///
/// It overwrites the createAlgorithm method of AlgorithmFactoryBase
/// for creating the AlgorithmClass template argument.
////////////////////////////////////////////////////////////////////////

template <typename AlgorithmClass>
class AlgorithmFactory : public AlgorithmFactoryBase
{
private:
    static const AlgorithmFactory mSingleton;

protected:
    AlgorithmFactory(const AlgorithmFactoryDescription &description)
        : AlgorithmFactoryBase(description)
    {
    }

    virtual std::unique_ptr<Algorithm> createAlgorithm(const Piano &piano) {
        // load all default params, before creating the algorithm
        loadDefaultParams();
        // create the alogrithm with all required params set
        return std::unique_ptr<Algorithm>(new AlgorithmClass(piano, mDescription));
    }
};

#endif // ALGORITHMFACTORY_H
