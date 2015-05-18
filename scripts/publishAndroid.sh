echo "This script will create a signed .apk file that build from the current git branch. This apk can be uploaded directly to the play store."

# global variables
BUILD_SUB_DIR=".publish_android"
PUBLISH_SUB_DIR="publish"
APK_FILE_NAME="EntropyPianoTuner_android"
QT_BIN_DIR="/Library/Qt/5.4/android_armv7/bin/"
export ANDROID_HOME="/Library/Android/sdk"
export ANDROID_NDK_HOST="darwin-x86_64"
export ADNROID_NDK_PLATFORM="android-9"
export ANDROID_NDK_ROOT="/Library/Android/android-ndk"
export ANDROID_NDK_TOOLCHAIN_PREFIX="arm-linux-androideabi"
export ANDROID_NDK_TOOLCHAIN_VERSION="4.9"
export ANDROID_NDK_TOOLS_PREFIX="arm-linux-androideabi"
export ANDROID_SDK_ROOT="/Library/Android/sdk"
export JAVA_HOME="/Library/Java/JavaVirtualMachines/jdk1.8.0_40.jdk/Contents/Home"
export PATH=$QT_BIN_DIR:$PATH

# set working directory to the tuner root
cd ${0%/*}
cd ..

# tuner base dir
TUNER_BASE_DIR=${PWD}

# output dir
PUBLISH_DIR=${TUNER_BASE_DIR}/${PUBLISH_SUB_DIR}
BUILD_DIR=${TUNER_BASE_DIR}/${BUILD_SUB_DIR}
APK_FILE=${PUBLISH_DIR}/${APK_FILE_NAME}.apk

# check if qt installation exists
if [ ! -d "$QT_BIN_DIR" ]; then
	echo "The binary path of qt does not exist: $QT_BIN_DIR"
	exit
fi

# create the output dir
if [ ! -d "$PUBLISH_DIR" ]; then
	mkdir $PUBLISH_DIR
fi

rm -f ${APK_FILE}

echo "lrelease translations"
cd $TUNER_BASE_DIR/translations
for f in *.ts; do lrelease $f; done

echo "Creating temporary build directory"
mkdir -p $BUILD_DIR

cd $BUILD_DIR

${QT_BIN_DIR}qmake ../entropytuner.pro -r -spec android-g++
make -j4

echo "Installing."
make install INSTALL_ROOT=$BUILD_DIR

echo "Deploying for android"

${QT_BIN_DIR}androiddeployqt \
		--input "android-libEntropyTuner.so-deployment-settings.json" \
		--output "${BUILD_DIR}" \
		--sign "../ept_android_release.keystore" "entropyk" \


cp ${BUILD_DIR}/bin/QtApp-release-signed.apk ${APK_FILE}

cd ..

echo "Removing build dir"
rm -rf $BUILD_DIR

echo "Created ${APK_FILE}. This file can be uploaded to the google play store."
