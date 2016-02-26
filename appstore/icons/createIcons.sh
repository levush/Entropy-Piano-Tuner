# Qt window icon
echo "Creating Qt windows icon"
convert icon_no_bg_1024x1024.png -resize 256x256 ../../media/images/icon_256x256.png
convert icon_no_bg_1024x1024.png -resize 512x512 ../../media/images/icon_256x256@2x.png
convert icon_1024x1024.png -resize 512x512 icon_512x512.png
convert icon_1024x1024.png -resize 256x256 icon_256x256.png
convert icon_1024x1024.png -resize 128x128 icon_128x128.png

convert icon_no_bg_1024x1024.png -resize 256x256 icon_no_bg_256x256.png


convert icon_no_bg_1024x1024.png -resize 256x256 ../installer/config/windowicon.png
convert icon_no_bg_1024x1024.png -resize 64x64 ../installer/config/logo.png

# IOS AppIcons
# see http://doc.qt.io/qt-5/platform-notes-ios.html#icons
echo "Creating iOS icons"

convert icon_1024x1024.png -resize 29x29 ios/AppIcon29x29.png
convert icon_1024x1024.png -resize 58x58 ios/AppIcon29x29@2x.png
convert icon_1024x1024.png -resize 58x58 ios/AppIcon29x29@2x~ipad.png
convert icon_1024x1024.png -resize 29x29 ios/AppIcon29x29~ipad.png
convert icon_1024x1024.png -resize 80x80 ios/AppIcon40x40@2x.png
convert icon_1024x1024.png -resize 80x80 ios/AppIcon40x40@2x~ipad.png
convert icon_1024x1024.png -resize 40x40 ios/AppIcon40x40~ipad.png
convert icon_1024x1024.png -resize 100x100 ios/AppIcon50x50@2x~ipad.png
convert icon_1024x1024.png -resize 50x50 ios/AppIcon50x50~ipad.png
convert icon_1024x1024.png -resize 57x57 ios/AppIcon57x57.png
convert icon_1024x1024.png -resize 114x114 ios/AppIcon57x57@2x.png
convert icon_1024x1024.png -resize 120x120 ios/AppIcon60x60@2x.png
convert icon_1024x1024.png -resize 167x167 ios/AppIcon83.5x83.5@2x.png
convert icon_1024x1024.png -resize 144x144 ios/AppIcon72x72@2x~ipad.png
convert icon_1024x1024.png -resize 72x72 ios/AppIcon72x72~ipad.png
convert icon_1024x1024.png -resize 152x152 ios/AppIcon76x76@2x~ipad.png
convert icon_1024x1024.png -resize 76x76 ios/AppIcon76x76~ipad.png



# OsX AppIcons
# create icns file: see http://stackoverflow.com/questions/6337787/how-can-i-set-the-icon-for-a-mac-application-in-xcode
# add it to qt: see http://www.qtcentre.org/threads/54247-Qt-5-02-How-to-add-a-MacOS-Dock-Icon
echo "Creating Mac Os X icons"

mkdir entropytuner.iconset
convert icon_no_bg_1024x1024.png -resize 16x16 entropytuner.iconset/icon_16x16.png
convert icon_no_bg_1024x1024.png -resize 32x32 entropytuner.iconset/icon_16x16@2.png
convert icon_no_bg_1024x1024.png -resize 32x32 entropytuner.iconset/icon_32x32.png
convert icon_no_bg_1024x1024.png -resize 64x64 entropytuner.iconset/icon_32x32@2.png
convert icon_no_bg_1024x1024.png -resize 128x128 entropytuner.iconset/icon_128x128.png
convert icon_no_bg_1024x1024.png -resize 256x256 entropytuner.iconset/icon_128x128@2.png
convert icon_no_bg_1024x1024.png -resize 256x256 entropytuner.iconset/icon_256x256.png
convert icon_no_bg_1024x1024.png -resize 512x512 entropytuner.iconset/icon_256x256@2.png
convert icon_no_bg_1024x1024.png -resize 512x512 entropytuner.iconset/icon_512x512.png
convert icon_no_bg_1024x1024.png -resize 1024x1024 entropytuner.iconset/icon_512x512@2.png
iconutil -c icns entropytuner.iconset
cp entropytuner.ics ../installer/config/appicon.icns
rm -rf entropytuner.iconset


# android icons
# this icons will directly be written to ROOT/android/res/drawable-[hlm]dpi
echo "Creating Android icons"

convert icon_1024x1024.png -resize 192x192 ../../android/res/drawable-xxxhdpi/icon.png
convert icon_1024x1024.png -resize 144x144 ../../android/res/drawable-xxhdpi/icon.png
convert icon_1024x1024.png -resize 96x96 ../../android/res/drawable-xhdpi/icon.png
convert icon_1024x1024.png -resize 72x72 ../../android/res/drawable-hdpi/icon.png
convert icon_1024x1024.png -resize 48x48 ../../android/res/drawable-mdpi/icon.png
convert icon_1024x1024.png -resize 40x40 ../../android/res/drawable-ldpi/icon.png


# windows icons
# icons will we written into one .ico file
echo "Creating Windows icons"
convert icon_no_bg_1024x1024.png -depth 32 -define icon:auto-resize=256,128,64,48,32,16 entropytuner.ico
cp entropytuner.ico ../installer/config/appicon.ico

# windows rt icons
echo "Creating Windows RT icons"
convert icon_no_bg_1024x1024.png -resize 150x150 winrt/icon_150x150.png
convert icon_no_bg_1024x1024.png -resize 30x30 winrt/icon_30x30.png
convert icon_no_bg_1024x1024.png -resize 50x50 winrt/icon_50x50.png
convert icon_no_bg_1024x1024.png -resize 100x100 winrt/icon_100x100.png
