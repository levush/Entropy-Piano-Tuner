#include "jniobject.h"
#include "jnienvironment.h"

namespace midi {

JNIObjectData::JNIObjectData(jobject local)
{
    JNIEnv *env;
    JNIEnvironment::env(&env);
    mGlobal = env->NewGlobalRef(local);
}

JNIObjectData::~JNIObjectData() {
    JNIEnv *env;
    JNIEnvironment::env(&env);
    env->DeleteGlobalRef(mGlobal);
}

JNIClassData::JNIClassData(jclass local) {
    JNIEnv *env;
    JNIEnvironment::env(&env);
    mGlobal = static_cast<jclass>(env->NewGlobalRef(local));
}

JNIClassData::~JNIClassData() {
    JNIEnv *env;
    JNIEnvironment::env(&env);
    env->DeleteGlobalRef(mGlobal);
}

}  // namespace midi
