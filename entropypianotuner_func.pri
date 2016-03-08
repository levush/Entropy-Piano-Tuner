include($$PWD/entropypianotuner_config.pri)
include($$PWD/thirdparty/fftw3/fftw3_export.pri)

defineReplace(declareStaticLibrary) {
    libname = $$1

    TEMPLATE = lib
    CONFIG += staticlib c++11 noqt
    CONFIG -= qt
    DESTDIR = $$EPT_THIRDPARTY_OUT_DIR

    !isEmpty(libname) {
        SOURCES += $${libname}.cpp
        HEADERS += $${libname}.h
    }

    export(TEMPLATE)
    export(CONFIG)
    export(DESTDIR)
    export(SOURCES)
    export(HEADERS)

    return(true)
}

defineReplace(declareSharedLibrary) {
    libname = $$1

    TEMPLATE = lib

    # static or shared lib
    contains(EPT_CONFIG, allstatic) {
        CONFIG += staticlib
    } else {
        CONFIG += dll
    }

    CONFIG += c++11 noqt
    CONFIG -= qt

    DESTDIR = $$EPT_THIRDPARTY_OUT_DIR

    !isEmpty(libname) {
        SOURCES += $${libname}.cpp
        HEADERS += $${libname}.h
    }

    export(TEMPLATE)
    export(CONFIG)
    export(DESTDIR)
    export(SOURCES)
    export(HEADERS)

    return(true)
}

defineReplace(depends_dirent) {
    dirent {
        INCLUDEPATH += $$EPT_THIRDPARTY_DIR/dirent/dirent
    }

    export(INCLUDEPATH)

    return(true)
}

defineReplace(depends_fftw3) {
    fftw3 {
        !contains(EPT_THIRDPARTY_CONFIG, system_fftw3) {
            INCLUDEPATH += $$EPT_THIRDPARTY_DIR/fftw3/fftw3/api
        }

        !contains(EPT_CONFIG, allstatic) {
            win32 {
                DLLS += $$EPT_THIRDPARTY_OUT_DIR/fftw3.dll
            }
        }

        LIBS += $$FFTW_LIB_PATH $$FFTW_EXTERN_LIBS

        android {
            ANDROID_EXTRA_LIBS += $$EPT_THIRDPARTY_OUT_DIR/libfftw3.so
        }
    }

    export(INCLUDEPATH)
    export(LIBS)
    export(DLLS)
    export(ANDROID_EXTRA_LIBS)

    return(true)
}

defineReplace(depends_getmemorysize) {
    getmemorysize {
        !contains(EPT_THIRDPARTY_CONFIG, system_getmemorysize) {
            INCLUDEPATH += $$EPT_THIRDPARTY_DIR/getmemorysize
        }

        LIBS += -lgetmemorysize
    }

    export(INCLUDEPATH)
    export(LIBS)

    return(true)
}

defineReplace(depends_libuv) {
    libuv {
        !contains(EPT_THIRDPARTY_CONFIG, system_libuv) {
            INCLUDEPATH += $$EPT_THIRDPARTY_DIR/libuv/libuv/include
        }

        LIBS += -luv

        linux:LIBS += -ldl
        win32|win32-g++:LIBS += -lws2_32 -lpsapi -liphlpapi -lshell32 -luserenv -lkernel32 -ladvapi32
    }

    export(LIBS)
    export(INCLUDEPATH)

    return(true)
}

defineReplace(depends_qwt) {
    qwt {
        !contains(EPT_THIRDPARTY_CONFIG, system_qwt) {
            INCLUDEPATH += $$EPT_THIRDPARTY_DIR/qwt
            LIBS += -lqwt
        } else {
            LIBS += -lqwt-qt5
        }

        DEFINES += QWT_DLL

        android {
            ANDROID_EXTRA_LIBS += $$EPT_THIRDPARTY_OUT_DIR/libqwt.so
        }
    }

    export(INCLUDEPATH)
    export(LIBS)
    export(DEFINES)
    export(ANDROID_EXTRA_LIBS)

    return(true)
}

defineReplace(depends_rtmidi) {
    rtmidi {
        !contains(EPT_THIRDPARTY_CONFIG, system_rtmidi) {
            INCLUDEPATH += $$EPT_THIRDPARTY_DIR/rtmidi
        }

        LIBS += -lRtMidi
    }

    export(INCLUDEPATH)
    export(LIBS)

    return(true)
}

defineReplace(depends_timesupport) {
    timesupport {
        !contains(EPT_THIRDPARTY_CONFIG, system_timesupport) {
            INCLUDEPATH += $$EPT_THIRDPARTY_DIR/timesupport
        }

        LIBS += -ltimesupport
    }

    export(INCLUDEPATH)
    export(LIBS)

    return(true)
}

defineReplace(depends_tinyxml2) {
    tinyxml2 {
        !contains(EPT_THIRDPARTY_CONFIG, system_tinyxml2) {
            INCLUDEPATH += $$EPT_THIRDPARTY_DIR/tinyxml2/tinyxml2
        }

        LIBS += -ltinyxml2
    }

    export(INCLUDEPATH)
    export(LIBS)

    return(true)
}
