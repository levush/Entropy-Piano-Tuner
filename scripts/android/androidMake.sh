set -e

cd ${0%/*}

# load the user environment
. ./android_env.user.sh

DO_CLEAR=false
DO_BUILD=false
DO_APK=false
DO_UPLOAD=false
DO_TRANSLATIONS=false
DO_VERSION_UPDATE=false

echo "Parsing options."
# parse options
# -c clear
# -b compile and build binary
# -a make the apk file
# -u upload the files to the server
# -t release translations
while getopts ":cbautv" opt; do
	case $opt in
		c)
			echo "Clearing build at $BUILD_DIR."
			DO_CLEAR=true
			;;
		b) 
			echo "Building."
			DO_BUILD=true
			;;
		a)
			echo "Creating apk file."
			DO_APK=true
			;;
		u)
			echo "Uploading to webserver."
			DO_UPLOAD=true
			;;
		t)
			echo "Releasing translations."
			DO_TRANSLATIONS=true
			;;
		\?)
			echo "Invalid options: -$OPTARG" >&2
			exit -1
			;;	
	esac
done

if $DO_CLEAR ; then
	rm -rf $BUILD_DIR
	rm -f ${APK_FILE}
fi

if $DO_TRANSLATIONS ; then
	echo "lrelease translations"
	cd $TUNER_BASE_DIR/translations
	for f in *.ts; do lrelease $f; done
fi

if $DO_BUILD ; then
	echo "Creating temporary build directory"
	mkdir -p $BUILD_DIR
	cd $BUILD_DIR
	${QT_BIN_DIR}/qmake $TUNER_BASE_DIR/entropypianotuner.pro -r -spec android-g++
	make $MAKE_ARGS
fi

if $DO_APK ; then
	echo "Installing."
	cd $BUILD_DIR
	make install INSTALL_ROOT=$BUILD_DIR
	echo "Deploying for android"

	# Read Password
	echo -n "Password for keystore: "
	read -s keystorepassword
	echo

	${QT_BIN_DIR}/androiddeployqt \
		--input "${BUILD_DIR}/app/android-lib$BINARY_FILE_NAME.so-deployment-settings.json" \
		--output "${BUILD_DIR}" \
		--sign ${TUNER_BASE_DIR}/${ANDROID_KEYSTORE} "entropyk" \
		--storepass $keystorepassword \
		--verbose


	cp ${BUILD_DIR}/bin/QtApp-release-signed.apk ${APK_FILE}

	echo "Created ${APK_FILE}. This file can be uploaded to the google play store."
fi

if $DO_UPLOAD ; then
	echo "Uploading"
	cd $UNIX_SHARED
	. ./webpages_env.sh

	# upload the ipa file into the webpages
	server_push $APK_FILE $_SERVER_VERSION_DIR
	# create system link
	server_ln $_SERVER_VERSION_DIR $_SERVER_DOWNLOADS_DIR "${APK_FILE_NAME}.apk"
fi
