include(../../entropypianotuner_func.pri)
$$declareSharedLibrary(winrtbridge)

# MSVC 2017 requires VCIDEINSTALLDIR for vcpackages
# else use the default VCINSTALLDIR
VCIDEINSTALLDIR=$$(VCIDEINSTALLDIR)
!isEmpty(VCIDEINSTALLDIR) { VCINSTALLDIR=$$VCIDEINSTALLDIR }
else { VCINSTALLDIR=$$(VCINSTALLDIR) }

# mark as win rt code (ZW flag)
WINRTPACKAGES = \"$${VCINSTALLDIR}vcpackages\"
QMAKE_CXXFLAGS += -ZW /AI $$WINRTPACKAGES /EHsc

SOURCES += \
    winrtnativehelper.cpp

HEADERS += \
    winrtnativehelper.h

DEFINES += WINRTBRIDGE_BUILD
