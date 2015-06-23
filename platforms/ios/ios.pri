HEADERS += \
    $$PWD/iosnativewrapper.h \
    $$PWD/iosplatformtools.h

OBJECTIVE_SOURCES += \
    $$PWD/iosnativewrapper.mm

SOURCES += \
    $$PWD/iosplatformtools.cpp

# include pgmidi
include($$PWD/../../thirdparty/PGMidi/pgmidi.pri)
