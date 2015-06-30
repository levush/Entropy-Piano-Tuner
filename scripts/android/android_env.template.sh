###########################################################
# This file contains information about your platform that
# is needed for building a android apk file under mac or
# linux.
#
# This file is a template for an user file that will be
# used by the actual script.
#
# Copy this file to android_env.user.sh and adjust the path
# settings (QTDIR, ...)
###########################################################

# important paths (change these!)
###########################################################

# path to the qt base directory (e.g. "/Library/Qt/5.4/android_armv7")
export QTDIR="/Library/Qt/5.4/android_armv7"

# arguments for the make command (e.g. -j4)
export MAKE_ARGS=-j4

# path to the android keystore file relative to the base dir (same dir as entropytuner.pro)
export ANDROID_KEYSTORE="ept_android_release.keystore"

# path to the android SDK (e.g. /Library/Android/sdk)
export ANDROID_HOME="/Library/Android/sdk"

# host platform (e.g. darwin-x86_64 for mac or linux-x86_64 for linux)
export ANDROID_NDK_HOST="darwin-x86_64"

# path to the android ndk (e.g. "/Library/Android/android-ndk")
export ANDROID_NDK_ROOT="/Library/Android/android-ndk"

# the ndk platform to use (minimum android-9)
export ADNROID_NDK_PLATFORM="android-9"

# the ndk toolchain = android compiler (e.g. arm-linux-androideabi)
export ANDROID_NDK_TOOLCHAIN_PREFIX="arm-linux-androideabi"

# the ndk toolchain version = version of android gcc compiler (e.g. 4.9)
export ANDROID_NDK_TOOLCHAIN_VERSION="4.9"

# the ndk tools prefix (this can differ from ANDROID_NDK_TOOLCHAIN_PREFIX esp. for x86 builds, on arm builds its the same)
export ANDROID_NDK_TOOLS_PREFIX=$ANDROID_NDK_TOOLCHAIN_PREFIX

# path to yout java environment (e.g. "/Library/Java/JavaVirtualMachines/jdk1.8.0_40.jdk/Contents/Home")
export JAVA_HOME="/Library/Java/JavaVirtualMachines/jdk1.8.0_40.jdk/Contents/Home"

# minor important settings
###########################################################

# android sdk home
export ANDROID_SDK_ROOT=$ANDROID_HOME

# build directory name (IMPORTANT: DUE TO A BUG IN QT DONT CHANGE THIS!)
BUILD_SUB_DIR="android-build"

# publish directory name (where to store the output file)
PUBLISH_SUB_DIR="publish"

# the binary file name must be the same as in
# 'installer/config/config.xml' Tag <RunProgram>
# Elsewise the program wont start after intallation
BINARY_FILE_NAME="entropypianotuner"

# The file name of the resulting dmg file
APK_FILE_NAME="EntropyPianoTuner_android"

# validate paths
###########################################################

# Qt bin and lib dir    
export QT_BIN_DIR=${QTDIR}"/bin"
export QT_LIB_DIR=${QTDIR}"/lib"

if [ ! -d "$QTDIR" ]; then
    echo "$QTDIR is not a valid Qt installation"
    exit -1
fi

if [ ! -d "$QT_BIN_DIR" ]; then
	echo "The binary path of qt does not exist: $QT_BIN_DIR"
	exit
fi

# set other usefull paths
###########################################################

# set working directory to the tuner root
cd ${0%/*}
cd ../..

# useful directories
export TUNER_BASE_DIR=${PWD}

if [ ! -d "$TUNER_BASE_DIR" ]; then
    echo "Tuner base dir could not be located at $TUNER_BASE_DIR."
    exit -1
fi

if [ ! -f "$TUNER_BASE_DIR/$ANDROID_KEYSTORE" ]; then
	echo "The android keystore file could not be located at $TUNER_BASE_DIR/$ANDROID_KEYSTORE"
	exit -1
fi

if [ ! -d "$ANDROID_HOME" ]; then
	echo "Android home dir could not be located at $ANDROID_HOME"
	exit -1
fi

if [ ! -d "$ANDROID_NDK_ROOT" ]; then
	echo "Android ndk root could not be located at $ANDROID_NDK_ROOT"
	exit -1
fi

export UNIX_SHARED=${TUNER_BASE_DIR}/scripts/unix_shared
export SCRIPT_DIR=${TUNER_BASE_DIR}/scripts/mac
export PUBLISH_DIR=${TUNER_BASE_DIR}/${PUBLISH_SUB_DIR}
export BUILD_DIR=${TUNER_BASE_DIR}/${BUILD_SUB_DIR}
export INSTALLER_DIR=$TUNER_BASE_DIR/appstore/installer

export APK_FILE=${PUBLISH_DIR}/${APK_FILE_NAME}.apk

# set path
export PATH=$QT_BIN_DIR:/usr/bin:$PATH
