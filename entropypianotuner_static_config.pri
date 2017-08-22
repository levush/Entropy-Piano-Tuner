# Include guard
isEmpty(EPT_STATIC_CONFIG_INCLUDED):EPT_STATIC_CONFIG_INCLUDED=true
else:return()

#------------------------------------------------
# Global config

# include example algorithm by default in debug builds

# on mobile platforms and mac/ios  use static algorithm build
android|ios|winrt|winphone|macx: EPT_CONFIG += no_shared_algorithms static_algorithms static_core no_shared_core
else:                            EPT_CONFIG += shared_algorithms no_static_algorithms shared_core no_static_core


# add install rules
winrt|winphone: EPT_CONFIG += no_install
else: EPT_CONFIG += install


#------------------------------------------------
# third party modules

# select modules
CONFIG += fftw3

# add libuv when shared algorithms are enabled
contains(EPT_CONFIG, shared_algorithms) {
    CONFIG += libuv
}

winrt|winphone {
    CONFIG += winrtbridge
}
else:win32 {
    CONFIG += dirent
}

# tp3log
win32:contains(EPT_CONFIG, shared_core) {
    CONFIG += tp3LogDLL
    DEFINES += TP3LOG_DLL
}

#--------------------------------------------------
# global settings

# additional debug flags
linux-g++*:!android {
    QMAKE_CXXFLAGS_DEBUG += -D_GLIBCXX_DEBUG
}


# config
contains(EPT_CONFIG, no_shared_algorithms):DEFINES+="EPT_NO_SHARED_ALGORITHMS=1" "EPT_STATIC_ALGORITHMS=1"
contains(EPT_CONFIG, shared_algorithms):DEFINES+="EPT_SHARED_ALGORITHMS=1"

# set QWT_CONFIG for static/dynamic build
contains(EPT_CONFIG, static_qwt):QWT_CONFIG += QwtStatic
else:QWT_CONFIG += QwtDll

contains(EPT_CONFIG, static_core):DEFINES += EPT_STATIC_CORE
else:DEFINES += EPT_DYNAMIC_CORE
