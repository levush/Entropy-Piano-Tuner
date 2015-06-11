. ./loadVersion.sh

USERNAME=hinrichsen
SERVER=webpages.physik.uni-wuerzburg.de
ROOT_DIR=public_html/ept
DOWNLOADS_DIR=Resources/Public/Downloads

SERVER_DOWNLOADS=$SERVER:$ROOT_DIR/$DOWNLOADS_DIR

# set working directory to the tuner root
cd ${0%/*}
cd ..

# tuner base dir
TUNER_BASE_DIR=${PWD}

PUBLISH_DIR=$TUNER_BASE_DIR/publish
INSTALLER_DIR=$TUNER_BASE_DIR/appstore/installer


# upload fles

echo "Uploading files to $versionString"
# create directory if missing
ssh $USERNAME@$SERVER "mkdir -p $ROOT_DIR/$DOWNLOADS_DIR/$versionString"
rsync -vh "$PUBLISH_DIR/EntropyPianoTuner_MacOsX.dmg" $USERNAME@$SERVER_DOWNLOADS/$versionString
ssh $USERNAME@$SERVER "rm $ROOT_DIR/$DOWNLOADS_DIR/EntropyPianoTuner_MacOsX.dmg"
ssh $USERNAME@$SERVER "ln $ROOT_DIR/$DOWNLOADS_DIR/$versionString/EntropyPianoTuner_MacOsX.dmg $ROOT_DIR/$DOWNLOADS_DIR/"
rsync -vh "$INSTALLER_DIR/version.xml" $USERNAME@$SERVER_DOWNLOADS


echo "Uploading finished successfully"
