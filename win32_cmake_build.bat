@echo off
rem Batch to compile ingescape on windows
rem To compile ingescape with windows juste edit next 3 PATHS
rem IGS_*_DIRECTORY is the path to czmq, sodium, zmq, yajl and zyre includes and libraries
rem CMAKE_EXE is the path to cmake exe
rem Project is configured for 64bits system. Remove Win64 in -G"Visual Studio 14 2015 Win64" for 32bits

rem Define configurables path
set IGS_INCLUDE_DIRECTORY="C:\Program Files\Common Files\Ingescape\include\"
set IGS_LIBRARY_DIRECTORY="C:\Program Files\Common Files\Ingescape\x86_64\"
set CMAKE_EXE="c:\Program Files\CMake\bin\cmake.exe"

rem Keep current location and script location
set mypath=%cd%
set batchpath=%~dp0

rem Check if build folder exist
if exist "%batchpath%\build" (
	echo Build folder exist, cleaning it
	del /s /q "%batchpath%\build\*"
) else (
	echo Create build folder
	mkdir "%batchpath%\build"
)

rem Move in build folder
cd "%batchpath%\build"

rem Launch cmake command
%CMAKE_EXE% -G"Visual Studio 14 2015 Win64" .. -DCZMQ_LIBRARIES="%IGS_LIBRARY_DIRECTORY%libczmg.dll.a" -DCZMQ_INCLUDE_DIRS="%IGS_INCLUDE_DIRECTORY%" -DLIBSODIUM_LIBRARIES="%IGS_LIBRARY_DIRECTORY%libsodium.dll.a" -DLIBSODIUM_INCLUDE_DIRS="%IGS_INCLUDE_DIRECTORY%" -DLIBZMQ_LIBRARIES="%IGS_LIBRARY_DIRECTORY%libzmq.dll.a" -DLIBZMQ_INCLUDE_DIRS="%IGS_INCLUDE_DIRECTORY%" -DYAJL_LIBRARIES="%IGS_LIBRARY_DIRECTORY%libyajl.dll.a" -DYAJL_INCLUDE_DIRS="%IGS_INCLUDE_DIRECTORY%"
rem Compile ingescape
%CMAKE_EXE% --build . --target ALL_BUILD --config Release

rem Come back to the previous directory
cd "%mypath%"
