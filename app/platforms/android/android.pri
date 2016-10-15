
DISTFILES += \
    $$EPT_ANDROID_PACKAGE_SOURCE_DIR/AndroidManifest.xml \
    $$EPT_ANDROID_PACKAGE_SOURCE_DIR/src/org/uniwue/tp3/TunerApplication.java \

HEADERS += \
    $$PWD/androidplatformtools.h \
    $$PWD/androidnativewrapper.h

SOURCES += \
    $$PWD/androidplatformtools.cpp \
    $$PWD/androidnativewrapper.cpp

# location of the android package files
ANDROID_PACKAGE_SOURCE_DIR = $$EPT_ANDROID_PACKAGE_SOURCE_DIR
