include(../../entropypianotuner_config.pri)

FFTW_LIB_MODE_CONFIG = dll
DEFINES += "FFT_DLL=1"

FFTW_DESTDIR = $$EPT_THIRDPARTY_OUT_DIR
FFTW_SUBPART_OUT = $$FFTW_DESTDIR/fftw_subparts
FFTW_ROOT = $$PWD/fftw3
FFTW_CONFIG_PATH = $$PWD

