set CURRENTDIR=%cd%
cd C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC
CALL vcvarsall.bat

cd %CURRENTDIR%\libsodium\builds\msvc\build
CALL buildbase.bat ..\vs2015\libsodium.sln 14
ECHO
cd %CURRENTDIR%

cd libzmq\builds\msvc\
cd build
CALL buildbase.bat ..\vs2015\libzmq.sln 14
cd %CURRENTDIR%


cd czmq\builds\msvc
CALL configure.bat
cd vs2015
CALL build.bat
cd %CURRENTDIR%


cd zyre\builds\msvc\
CALL configure.bat
cd build
CALL buildbase.bat ..\vs2015\zyre.sln 14
cd %CURRENTDIR%

REM copie de tous les librairies dans un dossier dédié
@echo Start the copy in a same folder of all librairies ... 
@echo OFF
mkdir Win32
cd Win32
mkdir Debug
mkdir Release
cd ../

REM copier la librairie sodium
xcopy libsodium\bin\Win32\Debug\v140\dynamic\libsodium.* Win32\Debug
xcopy libsodium\bin\Win32\Release\v140\dynamic Win32\Release

REM copier la librairie zmq
xcopy libzmq\bin\Win32\Debug\v140\dynamic\libzmq.* Win32\Debug
xcopy libzmq\bin\Win32\Release\v140\dynamic\libzmq.* Win32\Release

REM copier la librairie czmq
xcopy czmq\bin\Win32\Debug\v140\dynamic\libczmq.* Win32\Debug
xcopy czmq\bin\Win32\Release\v140\dynamic\libczmq.* Win32\Release

REM copier la librairie zyre
xcopy zyre\bin\Win32\Debug\v140\dynamic\libzyre.* Win32\Debug
xcopy zyre\bin\Win32\Release\v140\dynamic\libzyre.* Win32\Release

@echo You can find the librairies in the local folder "Win32\Debug" OR "Win32\Release" 
@echo OFF

pause
