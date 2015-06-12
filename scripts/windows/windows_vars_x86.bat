:: Set required variables for win x86 build
set vcredist=x86
set postfix=%vcredist%
path=%path%;%QtDir_x86%;%QtIFW%

:: set the build platform variables from visual studio compiler
call %VSPath%\VC\vcvarsall.bat %vcredist% || exit /b
