include(../../../entropypianotuner_func.pri)
$$declareStaticLibrary()

HEADERS += \
    $$PWD/iOSVersionDetection.h \
    $$PWD/PGArc.h \
    $$PWD/PGMidi.h \
    $$PWD/PGMidiAllSources.h \
    $$PWD/PGMidiFind.h

OBJECTIVE_SOURCES += \
    $$PWD/PGMidi.mm \
    $$PWD/PGMidiAllSources.mm \
    $$PWD/PGMidiFind.mm

LIBS += -framework UIKit -framework Foundation -framework CoreMIDI
