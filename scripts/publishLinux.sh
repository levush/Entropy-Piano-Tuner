BUILD_SUB_DIR=".publish_linux"
PUBLISH_SUB_DIR="publish"
# the binary file name must be the same as in installer/config/config.xml Tag <RunProgram>
BINARY_FILE_NAME="EntropyTuner"
RUN_FILE_NAME="EntropyPianoTuner_Linux64"

export QTIFWDIR="$HOME/Qt/QtIFW2.0.0/bin"
export QTDIR="$HOME/Qt/5.4/gcc_64"

if [ ! -d "$QTDIR" ]; then
    export QTIFWDIR="/scratch/wick/Qt/QtIFW2.0.0/bin"
    export QTDIR="/scratch/wick/Qt/5.4/gcc_64"
fi
    
QT_BIN_DIR=${QTDIR}"/bin"
QT_LIB_DIR=${QTDIR}"/lib"

export PATH=$QT_BIN_DIR:$QTIFWDIR:/usr/bin:$PATH

# set working directory to the tuner root
cd ${0%/*}
cd ..

# tuner base dir
TUNER_BASE_DIR=${PWD}

# output dir
PUBLISH_DIR=${TUNER_BASE_DIR}/${PUBLISH_SUB_DIR}
BUILD_DIR=${TUNER_BASE_DIR}/${BUILD_SUB_DIR}
RUN_FILE=${PUBLISH_DIR}/${RUN_FILE_NAME}.run
INSTALLER_DIR=$TUNER_BASE_DIR/appstore/installer
INSTALLER_PACKAGE_DATA_DIR=$INSTALLER_DIR/packages/org.entropytuner.app/data

# automatic fail on errors
set -e

# check if qt installation exists
if [ ! -d "$QT_BIN_DIR" ]; then
	echo "The binary path of qt does not exist: $QT_BIN_DIR"
	exit
fi

if [ ! -d $QTIFWDIR ]; then
    echo "The Qt installer framework was not found at: $QTIFWDIR"
    exit
fi

# create the output dir
if [ ! -d "$PUBLISH_DIR" ]; then
	mkdir $PUBLISH_DIR
fi

# remove the old run file
if [ -f "$RUN_FILE" ]; then
    rm ${RUN_FILE}
fi

echo "Creating temporary build directory"
if [ ! -d "$BUILD_DIR" ]; then
    mkdir $BUILD_DIR
fi

echo "lrelease translations"
cd $TUNER_BASE_DIR/translations
for f in *.ts; do lrelease $f; done

cd $BUILD_DIR

qmake ../entropytuner.pro -r -spec linux-g++
make -j4

echo "Copy the files to the installer"

# create function that finds the linked library via ldd and copys it
reExtractLibraryPath="[[:alnum:][:punct:]]+[[:space:]]+=>[[:space:]]([[:alnum:][:punct:]]+)"
function copySharedLib {
    line=`ldd EntropyTuner | grep $1`

    if [[ $line =~ $reExtractLibraryPath ]]; then
	libPath=${BASH_REMATCH[1]}
	cp $libPath $INSTALLER_PACKAGE_DATA_DIR
	echo "copying $libPath to package"
    else
	exit -1
    fi
}

function copyQtLib {
    cp $QT_LIB_DIR/lib$1.so.5 $INSTALLER_PACKAGE_DATA_DIR
    echo "copying $QT_LIB_DIR/lib$1.so.5 to package"
}

cp EntropyTuner $INSTALLER_PACKAGE_DATA_DIR/$BINARY_FILE_NAME
cp $INSTALLER_DIR/scripts/EntropyTuner.sh $INSTALLER_PACKAGE_DATA_DIR/$BINARY_FILE_NAME.sh
chmod +x $INSTALLER_PACKAGE_DATA_DIR/$BINARY_FILE_NAME.sh
cp $INSTALLER_DIR/scripts/EntropyTunerMIME.xml $INSTALLER_PACKAGE_DATA_DIR
copySharedLib fftw
copySharedLib Qt5Core
copySharedLib Qt5Widgets
copySharedLib Qt5Gui
copySharedLib Qt5Multimedia
copySharedLib Qt5Network
copyQtLib Qt5DBus
copySharedLib icui
copySharedLib icuuc
copySharedLib icudata
mkdir -p $INSTALLER_PACKAGE_DATA_DIR/platforms
cp $QTDIR/plugins/platforms/libqxcb.so $INSTALLER_PACKAGE_DATA_DIR/platforms
cp $TUNER_BASE_DIR/appstore/icons/icon_no_bg_256x256.png $INSTALLER_PACKAGE_DATA_DIR/icon.png


echo "Creating installer"

cd $TUNER_BASE_DIR/appstore/installer

# we have to do a small hack to run EntropyTuner.sh instead of EntropyTuner
perl -i.bak -p -000 -e 's/(<RunProgram>\@TargetDir\@\/EntropyTuner)(<\/RunProgram>)/$1.sh$2/' config/config.xml

# create installer
binarycreator -c config/config.xml -p packages $RUN_FILE

# revert hack
perl -i.bak -p -000 -e 's/(<RunProgram>\@TargetDir\@\/EntropyTuner).sh(<\/RunProgram>)/$1$2/' config/config.xml

echo "Created ${RUN_FILE}. This file can be uploaded to the download server."
