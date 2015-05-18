Follow the Qt Installer Framework

http://doc.qt.io/qtinstallerframework

Download prebuilt binaries of the installer tool from
https://download.qt.io/official_releases/qt-installer-framework/


Deploy current version under windows
====================================

Open a cmd and add the path to your Qt installation with SHARED-dlls, so the default one
set path=%path%;C\Qt\5.4\msvc2013_64\bin

Run windeployqt with --dir set to tuner/appstore/installer/packages/org.entropytuner.app and binary from release build
windeployqt --dir C:\PATH_TO_TUNER\appstore\installer\packages\org.entropytuner.app\data C:\PATH_TO_TUNER_BUILD_RELEASE\release\EntropyTuner.exe
Copy EntropyTuner.exe by hand

Pack the files into installer
cd to appstore/installer
PATH_TO_INSTALLER_BIN\binarycreator -c config\config.xml -p packages EntropyPianoTuner_Setup.exe


Deploy current version under Mac Os X
=====================================

On Mac Os X we wont use the installer since this would be overkill. Just use macdeploy qt to create a .dmg file. The user just has to copy the app into the application directory

Run macdeployqt with -dmg on the release package of EntropyTuner.app
/Path/To/Qt/bin/macdeployqt /Path/To/EntropyTuner.app -dmg