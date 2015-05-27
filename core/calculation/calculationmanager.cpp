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

#include "calculationmanager.h"

#include <iostream>
#include <cmath>

#include "../config.h"
#include "../system/log.h"
#include "../system/timer.h"
#include "../messages/messagehandler.h"
#include "../messages/messagecaluclationprogress.h"
#include "../messages/messagechangetuningcurve.h"
#include "../adapters/filemanager.h"
#include "algorithmfactory.h"
#include "algorithminformationparser.h"

//-----------------------------------------------------------------------------
//                               Constructor
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Constructor, resets member variables
///////////////////////////////////////////////////////////////////////////////

CalculationManager::CalculationManager()
{
}

CalculationManager::~CalculationManager()
{
    stop();
}


CalculationManager &CalculationManager::getSingleton() {
    static CalculationManager THE_ONE_AND_ONLY;
    return THE_ONE_AND_ONLY;
}

//-----------------------------------------------------------------------------
//                           Start calculation
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Start the calculation thread. By calling this function, the current
/// piano is passed by reference and copied to the local mPiano member variable.
/// \param piano : Reference to the piano instance.
///////////////////////////////////////////////////////////////////////////////

void CalculationManager::start(const std::string &algorithmName, const Piano &piano)
{
    if (mAlgorithms.count(algorithmName) != 1) {
        EPT_EXCEPT(EptException::ERR_INVALIDPARAMS, "An algorithm with name '" + algorithmName + "' does not exist.");
    }

    // stop the old algorithm to be sure
    stop();

    // create and start new algorithm
    mCurrentAlgorithm = std::move(mAlgorithms[algorithmName]->createAlgorithm(piano));
    mCurrentAlgorithm->start();
}


//-----------------------------------------------------------------------------
//                             Stop calculation
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Stop the calculation thread.
///////////////////////////////////////////////////////////////////////////////

void CalculationManager::stop()
{
    if (mCurrentAlgorithm) {
        mCurrentAlgorithm->stop();
        mCurrentAlgorithm.reset();
    }
}

void CalculationManager::registerFactory(const std::string &name, AlgorithmFactoryBase* factory)
{
    if (mAlgorithms.count(name) == 1) {
        EPT_EXCEPT(EptException::ERR_DUPLICATE_ITEM, "An algorithm with name '" + name + "' already exists.");
    }

#if EPT_EXCLUDE_EXAMPLE_ALGORITHM
    // exclude example algorithm(s)
    if (name.find("example") != std::string::npos) {
        return;
    }
#endif

    mAlgorithms[name] = factory;
}

AlgorithmFactoryDescription &CalculationManager::getAlgorithmDescription(const std::string &algorithmName) const
{
    if (!hasAlgorithm(algorithmName)) {
        EPT_EXCEPT(EptException::ERR_DUPLICATE_ITEM, "An algorithm with name '" + algorithmName + "' does not exist.");
    }

    return mAlgorithms.at(algorithmName)->getDescription();
}

std::unique_ptr<const AlgorithmInformation> CalculationManager::loadAlgorithmInformation(const std::string &algorithmName) const
{
    // open the xml file for this algorithm and return the information in the current language
    AlgorithmInformationParser parser;
    return parser.parse(FileManager::getSingleton().getAlgorithmInformationFileContent(algorithmName));
}

bool CalculationManager::hasAlgorithm(const std::string &id) const {
    return mAlgorithms.count(id) == 1;
}

std::string CalculationManager::getDefaultAlgorithmId() const {
   EptAssert(hasAlgorithm("entropyminimizer"), "Default algorithm doesnt exist");
   return "entropyminimizer";
}
