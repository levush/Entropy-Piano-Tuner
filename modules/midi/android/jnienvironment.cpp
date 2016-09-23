#include "jnienvironment.h"
#include <android/log.h>
#include "androidnativewrapper.h"

namespace midi {

}  // namespace midi

// this method is called immediately after the module is load
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    __android_log_print(ANDROID_LOG_VERBOSE, "MIDI", "Initializing JNI for MIDI");
    midi::JNIEnvironment::instance().setVM(vm);

    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "MIDI", "Failed to get JNI for MIDI");
        return -1;
    }

    if (!midi::android_registerNatives(env)) {
        __android_log_print(ANDROID_LOG_ERROR, "MIDI", "Failed to register natives for MIDI");
        return -1;
    }

    return JNI_VERSION_1_6;
}


