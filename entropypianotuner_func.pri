include($$PWD/entropypianotuner_config.pri)

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

    linux-g++*:!android {
        QMAKE_CXXFLAGS_DEBUG += -D_GLIBCXX_DEBUG -Wall -Werror -Wpedantic
    }

    export(TEMPLATE)
    export(CONFIG)
    export(DESTDIR)
    export(SOURCES)
    export(HEADERS)
    export(QMAKE_CXXFLAGS_DEBUG)

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

    linux-g++*:!android {
        QMAKE_CXXFLAGS_DEBUG += -D_GLIBCXX_DEBUG -Wall -Werror -Wpedantic
    }

    export(TEMPLATE)
    export(CONFIG)
    export(DESTDIR)
    export(SOURCES)
    export(HEADERS)
    export(QMAKE_CXXFLAGS_DEBUG)

    return(true)
}

defineReplace(depends_core) {
    contains(EPT_CONFIG, static_core) {
        $$depends_tp3log()
    } else {
        android:ANDROID_EXTRA_LIBS += $$EPT_CORE_OUT_DIR/libcore.so
        win32:DLLS += $$EPT_CORE_OUT_DIR/core.dll
        tp3LogDLL { $$depends_tp3log() }
    }


    LIBS += -L$$EPT_CORE_OUT_DIR
    LIBS += -lcore

    INCLUDEPATH += $$EPT_CORE_DIR

    export(DLLS)
    export(INCLUDEPATH)
    export(LIBS)
    export(ANDROID_EXTRA_LIBS)

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
        contains(EPT_THIRDPARTY_CONFIG, system_fftw3) {
            LIBS += -lfftw3
        } else {
            include($$EPT_THIRDPARTY_DIR/fftw3/fftw3_export.pri)
            INCLUDEPATH += $$FFTW_INCLUDE_PATHS
            LIBS += $$FFTW_LIB_PATH $$FFTW_EXTERN_LIBS
        }

        # copy dlls or shared library
        contains(EPT_THIRDPARTY_CONFIG, system_fftw3) {
        } else:contains(FFTW_LIB_MODE_CONFIG, staticlib) {
        } else {
            win32 {
                DLLS += $$FFTW_DESTDIR/fftw3.dll
            } else:android {
                ANDROID_EXTRA_LIBS += $$FFTW_DESTDIR/libfftw3.so
            } else:linux {
                DLLS += $$FFTW_DESTDIR/libfftw3.so
            }
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
            INCLUDEPATH += $$EPT_THIRDPARTY_DIR
        }

        LIBS += -L$$EPT_ROOT_OUT_DIR/thirdparty/getmemorysize -lgetmemorysize
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

        win32 {
            DLLS += $$EPT_ROOT_OUT_DIR/thirdparty/libuv/uv.dll
        }
        android {
            ANDROID_EXTRA_LIBS += $$EPT_ROOT_OUT_DIR/thirdparty/libuv/libuv.so
        }

        LIBS += -L$$EPT_ROOT_OUT_DIR/thirdparty/libuv -luv

        linux:LIBS += -ldl
        #win32|win32-g++:LIBS += -lws2_32 -lpsapi -liphlpapi -lshell32 -luserenv -lkernel32 -ladvapi32
    }

    export(LIBS)
    export(INCLUDEPATH)
    export(DLLS)
    export(ANDROID_EXTRA_LIBS)

    return(true)
}

defineReplace(depends_qwt) {
    contains(EPT_THIRDPARTY_CONFIG, system_qwt) {
        CONFIG += qwt
    } else {
        INCLUDEPATH += $$EPT_THIRDPARTY_DIR/qwt-lib/qwt
        LIBS += -L$$EPT_ROOT_OUT_DIR/thirdparty/qwt-lib
    }

    win32 {
        # if only Qwt depends on OpenGL the module will not get copied
        CONFIG(debug, debug|release){
            LIBS += -L$$EPT_ROOT_OUT_DIR/thirdparty/qwt-lib/debug -lqwtd
            DLLS += $$EPT_ROOT_OUT_DIR/thirdparty/qwt-lib/debug/qwtd.dll
            DLLS += $$(QTDIR)/bin/Qt5OpenGLd.dll
        } else {
            LIBS += -L$$EPT_ROOT_OUT_DIR/thirdparty/qwt-lib/release -lqwt
            DLLS += $$EPT_ROOT_OUT_DIR/thirdparty/qwt-lib/release/qwt.dll
            DLLS += $$(QTDIR)/bin/Qt5OpenGL.dll
        }
    } else:macx {
        # use framework on mac
        LIBS += -F$$EPT_ROOT_OUT_DIR/thirdparty/qwt-lib -framework qwt
    } else:android {
        LIBS += -lqwt
        ANDROID_EXTRA_LIBS += \
            $$[QT_INSTALL_LIBS]/libQt5OpenGL.so \
            $$EPT_ROOT_OUT_DIR/thirdparty/qwt-lib/libqwt.so
    } else:!contains(EPT_THIRDPARTY_CONFIG, system_qwt) {
        LIBS += -lqwt
        DLLS += "$$EPT_ROOT_OUT_DIR/thirdparty/qwt-lib/libqwt.so*"
    } else {
        LIBS += -lqwt-qt5
    }


    DEFINES += QWT_DLL

    export(INCLUDEPATH)
    export(LIBS)
    export(DEFINES)
    export(DLLS)
    export(ANDROID_EXTRA_LIBS)
    export(CONFIG)

    return(true)
}

defineReplace(depends_timesupport) {
    timesupport {
        !contains(EPT_THIRDPARTY_CONFIG, system_timesupport) {
            INCLUDEPATH += $$EPT_THIRDPARTY_DIR/timesupport
        }

        LIBS += -L$$EPT_ROOT_OUT_DIR/thirdparty/timesupport -ltimesupport
    }

    export(INCLUDEPATH)
    export(LIBS)

    return(true)
}

defineReplace(depends_winrtbridge) {
    winrtbridge {
        INCLUDEPATH += $$EPT_THIRDPARTY_DIR/winrtbridge
        LIBS += -lwinrtbridge
    }

    export(INCLUDEPATH)
    export(LIBS)

    return(true)
}

defineReplace(depends_tp3log) {
    INCLUDEPATH += $$EPT_THIRDPARTY_DIR/tp3log
    LIBS += -L$$EPT_ROOT_OUT_DIR/thirdparty/tp3log/tp3log -ltp3log
    tp3LogDLL {
        android:ANDROID_EXTRA_LIBS += $$EPT_ROOT_OUT_DIR/thirdparty/tp3log/tp3log/libtp3log.so
        win32:DLLS += $$EPT_ROOT_OUT_DIR/thirdparty/tp3log/tp3log/tp3log.dll
    }

    export(LIBS)
    export(INCLUDEPATH)
    export(ANDROID_EXTRA_LIBS)
    export(DLLS)

    return(true)
}
