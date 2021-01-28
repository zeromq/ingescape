This is the main repo for the Ingescape library.

All dependencies are accessible as submodules. 
Compilation is supported via cmake, Xcode, Qt and Visual Studio.

Two scripts are provided at root of the repo:

- **bootstrapSysrootAndGenerateXcodeProjects.sh** is run without any parameter. It creates a sysroot directory at the same level as the repo to compile the library and its dependencies for use by other projects. This script also creates the Xcode projects for the dependencies, which are necessary for the main Xcode project located in builds/xcode and specific to each repo location.

- **cloneFromIngescapeGitlab.sh** is run with parameters and shows a contextual help if run without any. This script helps cloning repos for the ingescape gitlab server with the proper position in the directories tree so that they will find the ingescape library with additional configuration.
