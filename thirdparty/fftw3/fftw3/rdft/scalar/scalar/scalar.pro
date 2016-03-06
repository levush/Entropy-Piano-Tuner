include(../../../../fftw3_func.pri)

$$fftw3SubPart()

TARGET = rdftscalar

LIBS += -lrdftscalarr2cf

SOURCES += \
    ../hc2c.c \
    ../hfb.c \
    ../r2c.c \
    ../r2r.c \
