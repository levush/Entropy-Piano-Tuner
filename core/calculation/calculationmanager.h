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

//=============================================================================
//                          Calculation manager
//=============================================================================

#ifndef CALCULATIONMANAGER_H
#define CALCULATIONMANAGER_H

#include <memory>

#include "../system/simplethreadhandler.h"
#include "../piano/piano.h"
#include "algorithm.h"
#include "algorithminformation.h"


class AlgorithmFactoryBase;
class AlgorithmFactoryDescription;
class Algorithm;

///////////////////////////////////////////////////////////////////////////////
/// \brief Calculation-Manager
///
/// This class coordinates the calculation process. The calculation process
/// runs in an independent thread. By calling start, a local copy of the entire
/// piano is instantated. The calculation process manipulates the local copy
/// but it does not return it directly. Instead the whole communication with
/// the tuner is realized in terms of messages. In particular, each change of
/// the tuning curve is communicated by a message.
///////////////////////////////////////////////////////////////////////////////

class CalculationManager
{
private:
    CalculationManager();                       // Constructor

public:
    virtual ~CalculationManager();              // Destructor

    static CalculationManager &getSingleton();

    void start(const std::string &algorithmName, const Piano &piano);
    void stop();

    void registerFactory(const std::string &name, AlgorithmFactoryBase* factory);

    const std::map<std::string, AlgorithmFactoryBase*> &getAlgorithms() const {return mAlgorithms;}
    AlgorithmFactoryDescription &getAlgorithmDescription(const std::string &algorithmName) const;
    std::unique_ptr<const AlgorithmInformation> loadAlgorithmInformation(const std::string &algorithmName) const;
    bool hasAlgorithm(const std::string &id) const;
    std::string getDefaultAlgorithmId() const;

private:
    std::map<std::string, AlgorithmFactoryBase*> mAlgorithms;
    std::unique_ptr<Algorithm> mCurrentAlgorithm;
};

#endif // CALCULATIONMANAGER_H
