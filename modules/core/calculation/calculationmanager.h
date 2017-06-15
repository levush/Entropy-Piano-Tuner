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

//=============================================================================
//                          Calculation manager
//=============================================================================

#ifndef CALCULATIONMANAGER_H
#define CALCULATIONMANAGER_H

#include <memory>
#include <vector>

#include "prerequisites.h"
#include "core/system/sharedlibrary.h"
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
///
/// The calculation manager is a singleton class.
///////////////////////////////////////////////////////////////////////////////

class EPT_EXTERN CalculationManager
{
private:

private:
    CalculationManager();                       // Constructor

    void loadAlgorithms(const std::vector<std::string> &algorithmsDirs);
    void unloadAllAlgorithms();

public:
    virtual ~CalculationManager();              // Destructor

    static std::unique_ptr<CalculationManager> &getSingletonPtr();
    static CalculationManager &getSingleton();
    static void selfDelete();      // Delete the calculation manager

    void loadAlgorithms();

    void start(const Piano &piano);
    void stop();

    void registerFactory(const std::string &name, AlgorithmFactoryBase* factory);

    const std::map<std::string, AlgorithmFactoryBase*> &getAlgorithms() const {return mAlgorithms;}
    std::shared_ptr<const AlgorithmInformation> loadAlgorithmInformation(const std::string &algorithmName) const;
    bool hasAlgorithm(const std::string &id) const;
    std::string getDefaultAlgorithmId() const;
    const AlgorithmFactoryDescription &getAlgorithmFactoryDescriptionById(const std::string &id) const;

    const std::string &getCurrentAlgorithmId() {return getCurrentAlgorithmInformation()->getId();}
    const std::shared_ptr<const AlgorithmInformation> getCurrentAlgorithmInformation();
    void setCurrentAlgorithmInformation(const std::shared_ptr<const AlgorithmInformation> ai) {mCurrentAlgorithmInformation = ai;}
    void setCurrentAlgorithmInformationById(const std::string &algorithmName) {mCurrentAlgorithmInformation = loadAlgorithmInformation(algorithmName);}

private:
#ifdef EPT_SHARED_ALGORITHMS
    std::vector<SharedLibraryPtr> mLoadedAlgorithmLibraries;
#endif
    std::map<std::string, AlgorithmFactoryBase*> mAlgorithms;
    std::unique_ptr<Algorithm> mCurrentAlgorithm;
    std::shared_ptr<const AlgorithmInformation> mCurrentAlgorithmInformation;  ///< The current algorithm to use
};

#endif // CALCULATIONMANAGER_H
