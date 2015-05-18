set -e

# read version from version.h
versionfile="../core/system/version.h"
if [ -f "$versionfile" ]
then
	echo "$versionfile found."
else
	echo "$versionfile not found."
	exit 1
fi

versioncontents=`cat $versionfile`

reVersionString="EPT_VERSION_STRING[[:space:]]+\"([0-9\.]+)\""
reVersionRolling="EPT_VERSION_ROLLING[[:space:]]+([0-9]+)"


if [[ $versioncontents =~ $reVersionString ]]; then
    export versionString=${BASH_REMATCH[1]}
    echo "Detected version: $versionString"
fi

if [[ $versioncontents =~ $reVersionRolling ]]; then
    export versionRolling=${BASH_REMATCH[1]}
    echo "Detected rolling version: $versionRolling"
fi


# write it to AndroidManifest.xml

echo "Updating AndroidManifest.xml"

sed -i.bak "s/android:versionName=\"[[:digit:]\.]*\"/android:versionName=\"$versionString\"/" ../platforms/android/AndroidManifest.xml
sed -i.bak "s/android:versionCode=\"[[:digit:]]*\"/android:versionCode=\"$versionRolling\"/" ../platforms/android/AndroidManifest.xml


# write it to Info.plist

echo "Updating Info.plist"

# ios
perl -i.bak -p -000 -e 's/(<key>CFBundleShortVersionString<\/key>\s*<string>)[0-9\.]+(<\/string>)/$1$ENV{versionString}$2/' ../platforms/ios/Info.plist
perl -i.bak -p -000 -e 's/(<key>CFBundleVersion<\/key>\s*<string>)[0-9\.]+(<\/string>)/$1$ENV{versionRolling}$2/' ../platforms/ios/Info.plist

# osx
perl -i.bak -p -000 -e 's/(<key>CFBundleShortVersionString<\/key>\s*<string>)[0-9\.]+(<\/string>)/$1$ENV{versionString}$2/' ../platforms/osx/Info.plist
perl -i.bak -p -000 -e 's/(<key>CFBundleVersion<\/key>\s*<string>)[0-9\.]+(<\/string>)/$1$ENV{versionRolling}$2/' ../platforms/osx/Info.plist