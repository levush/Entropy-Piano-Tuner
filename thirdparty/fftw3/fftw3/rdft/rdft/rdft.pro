include(../../../fftw3_func.pri)

$$fftw3SubPart()

LIBS += -lrdftscalar -lrdftscalarr2cf

SOURCES += \
    ../buffered.c \
    ../buffered2.c \
    ../conf.c \
    ../ct-hc2c.c \
    ../ct-hc2c-direct.c \
    ../dft-r2hc.c \
    ../dht-r2hc.c \
    ../dht-rader.c \
    ../direct2.c \
    ../direct-r2c.c \
    ../direct-r2r.c \
    ../generic.c \
    ../hc2hc.c \
    ../hc2hc-direct.c \
    ../hc2hc-generic.c \
    ../indirect.c \
    ../khc2c.c \
    ../khc2hc.c \
    ../kr2c.c \
    ../kr2r.c \
    ../nop.c \
    ../nop2.c \
    ../plan.c \
    ../plan2.c \
    ../problem.c \
    ../problem2.c \
    ../rank0.c \
    ../rank0-rdft2.c \
    ../rank-geq2.c \
    ../rank-geq2-rdft2.c \
    ../rdft2-inplace-strides.c \
    ../rdft2-rdft.c \
    ../rdft2-strides.c \
    ../rdft2-tensor-max-index.c \
    ../rdft-dht.c \
    ../solve.c \
    ../solve2.c \
    ../vrank3-transpose.c \
    ../vrank-geq1.c \
    ../vrank-geq1-rdft2.c \

