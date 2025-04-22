Every window implementation should derive from Window interface.
Folder structure: 
- backend / libName / (Add platform specific subfolders if needed)
- backend / platformName / (platformImplementation)

function Window::createWindow should implement every backend based on
compilation flags.