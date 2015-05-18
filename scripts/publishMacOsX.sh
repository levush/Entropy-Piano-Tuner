set -e

BUILD_SUB_DIR=".publish_macosx"
PUBLISH_SUB_DIR="publish"
DMG_FILE_NAME="EntropyPianoTuner_MacOsX"

export QTDIR="/Library/Qt/5.4/clang_64"
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
DMG_FILE=${PUBLISH_DIR}/${DMG_FILE_NAME}.dmg

# check if qt installation exists
if [ ! -d "$QT_BIN_DIR" ]; then
	echo "The binary path of qt does not exist: $QT_BIN_DIR"
	exit
fi

# create the output dir
if [ ! -d "$PUBLISH_DIR" ]; then
	mkdir $PUBLISH_DIR
fi

rm -f ${DMG_FILE}

echo "lrelease translations"
cd $TUNER_BASE_DIR/translations
for f in *.ts; do lrelease $f; done

echo "Creating temporary build directory"
mkdir -p $BUILD_DIR

cd $BUILD_DIR

rm -rf EntropyTuner.app

qmake ../entropytuner.pro -r -spec macx-clang CONFIG+=x86_64
make -j4

# copy info.plist and icns files on our own. there is a bug in qt
cp ../platforms/osx/info.plist EntropyTuner.app/Contents/.
cp ../appstore/icons/entropytuner.icns EntropyTuner.app/Contents/Resources/.

echo "Creating dmg file"
macdeployqt EntropyTuner.app -dmg


mv EntropyTuner.dmg ${DMG_FILE}

rm -rf $BUILD_DIR

echo "Created ${DMG_FILE}. This file can be uploaded to the download server."
