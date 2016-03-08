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

#include "calculationmanager.h"

#include <iostream>
#include <cmath>

#ifdef EPT_SHARED_ALGORITHMS
#include <dirent.h>
#endif

#include "../config.h"
#include "../system/log.h"
#include "../system/timer.h"
#include "../messages/messagehandler.h"
#include "../messages/messagecaluclationprogress.h"
#include "../messages/messagechangetuningcurve.h"
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
    unloadAllAlgorithms();
}


CalculationManager &CalculationManager::getSingleton()
{
    static CalculationManager THE_ONE_AND_ONLY;
    return THE_ONE_AND_ONLY;
}

void CalculationManager::loadAlgorithms()
{
    std::vector<std::string> search_dirs = {
                                                "algorithms",
                                                "../algorithms",
                                                "../../algorithms"
                                            };
#ifdef __unix
    // add system directories on unix systems
    search_dirs.push_back("/usr/lib/entropypianotuner/algorithms");
    search_dirs.push_back("~/.entropypianotuner/algorithms");
#endif
    loadAlgorithms(search_dirs);

}

void CalculationManager::loadAlgorithms(const std::vector<std::string> &algorithmsDirs)
{
#ifdef EPT_NO_SHARED_ALGORITHMS
    (void)algorithmsDirs;  // empty function
#else
    auto has_suffix= [](const std::string &str, const std::string &suffix)
    {
        return str.size() >= suffix.size() &&
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    };

    for (auto algorithmsDir : algorithmsDirs) {
        DIR *dir = opendir(algorithmsDir.c_str());
        struct dirent *ent;
        if (!dir) {
            LogI("Algorithm plugins directory at '%s' could not be opened.", algorithmsDir.c_str());
            continue;
        } else {
            LogI("Loading algorithms from '%s'", algorithmsDir.c_str());
        }
        while ((ent = readdir(dir)) != nullptr) {
            const std::string filename = algorithmsDir + "/" + std::string(ent->d_name);
            if (has_suffix(filename, ".so") || has_suffix(filename, ".dll") || has_suffix(filename, ".dylib")) {
                LogI("Reading algorithm %s", filename.c_str());

                SharedLibraryPtr lib(new SharedLibrary(filename));
                if (lib->is_open()) {

                    // Get plugin descriptor and exports
                    AlgorithmPluginDetails* info;
                    if (!lib->sym("exports", reinterpret_cast<void**>(&info))) {
                        LogW("Symbol lookup error.");
                        continue;
                    }

                    // loaded info
                    LogI("Algorithm Info: API Version = %i, "
                         "File Name = %s, "
                         "Class Name = %s, "
                         "Plugin Name = %s, "
                         "Plugin Version = %s ",
                         info->apiVersion,
                         info->fileName,
                         info->className,
                         info->description.getAlgorithmName().c_str(),
                         info->description.getVersion().c_str());

                    // API Version checking
                    if (info->apiVersion != ALGORITHM_PLUGIN_API_VERSION) {
                        LogW("Plugin ABI version mismatch. Expected %i, got %i.",
                             ALGORITHM_PLUGIN_API_VERSION, info->apiVersion);
                        continue;
                    }

                    // Instantiate the plugin
                    AlgorithmFactoryBase *factory = reinterpret_cast<AlgorithmFactoryBase*>(info->factoryFunc());


                    // store lib
                    mLoadedAlgorithmLibraries.push_back(lib);
                    registerFactory(info->description.getAlgorithmName(), factory);
                } else {
                    LogW("Algorithm could not be added.");
                }
            }
        }

        closedir(dir);
    }
#endif
}

void CalculationManager::unloadAllAlgorithms() {
#ifdef EPT_SHARED_ALGORITHMS
    mLoadedAlgorithmLibraries.clear();
#endif
}

//-----------------------------------------------------------------------------
//                           Start calculation
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
/// \brief Start the calculation thread. By calling this function, the current
/// piano is passed by reference and copied to the local mPiano member variable.
/// \param piano : Reference to the piano instance.
///////////////////////////////////////////////////////////////////////////////

void CalculationManager::start(const Piano &piano)
{
    // stop the old algorithm to be sure
    stop();

    // create and start new algorithm
    mCurrentAlgorithm = std::move(mAlgorithms[getCurrentAlgorithmInformation()->getId()]->createAlgorithm(piano));
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

std::shared_ptr<const AlgorithmInformation> CalculationManager::loadAlgorithmInformation(const std::string &algorithmName) const
{
    // open the xml file for this algorithm and return the information in the current language
    AlgorithmInformationParser parser;
    return parser.parse(algorithmName);
}

bool CalculationManager::hasAlgorithm(const std::string &id) const {
    return mAlgorithms.count(id) == 1;
}

std::string CalculationManager::getDefaultAlgorithmId() const {
    EptAssert(hasAlgorithm("entropyminimizer"), "Default algorithm does not exits.");
    return "entropyminimizer";
}

const std::shared_ptr<const AlgorithmInformation> CalculationManager::getCurrentAlgorithmInformation() {
    if (!mCurrentAlgorithmInformation) {
        // load default
        LogI("Loading default algorithm information");
        mCurrentAlgorithmInformation = loadAlgorithmInformation(getDefaultAlgorithmId());
    }
    return mCurrentAlgorithmInformation;
}
