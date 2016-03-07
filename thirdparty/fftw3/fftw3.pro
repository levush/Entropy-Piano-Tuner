include(../../entropypianotuner_func.pri)
include(fftw3_func.pri)
include(fftw3_config.pri)
include(fftw3_export.pri)

TEMPLATE = subdirs
SUBDIRS += fftw3

# set the target architecture
isEmpty(QMAKE_TARGET.arch) {
    android {
        QMAKE_TARGET.arch = $${ANDROID_TARGET_ARCH}
    } else {
        QMAKE_TARGET.arch = $${QMAKE_HOST.arch}
    }
}

#SOURCES += $$FFTW_ROOT/reodft/*.c

# extra sources (optimizations) on x86/x86_64
contains(QMAKE_TARGET.arch, x86) |
contains(QMAKE_TARGET.arch, x86_64) {
    DEFINES += "HAVE_SSE2=1"
}

# Global header
HEADERS += config.h
