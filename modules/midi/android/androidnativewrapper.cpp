#include "androidnativewrapper.h"
#include "midiprerequisites.h"
#include "androidmidimanager.h"

#include <jni.h>
#include <android/log.h>
#include <sstream>

namespace midi {

void java_sendMidiMessage(JNIEnv *env, jobject thiz, jint eventId, jint byte1, jint byte2, jdouble deltaTime) {
    MIDI_UNUSED(env);
    MIDI_UNUSED(thiz);
}

void java_availableInputDevicesChanged(JNIEnv *env, jobject thiz) {
    MIDI_UNUSED(env);
    MIDI_UNUSED(thiz);
}

void java_inputConnectionStatusChanged(JNIEnv *env, jobject thiz, jstring deviceName) {
    MIDI_UNUSED(thiz);
}

void java_availableOutputDevicesChanged(JNIEnv *env, jobject thiz) {
    MIDI_UNUSED(env);
    MIDI_UNUSED(thiz);
}

void java_outputConnectionStatusChanged(JNIEnv *env, jobject thiz, jstring deviceName) {
    MIDI_UNUSED(thiz);
}

static JavaVM* s_javaVM = 0;
static jclass g_midiClass = 0;
static jmethodID g_methodUsbMidiDriverAdapterInstance = 0;

static JNINativeMethod jniMidiNativeMethods[] = {
    {"java_sendMidiMessage", "(IIID)V", (void *)java_sendMidiMessage},
    {"java_availableInputDevicesChanged", "()V", (void *)java_availableInputDevicesChanged},
    {"java_inputConnectionStatusChanged", "(Ljava/lang/String;)V", (void *)java_inputConnectionStatusChanged},
    {"java_availableOutputDevicesChanged", "()V", (void *)java_availableOutputDevicesChanged},
    {"java_outputConnectionStatusChanged", "(Ljava/lang/String;)V", (void *)java_outputConnectionStatusChanged}
};

JNIEXPORT void JNICALL initAndroidManagerJNI(jobject *usbmanager) {
    __android_log_print(ANDROID_LOG_VERBOSE, "MIDI", "initAndroidManagerJNI: Attaching");
    JNIEnv* env;
    s_javaVM->AttachCurrentThread(&env, NULL);

    __android_log_print(ANDROID_LOG_DEBUG, "MIDI", "initAndroidManagerJNI: CallStaticObjectMethod");
    jobject usbMidiManager = env->CallStaticObjectMethod(g_midiClass, g_methodUsbMidiDriverAdapterInstance);
    if (env->ExceptionCheck()) {
        return;
    }

    __android_log_print(ANDROID_LOG_DEBUG, "MIDI", "initAndroidManagerJNI: NewGlobalRef");
    // init midi manager
    *usbmanager = env->NewGlobalRef(usbMidiManager);
}

JNIEXPORT void JNICALL releaseAndroidManagerJNI(jobject usbmanager) {
    JNIEnv* env;
    s_javaVM->AttachCurrentThread(&env, NULL);

    if (usbmanager) {
        env->DeleteLocalRef(usbmanager);
    }
}

std::vector<std::string> android_listAvailableInputDevices(jobject usbmanager) {
    JNIEnv* env;
    s_javaVM->AttachCurrentThread(&env, NULL);

    jmethodID method = env->GetMethodID(g_midiClass, "getInputDeviceNames", "()Ljava/lang/String;");
    jstring names = static_cast<jstring>(env->CallObjectMethod(usbmanager, method));
    const char* raw = env->GetStringUTFChars(names, 0);
    std::istringstream namesfill(raw);
    env->ReleaseStringUTFChars(names, raw);

    // split namesfill
    std::vector<std::string> res;
    std::string temp;
    while (std::getline(namesfill, temp)) {
        res.push_back(temp);
    }

    return res;
}

}  // namespace midi

// this method is called immediately after the module is load
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    __android_log_print(ANDROID_LOG_VERBOSE, "MIDI", "Initializing JNI for MIDI");
    midi::s_javaVM = vm;

    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        __android_log_print(ANDROID_LOG_ERROR, "MIDI", "Failed to get JNI for MIDI");
        return -1;
    }

    __android_log_print(ANDROID_LOG_VERBOSE, "MIDI", "Searching for UsbMidiDriver");

    // search for UsbMidiDriver class
    jclass midiClass = env->FindClass("org/modules/midi/UsbMidiDriverAdapter");
    if (!midiClass) {
        __android_log_print(ANDROID_LOG_ERROR, "MIDI", "Failed to get usb midi driver adapter for MIDI");
        return -1;
    }

    midi::g_midiClass = static_cast<jclass>(env->NewGlobalRef(midiClass));

    midi::g_methodUsbMidiDriverAdapterInstance =
                env->GetStaticMethodID(midi::g_midiClass, "instance", "()Lorg/modules/midi/UsbMidiDriverAdapter;");


    __android_log_print(ANDROID_LOG_VERBOSE, "MIDI", "RegisterNatives");

    // register our native methods
    if (env->RegisterNatives(midiClass, midi::jniMidiNativeMethods, sizeof(midi::jniMidiNativeMethods) / sizeof(midi::jniMidiNativeMethods[0])) < 0)
    {
        __android_log_print(ANDROID_LOG_ERROR, "MIDI", "Failed to register natives JNI for MIDI");
        return -1;
    }

    __android_log_print(ANDROID_LOG_VERBOSE, "MIDI", "Connected to JNI");

    return JNI_VERSION_1_6;
}

