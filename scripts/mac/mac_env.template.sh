###########################################################
# This file contains information about your platform that
# is needed for building on mac
#
# This file is a template for an user file that will be
# used by the actual script.
#
# Copy this file to mac_env.user.sh and adjust the path
# settings (QTIFWDIR and QTDIR)
###########################################################


# important paths (change these!)
###########################################################

# path to the qt base directory (e.g. "/Library/Qt/5.4/clang_64")
export QTDIR="/Library/Qt/5.4/clang_64"

# arguments for the make command (e.g. -j4)
export MAKE_ARGS=-j4


# minor important settings
###########################################################

# build directory name
BUILD_SUB_DIR="publish_macosx"

# Post fix for the program
CONFIG="x86_64"

# publish directory name (where to store the output file)
PUBLISH_SUB_DIR="publish"

# the binary file name must be the same as in
# 'installer/config/config.xml' Tag <RunProgram>
# Elsewise the program wont start after intallation
BINARY_FILE_NAME="entropypianotuner"

# The file name of the resulting dmg file
DMG_FILE_NAME="EntropyPianoTuner_MacOsX_$CONFIG"


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

export UNIX_SHARED=${TUNER_BASE_DIR}/scripts/unix_shared
export SCRIPT_DIR=${TUNER_BASE_DIR}/scripts/mac
export PUBLISH_DIR=${TUNER_BASE_DIR}/${PUBLISH_SUB_DIR}
export DMG_FILE=${PUBLISH_DIR}/${DMG_FILE_NAME}.dmg
export BUILD_DIR=${TUNER_BASE_DIR}/${BUILD_SUB_DIR}
export INSTALLER_DIR=$TUNER_BASE_DIR/appstore/installer

# set path
export PATH=$QT_BIN_DIR:/usr/bin:$PATH
