#ifndef ANDROIDNATIVEWRAPPER_H
#define ANDROIDNATIVEWRAPPER_H

#include <jni.h>
#include <vector>
#include <string>

namespace midi {

void initAndroidManagerJNI(jobject *usbmanager);
void releaseAndroidManagerJNI(jobject usbmanager);

std::vector<std::string> android_listAvailableInputDevices(jobject usbmanager);

}  // namespace midi

#endif // ANDROIDNATIVEWRAPPER_H
