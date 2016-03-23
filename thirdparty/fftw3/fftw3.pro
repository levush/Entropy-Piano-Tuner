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

# Global header depends on platform (do not forget to set include path in fftw3_config.pri)
android {
    HEADERS += android/config.h
}
else:linux {
    HEADERS += linux/config.h
}
else:ios {
    HEADERS += ios/config.h
}
else:macx {
    HEADERS += osx/config.h
}
else:win32 {
    HEADERS += windows/config.h
}
