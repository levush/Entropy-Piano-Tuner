cd ${0%/*}
. ./loadVersion.sh

# write it to the openSUSE build service files
echo "Updating openSUSE build service files"
sed -i.bak "s/^pkgver=[[:digit:]\.]*/pkgver=$versionString/" $TUNER_BASE_DIR/appstore/openSUSE_Build_Service/PKGBUILD
sed -i.bak "s/^Version:[[:space:][:digit:]\.]*/Version: $versionString/" $TUNER_BASE_DIR/appstore/openSUSE_Build_Service/entropypianotuner.dsc
sed -i.bak "s/^Version:[[:space:][:digit:]\.]*/Version: $versionString/" $TUNER_BASE_DIR/appstore/openSUSE_Build_Service/entropypianotuner.spec

# write it to AndroidManifest.xml:

echo "Updating AndroidManifest.xml"

sed -i.bak "s/android:versionName=\"[[:digit:]\.]*\"/android:versionName=\"$versionString\"/" $TUNER_BASE_DIR/app/platforms/android/AndroidManifest.xml
sed -i.bak "s/android:versionCode=\"[[:digit:]]*\"/android:versionCode=\"$versionRolling\"/" $TUNER_BASE_DIR/app/platforms/android/AndroidManifest.xml


# write it to Info.plist

echo "Updating Info.plist"

# ios
perl -i.bak -p -000 -e 's/(<key>CFBundleShortVersionString<\/key>\s*<string>)[0-9\.]+(<\/string>)/$1$ENV{versionString}$2/' $TUNER_BASE_DIR/app/platforms/ios/Info.plist
perl -i.bak -p -000 -e 's/(<key>CFBundleVersion<\/key>\s*<string>)[0-9\.]+(<\/string>)/$1$ENV{versionRolling}$2/' $TUNER_BASE_DIR/app/platforms/ios/Info.plist

# osx
perl -i.bak -p -000 -e 's/(<key>CFBundleShortVersionString<\/key>\s*<string>)[0-9\.]+(<\/string>)/$1$ENV{versionString}$2/' $TUNER_BASE_DIR/app/platforms/osx/Info.plist
perl -i.bak -p -000 -e 's/(<key>CFBundleVersion<\/key>\s*<string>)[0-9\.]+(<\/string>)/$1$ENV{versionRolling}$2/' $TUNER_BASE_DIR/app/platforms/osx/Info.plist

echo Write it to the installer
sed -i.bak "s/<Version>[[:digit:]\.]*<\/Version>/<Version>$versionString<\/Version>/" $TUNER_BASE_DIR/appstore/installer/packages/org.entropytuner.app/meta/package.xml
sed -i.bak "s/<Version>[[:digit:]\.]*<\/Version>/<Version>$versionString<\/Version>/" $TUNER_BASE_DIR/appstore/installer/packages/org.entropytuner.algorithms/meta/package.xml
sed -i.bak "s/<Version>[[:digit:]\.]*<\/Version>/<Version>$versionString<\/Version>/" $TUNER_BASE_DIR/appstore/installer/config/config.xml
sed -i.bak "s/<Version>[[:digit:]\.]*<\/Version>/<Version>$depsVersionString<\/Version>/" $TUNER_BASE_DIR/appstore/installer/packages/org.entropytuner.deps/meta/package.xml

echo Update date in the installer
sed -i.bak "s/<ReleaseDate>[[:digit:]\-]*<\/ReleaseDate>/<ReleaseDate>$releaseDate<\/ReleaseDate>/" $TUNER_BASE_DIR/appstore/installer/packages/org.entropytuner.app/meta/package.xml
sed -i.bak "s/<ReleaseDate>[[:digit:]\-]*<\/ReleaseDate>/<ReleaseDate>$releaseDate<\/ReleaseDate>/" $TUNER_BASE_DIR/appstore/installer/packages/org.entropytuner.deps/meta/package.xml
sed -i.bak "s/<ReleaseDate>[[:digit:]\-]*<\/ReleaseDate>/<ReleaseDate>$releaseDate<\/ReleaseDate>/" $TUNER_BASE_DIR/appstore/installer/packages/org.entropytuner.algorithms/meta/package.xml

echo Update the version.xml file
echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<version>
	<app rolling=\"$versionRolling\" string=\"$versionString\" />
	<dependencies rolling=\"$depsVersionRolling\" string=\"$depsVersionString\" />
</version>" > $TUNER_BASE_DIR/appstore/installer/version.xml

# app.pro (qmake)
echo Updating app.pro
sed -i.bak "s/WINRT_MANIFEST\.version = [[:digit:]\.]*/WINRT_MANIFEST\.version = $versionString\.0/" $TUNER_BASE_DIR/app/app.pro
