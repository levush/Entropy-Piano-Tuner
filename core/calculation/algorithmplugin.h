#ifndef ALGORITHMPLUGIN_H
#define ALGORITHMPLUGIN_H

#include "core/config.h"

#include "algorithmfactorydescription.h"
#include "core/calculation/algorithm.h"
#include "core/calculation/algorithmfactory.h"
#include "core/messages/messagelistener.h"


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
  AlgorithmFactoryDescription description;
  GetAlgorithmFactoryFunc factoryFunc;
};

#if EPT_ALL_STATIC

// we need QResource for Q_INIT_RESOURCE
#  include <QResource>
#  define ALGORITHM_INIT_RESOURCE_FUNC(algorithmName)                   \
    inline void initResource##algorithmName() {                         \
        Q_INIT_RESOURCE(algorithmName);                                 \
    }                                                                   \

#  define ALGORITHM_INIT_FUNC_NAME(algorithmName)                       \
    initResource##algorithmName();                                      \


// empty plugin
#  define ALGORITHM_CPP_START(algorithmName)                          \
    namespace algorithmName {

#  define ALGORITHM_CPP_END }

#else
#  define ALGORITHM_INIT_RESOURCE_FUNC(algorithmName)
#  define ALGORITHM_INIT_FUNC_NAME(algorithmName)

#  define ALGORITHM_CPP_START(algorithmName)                          \
    extern "C" {                                                      \
        ALGORITHM_PLUGIN_EXPORT AlgorithmFactoryBase *getFactory()    \
        {                                                             \
            static algorithmName::Factory mSingleton(algorithmName::getFactoryDescription());       \
            return &mSingleton;                                       \
        }                                                             \
        ALGORITHM_PLUGIN_EXPORT AlgorithmPluginDetails exports =      \
        {                                                             \
            ALGORITHM_PLUGIN_API_VERSION,                             \
            __FILE__,                                                 \
            #algorithmName,                                           \
            algorithmName::getFactoryDescription(),                   \
            getFactory                                                \
        };                                                            \
    }                                                                 \
    namespace algorithmName {

#  define ALGORITHM_CPP_END }
#endif

#define ALGORITHM_H_START(algorithmName, algorithmVersion)              \
    ALGORITHM_INIT_RESOURCE_FUNC(algorithmName)                         \
    namespace algorithmName {                                           \
                                                                        \
    inline AlgorithmFactoryDescription getFactoryDescription() {        \
        return AlgorithmFactoryDescription(#algorithmName, algorithmVersion);\
    }                                                                   \
                                                                        \
    inline AlgorithmFactoryDescription getInitFactoryDescription() {    \
        ALGORITHM_INIT_FUNC_NAME(algorithmName)                         \
        return getFactoryDescription();                                 \
    }                                                                   \

#define ALGORITHM_H_END(Class)                                          \
    typedef AlgorithmFactory<Class> Factory; }



#endif // ALGORITHMPLUGIN_H
