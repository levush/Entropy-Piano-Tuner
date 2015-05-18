set -e

./updateVersion.sh
./publishAndroid.sh
./publishMacOsX.sh
./publishiOS.sh