TEMPLATE = lib

CONFIG += staticlib

SOURCES = getmemorysize.cpp
HEADERS = getmemorysize.h

include(../../entropypianotuner_config.pri)

DESTDIR = $$EPT_THIRDPARTY_OUT_DIR
