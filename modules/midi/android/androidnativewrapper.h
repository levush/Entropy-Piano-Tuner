#ifndef ANDROIDNATIVEWRAPPER_H
#define ANDROIDNATIVEWRAPPER_H

#include <jni.h>
#include <vector>
#include <string>

#include "jniobject.h"
#include "midiprerequisites.h"
#include "midimanager.h"

namespace midi {

bool android_registerNatives(JNIEnv* env);

void initAndroidManagerJNI(JNIObject *usbmanager);
void releaseAndroidManagerJNI(JNIObject usbmanager);

std::vector<std::string> android_listAvailableInputDevices(JNIObject usbmanager);
MidiManager::MidiInDevRes android_createInputDevice(const std::string &id, JNIObject usbmanager);
MidiManager::MidiOutDevRes android_createOutputDevice(const std::string &id, JNIObject usbmanager);
MidiResult android_deleteInputDevice(const std::string &id, JNIObject usbmanager);
MidiResult android_deleteOutputDevice(const std::string &id, JNIObject usbmanager);

}  // namespace midi

#endif // ANDROIDNATIVEWRAPPER_H
