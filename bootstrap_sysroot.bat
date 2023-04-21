@echo off

SET SCRIPT_DIR=%CD%

cd %SCRIPT_DIR%

SET IGS=%CD%
cd ..
SET CODEROOT=%CD%
echo This script will compile ingescape and its dependencies in a local sysroot directory at %CODEROOT%/sysroot/Program Files/Ingescape/library. This is a nice way to have everything you need to debug/contribute to Ingescape without installing anything on your system.
cd %IGS%

set /p choice=Do you want to continue? [yN]
if '%choice%'=='y' goto run
if '%choice%'=='Y' goto run
goto return

:run
REM make sure all submodules are here
git submodule update --init --recursive 
mkdir %CODEROOT%/sysroot

echo Deploying ingescape library and its dependencies in %CODEROOT%/sysroot...
mkdir build
cmake -S . -B build -A x64 -DCMAKE_INSTALL_PREFIX="%CODEROOT%/sysroot/Program Files/Ingescape/library" -DCMAKE_BUILD_TYPE=Release -DWITH_DEPS=ON
cmake --build build --target ALL_BUILD --config Release -- /nologo /verbosity:minimal /maxcpucount 
cmake --install build --config Release
rmdir /s /q build

cd ..

:return
EXIT /B

