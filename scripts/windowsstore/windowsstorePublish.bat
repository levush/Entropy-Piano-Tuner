@echo off

set password=

:: loop through arguments
: argsloop
IF NOT "%1"=="" (
	if "%1"=="-pwd" (
		set password=%2
		SHIFT
	)
    SHIFT
    GOTO :argsloop
)

if "%password%"=="" (
	echo Error:	Required certificate password.
	echo Set password of the certificate via "-pwd password"
	goto :eof
)

call winstorebuild.bat -winkitarch=x64 -vcplatform x64 -qtversion winrt_x64_msvc2015 -msvc_spec winrt-x64-msvc2015 -c -b -p -pwd %password%
call winstorebuild.bat -winkitarch=x86 -vcplatform x86 -qtversion winrt_x86_msvc2015 -msvc_spec winrt-x86-msvc2015 -c -b -p -pwd %password%
call winstorebuild.bat -winkitarch=x64 -vcplatform amd64_arm -qtversion winrt_armv7_msvc2015 -msvc_spec winrt-arm-msvc2015 -c -b -p -pwd %password%