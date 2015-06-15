cd ..

BASE_DIR=$PWD
SCRIPTS_DIR=$BASE_DIR/scripts
DEBIAN_DIR=$SCRIPTS_DIR/debian
SRC_DIR=$DEBIAN_DIR/src

# create pbuilder environment using debian jessie
# be sure to create the .pbuilderrc in /root
# sudo pbuilder create --distribution jessie --mirror ftp://ftp.us.debian.org/debian/ --debootstrapopts "--variant=buildd --keyring=/usr/share/keyrings/debian-archive-keyring.gpg"

# copy required source files to the src directory
mkdir -p $SRC_DIR
mkdir -p $SRC_DIR/appstore/icons
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

rm -f $SCRIPTS_DIR/entropypianotuner_1.0.5.orig.tar.gz
tar -zcf $SCRIPTS_DIR/entropypianotuner_1.0.5.orig.tar.gz -C $DEBIAN_DIR .

cd $DEBIAN_DIR
dh_make -e info@entropy-tuner.org -s -p entropypianotuner_1.0.5 -copyright gpl3 -y
# pdebuild --buildsourceroot fakeroot --debbuildopts "-j8 -sa" -- --save-after-exec --save-after-login
dpkg-buildpackage
