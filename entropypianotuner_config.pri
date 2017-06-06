# Include guard
isEmpty(EPT_CONFIG_INCLUDED):EPT_CONFIG_INCLUDED=true
else:return()


# tests on desktop platforms
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
