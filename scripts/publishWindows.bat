:: basic system environment
set QtIFW="C:\Qt\QtIFW2.0.0_nightly\bin"
set QtCreatorPath="C:\Qt\Tools\QtCreator\bin"
set VSPath="C:\Program Files (x86)\Microsoft Visual Studio 12.0"
set msvc_spec=win32-msvc2013

:: create 32 and 64 bit installers
call publishWindowsPlatform.bat x86 "C:\Qt\5.4\msvc2013\bin" %msvc_spec% %QtIFW% %VSPath% %QtCreatorPath% || exit /b
call publishWindowsPlatform.bat x64 "C:\Qt\5.4\msvc2013_64\bin" %msvc_spec% %QtIFW% %VSPath% %QtCreatorPath% || exit /b