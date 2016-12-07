function Component()
{
}

// here we are creating the operation chain which will be processed at the real installation part later
Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows") {
		// register file type
        component.addOperation("RegisterFileType",
                               "ept",
                               "@TargetDir@\\entropypianotuner.exe '%1'",
                               "Entropy Piano Tuner file",
                               "application/xml",
                               "@TargetDir@\\entropytuner.ico");
    
		// create desktop/startmenu entry
		component.addOperation("CreateShortcut",
                               "@TargetDir@/maintenancetool.exe",
                               "@StartMenuDir@/Uninstall Entropy Piano Tuner.lnk",
                               "workingDirectory=@TargetDir@",
                               "iconPath=@TargetDir@\\entropytuner.ico");
		component.addOperation("CreateShortcut",
                               "@TargetDir@/entropypianotuner.exe",
                               "@DesktopDir@/Entropy Piano Tuner.lnk",
                               "workingDirectory=@TargetDir@", 
                               "iconPath=@TargetDir@\\entropytuner.ico");
		component.addOperation("CreateShortcut",
                               "@TargetDir@/entropypianotuner.exe",
                               "@StartMenuDir@/Entropy Piano Tuner.lnk",
                               "workingDirectory=@TargetDir@", 
                               "iconPath=@TargetDir@\\entropytuner.ico");
    }
    if (systemInfo.kernelType === "linux") {
        // create desktop entry
        component.addOperation("CreateDesktopEntry",
                               "entropypianotuner.desktop",
                               "Version=1.0\nType=Application\nTerminal=false\nExec=@TargetDir@/entropypianotuner.sh %f\nName=EntropyTuner\nIcon=@TargetDir@/icon.png\nName[en_US]=Entropy Piano Tuner\nMimeType=application/ept;\n");
        component.addOperation("Execute", "mkdir", "-p", "@HomeDir@/.local/share/mime/packages");
        component.addOperation("Execute", "mkdir", "-p", "@HomeDir@/.local/share/icons");
        component.addOperation("Move", "@TargetDir@/entropypianotuner-mime.xml", "@HomeDir@/.local/share/mime/packages/entropypianotuner-mime.xml");
        component.addOperation("Copy", "@TargetDir@/icon.png", "@HomeDir@/.local/share/icons/application-ept.png");
        component.addOperation("Execute", "update-mime-database", "@HomeDir@/.local/share/mime");
    }
}
