#ifndef ALGORITHMPLUGIN_H
#define ALGORITHMPLUGIN_H

#include "core/config.h"

#include "algorithmfactorydescription.h"

class AlgorithmFactoryBase;

// Define the API version
// This value shall be incremented whenever there are API breaking changes.
#define ALGORITHM_PLUGIN_API_VERSION 1

#ifdef WIN32
#  define ALGORITHM_PLUGIN_EXPORT __declspec(dllexport)
#else
#  define ALGORITHM_PLUGIN_EXPORT  // empty
#endif


// Define a type for the static function pointer.
EPT_EXTERN typedef AlgorithmFactoryBase* (*GetAlgorithmFactoryFunc)();

// Algorithm plugin details structure that is exposed to the application
struct AlgorithmPluginDetails {
  int apiVersion;
  const char* fileName;
  const char* className;
  const char* pluginName;
  const char* pluginVersion;
  GetAlgorithmFactoryFunc factoryFunc;
};

#define ALGORITHM_PLUGIN(classType, algorithmFactoryDescription, pluginVersion)        \
    extern "C" {                                                      \
        ALGORITHM_PLUGIN_EXPORT AlgorithmFactoryBase *getFactory()    \
        {                                                             \
            static classType mSingleton(algorithmFactoryDescription); \
            return &mSingleton;                                       \
        }                                                             \
        ALGORITHM_PLUGIN_EXPORT AlgorithmPluginDetails exports =      \
        {                                                             \
            ALGORITHM_PLUGIN_API_VERSION,                             \
            __FILE__,                                                 \
            #classType,                                               \
            algorithmFactoryDescription.getAlgorithmName().c_str(),   \
            pluginVersion,                                            \
            getFactory                                                \
        };                                                            \
    }



#endif // ALGORITHMPLUGIN_H
