# automatic fail on errors
set -e

# server config
USERNAME=hinrichsen
SERVER=webpages.physik.uni-wuerzburg.de
ROOT_DIR=public_html/ept
DOWNLOADS_DIR=Resources/Public/Downloads

SERVER_DOWNLOADS=$SERVER:$ROOT_DIR/$DOWNLOADS_DIR

# load the user environment
cd ${0%/*}
. ./linux_env.user.sh

# upload fles

echo "Uploading files"
rsync -vh "$PUBLISH_DIR/EntropyPianoTuner_Linux_online_x64.run" $USERNAME@$SERVER_DOWNLOADS
rsync -vh -r "$INSTALLER_DIR/linux_repository_x64" $USERNAME@$SERVER_DOWNLOADS/Repository
rsync -vh "$INSTALLER_DIR/version.xml" $USERNAME@$SERVER_DOWNLOADS
