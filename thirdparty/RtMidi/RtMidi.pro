TEMPLATE = lib

CONFIG += c++11
CONFIG += staticlib

QMAKE_CXXFLAGS_DEBUG += -D_GLIBCXX_DEBUG

SOURCES = RtMidi.cpp
HEADERS = RtMidi.h

include(../../entropypianotuner_config.pri)

DESTDIR = $$EPT_THIRDPARTY_OUT_DIR
