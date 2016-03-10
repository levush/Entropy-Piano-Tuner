#------------------------------------------------
# Global config

CONFIG += c++11

# include example algorithm by default in debug builts

# on mobile platforms use static algorithm build
android|ios|winrt|winphone: EPT_CONFIG += no_shared_algorithms static_algorithms
else:                       EPT_CONFIG += shared_algorithms no_static_algorithms

# on linux build static core
linux:!android: EPT_CONFIG += static_core

#------------------------------------------------
# Path defines

EPT_ROOT_DIR = $$PWD

EPT_BASE_DIR = $$EPT_ROOT_DIR
EPT_CORE_DIR = $$EPT_BASE_DIR/core
EPT_APPSTORE_DIR = $$EPT_ROOT_DIR/appstore
EPT_ALGORITHMS_DIR = $$EPT_ROOT_DIR/algorithms
EPT_THIRDPARTY_DIR = $$EPT_ROOT_DIR/thirdparty
EPT_DEPENDENCIES_DIR = $$EPT_ROOT_DIR/dependencies
EPT_TRANSLATIONS_DIR = $$EPT_BASE_DIR/translations
EPT_TUTORIAL_DIR = $$EPT_ROOT_DIR/tutorial

EPT_THIRDPARTY_OUT_DIR = $$shadowed($$PWD)/lib
EPT_CORE_OUT_DIR = $$shadowed($$PWD)
EPT_ALGORITHMS_OUT_DIR = $$shadowed($$PWD)/algorithms

EPT_ANDROID_PACKAGE_SOURCE_DIR = $$EPT_ROOT_DIR/platforms/android

#------------------------------------------------
# third party modules
CONFIG += tinyxml2 qwt fftw3 getmemorysize

# add libuv when shared algorithms are enabled
contains(EPT_CONFIG, shared_algorithms) {
    CONFIG += libuv
}

winrt|winphone {
    CONFIG += timesupport
}
else:win32 {
    CONFIG += timesupport rtmidi dirent
}

linux:!android {
    CONFIG += getmemorysize rtmidi

    # on linux target use library from system
    EPT_THIRDPARTY_CONFIG += system_fftw3 system_qwt system_tinyxml2 system_libuv

    # there seems to be a bug in the RtMidi dependencies which is why
    # we compile this library on our own.
}

android {
    CONFIG += getmemorysize
}

macx {
    CONFIG += getmemorysize rtmidi
}

#--------------------------------------------------
# global settings

linux-g++*:!android {
    QMAKE_CXXFLAGS_DEBUG += -D_GLIBCXX_DEBUG
}


contains(EPT_CONFIG, no_shared_algorithms):DEFINES+="EPT_NO_SHARED_ALGORITHMS=1" "EPT_STATIC_ALGORITHMS=1"
contains(EPT_CONFIG, shared_algorithms):DEFINES+="EPT_SHARED_ALGORITHMS=1"

LIBS += -L$$EPT_THIRDPARTY_OUT_DIR
