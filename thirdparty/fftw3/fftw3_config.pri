include(../../entropypianotuner_config.pri)

contains(EPT_CONFIG, static_fftw) {
    FFTW_LIB_MODE_CONFIG = staticlib
} else {
    FFTW_LIB_MODE_CONFIG = dll
    DEFINES += "FFT_DLL=1"
}

FFTW_DESTDIR = $$EPT_THIRDPARTY_OUT_DIR
FFTW_SUBPART_OUT = $$FFTW_DESTDIR/fftw_subparts
FFTW_ROOT = $$PWD/fftw3
FFTW_CONFIG_PATH = $$PWD

android {
    INCLUDEPATH += $$PWD/android
}
else:linux {
    INCLUDEPATH += $$PWD/linux
}
else:ios {
    INCLUDEPATH += $$PWD/ios
}
else:macx {
    INCLUDEPATH += $$PWD/osx
}
else:win {
    INCLUDEPATH += $$PWD/windows
}
