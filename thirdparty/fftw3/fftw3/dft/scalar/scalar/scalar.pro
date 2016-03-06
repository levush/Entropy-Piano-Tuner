include(../../../../fftw3_func.pri)

$$fftw3SubPart()

TARGET = dftscalar

SOURCES += \
    ../n.c \
    ../t.c \
