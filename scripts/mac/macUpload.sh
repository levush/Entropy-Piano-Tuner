# set error on exit
set -e

# check that environment is loaded
if [ -z "$TUNER_BASE_DIR" ] ; then
	echo "Environment has to be loaded before executing this script!"
	exit -1
fi

# load the version
cd $UNIX_SHARED
. ./webpages_env.sh

# upload fles

echo "Uploading files to $versionString"
#upload dmg
rsync -vh $DMG_FILE $US_BINARY
# create system link
ssh $SERVER_USERNAME@$SERVER_ADDRESS "rm -f $_SERVER_ROOT_DIR/$_SERVER_DOWNLOADS_DIR/$DMG_FILE_NAME.dmg"
ssh $SERVER_USERNAME@$SERVER_ADDRESS "ln $_SERVER_ROOT_DIR/$_SERVER_DOWNLOADS_DIR/$versionString/$DMG_FILE_NAME.dmg $_SERVER_ROOT_DIR/$_SERVER_DOWNLOADS_DIR/"
# upload version
rsync -vh "$INSTALLER_DIR/version.xml" $US_DOWNLOADS


echo "Uploading finished successfully"
