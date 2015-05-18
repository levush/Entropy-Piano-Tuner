# android
echo "Creating Android splash"

convert ../icons/icon_1024x1024.png -resize 198x198 -bordercolor none -border 1 -fill black -draw 'point 1,0' -draw 'point 0,1' -draw 'point 0,198' -draw 'point 198,0' ../../android/res/drawable-xhdpi/splash.9.png
convert ../icons/icon_1024x1024.png -resize 140x140 -bordercolor none -border 1 -fill black -draw 'point 1,0' -draw 'point 0,1' -draw 'point 0,140' -draw 'point 140,0' ../../android/res/drawable-hdpi/splash.9.png
convert ../icons/icon_1024x1024.png -resize 100x100 -bordercolor none -border 1 -fill black -draw 'point 1,0' -draw 'point 0,1' -draw 'point 0,100' -draw 'point 100,0' ../../android/res/drawable-mdpi/splash.9.png
convert ../icons/icon_1024x1024.png -resize 60x60   -bordercolor none -border 1 -fill black -draw 'point 1,0' -draw 'point 0,1' -draw 'point 0,60' -draw 'point 60,0' ../../android/res/drawable-ldpi/splash.9.png

# windows rt
echo "Creating Windows RT splash"

convert ../icons/icon_no_bg_1024x1024.png -resize 300x300 -bordercolor none -border 160x0 splash_620x300.png