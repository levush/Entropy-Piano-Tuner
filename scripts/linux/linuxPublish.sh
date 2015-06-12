###########################################################
# This is the main file for the update and upload process
#
# It will clear the old build and do a clean rebuild.
# Afterwards it will create the installer packages
# and the installer.
# Finally the created package and installer will be
# uploaded to the webserver.
###########################################################

# automatic fail on errors
set -e

cd ${0%/*}

# load the user environment
. ./linux_env.user.sh

$UNIX_SHARED/updateVersion.sh
# clear make and create qt translations, to keep them up to date
$SCRIPT_DIR/linuxMake.sh -cq
$SCRIPT_DIR/linuxCreateInstaller.sh
$SCRIPT_DIR/linuxUpload.sh

echo The new version was published without any error
