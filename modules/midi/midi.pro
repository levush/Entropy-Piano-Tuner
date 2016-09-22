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

HEADERS += \
    midimanager.h \
    midimanagerlistener.h \
    mididevice.h \
    midiinputdevice.h \
    midioutputdevice.h \
    midiinputlistener.h \
    midioutputsender.h \
    midideviceidentifier.h \
    rtmidi/rtmidimanager.h \
    midiprerequisites.h \
    rtmidi/rtmidideviceidentifier.h \
    rtmidi/rtmidiinputdevice.h \
    midibasecallback.h \
    midisystem.h

SOURCES += \
    midimanager.cpp \
    mididevice.cpp \
    midiinputdevice.cpp \
    midioutputdevice.cpp \
    midideviceidentifier.cpp \
    rtmidi/rtmidimanager.cpp \
    rtmidi/rtmidideviceidentifier.cpp \
    rtmidi/rtmidiinputdevice.cpp \
    midibasecallback.cpp \
    midisystem.cpp


$$depends_rtmidi()
