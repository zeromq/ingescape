@echo Modifier certains de czmq/zyre pour la compilation windows ...
@echo off
REM copier les lignes qu'il faut dans le fichier czmq_selftest.vcxproj
fart.exe czmq\builds\msvc\vs2015\czmq_selftest\czmq_selftest.vcxproj "<ClCompile Include=\"..\..\..\..\src\czmq_selftest.c\" />" "<ClCompile Include=\"..\..\..\..\src\czmq_selftest.c\" /><ClCompile Include=\"..\..\..\..\src\czmq_private_selftest.c\" /><ClCompile Include=\"..\..\..\..\src\zgossip_msg.c\" />"
REM copy build.bat for CZMQ so it uses vs2017 with vs2015 toolchain.
xcopy /Y "filesToCopy\build.bat" "czmq\builds\msvc\vs2015\"
REM copy build.bat for CZMQ so it uses vs2017 with vs2015 toolchain.
xcopy /Y "filesToCopy\buildbase.bat" "zyre\builds\msvc\build\"