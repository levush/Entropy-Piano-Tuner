@echo off

:: load the user environment variables
call windowsstore_env.user.bat

:: the platform is irrelevant use x64
set sdkdir=%WinKitPath%\x64
set publisherName="CN=1A48CBB8-649C-4CBD-908D-A767026541C7"
::set expirationDate=01/01/2020
set keyName=entropypianotuner_publish_key

:: loop through arguments
: argsloop
IF NOT "%1"=="" (
    IF "%1"=="-p" (
        SET password=%2
        SHIFT
    )
    SHIFT
    GOTO :argsloop
)

IF "%password%" == "" (
    echo Error:
	echo Set passwort via -p
	goto :eof
)

%sdkdir%\MakeCert /n %publisherName% /r /h 0 /eku "1.3.6.1.5.5.7.3.3,1.3.6.1.4.1.311.10.3.13" /sv %keyName%.pvk %keyName%.cer
%sdkdir%\Pvk2Pfx /pvk %keyName%.pvk /pi %password% /spc %keyName%.cer /pfx %keyName%.pfx