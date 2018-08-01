@echo Modifier certains de czmq pour la compilation windows ...
@echo OFF
REM copier les lignes qu'il faut dans le fichier czmq_selftest.vcxproj
fart.exe czmq\builds\msvc\vs2015\czmq_selftest\czmq_selftest.vcxproj "<ClCompile Include=\"..\..\..\..\src\czmq_selftest.c\" />" "<ClCompile Include=\"..\..\..\..\src\czmq_selftest.c\" /><ClCompile Include=\"..\..\..\..\src\czmq_private_selftest.c\" /><ClCompile Include=\"..\..\..\..\src\zgossip_msg.c\" />"