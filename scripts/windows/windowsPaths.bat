:: cd to tuner
cd ../..

set postfix=%1

set tunerdir=%CD%
set builddir=%tunerdir%\.publishWindows%postfix%
set setupname=EntropyPianoTuner_Windows_online_%postfix%.exe

set appDataDir=%tunerdir%\appstore\installer\packages\org.entropytuner.app\data
set depsDataDir=%tunerdir%\appstore\installer\packages\org.entropytuner.deps\data
set relConfigFile=config\windows_config_%postfix%.xml
set repository=windows_repository_%postfix%
