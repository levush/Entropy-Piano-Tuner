TEMPLATE = subdirs

include(../entropypianotuner_config.pri)

# note: double sub dirs are required to disable conflict with system
#       library headers (e.g. linux systems)
libuv {
    !contains(EPT_THIRDPARTY_CONFIG, system_libuv) {
        SUBDIRS += libuv/libuv
    }
}

qwt {
    !contains(EPT_THIRDPARTY_CONFIG, system_qwt) {
        SUBDIRS += qwt/qwt
    }
}

getmemorysize {
    SUBDIRS += getmemorysize/getmemorysize
}

rtmidi {
    SUBDIRS += rtmidi/RtMidi
}

timesupport {
    SUBDIRS += timesupport/timesupport
}

tinyxml2 {
    !contains(EPT_THIRDPARTY_CONFIG, system_tinyxml2) {
        SUBDIRS += tinyxml2/tinyxml2
    }
}
