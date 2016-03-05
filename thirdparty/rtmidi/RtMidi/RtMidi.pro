include(../../../entropypianotuner_func.pri)
$$declareStaticLibrary(RtMidi)

linux:!android {
    # chose RtMidi backend
    DEFINES += __LINUX_ALSA__
}
