:: create 32 and 64 bit installers
call windowsPlatformPublish.bat -p x86 -c -b -up -ud -ut -i -publish || exit /b
:: call windowsPlatformPublish.bat -p x64 -c -b -up -ud -ut -i -publish || exit /b