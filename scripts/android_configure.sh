# this script allows to compile open source engines for android
# it has been tested for gsl and fftw3
#
# INSTRUCTIONS
# - copy this file to the root directory of the library to compile (download it)
# - adjust the ANDROID_NDK_PATH variable to your android ndk installation
# - set the disired platform (linux, darwin)
# - set the architecture (arm and x86 are tested)
# - run this file

# path to replace
ANDROID_NDK_PATH=/Users/christoph/Library/Android/ndk
# platform: linux, darwin (Mac)
PLATFORM=darwin-x86_64
# platform version:
PLATFORM_VERSION=21
# architecture: arm, arm64, mips, mips64, x86, x86_64
ARCHITECTURE=x86
# toolchain version (current 4.9)
TOOLCHAIN_VERSION=4.9

# x86 is labled i686 in the prebuilt directory
PREBUILT_ARCHITECTURE=$ARCHITECTURE
PREBUILT_SUFFIX=-linux-android
# change the desired android compile if necessary
TOOLCHAIN_PREFIX=$ARCHITECTURE
TOOLCHAIN_SUFFIX=-$TOOLCHAIN_VERSION
TOOLCHAIN_MID=""

if [ $ARCHITECTURE == x86 ]; then
	PREBUILT_ARCHITECTURE=i686
elif [ $ARCHITECTURE == arm ]; then
	TOOLCHAIN_MID=-linux-androideabi
	PREBUILT_SUFFIX=-linux-androideabi
fi

TOOLCHAIN=$TOOLCHAIN_PREFIX$TOOLCHAIN_MID$TOOLCHAIN_SUFFIX
PREBUILT=$PREBUILT_ARCHITECTURE$PREBUILT_SUFFIX

export INSTALL_PREFIX=$ANDROID_NDK_PATH/toolchains/$TOOLCHAIN/prebuilt/$PLATFORM/$PREBUILT
export ANDROID_BIN_PATH=$ANDROID_NDK_PATH/toolchains/$TOOLCHAIN/prebuilt/$PLATFORM/bin/
export ANDROID_SYSROOT=$ANDROID_NDK_PATH/platforms/android-$PLATFORM_VERSION/arch-$ARCHITECTURE


# dont change anything here
export PATH=$PATH:$ANDROID_BIN_PATH

export CPP=$PREBUILT-cpp
export CC=$PREBUILT-gcc
export CXX=$PREBUILT-g++
export LD=$PREBUILT-ld
export AS=$PREBUILT-as
export AR=$PREBUILT-ar
export RANLIB=$PREBUILT-ranlib

export CPPFLAGS="--sysroot=$ANDROID_SYSROOT -O3"
export CFLAGS="--sysroot=$ANDROID_SYSROOT -O3"
export CXXFLAGS="--sysroot=$ANDROID_SYSROOT -O3"

[ -f $ANDROID_BIN_PATH$CPP ] && echo "CPP found" || { echo "CPP not found at $ANDROID_BIN_PATH$CPP"; exit 1; }
[ -f $ANDROID_BIN_PATH$CC ] && echo "CC found" || { echo "CC not found at $ANDROID_BIN_PATH$CC"; exit 1; }
[ -f $ANDROID_BIN_PATH$CXX ] && echo "CXX found" || { echo "CXX not found at $ANDROID_BIN_PATH$CXX"; exit 1; }
[ -f $ANDROID_BIN_PATH$LD ] && echo "LD found" || { echo "LD not found at $ANDROID_BIN_PATH$LD"; exit 1; }
[ -f $ANDROID_BIN_PATH$AS ] && echo "AS found" || { echo "AS not found at $ANDROID_BIN_PATH$AS"; exit 1; }
[ -f $ANDROID_BIN_PATH$AR ] && echo "AR found" || { echo "AR not found at $ANDROID_BIN_PATH$AR"; exit 1; }
[ -f $ANDROID_BIN_PATH$RANLIB ] && echo "RANLIB found" || { echo "RANLIB not found at $ANDROID_BIN_PATH$RANLIB"; exit 1; }
[ -d $INSTALL_PREFIX ] && echo "Install dir exists" || { echo "Install dir does not exist at $INSTALL_PREFIX"; exit 1; }

./configure --host=$PREBUILT_ARCHITECTURE --prefix=$INSTALL_PREFIX
make -j4
make install
