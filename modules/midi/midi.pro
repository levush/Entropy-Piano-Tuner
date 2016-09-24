include(../../entropypianotuner_config.pri)
include(../../entropypianotuner_func.pri)

# create the core as a lib
TEMPLATE = lib

# on msvc compilers we need to instanciate std::string,
# but this is already done in Qt
# If we use Qt and core in the same subproject (app) the
# compiler does not know which one to use.
# Therefore also use the instance of Qt in the core
QT += core

# configuration
CONFIG += dll
CONFIG += c++11

# just use shadowed path for output (no debug/release)
DESTDIR = $$shadowed($$PWD)

HEADERS += \
    midimanager.h \
    midimanagerlistener.h \
    mididevice.h \
    midiinputdevice.h \
    midioutputdevice.h \
    midiinputlistener.h \
    midioutputsender.h \
    midideviceidentifier.h \
    midiprerequisites.h \
    midibasecallback.h \
    midisystem.h \
    mididevicewatcher.h \
    midiexport.h \

SOURCES += \
    midimanager.cpp \
    mididevice.cpp \
    midiinputdevice.cpp \
    midioutputdevice.cpp \
    midideviceidentifier.cpp \
    midibasecallback.cpp \
    midisystem.cpp \
    mididevicewatcher.cpp \

android {
    DEFINES += MIDI_USE_ANDROID

    HEADERS += \
        android/androidmidimanager.h \
        android/androidnativewrapper.h \
        android/androidmidiinputdevice.h \
        android/jniobject.h \
        android/jnienvironment.h \
        android/androidmidioutputdevice.h

    SOURCES += \
        android/androidmidimanager.cpp \
        android/androidnativewrapper.cpp \
        android/androidmidiinputdevice.cpp \
        android/jniobject.cpp \
        android/jnienvironment.cpp \
        android/androidmidioutputdevice.cpp

} else:winrt|winphone {
    DEFINES += MIDI_BUILD
    DEFINES += MIDI_USE_WINRT

    HEADERS += \
        winrt/winrtmidimanager.h

    SOURCES += \
        winrt/winrtmidimanager.cpp

} else:linux|win32 {
    linux {
        DEFINES += __LINUX_ALSA__
    } win32 {
        DEFINES += __WINDOWS_MM__
        DEFINES += MIDI_BUILD
    }
    DEFINES += MIDI_USE_RTMIDI

    HEADERS += \
        rtmidi/rtmidimanager.h \
        rtmidi/rtmidideviceidentifier.h \
        rtmidi/rtmidiinputdevice.h \

    SOURCES += \
        rtmidi/rtmidimanager.cpp \
        rtmidi/rtmidideviceidentifier.cpp \
        rtmidi/rtmidiinputdevice.cpp \

    $$depends_rtmidi()
}


