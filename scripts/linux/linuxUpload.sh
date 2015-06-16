# automatic fail on errors
set -e

# load the user environment
cd ${0%/*}
. ./linux_env.user.sh

cd $UNIX_SHARED
. ./webpages_env.sh


# upload fles

echo "Uploading files"
rsync -vh "$PUBLISH_DIR/EntropyPianoTuner_Linux_online_x64.run" $US_DOWNLOADS
rsync -vh -r "$INSTALLER_DIR/linux_repository_x64" $US_REPOSITORY
rsync -vh "$INSTALLER_DIR/version.xml" $US_DOWNLOADS
