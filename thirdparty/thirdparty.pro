TEMPLATE = subdirs

include(../entropypianotuner_config.pri)

# note: double sub dirs are required to disable conflict with system
#       library headers (e.g. linux systems)

dirent {
    SUBDIRS += dirent
}

fftw3 {
    !contains(EPT_THIRDPARTY_CONFIG, system_fftw3) {
        SUBDIRS += fftw3
    }
}

libuv {
    !contains(EPT_THIRDPARTY_CONFIG, system_libuv) {
        SUBDIRS += libuv/libuv
    }
}

qwt {
    !contains(EPT_THIRDPARTY_CONFIG, system_qwt):SUBDIRS += qwt/qwt
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

pgmidi {
    SUBDIRS += pgmidi/PgMidi
}
