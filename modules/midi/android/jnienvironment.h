#ifndef JNIENVIRONMENT_H
#define JNIENVIRONMENT_H

#include <jni.h>

namespace midi {

class JNIEnvironment
{
public:
    static JNIEnvironment &instance() {
        static JNIEnvironment i;
        return i;
    }

    inline static void env(JNIEnv **env) {
        instance().mVM->AttachCurrentThread(env, 0);
    }

private:
    JNIEnvironment() {}

public:
    void setVM(JavaVM *vm){
        mVM = vm;
    }

private:
    JavaVM *mVM = nullptr;
};

}  // namespace midi

#endif // JNIENVIRONMENT_H
