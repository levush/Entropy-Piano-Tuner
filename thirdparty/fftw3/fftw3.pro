include(../../entropypianotuner_func.pri)
$$declareSharedLibrary()

TARGET = fftw

# configuration
# ----------------------------------------------------------

# some files have the same name, this will prevent naming
# conflicts when generating the object files
CONFIG += object_parallel_to_source

# set the target architecture
isEmpty(QMAKE_TARGET.arch) {
    android {
        QMAKE_TARGET.arch = $${ANDROID_TARGET_ARCH}
    } else {
        QMAKE_TARGET.arch = $${QMAKE_HOST.arch}
    }
}

# root path for fftw3 source
FFTW_ROOT = $$PWD/fftw3

# include paths

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


# sources that are relevant for all platforms

SOURCES += $$FFTW_ROOT/api/*.c
SOURCES += $$FFTW_ROOT/dft/*.c
SOURCES += $$FFTW_ROOT/dft/scalar/*.c
SOURCES += $$FFTW_ROOT/dft/scalar/codelets/*.c

SOURCES += $$FFTW_ROOT/kernel/*.c

SOURCES += $$FFTW_ROOT/rdft/*.c
SOURCES += $$FFTW_ROOT/rdft/scalar/*.c
SOURCES += $$FFTW_ROOT/rdft/scalar/r2cb/*.c
SOURCES += $$FFTW_ROOT/rdft/scalar/r2cf/*.c
SOURCES += $$FFTW_ROOT/rdft/scalar/r2r/*.c

SOURCES += \
    $$FFTW_ROOT/simd-support/taint.c \
    $$FFTW_ROOT/simd-support/avx.c \
    $$FFTW_ROOT/simd-support/altivec.c

SOURCES += $$FFTW_ROOT/reodft/*.c

# extra sources (optimizations) on x86/x86_64
contains(QMAKE_TARGET.arch, x86) |
contains(QMAKE_TARGET.arch, x86_64) {
    DEFINES += "HAVE_SSE2=1"

    SOURCES += $$FFTW_ROOT/simd-support/*.c
    SOURCES += $$FFTW_ROOT/dft/simd/sse2/*.c
    SOURCES += $$FFTW_ROOT/rdft/simd/sse2/*.c
}

# headers (not all decalred)
HEADERS += config.h
