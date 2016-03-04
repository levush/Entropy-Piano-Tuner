TEMPLATE = lib

CONFIG += staticlib

SOURCES = timesupport.cpp
HEADERS = timesupport.h

include(../../entropypianotuner_config.pri)

DESTDIR = $$EPT_THIRDPARTY_OUT_DIR

