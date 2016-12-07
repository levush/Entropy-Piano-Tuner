:: cd to tuner
cd ../..

set postfix=%1

set tunerdir=%CD%
set builddir=%tunerdir%\.publishWindows%postfix%
set setupname=EntropyPianoTuner_Windows_%installer_type%_%postfix%.exe

set appDataDir=%tunerdir%\appstore\installer\packages\org.entropytuner.app\data
set depsDataDir=%tunerdir%\appstore\installer\packages\org.entropytuner.deps\data
set commonAlgsDataDir=%tunerdir%\appstore\installer\packages\org.entropytuner.algorithms\data
set relConfigFile=config\windows_config_%postfix%.xml
set repository=windows_repository_%postfix%
