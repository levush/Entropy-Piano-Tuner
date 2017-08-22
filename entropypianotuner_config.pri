# Include guard
isEmpty(EPT_CONFIG_INCLUDED):EPT_CONFIG_INCLUDED=true
else:return()

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

LIBS += -L$$EPT_THIRDPARTY_OUT_DIR

# tests on desktop platforms
#------------------------------------------------
win|linux:!android {
    # run tests
    load(configure)

    # use modules that are installed on the system
    qtCompileTest(qwt_exists) {
        EPT_THIRDPARTY_CONFIG+=system_qwt
    }
    qtCompileTest(qwt6-qt5_exists) {
        EPT_THIRDPARTY_CONFIG+=system_qwt
    }

    packagesExist(qwt):       EPT_THIRDPARTY_CONFIG+=system_qwt
    packagesExist(fftw3):     EPT_THIRDPARTY_CONFIG+=system_fftw3
    packagesExist(libuv):     EPT_THIRDPARTY_CONFIG+=system_libuv
}

include($$PWD/entropypianotuner_static_config.pri)
