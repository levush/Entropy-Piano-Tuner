
DISTFILES += \
    $$ANDROID_PACKAGE_SOURCE_DIR/build.gradle \
    $$ANDROID_PACKAGE_SOURCE_DIR/settings.gradle \
    $$ANDROID_PACKAGE_SOURCE_DIR/gradle/wrapper/gradle-wrapper.jar \
    $$ANDROID_PACKAGE_SOURCE_DIR/gradle/wrapper/gradle-wrapper.properties \
    $$ANDROID_PACKAGE_SOURCE_DIR/gradlew \
    $$ANDROID_PACKAGE_SOURCE_DIR/gradlew.bat \
    $$ANDROID_PACKAGE_SOURCE_DIR/AndroidManifest.xml \
    $$ANDROID_PACKAGE_SOURCE_DIR/res/values/libs.xml \
    $$ANDROID_PACKAGE_SOURCE_DIR/build.gradle \
    $$ANDROID_PACKAGE_SOURCE_DIR/src/org/uniwue/tp3/TunerApplication.java \
    $$ANDROID_PACKAGE_SOURCE_DIR/src/org/uniwue/tp3/UsbMidiDriverAdapter.java \

HEADERS += \
    $$PWD/androidplatformtools.h \
    $$PWD/androidnativewrapper.h

SOURCES += \
    $$PWD/androidplatformtools.cpp \
    $$PWD/androidnativewrapper.cpp
