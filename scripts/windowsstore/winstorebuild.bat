@Echo off
setlocal

:: load the user environment variables
call windowsstore_env.user.bat

:: default build settings
set clearBuild=0
set build=0
set updateTranslations=0
set createPackage=0
set password="unset"

:: loop through arguments
: argsloop
IF NOT "%1"=="" (
    IF "%1"=="-vcplatform" (
        SET vcplatform=%2
        SHIFT
    )
	if "%1"=="-winkitarch" (
		set winkitarch=%2
	)
	if "%1"=="-qtversion" (
		set qtversion=%2
		SHIFT
	)
	if "%1"=="-msvc_spec" (
		set msvc_spec=%2
		SHIFT
	)
	if "%1"=="-ut" (
		set updateTranslations=1
	)
	if "%1"=="-c" (
	    set clearBuild=1
	)
    IF "%1"=="-b" (
        set build=1
    )
	if "%1"=="-p" (
		set createPackage=1
	)
	if "%1"=="-pwd" (
		set password=%2
		SHIFT
	)
    SHIFT
    GOTO :argsloop
)

:: prepare paths
set postfix=%vcplatform%
set builddir=%tunerdir%\\.build_%postfix%
set QTDIR=%QtHome%\\%qtversion%
set qtbindir=%QtHome%\\%qtversion%\bin
set winkitbindir=%WinKitPath%\\%winkitarch%
set path=%qtbindir%;%winkitbindir%;%QtCreatorPath%;%path%
set binarydir="%builddir%\\app\\release"
set binary="%binarydir%\\entropypianotuner.exe"

:: setup environment
echo Setting up vc platform for %vcplatform% using qtversion %qtversion% and msvc spec %msvc_spec%
call %VSPath%\\VC\\vcvarsall.bat %vcplatform% || exit /b

:: =================================================================================
:: start building

echo Starting the building process in %builddir%

:: update translations (lupdate on all ts files)
if %updateTranslations%==1 (
	echo Updating translations.
	forfiles /p %tunerdir%\\translations /m *.ts /c "cmd /c lrelease @file"
	echo Updating translations finished.
)

:: clear build directory
if %clearBuild%==1 (
	echo Clear build started.
	rd %builddir% /s /q
	echo Clear build finished.
)

:: build
if %build%==1 (
	echo Build started.
	:: make build directory
	mkdir %builddir%
	cd %builddir%
	
	call qmake %tunerdir%\\entropypianotuner.pro -r -spec %msvc_spec% || exit /b
	call jom %jomargs%
	call jom %jomargs%
	call jom %jomargs% || exit /b
	
	
	echo Build finished.
)

:: create package
if %createPackage%==1 (
	echo Creating package.
	
	if "%password%"=="" (
		echo Error:	Required certificate password.
		echo Set password of the certificate via "-pwd password"
		goto :eof
	)
	
	
	:: erase mp:PhoneIdentity tag in ApplicationManifest file
	cd %binarydir%
	
	
	echo Calling windeploy.
	call windeployqt %binary%
	
	cd %windowsstoredir%
	call %windowsstoredir%\createPackage -pwd %password% -k %winkitbindir% -d %binarydir% -postfix %postfix%
)

endlocal
