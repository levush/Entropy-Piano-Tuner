set -e

BUILD_SUB_DIR=".publish_macosx"
PUBLISH_SUB_DIR="publish"
APP_FILE_NAME="EntropyPianoTuner_ios"
DEVELOPER_NAME="iPhone Distribution"
PROVISONING_PROFILE="../EntropyTuner.mobileprovision"

export QTDIR="/Library/Qt/5.4/ios"
QT_BIN_DIR=${QTDIR}"/bin"

export PATH=$PATH:$QT_BIN_DIR:/usr/bin

# set working directory to the tuner root
cd ${0%/*}
cd ..

# tuner base dir
TUNER_BASE_DIR=${PWD}

# output dir
PUBLISH_DIR=${TUNER_BASE_DIR}/${PUBLISH_SUB_DIR}
BUILD_DIR=${TUNER_BASE_DIR}/${BUILD_SUB_DIR}
APP_FILE=${PUBLISH_DIR}/${APP_FILE_NAME}.app
IPA_FILE=${PUBLISH_DIR}/${APP_FILE_NAME}.ipa

# check if qt installation exists
if [ ! -d "$QT_BIN_DIR" ]; then
	echo "The binary path of qt does not exist: $QT_BIN_DIR"
	exit
fi

# create the output dir
mkdir -p $PUBLISH_DIR

rm -rf ${APP_FILE}

echo "lrelease translations"
cd $TUNER_BASE_DIR/translations
for f in *.ts; do lrelease $f; done

echo "Creating temporary build directory"
mkdir -p $BUILD_DIR

cd $BUILD_DIR

rm -rf EntropyTuner.app

qmake ../entropytuner.pro -r -spec macx-ios-clang CONFIG+=release CONFIG+=iphoneos
make -j4

mv Release-iphoneos/entropypianotuner.app ${APP_FILE}


echo "Creating ipa file."

# copy ResourceRules.plist
cp /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk/ResourceRules.plist ${APP_FILE}/.

# create ipa package
/usr/bin/xcrun -sdk iphoneos PackageApplication -v "${APP_FILE}" -o "${IPA_FILE}" --sign "${DEVELOPER_NAME}" --embed "${PROVISONING_PROFILE}"


rm -rf $BUILD_DIR
rm -rf ${APP_FILE}

echo "Created ${IPA_FILE}. Use the application loader to upload it to the app store."
