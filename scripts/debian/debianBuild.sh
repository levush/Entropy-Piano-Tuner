cd ../..

BASE_DIR=$PWD

echo "Base dir is $BASE_DIR"

SCRIPTS_DIR=$BASE_DIR/scripts
UNIX_SHARED_DIR=$SCRIPTS_DIR/unix_shared
DEBIAN_DIR=$SCRIPTS_DIR/debian
PACKAGE_DIR=$DEBIAN_DIR/package
SRC_DIR=$PACKAGE_DIR/src

# load version
cd $UNIX_SHARED_DIR
. ./loadVersion.sh
PACKAGE_VERSION=$versionString

echo "Parsing options"
DO_BUILD=false
DO_UPLOAD=false
DO_UPDATE_PACKAGE=false
while getopts ":v:ubp" opt; do
	case $opt in
		p)
			echo "Updating package"
			DO_UPDATE_PACKAGE=true
			;;
		v)
			echo "Overwriting version to $OPTARG"
			PACKAGE_VERSION=$OPTARG
			;;
		b)
			echo "Building."
			DO_BUILD=true
			;;
		u)
			echo "Uploading."
			DO_UPLOAD=true
			;;
		\?)
			echo "Invalid options: $OPTARG" >&2
			exit -1
			;;
	esac
done

# set the package name
PACKAGE_NAME=entropypianotuner_$PACKAGE_VERSION

# create pbuilder environment using debian jessie
# be sure to create the .pbuilderrc in /root
# sudo pbuilder create --distribution jessie --mirror ftp://ftp.us.debian.org/debian/ --debootstrapopts "--variant=buildd --keyring=/usr/share/keyrings/debian-archive-keyring.gpg"

if $DO_UPDATE_PACKAGE ; then
	mkdir -p $SRC_DIR/appstore/installer/scripts
	cp $BASE_DIR/*.pro $SRC_DIR/src.pro
	rsync $BASE_DIR/appstore/icons/entropypianotuner.png $SRC_DIR/appstore/icons
	rsync $BASE_DIR/appstore/icons/application-ept.png $SRC_DIR/appstore/icons
	rsync $BASE_DIR/appstore/installer/scripts/entropypianotuner-mime.xml $SRC_DIR/appstore/installer/scripts
	rsync $BASE_DIR/appstore/installer/scripts/entropypianotuner.desktop $SRC_DIR/appstore/installer/scripts
	rsync -r $BASE_DIR/core $SRC_DIR
	rsync -r $BASE_DIR/Qt $SRC_DIR
	rsync -r $BASE_DIR/thirdparty $SRC_DIR
	rsync -r $BASE_DIR/algorithms $SRC_DIR
	rsync -r $BASE_DIR/translations $SRC_DIR
	rsync -r $BASE_DIR/media $SRC_DIR
	rsync -r $BASE_DIR/tutorial $SRC_DIR

	cd $PACKAGE_DIR

	rm -f $DEBIAN_DIR/$PACKAGE_NAME.orig.tar.gz
	tar -zcf $DEBIAN_DIR/$PACKAGE_NAME.orig.tar.gz -C $PACKAGE_DIR .
fi

if $DO_BUILD ; then
	cd $PACKAGE_DIR
	dh_make -e info@entropy-tuner.org -s -p $PACKAGE_NAME -copyright gpl3 -y || echo "Resuming execution"
	# pdebuild --buildsourceroot fakeroot --debbuildopts "-j8 -sa" -- --save-after-exec --save-after-login
	dpkg-buildpackage
fi

if $DO_UPLOAD ; then
	cd $UNIX_SHARED_DIR
	. ./webpages_env.sh
	BINARY_LINK_NAME=entropypianotuner_amd64.deb
	BINARY_OUTPUT_NAME=$PACKAGE_NAME-1_amd64.deb
	ssh $SERVER_USERNAME@$SERVER_ADDRESS "mkdir -p $_SERVER_ROOT_DIR/$_SERVER_DOWNLOADS_DIR/$PACKAGE_VERSION"
	rsync $DEBIAN_DIR/$BINARY_OUTPUT_NAME $US_DOWNLOADS/$PACKAGE_VERSION
	ssh $SERVER_USERNAME@$SERVER_ADDRESS "rm -f $_SERVER_ROOT_DIR/$_SERVER_DOWNLOADS_DIR/$PACKAGE_VERSION/$BINARY_LINK_NAME"
	ssh $SERVER_USERNAME@$SERVER_ADDRESS "ln $_SERVER_ROOT_DIR/$_SERVER_DOWNLOADS_DIR/$PACKAGE_VERSION/$BINARY_OUTPUT_NAME $_SERVER_ROOT_DIR/$_SERVER_DOWNLOADS_DIR/$BINARY_LINK_NAME"
fi

echo "Script finished"
