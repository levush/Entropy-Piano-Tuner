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
server_push $DMG_FILE $_SERVER_VERSION_DIR
# create system link
server_ln $_SERVER_VERSION_DIR $_SERVER_DOWNLOADS_DIR $DMG_FILE_NAME.dmg
# upload version
server_push "$INSTALLER_DIR/version.xml" $_SERVER_DOWNLOADS_DIR


echo "Uploading finished successfully"
