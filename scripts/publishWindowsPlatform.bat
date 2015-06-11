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
set setupname=EntropyPianoTuner_Windows_online_%postfix%.exe

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

set appDataDir=%tunerdir%\appstore\installer\packages\org.entropytuner.app\data
set depsDataDir=%tunerdir%\appstore\installer\packages\org.entropytuner.deps\data

:: clear data dirs
move %appDataDir%\.keepme %publishDir%\
rd %appDataDir% /s /q
mkdir %appDataDir%
move %publishDir%\.keepme %appDataDir%\

move %depsDataDir%\.keepme %publishDir%\
rd %depsDataDir% /s /q
mkdir %depsDataDir%
move %publishDir%\.keepme %depsDataDir%\

windeployqt --compiler-runtime --dir %depsDataDir% %publishdir%\release\EntropyTuner.exe
echo Copy EntropyTuner.exe and fftw dll
copy /Y %publishdir%\release\EntropyTuner.exe %appDataDir%
copy /Y %publishdir%\release\libfftw3-3.dll %depsDataDir%
copy /Y %tunerdir%\appstore\icons\entropytuner.ico %appDataDir%
:: move vcredist_xxx to vcredist
echo Moving vcredist
del %depsDataDir%\vcredist.exe
move %depsDataDir%\vcredist_%vcredist%.exe %depsDataDir%\vcredist.exe || exit /b

:: cd to installer
cd %tunerdir%\appstore\installer
echo Creating installer. This may take a while.
:: create online installer packages
rd windows_repository_%postfix% /s /q
del config\windows_config_%postfix%.xml
repogen -p packages windows_repository_%postfix%
:: create windows config pointing to the correct repository
call %tunerdir%\scripts\BatchSubstitute.bat dummy_repository windows_repository_%postfix% config\config.xml > config\windows_config_%postfix%.xml
:: create online/offline installer
binarycreator -v -n -c config\windows_config_%postfix%.xml -p packages %setupname%

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
