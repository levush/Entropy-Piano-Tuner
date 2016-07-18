:: ========================================================================
:: basic system environment
:: 
:: Set the variables to the correct paths
:: ========================================================================

:: Specific arguments to jom (e.g. -j4)
set jomargs=-j6

:: Path to the visual studio installation (e.g. "C:\Program Files (x86)\Microsoft Visual Studio 12.0")
set VSPath="C:\Program Files (x86)\Microsoft Visual Studio 14.0"

:: Path to the Windows kit (e.g. "C:\Program Files (x86)\Windows Kits\10")
set WinKitPath="C:\Program Files (x86)\Windows Kits\10\bin"

:: Path to the 32 bit Qt base dir (e.g. "C:\Qt\5.4\msvc2013\bin")
set QtHome="C:\Qt\5.7"

:: Path to the Qt Creator Binary dir, we need jom to build (e.g. "C:\Qt\Tools\QtCreator\bin")
set QtCreatorPath="C:\Qt\Tools\QtCreator\bin"

:: Set output name
set outname="entropypianotuner"

:: set tunerdir path
cd ..\..

set tunerdir=%CD%
set publishdir=%tunerdir%\publish
set windowsstoredir=%tunerdir%\scripts\windowsstore

cd scripts\windowsstore

