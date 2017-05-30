[WINDOWS]
1)Open the project in Qt Creator
2)Choose a compilation Kit based on MSVC 2015 32 Bits : The project is fully tested with "Qt 5.6.0 MSVC2015 32bit"
3)Use the step "install" in argument of Make
4)Build the project
Notes : The librairy and its dependencies will be copied in "C:\mastic":
		- include : contains all headers
		- lib/debug : all the librairies for debugging
		- lib/release : all the librairies to use in release notes

	TODO : explain how to link with the windows agent


[RASPBERRY]
1)Open the project in Qt Creator
2)Choose a compilation Kit based on Desktop
3)Use the step "install" in argument of Make
4)Build the project
Notes : The librairy and its dependencies will be copied in "usr/local":
		- include : contains all headers
		- lib : all the librairies
		
Informations :
if you meet the error : "Makefile warning Warning File `mastic.pro' has modification time XXXX s in the future"
Run in a terminal from the root directory (MASTIC) : sudo find -type f -exec touch {} +

if you meet the error : error while loading shared libraries: libmastic.so.1: cannot open shared object file: No such file or directory
Run in a terminal : sudo ldconfig -v