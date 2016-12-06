include(../../entropypianotuner_func.pri)
$$declareSharedLibrary(winrtbridge)

# mark as win rt code (ZW flag)
WINRTPACKAGES = \"$$(VCINSTALLDIR)vcpackages\"
QMAKE_CXXFLAGS += -ZW /AI $$WINRTPACKAGES

SOURCES += \
    winrtnativehelper.cpp

HEADERS += \
    winrtnativehelper.h

DEFINES += WINRTBRIDGE_BUILD
