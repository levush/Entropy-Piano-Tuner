# automatic fail on errors
set -e

# load the user environment
cd ${0%/*}
. ./linux_env.user.sh

cd $UNIX_SHARED
. ./webpages_env.sh


# upload fles

echo "Uploading files"
server_push "$PUBLISH_DIR/EntropyPianoTuner_Linux_online_x64.run" $_SERVER_DOWNLOADS_DIR
server_push_dir "$INSTALLER_DIR/linux_repository_x64" $_SERVER_REPOSITORY_DIR
server_push "$INSTALLER_DIR/version.xml" $_SERVER_DOWNLOADS_DIR
