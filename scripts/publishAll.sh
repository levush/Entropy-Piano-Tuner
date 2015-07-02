set -e

cd ${0%/*}
THIS_DIR=$PWD

# macPublish will also update the version
cd $THIS_DIR/mac
./macPublish.sh

cd $THIS_DIR/android
./androidPublish.sh

cd $THIS_DIR/iOS
./iOSPublish.sh