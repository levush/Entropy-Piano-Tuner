set -e


cd ${0%/*}

# load the user environment
. ./iOS_env.user.sh

DO_CLEAR=false
DO_BUILD=false
DO_IPA=false
DO_UPLOAD=false
DO_TRANSLATIONS=false
DO_VERSION_UPDATE=false

_TARGET_DIR=bin

echo "Parsing options."
# parse options
# -c clear
# -b compile and build binary
# -i make the ipa file
# -u upload the files to the server
# -t release translations
while getopts ":cbiutv" opt; do
	case $opt in
		c)
			echo "Clearing build at $BUILD_DIR."
			DO_CLEAR=true
			;;
		b) 
			echo "Building."
			DO_BUILD=true
			;;
		i)
			echo "Creating ipa file."
			DO_IPA=true
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

echo "Options parsed."

if $DO_CLEAR ; then
	echo "Clearing."
	rm -rf $APP_FILE
	rm -rf $BUILD_DIR
	echo "Done."
fi

if $DO_TRANSLATIONS ; then
	echo "lrelease translations"
	cd $TUNER_BASE_DIR/translations
	for f in *.ts; do lrelease $f; done
	echo "Done."
fi

if $DO_BUILD ; then
	echo "Building."

	# prepare directory
	mkdir -p $BUILD_DIR
	cd $BUILD_DIR
	rm -rf $BINARY_FILE_NAME.app

	# make app file
	qmake ../entropypianotuner.pro -r -spec macx-ios-clang CONFIG+=release CONFIG+=iphoneos CONFIG+=device && /usr/bin/make qmake_all
	make $MAKE_ARGS
	#xcodebuild -verbose -jobs 4 -sdk iphoneos CODE_SIGN_IDENTITY="" CODE_SIGNING_REQUIRED=NO
	cd app
	xcodebuild -alltargets -jobs 4 -configuration "Release" -scheme "entropypianotuner" -sdk iphoneos archive -archivePath "${PUBLISH_DIR}/entropypianotuner.xcarchive"

	# move it to the  desired output location
	rm -rf ${APP_FILE}
	mv Release-iphoneos/entropypianotuner.app ${APP_FILE}
	echo "Done."
fi

if $DO_IPA ; then
	echo "Creating ipa file."

	# create ipa package
	cd $BUILD_DIR
	xcodebuild -exportArchive -archivePath "${PUBLISH_DIR}/entropypianotuner.xcarchive" -exportOptionsPlist "${TUNER_BASE_DIR}/app/platforms/ios/exportOptions.plist" -exportPath ${PUBLISH_DIR}

	echo "Created ${IPA_FILE}. Use the application loader to upload it to the app store."
fi

if $DO_UPLOAD ; then
	cd $UNIX_SHARED
	. ./webpages_env.sh

	# upload the ipa file into the webpages
	server_push $IPA_FILE $_SERVER_VERSION_DIR
	# create system link
	server_ln $_SERVER_VERSION_DIR $_SERVER_DOWNLOADS_DIR $APP_FILE_NAME.ipa
fi
