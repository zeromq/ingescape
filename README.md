#Ingescape library

This is the main repo for the Ingescape library.

All dependencies are accessible as submodules.  
Compilation is supported via cmake, Xcode, Qt and Visual Studio.

## Helper scripts
Two scripts are provided at root of the repo:

- **bootstrapSysrootAndGenerateXcodeProjects.sh** is run without any parameter. It creates a sysroot directory at the same level as the repo to compile the library and its dependencies for use by other projects. This script also creates the Xcode projects for the dependencies, which are necessary for the main Xcode project located in builds/xcode and specific to each repo location.

- **cloneFromIngescapeGitlab.sh** is run with parameters and shows a contextual help if run without any. This script helps cloning repos for the ingescape gitlab server with the proper position in the directories tree so that they will find the ingescape library with additional configuration.

## Manual build
To build ingescape and its dependencies manually via CMake, here are the steps:

```
cd <clone_dir>
cmake -S . -B build_dir -DCMAKE_INSTALL_PREFIX=<path/to/installation>
make -C build_dir install
```
The variable `CMAKE_INSTALL_PREFIX` let you specify the directory in which ingescape and its dependencies will be installed.  
The default value is system dependendant and may require administrator/superuser priviledges.  
*It is advised on a development environnement to install ingescape and its dependencies on a separate directory, controlled by your user, to avoid conflicts with any library that may already be installed on yor system.*

Once installed, you can link against ingescape and its dependencies by adding the installation path to the `CMAKE_PREFIX_PATH` of your project. Example for a CMake project:

```
cmake -S . -B build_dir -DCMAKE_PREFIX_PATH=<path/to/installation>
```
If ingescape and its dependencies are installed on a standard directory on your system (e.g. `/usr/local/lib` on \*nix systems), you may not have to specify any `CMAKE_PREFIX_PATH` for CMake to find ingescape.

