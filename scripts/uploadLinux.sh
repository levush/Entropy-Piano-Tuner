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

echo "Uploading files"
rsync -vh "$PUBLISH_DIR/EntropyPianoTuner_Linux_online_x64.run" $USERNAME@$SERVER_DOWNLOADS
rsync -vh -r "$INSTALLER_DIR/linux_repository_x64" $USERNAME@$SERVER_DOWNLOADS/Repository
rsync -vh "$INSTALLER_DIR/version.xml" $USERNAME@$SERVER_DOWNLOADS
