:: ========================================================================
:: basic system environment
:: 
:: Set the variables to the correct paths
:: ========================================================================

:: Path to the Qt InstallerFramework (e.g. "C:\Qt\QtIFW2.0.0_nightly\bin")
set QtIFW="C:\Qt\QtIFW2.0.0_nightly\bin"

:: Path to the Qt Creator Binary dir, we need jom to build (e.g. "C:\Qt\Tools\QtCreator\bin")
set QtCreatorPath="C:\Qt\Tools\QtCreator\bin"

:: Specific arguments to jom (e.g. -j4)
set jomargs=-j4

:: Path to the visual studio installation (e.g. "C:\Program Files (x86)\Microsoft Visual Studio 12.0")
set VSPath="C:\Program Files (x86)\Microsoft Visual Studio 12.0"

:: Path to the 32 bit Qt base dir (e.g. "C:\Qt\5.4\msvc2013\bin")
set QtDir_x86="C:\Qt\5.4\msvc2013\bin"

:: Path to the 64 bit Qt base dir (e.g. "C:\Qt\5.4\msvc2013_64\bin")
set QtDir_x64="C:\Qt\5.4\msvc2013_64\bin"

:: Path to the cygwin bin dir (e.g. "C:\cygwin64\bin")
set CygwinBin="C:\cygwin64\bin"

:: The used msvc_spec (default win32-msvc2013)
set msvc_spec=win32-msvc2013