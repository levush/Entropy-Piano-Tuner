TEMPLATE = lib

CONFIG += staticlib

SOURCES = tinyxml2.cpp
HEADERS = tinyxml2.h

include(../../../entropypianotuner_config.pri)

DESTDIR = $$EPT_THIRDPARTY_OUT_DIR
