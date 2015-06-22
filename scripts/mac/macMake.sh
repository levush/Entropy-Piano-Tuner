###########################################################
# This file will handle the creation of compiling and
# creating the mac dmg file
###########################################################

# automatic fail on errors
set -e

cd ${0%/*}

# load the user environment
. ./mac_env.user.sh

DO_CLEAR=false
DO_BUILD=false
DO_DMG=false
DO_UPLOAD=false
DO_TRANSLATIONS=false
DO_VERSION_UPDATE=false

echo "Parsing options."
# parse options
# -c clear
# -b compile and build binary
# -d make the dmg file
# -u upload the files to the server
# -t release translations
# -v update version code
while getopts ":cbdutv" opt; do
	case $opt in
		c)
			echo "Clearing build at $BUILD_DIR."
			DO_CLEAR=true
			;;
		b) 
			echo "Building."
			DO_BUILD=true
			;;
		d)
			echo "Creating dmg file."
			DO_DMG=true
			;;
		u)
			echo "Uploading to webserver."
			DO_UPLOAD=true
			;;
		t)
			echo "Releasing translations."
			DO_TRANSLATIONS=true
			;;
		v)
			echo "Updating version."
			DO_VERSION_UPDATE=true
			;;
		\?)
			echo "Invalid options: -$OPTARG" >&2
			exit -1
			;;	
	esac
done

echo "Options parsed."

# Version update
###########################################################
if $DO_VERSION_UPDATE ; then
	echo "Updating version."
	cd $UNIX_SHARED
	./updateVersion.sh
	echo "Done."
fi

# Clear
###########################################################
if $DO_CLEAR ; then
	echo "Clearing build at $BUILD_DIR."
	rm -rf $BUILD_DIR

	if $DO_DMG ; then
		echo "Removing old dmg at $DMG_FILE."
		rm -f $DMG_FILE
	fi

	echo "Done."
fi

# Release translations
###########################################################
if $DO_TRANSLATIONS ; then
	echo "lrelease translations."
	cd $TUNER_BASE_DIR/translations
	for f in *.ts; do lrelease $f; done
	echo "Done."
fi

# building
###########################################################
if $DO_BUILD ; then
	echo "Building."
	echo "Creating temporary build directory"
	mkdir -p $BUILD_DIR
	cd $BUILD_DIR
	rm -rf $BINARY_FILE_NAME.app
	qmake $TUNER_BASE_DIR/entropytuner.pro -r -spec macx-clang CONFIG+=$CONFIG DEFINES+="CONFIG_ENABLE_UPDATE_TOOL"
	make $MAKE_ARGS

	# copy info.plist and icns files on our own. there is a bug in qt
	cp ../platforms/osx/info.plist $BINARY_FILE_NAME.app/Contents/.
	cp ../appstore/icons/entropytuner.icns $BINARY_FILE_NAME.app/Contents/Resources/.

	echo "Done."
fi

# create dmg
###########################################################
if $DO_DMG ; then
	echo "Creating dmg."
	cd $BUILD_DIR
	mv $BINARY_FILE_NAME.app "EntropyPianoTuner.app"
	macdeployqt "EntropyPianoTuner.app" -dmg
	mv "EntropyPianoTuner.dmg" ${DMG_FILE}
	echo "Done."
fi

# upload to webserver
###########################################################
if $DO_UPLOAD ; then
	echo "Uploading file."
	. $SCRIPT_DIR/macUpload.sh
	echo "Done."
fi




