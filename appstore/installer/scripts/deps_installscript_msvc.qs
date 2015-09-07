function Component()
{
}

// install the vcredist under windows
Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.kernelType === "winnt") {
		// install vs redist
		component.addElevatedOperation("Execute", "@TargetDir@\\vcredist.exe", "/install", "/quiet");
		// let the vsredist installed, this might be useful later
		// to uninstall add: "UNDOEXECUTE", "@TargetDir@\\vcredist.exe", "/uninstall", "/quiet" as params to the upper cmd
		
    }
    if (systemInfo.kernelType === "linux") {
    }
}
