# run this file in cygwin with installed packages: rsync,openssh
PATH=$PATH:/usr/local/bin:/usr/bin

# add ssh-key
eval `ssh-agent -s`
for sshkey in ~/.ssh/*id_rsa
do
	echo Adding ssh-key: $sshkey
	ssh-add $sshkey
done

# set working directory to the tuner root
cd $(dirname $0)
cd ../..

# tuner base dir
TUNER_BASE_DIR=${PWD}

PUBLISH_DIR=$TUNER_BASE_DIR/publish
INSTALLER_DIR=$TUNER_BASE_DIR/appstore/installer

# load server environment
cd $TUNER_BASE_DIR/scripts/unix_shared
. ./webpages_env.sh
cd $TUNER_BASE_DIR

# upload fles
echo "Uploading files"
#server_push "$PUBLISH_DIR/EntropyPianoTuner_Windows_online_x64.exe" $_SERVER_DOWNLOADS_DIR
server_push "$PUBLISH_DIR/EntropyPianoTuner_Windows_online-only_x86.exe" $_SERVER_DOWNLOADS_DIR
#server_push_dir "$INSTALLER_DIR/windows_repository_x64" "$_SERVER_REPOSITORY_DIR"
server_push_dir "$INSTALLER_DIR/windows_repository_x86" "$_SERVER_REPOSITORY_DIR"
fix_permissions  $_SERVER_DOWNLOADS_DIR

# version file is created under linux/mac, and is uploaded from there
# server_push "$INSTALLER_DIR/version.xml" $_SERVER_DOWNLOADS_DIR
