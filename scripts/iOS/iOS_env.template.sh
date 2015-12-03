###########################################################
# This file contains information about your platform that
# is needed for building a iOS ipk file under mac
#
# This file is a template for an user file that will be
# used by the actual script.
#
# Copy this file to mac_env.user.sh and adjust the path
# settings (QTDIR, ...)
###########################################################

# important paths (change these!)
###########################################################

# path to the qt base directory (e.g. "/Library/Qt/5.4/ios")
export QTDIR="/Library/Qt/5.4/ios"

# arguments for the make command (e.g. -j4)
export MAKE_ARGS=-j4

# path to the provisioning profile relative to the base directory (where entropytuner.pro lies)
export PROVISIONING_PROFILE="EntropyTuner_AppStore.mobileprovision"

# minor important settings
###########################################################

# build directory name
BUILD_SUB_DIR="publish_ios"

# publish directory name (where to store the output file)
PUBLISH_SUB_DIR="publish"

# the binary file name must be the same as in
# 'installer/config/config.xml' Tag <RunProgram>
# Elsewise the program wont start after intallation
BINARY_FILE_NAME="entropypianotuner"

# The file name of the resulting app file
APP_FILE_NAME="EntropyPianoTuner_iOS"

# The name of the developer
DEVELOPER_NAME="iPhone Distribution"

# path to the ResourceRules.plis file of the iPhoneOS.sdk (e.g.: /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk/ResourceRules.plist)
RESOURCE_RULES_PLIST="/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk/ResourceRules.plist"

# path to xcrun (e.g. /usr/bin/xcrun)
XCRUN=/usr/bin/xcrun

# You MAY specify a target device family (1: iphone, 2: ipad, 1,2: both)
QMAKE_IOS_TARGETED_DEVICE_FAMILY=1,2


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
export BUILD_DIR=${TUNER_BASE_DIR}/${BUILD_SUB_DIR}
export INSTALLER_DIR=$TUNER_BASE_DIR/appstore/installer

export APP_FILE=${PUBLISH_DIR}/${APP_FILE_NAME}.app
export IPA_FILE=${PUBLISH_DIR}/${APP_FILE_NAME}.ipa

# set path
export PATH=$QT_BIN_DIR:/usr/bin:$PATH
