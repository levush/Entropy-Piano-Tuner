set -e

cd $(dirname $0)
cd ../..

if [ -z "$TUNER_BASE_DIR" ] ; then
	export TUNER_BASE_DIR=$PWD
fi

# read version from version.h
versionfile="$TUNER_BASE_DIR/modules/core/system/version.h"
if [ -f "$versionfile" ]
then
	echo "$versionfile found."
else
	echo "$versionfile not found."
	exit 1
fi

versioncontents=`cat $versionfile`
releaseDate=$(date +"%Y-%m-%d")

reVersionString="EPT_VERSION_STRING[[:space:]]+\"([0-9\.]+)\""
reVersionRolling="EPT_VERSION_ROLLING[[:space:]]+([0-9]+)"

reDepsVersionString="EPT_DEPS_VERSION_STRING[[:space:]]+\"([0-9\.]+)\""
reDepsVersionRolling="EPT_DEPS_VERSION_ROLLING[[:space:]]+([0-9]+)"


if [[ $versioncontents =~ $reVersionString ]]; then
    export versionString=${BASH_REMATCH[1]}
    echo "Detected version: $versionString"
fi

if [[ $versioncontents =~ $reVersionRolling ]]; then
    export versionRolling=${BASH_REMATCH[1]}
    echo "Detected rolling version: $versionRolling"
fi


if [[ $versioncontents =~ $reDepsVersionString ]]; then
	export depsVersionString=${BASH_REMATCH[1]}
	echo "Detected dependencies version: $depsVersionString"
fi

if [[ $versioncontents =~ $reDepsVersionRolling ]]; then
	export depsVersionRolling=${BASH_REMATCH[1]}
	echo "Detected dependencies rolling version: $depsVersionRolling"
fi
