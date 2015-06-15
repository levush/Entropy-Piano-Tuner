set -e

cd ${0%/*}
THIS_DIR=$PWD

# macPublish will also update the version
cd $THIS_DIR/mac
./macPublish.sh

cd $THIS_DIR
./publishAndroid.sh
./publishiOS.sh