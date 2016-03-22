HEADERS += \
    $$PWD/iosnativewrapper.h \
    $$PWD/iosplatformtools.h \
    $$PWD/iosmidiadapter.h

OBJECTIVE_SOURCES += \
    $$PWD/iosnativewrapper.mm

SOURCES += \
    $$PWD/iosplatformtools.cpp \
    $$PWD/iosmidiadapter.cpp

# include pgmidi
$$depends_pgmidi()
