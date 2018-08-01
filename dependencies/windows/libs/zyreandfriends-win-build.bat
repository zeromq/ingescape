@echo clonage des projets zyre, czmq, zmq et sodium
@echo OFF
:setVersion
set /p zmqVersion="enter the version of ZMQ (eg. 4.2.3) : "
set /p czmqVersion="enter the version of czmq (eg. 4.1.0) : "
set /p zyreVersion="enter the version of zyre (eg. 2.0.0) : "

@echo version of zyre : %zyreVersion%, version of czmq : %czmqVersion%, version of zmq : %zmqVersion%
@echo off
:choice
set /P c=Are you sure the versions of zyre and friends are Ok [y/n]?
if /I "%c%" EQU "y" goto :clonage
if /I "%c%" EQU "n" goto :setVersion
goto :choice

:clonage
@echo off
git clone --depth 1 -b stable https://github.com/jedisct1/libsodium.git
git clone git://github.com/zeromq/libzmq.git
cd libzmq
git checkout tags/v%zmqVersion%
cd ../
git clone git://github.com/zeromq/czmq.git
cd czmq
git checkout tags/v%czmqVersion%
cd ../
git clone  git://github.com/zeromq/zyre.git 
cd zyre
git checkout tags/v%zyreVersion%
cd ../

:choiceVS
set /P c=Which Visual Studio version do you want to use [2015/2017]?
if /I "%c%" EQU "2015" goto :modifyfiles2015
if /I "%c%" EQU "2017" goto :modifyfiles2017
goto :choiceVS

:modifyfiles2015
CALL modify_files_vs2015.bat
goto :compilation2015

:modifyfiles2017
CALL modify_files_vs2017.bat
goto :compilation2017


:compilation2015
CALL compilation_vs2015.bat

:compilation2017
CALL compilation_vs2017.bat
