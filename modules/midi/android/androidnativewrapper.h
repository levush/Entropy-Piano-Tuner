#ifndef ANDROIDNATIVEWRAPPER_H
#define ANDROIDNATIVEWRAPPER_H

#include <jni.h>

namespace midi {

void initAndroidManagerJNI(jobject *usbmanager);
void releaseAndroidManagerJNI(jobject usbmanager);

}  // namespace midi

#endif // ANDROIDNATIVEWRAPPER_H
