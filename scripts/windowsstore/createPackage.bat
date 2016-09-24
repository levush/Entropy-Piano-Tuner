:: This file is used to create a appx package that contains the packed application
::
:: for more information see
:: https://msdn.microsoft.com/en-us/library/windows/desktop/hh446767(v=vs.85).aspx#create_package_using_mapfile

:: load the user environment variables
call windowsstore_env.user.bat

set sdkBinDir=""
set builddir=""
set cert_pfx="entropypianotuner_publish_key.pfx"
set cert_cer="entropypianotuner_publish_key.cer"
set cert_pass="unset"

:: loop through arguments
: argsloop
IF NOT "%1"=="" (
    IF "%1"=="-k" (
        SET sdkBinDir=%2
        SHIFT
    )
	if "%1"=="-d" (
		set builddir=%2
		SHIFT
	)
	if "%1"=="-pwd" (
		set cert_pass=%2
		SHIFT
	)
	if "%1"=="-postfix" (
		set postfix=%2
		SHIFT
	)
    SHIFT
    GOTO :argsloop
)

mkdir %publishdir%

cd %windowsstoredir%

:: create the mapping file
call %tunerdir%\\scripts\\windows\\BatchSubstitute.bat @BUILD_DIR@ %builddir% %windowsstoredir%\Mapping.template > Mapping.txt

set appx=%publishdir%\\%outname%_%postfix%.appx
:: create a package out of the files that are defined in the mapping.txt (/o to overwrite existing)
%sdkBinDir%\\makeappx.exe pack /o /f Mapping.txt /p %appx%

:: sign the package with the cert
%sdkBinDir%\\signtool.exe sign /a /fd SHA256 /p %cert_pass% /f %cert_pfx% /v %appx%

:: bundle the package (todo maybe several platforms x86/x64/arm/armv7 at once possible?)
:: %sdkBinDir%\makeappx.exe bundle /f BundleMapping.txt /p %outname%.appxbundle
