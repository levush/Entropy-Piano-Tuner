include(fftw3_config.pri)

FFTW_LIB_PATH += -L$$FFTW_DESTDIR

contains(FFTW_LIB_MODE_CONFIG, staticlib) {
    # there are circular dependencies, which is why we have to use the '{start/end}-group' flag on linux/mingw gcc/ld. No problems on msvc or darwin clang
    win32-g++:FFTW_EXTERN_LIBS += -Wl,--start-group -lfftw3 -ldftscalar -ldftscalarcodelets -ldft -lrdftscalar -lrdftscalarr2cf -lrdftscalarr2r -lrdftscalarr2cb -lrdft -lreodft -lkernel -Wl,--end-group
    else:linux-g++|android-g++:FFTW_EXTERN_LIBS += -Xlinker -start-group -lfftw3 -ldftscalar -ldftscalarcodelets -ldft -lrdftscalar -lrdftscalarr2cf -lrdftscalarr2r -lrdftscalarr2cb -lrdft -lreodft -lkernel -Xlinker -end-group
    else:FFTW_EXTERN_LIBS += -lfftw3 -ldftscalar -ldftscalarcodelets -ldft -lrdftscalar -lrdftscalarr2cf -lrdftscalarr2r -lrdftscalarr2cb -lrdft -lreodft -lkernel

    FFTW_LIB_PATH += -L$$FFTW_SUBPART_OUT
} else {
    FFTW_EXTERN_LIBS += -lfftw3

    FFTW_LIB_MODE_CONFIG = dll
    DEFINES += "FFTW_DLL=1"
}
