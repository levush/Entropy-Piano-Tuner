#ifndef JNIOBJECT_H
#define JNIOBJECT_H


#include <memory>
#include <jni.h>

namespace midi {

class JNIObjectData
{
public:
    JNIObjectData(jobject local);
    ~JNIObjectData();

    jobject &get() {return mGlobal;}

private:
    jobject mGlobal;
};

typedef std::shared_ptr<JNIObjectData> JNIObject;

class JNIClassData {
public:
    JNIClassData(jclass local);
    ~JNIClassData();

    jclass &get() {return mGlobal;}

private:
    jclass mGlobal;
};

typedef std::shared_ptr<JNIClassData> JNIClass;

}  // namespace midi

#endif // JNIOBJECT_H
