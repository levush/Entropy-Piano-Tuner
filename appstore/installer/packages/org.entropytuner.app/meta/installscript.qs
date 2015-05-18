function Component()
{
}

// here we are creating the operation chain which will be processed at the real installation part later
Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.kernelType === "winnt") {
		// install vs redist
		component.addElevatedOperation("Execute", "@TargetDir@\\vcredist.exe", "/install", "/quiet");
		// let the vsredist installed, this might be useful later
		// to uninstall add: "UNDOEXECUTE", "@TargetDir@\\vcredist.exe", "/uninstall", "/quiet" as params to the upper cmd
		
		// register file type
        component.addOperation("RegisterFileType",
                               "ept",
                               "@TargetDir@\\EntropyTuner.exe '%1'",
                               "Entropy Piano Tuner file",
                               "application/xml",
                               "@TargetDir@\\entropytuner.ico",
                               "ProgId=entropytuner.ept");
	
	// create desktop entry
	component.addOperation("CreateShortcut", "@TargetDir@/EntropyTuner.exe", "@DesktopDir@/Entropy Piano Tuner.lnk");
	component.addOperation("CreateShortcut", "@TargetDir@/EntropyTuner.exe", "@StartMenuDir@/Entropy Piano Tuner.lnk");
	// component.addOperation("CreateShortcut", "@TargetDir@/maintenancetool.exe", "@StartMenuDir@/maintenancetool.lnk", "iconPath=%TargetDir%\\entropytuner.ico");
    }
    if (systemInfo.kernelType === "linux") {
	// create desktop entry
	component.addOperation("CreateDesktopEntry",
			       "EntropyPianoTuner.desktop",
			       "Version=1.0\nType=Application\nTerminal=false\nExec=@TargetDir@/EntropyTuner.sh %f\nName=EntropyTuner\nIcon=@TargetDir@/icon.png\nName[en_US]=Entropy Piano Tuner\nMimeType=application/ept;\nActions=Gallery;Create;");
	component.addOperation("Execute", "mkdir", "-p", "@HomeDir@/.local/share/mime/packages");
        component.addOperation("Execute", "mkdir", "-p", "@HomeDor@/.local/share/icons");
	component.addOperation("Move", "@TargetDir@/EntropyTunerMIME.xml", "@HomeDir@/.local/share/mime/packages/EntropyPianoTuner.xml");
	component.addOperation("Copy", "@TargetDir@/icon.png", "@HomeDir@/.local/share/icons/application-ept.png");
	component.addOperation("Execute", "update-mime-database", "@HomeDir@/.local/share/mime");
    }
}
