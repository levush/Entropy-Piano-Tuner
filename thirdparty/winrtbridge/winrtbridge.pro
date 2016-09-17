include(../../entropypianotuner_func.pri)
$$declareSharedLibrary(winrtbridge)

# mark as win rt code (ZW flag)
WINRTPACKAGES = \"$$(VCINSTALLDIR)vcpackages\"
QMAKE_CXXFLAGS += -ZW /AI $$WINRTPACKAGES

SOURCES += \
    $$PWD/mididevicewatcher.cpp \
    winrtmidiadapter.cpp \
    winrtnativehelper.cpp

HEADERS += \
    $$PWD/mididevicewatcher.h \
    winrtmidiadapter.h \
    winrtnativehelper.h

DEFINES += WINRTBRIDGE_BUILD
