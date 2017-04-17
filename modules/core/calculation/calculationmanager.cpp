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

#include <array>
#include <iostream>
#include <cmath>
#include <regex>

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

#if defined(_WIN32)
#include <windows.h>
#elif defined(__unix)
#include <libgen.h>
#include <unistd.h>
#endif

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

std::unique_ptr<CalculationManager> &CalculationManager::getSingletonPtr()
{
    static std::unique_ptr<CalculationManager> mInstance;
    return mInstance;
}

CalculationManager &CalculationManager::getSingleton()
{
    if (!getSingletonPtr()) {
        getSingletonPtr().reset(new CalculationManager);
    }

    return *getSingletonPtr();
}

void CalculationManager::selfDelete() {
    getSingletonPtr().reset();
}

void CalculationManager::loadAlgorithms()
{
    std::vector<std::string> search_dirs = {
                                                "algorithms",
                                                "../algorithms"
                                            };

#if defined(EPT_ALGORITHMS_DIR)
    // add ept algorithms install dir
    search_dirs.push_back(EPT_ALGORITHMS_DIR);
#endif
#if defined(_WIN32)
#   if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
        // add current executable dir (required when starting with wrong working dir)
        char filename[MAX_PATH];
        if (GetModuleFileNameA(NULL, filename, MAX_PATH)) {
            // remove name of exe to get dir
            std::string dir = filename;
            dir = dir.substr(0, dir.rfind('\\'));
            search_dirs.push_back(dir + "\\algorithms");
        } else {
            LogW("Executable dir could not be added to the algorithm search path: Error in GetModuleFileName");
        }
#   endif
#elif defined(__unix)
    // add system directories on unix systems
    search_dirs.push_back("/usr/lib/entropypianotuner/algorithms");
    search_dirs.push_back("/usr/lib64/entropypianotuner/algorithms");
    search_dirs.push_back("~/.entropypianotuner/algorithms");


    // add current executable dir (required when starting with wrong working dir)
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    const char *path;
    if (count != -1) {
        path = dirname(result);
    }
    search_dirs.push_back(std::string(path) + "/algorithms");
#endif
    loadAlgorithms(search_dirs);

}

void CalculationManager::loadAlgorithms(const std::vector<std::string> &algorithmsDirs)
{
#ifdef EPT_NO_SHARED_ALGORITHMS
    (void)algorithmsDirs;  // empty function
#else

    auto is_library_ending = [](const std::string &str, const std::string &type) {
        const size_t pos = str.find(type);
        // file does not contain file ending
        if (pos == std::string::npos) {return false;}

        // library ends without version (not required)
        if (str.size() - pos == type.size()) {return true;}

        // check if end matches a version
        const std::string ending = str.substr(pos);
        const std::string regex_str = type + "(\\.[0-9]+){1,3}";
        std::regex reg(regex_str);
        std::smatch pieces_match;

        return std::regex_match(ending, pieces_match, reg);
    };

    auto is_library_type = [is_library_ending](const std::string &str, const std::string &suffix) {
        if (is_library_ending(str, suffix)) {return true;}

        return false;
    };

    auto is_library = [is_library_type](const std::string &str) {
        return is_library_type(str, ".so") || is_library_type(str, ".dll") || is_library_type(str, ".dylib");
    };

    auto parse_version = [](const std::string &str, int &major, int &minor, int &patch) {
        try {
            std::array<int *, 3> v_out = {{&major, &minor, &patch}};

            const std::string regex_str = "(([0-9]+)\\.)?(([0-9]+)\\.)?([0-9]+)";
            std::regex reg(regex_str);
            std::smatch pieces_match;

            EptAssert(std::regex_match(str, pieces_match, reg), "RegEx must match.");

            int out_index = 0;
            for (size_t i = 1; i < pieces_match.size(); ++i) {
                std::string m = pieces_match[i];
                if (m.find(".") != std::string::npos) {continue;}

                *v_out[out_index] = std::atoi(m.c_str());
                ++out_index;
            }
        } catch (const std::regex_error &e) {
            LogE("Regex Error (code %d): %s", e.code(), e.what());
        }
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
            if (is_library(filename)) {
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

                    int cmajor = 0, cminor = 0, cpatch = 0;
                    int nmajor = 0, nminor = 0, npatch = 0;
                    // check if algorithms already exists
                    if (hasAlgorithm(info->description.getAlgorithmName())) {
                        // check version
                        parse_version(info->description.getVersion(), nmajor, nminor, npatch);
                        parse_version(getAlgorithmFactoryDescriptionById(info->description.getAlgorithmName()).getVersion(), cmajor, cminor, cpatch);

                        if (nmajor > cmajor || (nmajor == cmajor && nminor > cminor) || (nmajor == cmajor && nminor == cminor && npatch > cpatch)) {
                            LogI("Overwriting algorithm with newer version");
                        } else {
                            LogI("Algorithm already present, skipping.");
                            continue;
                        }
                    }

                    // Instantiate the plugin
                    // Upon first call the AlgorithmFactory will register itself in the constructor
                    // to the CalculationManager
                    AlgorithmFactoryBase *factory = reinterpret_cast<AlgorithmFactoryBase*>(info->factoryFunc());

                    // store lib
                    this->registerFactory(factory->getDescription().getAlgorithmName(), factory);
                    mLoadedAlgorithmLibraries.push_back(lib);
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
    mCurrentAlgorithm = mAlgorithms[getCurrentAlgorithmInformation()->getId()]->createAlgorithm(piano);
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

    mAlgorithms[name] = factory;
}

std::shared_ptr<const AlgorithmInformation> CalculationManager::loadAlgorithmInformation(const std::string &algorithmName) const
{
    // open the xml file for this algorithm and return the information in the current language
    AlgorithmInformationParser parser;
    return parser.parse(getAlgorithmFactoryDescriptionById(algorithmName));
}

bool CalculationManager::hasAlgorithm(const std::string &id) const {
    return mAlgorithms.count(id) == 1;
}

std::string CalculationManager::getDefaultAlgorithmId() const {
    EptAssert(hasAlgorithm("entropyminimizer"), "Default algorithm does not exits.");
    return "entropyminimizer";
}

const AlgorithmFactoryDescription &CalculationManager::getAlgorithmFactoryDescriptionById(const std::string &id) const {
    EptAssert(hasAlgorithm(id), "Algorithm does not exist");
    return mAlgorithms.at(id)->getDescription();
}

const std::shared_ptr<const AlgorithmInformation> CalculationManager::getCurrentAlgorithmInformation() {
    if (!mCurrentAlgorithmInformation) {
        // load default
        LogI("Loading default algorithm information");
        mCurrentAlgorithmInformation = loadAlgorithmInformation(getDefaultAlgorithmId());
    }
    return mCurrentAlgorithmInformation;
}
