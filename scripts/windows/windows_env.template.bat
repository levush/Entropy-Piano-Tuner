:: ========================================================================
:: basic system environment
:: 
:: Set the variables to the correct paths
:: ========================================================================

:: Path to the Qt InstallerFramework (e.g. "C:\Qt\QtIFW2.0.0_nightly\bin")
set QtIFW="C:\Qt\Tools\QtInstallerFramework\2.0\bin"

:: Installer type (--online, --offline, --offline-only, online-only)
set installer_type=online-only

:: Path to the Qt Creator Binary dir, we need jom to build (e.g. "C:\Qt\Tools\QtCreator\bin")
set QtCreatorPath="C:\Qt\Tools\QtCreator\bin"

:: Compiler to use: msvc or mingw
set compiler=mingw

:: Specific arguments to jom (e.g. -j4)
set jomargs=-j6

:: Path to the visual studio installation (e.g. "C:\Program Files (x86)\Microsoft Visual Studio 12.0")
set VSPath="C:\Program Files (x86)\Microsoft Visual Studio 14.0"

:: Path to mingw (e.g. C:\Qt\Qt5.5.0\Tools\mingw492_32\bin), there is a bug: NO "..." allowed!!!
set MinGWPath=C:\Qt\Tools\mingw530_32\bin

:: QtHome dir
set QtHome="C:\Qt\5.7"

:: Path to the 32 bit Qt base dir (e.g. "C:\Qt\5.4\msvc2013")
set QtDir_x86="C:\Qt\5.7\mingw53_32"

:: Path to the 64 bit Qt base dir (e.g. "C:\Qt\5.4\msvc2013_64")
set QtDir_x64="C:\Qt\5.7\msvc2015_64"

:: Path to the cygwin bin dir (e.g. "C:\cygwin64\bin")
set CygwinBin="C:\cygwin64\bin"

:: The used msvc_spec (default win32-msvc2013, also win32-g++ for MinGW)
set msvc_spec=win32-g++