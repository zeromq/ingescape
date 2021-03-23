# Ingescape library

This is the main repository for the Ingescape library.

All dependencies are accessible as submodules.  
Compilation is supported via cmake, Xcode, Qt and Visual Studio.

## Helper scripts
Two scripts are provided at root of the repo:

- **bootstrapSysrootAndGenerateXcodeProjects.sh** is run without any parameter. It creates a sysroot directory at the same level as the repo to compile the library and its dependencies for use by other projects. This script also creates the Xcode projects for the dependencies, which are necessary for the main Xcode project located in builds/xcode and specific to each repo location.

- **cloneFromIngescapeGitlab.sh** is run with parameters and shows a contextual help if run without any. This script helps cloning repos for the ingescape gitlab server with the proper position in the directories tree so that they will find the ingescape library with additional configuration.

## Manual build
### On macOS/linux environnements
To build ingescape and its dependencies manually via CMake, here are the steps:  
```
cd <clone_dir>
cmake -S . -B build_dir
make -C build_dir DESTDIR=<path/to/installation> install
```
The variable `DESTDIR` on the install step let you specify the directory in which ingescape and its dependencies will be installed.  
The given path is an installation prefix. The library will finally be installed under `<path/to/installation>/usr/local/lib`, `<path/to/installation>/usr/local/bin`, etc.  
The default value is system dependendant and may require administrator/superuser priviledges.  
**It is advised on a development environnement to install ingescape and its dependencies on a separate directory, controlled by your user, to avoid conflicts with any library that may already be installed on yor system.**

### On Windows environnements (with Visual Studio)
The build process is similar to *nix systems but you will need to specify the generator, i.e. which version of Visual Studio to use.  
Example for Visual Studio 2017 (x64) target:  
```
cd <clone_dir>
cmake -S . -B build_dir -G"Visual Studio 15 2017 Win64"
cmake --build build_dir --target ALL_BUILD --config <Debug/Release>
cmake --install build_dir --config <Debug/Release> --prefix <path/to/installation>
```
If the `-G` argument is unknown, CMake will list all the available generators it could find on your environmenent.

The first cmake command will generate under `build_dir` a complete Visual Studio solution (`ingescape.sln`) that regroup ingescape itself and all its dependencies.  
You can open it in Visual Studio then select which configuration to build (e.g. Debug, Release, etc.).  
The special project `ALL_BUILD` will build all the subprojects, including the dependencies.

**NB: You cannot generate from multiple CMake generation (i.e. version of VS) inside the same directory.**

### Link against ingescape
Once installed, you can link against ingescape and its dependencies by adding the installation path to the `CMAKE_PREFIX_PATH` of your project.  
Example for a CMake project on a *nix environment:
```
cmake -S . -B build_dir -DCMAKE_PREFIX_PATH=<path/to/installation>/usr/local
```
Example for a CMake project opn a Windows environment:
```
cmake -S . -B build_dir -G"Visual Studio 15 2017 Win64" -DCMAKE_PREFIX_PATH=<path/to/installation>
```
If ingescape and its dependencies are installed on a standard directory on your system (e.g. `/usr/local/lib` on \*nix systems), you may not have to specify any `CMAKE_PREFIX_PATH` for CMake to find ingescape.

