###########################################################
# this file will publish the dependencies and create
# the installer
###########################################################

# automatic fail on errors
set -e

cd ${0%/*}

# load the user environment
. ./linux_env.user.sh

UPDATE_DEPENDENCIES=true
UPDATE_PACKAGES=true
UPDATE_INSTALLER=true

# if -i is provided, we only update this installer
while getopts ":i" opt; do
  case $opt in
    i)
      echo "Updating installer"
      UPDATE_DEPENDENCIES=false
      UPDATE_PACKAGES=false
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit -1
      ;;
  esac
done

# Copy relevant files
###########################################################
echo "Copy the files to the installer"

# create function that finds the linked library via ldd and copys it
reExtractLibraryPath="[[:alnum:][:punct:]]+[[:space:]]+=>[[:space:]]([[:alnum:][:punct:]]+)"
function copySharedLib {
    line=`ldd $INSTALLER_PACKAGE_APP_DATA_DIR/$BINARY_FILE_NAME | grep $1`

    if [[ $line =~ $reExtractLibraryPath ]]; then
	libPath=${BASH_REMATCH[1]}
	cp $libPath $INSTALLER_PACKAGE_DEPS_DATA_DIR
	echo "copying $libPath to package"
    else
	exit -1
    fi
}

function copyQtLib {
    cp $QT_LIB_DIR/lib$1.so.5 $INSTALLER_PACKAGE_DEPS_DATA_DIR
    echo "copying $QT_LIB_DIR/lib$1.so.5 to package"
}

# copy app to app data dir
mkdir -p $INSTALLER_PACKAGE_APP_DATA_DIR
mkdir -p $INSTALLER_PACKAGE_DEPS_DATA_DIR

cp $BUILD_DIR/$BINARY_FILE_NAME $INSTALLER_PACKAGE_APP_DATA_DIR/$BINARY_FILE_NAME
cp $INSTALLER_DIR/scripts/$BINARY_FILE_NAME.sh $INSTALLER_PACKAGE_APP_DATA_DIR/$BINARY_FILE_NAME.sh
chmod +x $INSTALLER_PACKAGE_APP_DATA_DIR/$BINARY_FILE_NAME.sh
cp $INSTALLER_DIR/scripts/$BINARY_FILE_NAME-mime.xml $INSTALLER_PACKAGE_APP_DATA_DIR
cp $TUNER_BASE_DIR/appstore/icons/icon_no_bg_256x256.png $INSTALLER_PACKAGE_APP_DATA_DIR/icon.png

# copy deps
if $UPDATE_DEPENDENCIES ; then
  mkdir -p $INSTALLER_PACKAGE_DEPS_DATA_DIR
  copySharedLib fftw
  copySharedLib Qt5Core
  copySharedLib Qt5Widgets
  copySharedLib Qt5Gui
  copySharedLib Qt5Multimedia
  copySharedLib Qt5Network
  copyQtLib Qt5DBus
  copySharedLib icui
  copySharedLib icuuc
  copySharedLib icudata
  # copy plugins: platform
  mkdir -p $INSTALLER_PACKAGE_DEPS_DATA_DIR/platforms
  cp $QT_PLUGINS_DIR/platforms/libqxcb.so $INSTALLER_PACKAGE_DEPS_DATA_DIR/platforms
  # copy plugins: audio
  mkdir -p $INSTALLER_PACKAGE_DEPS_DATA_DIR/audio
  cp $QT_PLUGINS_DIR/audio/* $INSTALLER_PACKAGE_DEPS_DATA_DIR/audio
fi


# create installer
###########################################################
echo "Creating installer"

cd $TUNER_BASE_DIR/appstore/installer

# installer packages
if $UPDATE_PACKAGES ; then
  rm -rf linux_repository_$POSTFIX
  repogen -p packages linux_repository_$POSTFIX
  echo "Packages created"
fi

# create online/offline installer
if $UPDATE_INSTALLER ; then
  # create linux config pointing to the correct repository, and run .sh instead of normal executable
  rm -f config/linux_config_$POSTFIX.xml
  sed "s/dummy_repository/linux_repository_$POSTFIX/" config/config.xml > config/linux_config_$POSTFIX.xml
  perl -i.bak -p -000 -e 's/(<RunProgram>\@TargetDir\@\/entropypianotuner)(<\/RunProgram>)/$1.sh$2/' config/linux_config_$POSTFIX.xml
  binarycreator -n -c config/linux_config_$POSTFIX.xml -p packages $RUN_FILE
  echo "Setup created"
fi

echo "Created ${RUN_FILE}. This file can be uploaded to the download server."
