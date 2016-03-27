include(../../../entropypianotuner_func.pri)
$$declareStaticLibrary(RtMidi)

linux:!android {
    # chose RtMidi backend
    DEFINES += __LINUX_ALSA__
}

winrt|winphone {}
else:win32:DEFINES += __WINDOWS_MM__
