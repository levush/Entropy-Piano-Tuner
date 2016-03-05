include($$PWD//entropypianotuner_config.pri)

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

defineReplace(depends_fftw3) {
    fftw3 {
        !contains(EPT_THIRDPARTY_CONFIG, system_fftw3) {
            INCLUDEPATH += $$EPT_THIRDPARTY_DIR/fftw3/fftw3/api
        }

        LIBS += -lfftw
    }

    export(INCLUDEPATH)
    export(LIBS)

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
        linux {
            LIBS += -ldl
        }
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
    }

    export(INCLUDEPATH)
    export(LIBS)

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
