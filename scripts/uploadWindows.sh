# run this file in cygwin with installed packages: rsync,openssh

# add ssh-key
eval `ssh-agent -s`
for sshkey in ~/.ssh/*id_rsa
do
	echo Adding ssh-key: $sshkey
	ssh-add $sshkey
done

# normal upload script
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
rsync -vh "$PUBLISH_DIR/EntropyPianoTuner_Windows_online_x64.exe" $USERNAME@$SERVER_DOWNLOADS
rsync -vh "$PUBLISH_DIR/EntropyPianoTuner_Windows_online_x86.exe" $USERNAME@$SERVER_DOWNLOADS
rsync -vh -r "$INSTALLER_DIR/windows_repository_x64" $USERNAME@$SERVER_DOWNLOADS/Repository
rsync -vh -r "$INSTALLER_DIR/windows_repository_x86" $USERNAME@$SERVER_DOWNLOADS/Repository

# version file is created under linux/mac, and is uploaded from there
# rsync -vh "$INSTALLER_DIR/version.xml" $USERNAME@$SERVER_DOWNLOADS
