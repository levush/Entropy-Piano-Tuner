# set error on exit
set -e

# check that environment is loaded
if [ -z "$TUNER_BASE_DIR" ] ; then
	echo "Environment has to be loaded before executing this script!"
	exit -1
fi

# load the version
. $UNIX_SHARED/loadVersion.sh

USERNAME=hinrichsen
SERVER=webpages.physik.uni-wuerzburg.de
ROOT_DIR=public_html/ept
DOWNLOADS_DIR=Resources/Public/Downloads

SERVER_DOWNLOADS=$SERVER:$ROOT_DIR/$DOWNLOADS_DIR


# upload fles

echo "Uploading files to $versionString"
# create directory if missing
ssh $USERNAME@$SERVER "mkdir -p $ROOT_DIR/$DOWNLOADS_DIR/$versionString"
#upload dmg
rsync -vh $DMG_FILE $USERNAME@$SERVER_DOWNLOADS/$versionString
# create system link
ssh $USERNAME@$SERVER "rm -f $ROOT_DIR/$DOWNLOADS_DIR/$DMG_FILE_NAME.dmg"
ssh $USERNAME@$SERVER "ln $ROOT_DIR/$DOWNLOADS_DIR/$versionString/$DMG_FILE_NAME.dmg $ROOT_DIR/$DOWNLOADS_DIR/"
# upload version
rsync -vh "$INSTALLER_DIR/version.xml" $USERNAME@$SERVER_DOWNLOADS


echo "Uploading finished successfully"
