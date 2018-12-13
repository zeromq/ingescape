@echo off
rem Batch to compile ingescape on windows
rem To compile ingescape with windows juste edit next 4 PATHS
rem Qt5_DIR is the path to cmake qt dir
rem IGS_*_DIRECTORY is the path to czmq, sodium, zmq, yajl and zyre includes and libraries
rem CMAKE_EXE is the path to cmake exe
rem Set HOST_ARCH empty to compile in 32bits system

rem Define configurables path
set Qt5_DIR="D:/Qt/Qt5.9.6/5.9.6/msvc2015/lib/cmake/Qt5"
set IGS_INCLUDE_DIRECTORY="C:\Program Files\Common Files\Ingescape\include\"
set IGS_LIBRARY_DIRECTORY="C:\Program Files\Common Files\Ingescape\x86_64\"
set CMAKE_EXE="c:\Program Files\CMake\bin\cmake.exe"
set HOST_ARCH=Win64

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
%CMAKE_EXE% -G"Visual Studio 14 2015 %HOST_ARCH%" ..
rem Compile ingescape
%CMAKE_EXE% --build . --target ALL_BUILD --config Release

rem Come back to the previous directory
cd "%mypath%"
