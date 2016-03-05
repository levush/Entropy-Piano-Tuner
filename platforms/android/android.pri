
DISTFILES += \
    $$EPT_ANDROID_PACKAGE_SOURCE_DIR/build.gradle \
    $$EPT_ANDROID_PACKAGE_SOURCE_DIR/settings.gradle \
    $$EPT_ANDROID_PACKAGE_SOURCE_DIR/gradle/wrapper/gradle-wrapper.jar \
    $$EPT_ANDROID_PACKAGE_SOURCE_DIR/gradle/wrapper/gradle-wrapper.properties \
    $$EPT_ANDROID_PACKAGE_SOURCE_DIR/gradlew \
    $$EPT_ANDROID_PACKAGE_SOURCE_DIR/gradlew.bat \
    $$EPT_ANDROID_PACKAGE_SOURCE_DIR/AndroidManifest.xml \
    $$EPT_ANDROID_PACKAGE_SOURCE_DIR/res/values/libs.xml \
    $$EPT_ANDROID_PACKAGE_SOURCE_DIR/build.gradle \
    $$EPT_ANDROID_PACKAGE_SOURCE_DIR/src/org/uniwue/tp3/TunerApplication.java \
    $$EPT_ANDROID_PACKAGE_SOURCE_DIR/src/org/uniwue/tp3/UsbMidiDriverAdapter.java \

HEADERS += \
    $$PWD/androidplatformtools.h \
    $$PWD/androidnativewrapper.h

SOURCES += \
    $$PWD/androidplatformtools.cpp \
    $$PWD/androidnativewrapper.cpp

# location of the android package files
ANDROID_PACKAGE_SOURCE_DIR = $$EPT_ANDROID_PACKAGE_SOURCE_DIR
