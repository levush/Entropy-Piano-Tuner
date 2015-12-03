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
	qmake ../entropytuner.pro -r -spec macx-ios-clang CONFIG+=release CONFIG+=iphoneos QMAKE_IOS_TARGETED_DEVICE_FAMILY=$QMAKE_IOS_TARGETED_DEVICE_FAMILY
	#make -j4
	#xcodebuild -verbose -jobs 4 -sdk iphoneos CODE_SIGN_IDENTITY="" CODE_SIGNING_REQUIRED=NO
	xcodebuild -alltargets -jobs 4 -configuration "Release" -scheme "entropypianotuner" -sdk iphoneos archive -archivePath "${PUBLISH_DIR}/entropypianotuner.xcarchive"

	# move it to the  desired output location
	rm -rf ${APP_FILE}
	mv Release-iphoneos/entropypianotuner.app ${APP_FILE}
	echo "Done."
fi

if $DO_IPA ; then
	echo "Creating ipa file."

	# copy ResourceRules.plist
	#cp $RESOURCE_RULES_PLIST ${APP_FILE}/.

	# new file, we have to sign the code again
	#codesign --verbose --force --sign "iPhone Distribution: Haye Hinrichsen" ${APP_FILE}

	# verify result
	#codesign --verify ${APP_FILE} || exit 1
	
	#echo "Valididating the certificate successfull"

	# create ipa package
    #$XCRUN 	-sdk iphoneos PackageApplication -v "${APP_FILE}" -o "${IPA_FILE}" --sign "${DEVELOPER_NAME}" --embed "$TUNER_BASE_DIR/$PROVISIONING_PROFILE"
    #$XCRUN 	-sdk iphoneos PackageApplication --verbose "${APP_FILE}" -o "${IPA_FILE}" --sign "iPhone Distribution: Haye Hinrichsen" --embed "$TUNER_BASE_DIR/$PROVISIONING_PROFILE"
    cd $BUILD_DIR
    #xcodebuild -alltargets -jobs 4 -configuration "Release" -scheme "entropypianotuner" -archivePath "${PUBLISH_DIR}/test.xcarchive" archive
    #
    #echo "1"
    xcodebuild -exportArchive -archivePath "${PUBLISH_DIR}/entropypianotuner.xcarchive" -exportOptionsPlist "${TUNER_BASE_DIR}/platforms/ios/exportOptions.plist" -exportPath ${PUBLISH_DIR}
    #xcodebuild -verbose -exportProvisioningProfile "EntropyPianoTuner_AppStore" -exportArchive -exportFormat IPA -archivePath "${PUBLISH_DIR}/test.xcarchive" -exportPath "${PUBLISH_DIR}/test.ipa" CODE_SIGN_IDENTITY="${DEVELOPER_NAME}"

	#cd $PUBLISH_DIR
	#mkdir -p Payload 
	#cp -R ${APP_FILE} Payload
	#zip -r ${IPA_FILE} Payload
	#rm -rf Payload


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
