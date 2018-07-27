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

:modifyfiles
CALL modify_files.bat


:compilation
CALL compilation-win.bat
