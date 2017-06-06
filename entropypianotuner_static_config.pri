# Include guard
isEmpty(EPT_STATIC_CONFIG_INCLUDED):EPT_STATIC_CONFIG_INCLUDED=true
else:return()

#------------------------------------------------
# Global config

# include example algorithm by default in debug builds

# on mobile platforms and mac/ios  use static algorithm build
android|ios|winrt|winphone|macx: EPT_CONFIG += no_shared_algorithms static_algorithms static_core no_shared_core
else:                            EPT_CONFIG += shared_algorithms no_static_algorithms shared_core no_static_core


# add install rules
winrt|winphone: EPT_CONFIG += no_install
else: EPT_CONFIG += install

#------------------------------------------------
# Path defines

# root dir
EPT_ROOT_DIR = $$PWD

# Source files dirs
EPT_BASE_DIR = $$EPT_ROOT_DIR
EPT_APP_DIR = $$EPT_BASE_DIR/app
EPT_MODULES_DIR = $$EPT_ROOT_DIR/modules
EPT_CORE_DIR = $$EPT_MODULES_DIR/core
EPT_APPSTORE_DIR = $$EPT_ROOT_DIR/appstore
EPT_ALGORITHMS_DIR = $$EPT_MODULES_DIR/algorithms
EPT_THIRDPARTY_DIR = $$EPT_ROOT_DIR/thirdparty
EPT_DEPENDENCIES_DIR = $$EPT_ROOT_DIR/dependencies
EPT_TRANSLATIONS_DIR = $$EPT_BASE_DIR/translations
EPT_TUTORIAL_DIR = $$EPT_ROOT_DIR/tutorial
EPT_PLATFORMS_DIR = $$EPT_APP_DIR/platforms
EPT_ANDROID_PACKAGE_SOURCE_DIR = $$EPT_PLATFORMS_DIR/android

# Build output dirs
EPT_ROOT_OUT_DIR = $$shadowed($$PWD)

EPT_TARGET_OUT_DIR = $$EPT_ROOT_OUT_DIR/bin
winrt|winphone {
    # debug version requires debug dir for execution
    CONFIG(debug, debug|release) :EPT_TARGET_OUT_DIR = $$shadowed($$PWD)/app/debug
    else                         :EPT_TARGET_OUT_DIR = $$shadowed($$PWD)/app/release
}

EPT_THIRDPARTY_OUT_DIR = $$EPT_TARGET_OUT_DIR
EPT_CORE_OUT_DIR = $$EPT_TARGET_OUT_DIR
EPT_ALGORITHMS_OUT_DIR = $$EPT_TARGET_OUT_DIR/algorithms


# Install paths if not defines
isEmpty(EPT_INSTALL_BIN_RDIR):EPT_INSTALL_BIN_RDIR=bin
isEmpty(EPT_INSTALL_DATA_RDIR):EPT_INSTALL_DATA_RDIR=shared
isEmpty(EPT_INSTALL_LIB_RDIR):EPT_INSTALL_LIB_RDIR=lib

# Create paths in package (PKGDIR). Arch Linux requires install into the pkgdir
# but the rpath needs to be set to RDIR
isEmpty(EPT_INSTALL_BIN_DIR):EPT_INSTALL_BIN_DIR=$${PKGDIR}$${EPT_INSTALL_BIN_RDIR}
isEmpty(EPT_INSTALL_DATA_DIR):EPT_INSTALL_DATA_DIR=$${PKGDIR}$${EPT_INSTALL_DATA_RDIR}
isEmpty(EPT_INSTALL_LIB_DIR):EPT_INSTALL_LIB_DIR=$${PKGDIR}$${EPT_INSTALL_LIB_RDIR}

#------------------------------------------------
# third party modules

# select modules
CONFIG += fftw3

# add libuv when shared algorithms are enabled
contains(EPT_CONFIG, shared_algorithms) {
    CONFIG += libuv
}

winrt|winphone {
    CONFIG += winrtbridge
}
else:win32 {
    CONFIG += dirent
}

# tp3log
win32:contains(EPT_CONFIG, shared_core) {
    CONFIG += tp3LogDLL
    DEFINES += TP3LOG_DLL
}

#--------------------------------------------------
# global settings

# additional debug flags
linux-g++*:!android {
    QMAKE_CXXFLAGS_DEBUG += -D_GLIBCXX_DEBUG
}


# config
contains(EPT_CONFIG, no_shared_algorithms):DEFINES+="EPT_NO_SHARED_ALGORITHMS=1" "EPT_STATIC_ALGORITHMS=1"
contains(EPT_CONFIG, shared_algorithms):DEFINES+="EPT_SHARED_ALGORITHMS=1"

LIBS += -L$$EPT_THIRDPARTY_OUT_DIR

# set QWT_CONFIG for static/dynamic build
contains(EPT_CONFIG, static_qwt):QWT_CONFIG += QwtStatic
else:QWT_CONFIG += QwtDll

contains(EPT_CONFIG, static_core):DEFINES += EPT_STATIC_CORE
else:DEFINES += EPT_DYNAMIC_CORE
