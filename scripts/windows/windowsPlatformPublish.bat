@Echo off
setlocal

:: load the user environment variables
call windows_env.user.bat || exit 1

:: default platform
set vcredist=x86

:: default build settings
set clearBuild=0
set build=0
set updatePackages=0
set updateDependencies=0
set updateTranslations=0
set createInstaller=0
set publish=0

:: loop through arguments
: argsloop
IF NOT "%1"=="" (
    IF "%1"=="-p" (
        SET vcredist=%2
        SHIFT
    )
	if "%1"=="-c" (
	    set clearBuild=1
	)
    IF "%1"=="-b" (
        set build=1
    )
	if "%1"=="-up" (
		set updatePackages=1
	)
	if "%1"=="-ud" (
		set updateDependencies=1
	)
	if "%1"=="-ut" (
		set updateTranslations=1
	)
	if "%1"=="-i" (
		set createInstaller=1
	)
	if "%1"=="-publish" (
		set publish=1
	)
    SHIFT
    GOTO :argsloop
)

if %vcredist% EQU x86 (
	call windows_vars_x86.bat
) else (
	if %vcredist% EQU x64 (
		call windows_vars_x64.bat
	) else (
		echo Platform %vcredist% is not supported.
		exit /b
	)
)

call windowsPaths.bat %vcredist%

echo Platfrom variables load.

:: update translations (lupdate on all ts files)
if %updateTranslations%==1 (
	echo Updating translations.
	forfiles /p %tunerdir%\translations /m *.ts /c "cmd /c lrelease @file"
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
	
	:: qmake + jom
	qmake %tunerdir%\entropypianotuner.pro -r -spec %msvc_spec% DEFINES+="CONFIG_ENABLE_UPDATE_TOOL" || exit /b
	
	if %compiler% EQU msvc (
		:: silent + other options
		"%QtCreatorPath%\jom.exe" -S %jomargs% || exit /b
	) else (
		"%MinGWPath%\mingw32-make.exe" %jomargs% || exit /b
	)
	
	echo Build finished.
)

:: deploy dlls
cd %tunerdir%

:: update dependencies
if %updateDependencies%==1 (
	echo Updating dependencies started.
	
	move "%appDataDir%\.keepme" %builddir%
	rd %appDataDir% /s /q
	mkdir %appDataDir%
	move "%builddir%\.keepme" %appDataDir%

	move "%depsDataDir%\.keepme" %builddir%
	rd %depsDataDir% /s /q
	mkdir %depsDataDir%
	move "%builddir%\.keepme" %depsDataDir%

	echo Calling windeployqt
	windeployqt --compiler-runtime --dir %depsDataDir% "%builddir%\bin\entropypianotuner.exe" || exit 1
	
	copy /Y "%builddir%\bin\fftw3.dll" %depsDataDir% || exit 1
	copy /Y "%builddir%\bin\qwt.dll" %depsDataDir% || exit 1
	copy /Y "%builddir%\bin\uv.dll" %depsDataDir% || exit 1
	
	:: Additional Qt dlls due to bug in windeployqt
	copy /Y "%QTDIR%\bin\Qt5OpenGL.dll" %depsDataDir% || exit 1
	copy /Y "%QTDIR%\bin\Qt5PrintSupport.dll" %depsDataDir% || exit 1
	
	:: Midi Plugin is not deployed aswell
	mkdir %depsDataDir%\midi
	copy /Y "%QTDIR%\plugins\midi\qtwinmm_midi.dll" %depsDataDir%\midi || exit 1
	
	if %compiler% EQU msvc (
		:: move vcredist_xxx to vcredist
		echo Moving vcredist
		del %depsDataDir%\vcredist.exe
		move %depsDataDir%\vcredist_%vcredist%.exe %depsDataDir%\vcredist.exe || exit 1
	) else (
		:: move the mingw runtime done by Qt
		echo MinGW runtime moved
	)
	
	echo Updating dependencies finished.
)

:: if build, then copy new exe
if %build%==1 (
	echo Copying built exe and icon started.
	rd %appDataDir% /s /q
	mkdir %appDataDir% || exit /b
	copy /Y %builddir%\bin\entropypianotuner.exe %appDataDir% || exit 1
	copy /Y %builddir%\bin\core.dll %appDataDir% || exit 1
	copy /Y %tunerdir%\appstore\icons\entropytuner.ico %appDataDir% || exit 1
	echo Copying build exe and icon finished.
	
	echo Copying common algorithms started.
	rd %commonAlgsDataDir% /s /q
	mkdir %commonAlgsDataDir% || exit 1
	mkdir %commonAlgsDataDir%\algorithms || exit 1
	copy /Y %builddir%\bin\algorithms\entropyminimizer.dll %commonAlgsDataDir%\algorithms || exit 1
	copy /Y %builddir%\bin\algorithms\pitchraise.dll %commonAlgsDataDir%\algorithms || exit 1
	copy /Y %builddir%\bin\algorithms\resettorecording.dll %commonAlgsDataDir%\algorithms || exit 1
	echo Copying common algorithms done.
)

:: cd to installer
cd %tunerdir%\appstore\installer
:: create online installer packages
if %updatePackages%==1 (
	echo Updating packages started. This may take some time
	
	rd %repository% /s /q
	repogen -p packages %repository% || exit 1
	
	echo Updating packages finished.
)
if %createInstaller%==1 (
	echo Creating installer started.
	:: cd to installer
	cd %tunerdir%\appstore\installer
	
	del %relConfigFile%
	:: create windows config pointing to the correct repository
	call %tunerdir%\scripts\windows\BatchSubstitute.bat dummy_repository %repository% config\config.xml > %relConfigFile%.tmp || exit 1
	:: correct path for windows for default install dir
	call %tunerdir%\scripts\windows\BatchSubstitute.bat "<TargetDir>@HomeDir@/EntropyPianoTuner</TargetDir>" "<TargetDir>@ApplicationsDir@/EntropyPianoTuner</TargetDir>" %relConfigFile%.tmp > %relConfigFile% || exit 1
	
	if %compiler% EQU msvc (
		:: use correct script
		copy /Y scripts\deps_installscript_msvc.qs packages\org.entropytuner.deps\meta\installscript.qs || exit 1
	) else (
		:: use dummy script
		copy /Y scripts\deps_installscript_dummy.qs packages\org.entropytuner.deps\meta\installscript.qs || exit 1
	)
	
	:: create online/offline installer
	binarycreator --%installer_type% -v -c %relConfigFile% -p packages %setupname% || exit 1

	:: move installer to publish dir
	cd %tunerdir%
	mkdir publish
	cd publish
	echo Moving installer to %CD%\%setupname%
	move %tunerdir%\appstore\installer\%setupname% . || exit 1
	
	echo Creating installer finished.
)

if %publish%==1 (
	echo Publishing updates started.
	@echo off
	C:
	chdir %CygwinBin%
	bash.exe -o igncr "%tunerdir%\scripts\windows\windowsUpload.sh" || exit 1
	@echo on
	echo Publishing updates finished.
)

endlocal
