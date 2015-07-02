###########################################################
# This file contains information about your platform that
# is needed for building on linux
#
# This file is a template for an user file that will be
# used by the actual script.
#
# Copy this file to linux_env.user.sh and adjust the path
# settings (QTIFWDIR and QTDIR)
###########################################################


# important paths (change these!)
###########################################################

# path to the installer framework binary dir (e.g. "$HOME/QtIFW/bin")
export QTIFWDIR=

# path to the qt base directory (e.g. "$HOME/Qt/5.4/gcc_64")
export QTDIR=

# arguments for the make command (e.g. -j4)
export MAKE_ARGS=

# directory for the qt translations (e.g. $QTDIR/translations)
QTLANGDIR=


# minor important settings
###########################################################

# build directory name
BUILD_SUB_DIR=".publish_linux"

# Post fix for the program
POSTFIX="x64"

# Output file name
RUN_FILE_NAME="EntropyPianoTuner_Linux_online_$POSTFIX"

# publish directory name (where to store the output file)
PUBLISH_SUB_DIR="publish"

# the binary file name must be the same as in
# 'installer/config/config.xml' Tag <RunProgram>
# Elsewise the program wont start after intallation
BINARY_FILE_NAME="entropypianotuner"


# validate paths
###########################################################

# Qt bin and lib dir    
export QT_BIN_DIR=${QTDIR}"/bin"
export QT_LIB_DIR=${QTDIR}"/lib"
export QT_LANG_DIR=${QTDIR}"/translations"
export QT_PLUGINS_DIR=${QTDIR}"/plugins"

if [ ! -d "$QTDIR" ]; then
    echo "$QTDIR is not a valid Qt installation"
    exit -1
fi

if [ ! -d "$QT_BIN_DIR" ]; then
	echo "The binary path of qt does not exist: $QT_BIN_DIR"
	exit
fi

if [ ! -d "$QT_LANG_DIR" ]; then
	echo "The lang path of qt does not exist: $QT_LANG_DIR"
	exit
fi

if [ ! -d "$QT_PLUGINS_DIR" ]; then
	echo "The plugins path of qt does not exist: $QT_PLUGINS_DIR"
	exit
fi

if [ ! -d $QTIFWDIR ]; then
    echo "The Qt installer framework was not found at: $QTIFWDIR"
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
    echo "Tuner base dir coult not be located at $TUNER_BASE_DIR."
    exit -1
fi

export UNIX_SHARED=${TUNER_BASE_DIR}/scripts/unix_shared
export SCRIPT_DIR=${TUNER_BASE_DIR}/scripts/linux
export PUBLISH_DIR=${TUNER_BASE_DIR}/${PUBLISH_SUB_DIR}
export BUILD_DIR=${TUNER_BASE_DIR}/${BUILD_SUB_DIR}
export RUN_FILE=${PUBLISH_DIR}/${RUN_FILE_NAME}.run
export INSTALLER_DIR=$TUNER_BASE_DIR/appstore/installer
export INSTALLER_PACKAGE_APP_DATA_DIR=$INSTALLER_DIR/packages/org.entropytuner.app/data
export INSTALLER_PACKAGE_DEPS_DATA_DIR=$INSTALLER_DIR/packages/org.entropytuner.deps/data


# set path
export PATH=$QT_BIN_DIR:$QTIFWDIR:/usr/bin:$PATH
