include(../../entropypianotuner_config.pri)

contains(EPT_CONFIG, allstatic) {
    FFTW_LIB_MODE_CONFIG = staticlib
} else {
    FFTW_LIB_MODE_CONFIG = dll
    DEFINES += "DLL_EXPORT=1"
    DEFINES += "FFT_DLL=1"
}

FFTW_DESTDIR = $$EPT_THIRDPARTY_OUT_DIR
FFTW_SUBPART_OUT = $$FFTW_DESTDIR/fftw_subparts
FFTW_ROOT = $$PWD/fftw3
FFTW_CONFIG_PATH = $$PWD

defineReplace(fftw3Core) {
    CONFIG += c++11 noqt
    CONFIG -= qt

     # include paths (complete library)
    INCLUDEPATH += $$FFTW_CONFIG_PATH
    INCLUDEPATH += $$FFTW_ROOT/api
    INCLUDEPATH += $$FFTW_ROOT/dft
    INCLUDEPATH += $$FFTW_ROOT/dft/scalar
    INCLUDEPATH += $$FFTW_ROOT/dft/simd
    INCLUDEPATH += $$FFTW_ROOT/kernel
    INCLUDEPATH += $$FFTW_ROOT/rdft
    INCLUDEPATH += $$FFTW_ROOT/rdft/scalar
    INCLUDEPATH += $$FFTW_ROOT/rdft/simd
    INCLUDEPATH += $$FFTW_ROOT/reodft
    INCLUDEPATH += $$FFTW_ROOT/simd-support

    LIBS += -L$$FFTW_SUBPART_OUT

    export(CONFIG)
    export(INCLUDEPATH)
    export(LIBS)

    return(true)
}

defineReplace(fftw3SubPart) {
    $$fftw3Core()

    TEMPLATE = lib
    CONFIG += staticlib c++11 noqt
    CONFIG -= qt
    DESTDIR = $$FFTW_SUBPART_OUT

    export(TEMPLATE)
    export(CONFIG)
    export(DESTDIR)
    export(INCLUDEPATH)
    export(LIBS)

    return(true)
}

SUBDIRS += \
    $$PWD/fftw3/dft/scalar/scalar/scalar.pro
