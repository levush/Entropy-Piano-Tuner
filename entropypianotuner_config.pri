#------------------------------------------------
# Global config

# include example algorithm by default in debug builts
CONFIG(debug, debug|release) {
    EPT_CONFIG += include_example_algorithm
}

# on mobile platforms build all plugins/libs static
android|ios {
    message(Enabling all static build)
    EPT_CONFIG += allstatic
}

#------------------------------------------------
# Path defines

EPT_ROOT_DIR = $$PWD

EPT_BASE_DIR = $$EPT_ROOT_DIR
EPT_APPSTORE_DIR = $$EPT_ROOT_DIR/appstore
EPT_ALGORITHMS_DIR = $$EPT_ROOT_DIR/algorithms
EPT_THIRDPARTY_DIR = $$EPT_ROOT_DIR/thirdparty
EPT_DEPENDENCIES_DIR = $$EPT_ROOT_DIR/dependencies
EPT_TRANSLATIONS_DIR = $$EPT_BASE_DIR/translations
EPT_TUTORIAL_DIR = $$EPT_ROOT_DIR/tutorial

EPT_THIRDPARTY_OUT_DIR = $$shadowed($$PWD)/lib
EPT_CORE_OUT_DIR = $$shadowed($$PWD)
EPT_ALGORITHMS_OUT_DIR = $$shadowed($$PWD)/algorithms

#------------------------------------------------
# third party modules
CONFIG += tinyxml2 qwt libuv

win32 {
    CONFIG += timesupport memorysize rtmidi
}

linux {
    CONFIG += getmemorysize rtmidi
}

macx {
    CONFIG += getmemorysize rtmidi
}

#--------------------------------------------------
# global settings
CONFIG += c++11

linux-g++*:!android {
    QMAKE_CXXFLAGS_DEBUG += -D_GLIBCXX_DEBUG
}

contains(EPT_CONFIG, allstatic) {
    DEFINES += "EPT_ALL_STATIC=1"
}
