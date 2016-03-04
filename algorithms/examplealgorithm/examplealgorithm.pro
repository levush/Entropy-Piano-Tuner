include(../../entropypianotuner_config.pri)

TEMPLATE = lib

CONFIG += dll c++11 skip_target_version_ext

DESTDIR = $$EPT_ALGORITHMS_OUT_DIR
INCLUDEPATH += $$EPT_BASE_DIR

HEADERS = examplealgorithm.h
SOURCES = examplealgorithm.cpp
