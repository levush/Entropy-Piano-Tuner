:: pass all arguments to call with %*
::call winstorebuild.bat -winkitarch=x64 -vcplatform x64 -qtversion winrt_x64_msvc2015 -msvc_spec winrt-x64-msvc2015 -p %*
::call winstorebuild.bat -winkitarch=x86 -vcplatform x86 -qtversion winrt_x86_msvc2015 -msvc_spec winrt-x86-msvc2015 -p %*
call winstorebuild.bat -winkitarch=x64 -vcplatform amd64_arm -qtversion winrt_armv7_msvc2015 -msvc_spec winrt-arm-msvc2015 -p %*