# Example that shows how a new algorithm can be implemented
#
# This file defines the source and header files for the algorithm
# It will be parsed by qmake using entropytuner.pro

# add all source files to this variable
ALGORITHM_SOURCES = \
    examplealgorithm.cpp \

# add all header files to this variable
ALGORITHM_HEADERS = \
    examplealgorithm.h \

DISTFILES += \
    $$PWD/examplealgorithm.xml
