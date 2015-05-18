@Echo off
setlocal
:: build and publish EntropyTuner for given x86 or x64 as argument

Echo Building for platform %1 using Qt from %2 with %3 spec and Qt IFW form %4
Echo Location of Visual Studio %5
Echo Location of jom %6

set vslocation=%5
set jomlocation=%6
set msvc_spec=%3

:: select 64 bit build
path=%path%;%2;%4
set vcredist=%1
call %vslocation%\VC\vcvarsall.bat %vcredist%
set postfix=%1

:: cd to tuner
cd ..

set tunerdir=%CD%
set publishdir=%tunerdir%\.publishWindows%postfix%
set setupname=EntropyPianoTuner_Setup_%postfix%

:: update translations (lupdate on all ts files)
forfiles /p %tunerdir%\translations /m *.ts /c "cmd /c lrelease @file"

:: make build directory
mkdir %publishdir%
cd %publishdir%


:: build
qmake ..\entropytuner.pro -r -spec %msvc_spec%
%jomlocation%\jom.exe -j4 || exit /b

:: deploy dlls
cd %tunerdir%

set dataDir=%tunerdir%\appstore\installer\packages\org.entropytuner.app\data

:: clear data dir
move %dataDir%\.keepme %publishDir%\
rd %dataDir% /s /q
mkdir %dataDir%
move %publishDir%\.keepme %dataDir%\

windeployqt --compiler-runtime --dir %tunerdir%\appstore\installer\packages\org.entropytuner.app\data %publishdir%\release\EntropyTuner.exe
echo Copy EntropyTuner.exe and fftw dll
copy /Y %publishdir%\release\EntropyTuner.exe %tunerdir%\appstore\installer\packages\org.entropytuner.app\data
copy /Y %publishdir%\release\libfftw3-3.dll %tunerdir%\appstore\installer\packages\org.entropytuner.app\data
copy /Y %tunerdir%\appstore\icons\entropytuner.ico %dataDir%
:: move vcredist_xxx to vcredist
echo Moving vcredist
del %dataDir%\vcredist.exe
move %dataDir%\vcredist_%vcredist%.exe %dataDir%\vcredist.exe

:: cd to installer
cd %tunerdir%\appstore\installer
echo Creating installer. This may take a while.
binarycreator -v -c config\config.xml -p packages %setupname%

:: move installer to publish dir
cd %tunerdir%
mkdir publish
cd publish
echo Moving installer to %CD%\%setupname%
move %tunerdir%\appstore\installer\%setupname% .

:: cleanup
echo Cleaning up
cd %tunerdir%

::rmdir /S /Q %publishdir%

endlocal