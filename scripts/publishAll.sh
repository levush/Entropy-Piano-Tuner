set -e

./updateVersion.sh
./publishAndroid.sh
./mac/publishMacOsX.sh
./publishiOS.sh