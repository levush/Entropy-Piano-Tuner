include($$PWD//entropypianotuner_config.pri)

defineReplace(depends_getmemorysize) {
    getmemorysize {
        !contains(EPT_THIRDPARTY_CONFIG, system_getmemorysize) {
            INCLUDEPATH += $$EPT_THIRDPARTY_DIR/getmemorysize
        }

        LIBS += -lgetmemorysize
    }

    export(INCLUDEPATH)
    export(LIBS)
}

defineReplace(depends_libuv) {
    libuv {
        !contains(EPT_THIRDPARTY_CONFIG, system_libuv) {
            INCLUDEPATH += $$EPT_THIRDPARTY_DIR/libuv/libuv/include
        }

        LIBS += -luv
        linux:!android {
            LIBS += -ldl
        }
    }

    export(LIBS)
    export(INCLUDEPATH)
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
}
