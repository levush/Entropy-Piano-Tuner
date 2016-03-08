include(../../fftw3_func.pri)

$$fftw3Core()

TARGET = fftw3
TEMPLATE = lib

DESTDIR = $$FFTW_DESTDIR
CONFIG += $$FFTW_LIB_MODE_CONFIG

gcc:LIBS += -Xlinker -Wl,--start-group -ldftscalar -ldftscalarcodelets -ldft -lrdftscalar -lrdftscalarr2cf -lrdftscalarr2r -lrdftscalarr2cb -lrdft -lreodft -lkernel -Xlinker --end-group
else:LIBS += -ldftscalar -ldftscalarcodelets -ldft -lrdftscalar -lrdftscalarr2cf -lrdftscalarr2r -lrdftscalarr2cb -lrdft -lreodft -lkernel

HEADERS += \
    api.h \
    f77funcs.h \
    fftw3.h \
    guru.h \
    guru64.h \
    plan-guru-dft.h \
    plan-guru-dft-c2r.h \
    plan-guru-dft-r2c.h \
    plan-guru-r2r.h \
    plan-guru-split-dft.h \
    plan-guru-split-dft-c2r.h \
    plan-guru-split-dft-r2c.h \
    x77.h \


SOURCES += \
    apiplan.c \
    configure.c \
    execute-dft-c2r.c \
    execute-dft-r2c.c \
    execute-dft.c \
    execute-r2r.c \
    execute-split-dft-c2r.c \
    execute-split-dft-r2c.c \
    execute-split-dft.c \
    execute.c \
    export-wisdom-to-file.c \
    f77api.c \
    flops.c \
    forget-wisdom.c \
    import-system-wisdom.c \
    import-wisdom-from-file.c \
    import-wisdom-from-string.c \
    import-wisdom.c \
    malloc.c \
    map-r2r-kind.c \
    mapflags.c \
    mkprinter-file.c \
    mkprinter-str.c \
    mktensor-iodims.c \
    mktensor-iodims64.c \
    mktensor-rowmajor.c \
    plan-dft-1d.c \
    plan-dft-2d.c \
    plan-dft-3d.c \
    plan-dft-c2r-1d.c \
    plan-dft-c2r-2d.c \
    plan-dft-c2r-3d.c \
    plan-dft-c2r.c \
    plan-dft-r2c-1d.c \
    plan-dft-r2c-2d.c \
    plan-dft-r2c-3d.c \
    plan-dft-r2c.c \
    plan-dft.c \
    plan-guru-dft-c2r.c \
    plan-guru-dft-r2c.c \
    plan-guru-dft.c \
    plan-guru-r2r.c \
    plan-guru-split-dft-c2r.c \
    plan-guru-split-dft-r2c.c \
    plan-guru-split-dft.c \
    plan-guru64-dft-c2r.c \
    plan-guru64-dft-r2c.c \
    plan-guru64-dft.c \
    plan-guru64-split-dft-c2r.c \
    plan-guru64-split-dft-r2c.c \
    plan-guru64-split-dft.c \
    plan-many-dft-c2r.c \
    plan-many-dft-r2c.c \
    plan-many-dft.c \
    plan-many-r2r.c \
    plan-r2r-1d.c \
    plan-r2r-2d.c \
    plan-r2r-3d.c \
    plan-r2r.c \
    print-plan.c \
    rdft2-pad.c \
    the-planner.c \
    version.c \


