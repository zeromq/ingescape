#!/bin/sh
#Homebrew has to be installed on your system before continuing.
#Go to https://brew.sh

#The following line will install ZMQ ingeScape dependencies in /usr/local
brew install libsodium zyre

#The following lines will install the Yajl JSON parser
#based on the source code in ../yajl (patched version)
#Warning: cmake is required for the compilation
cd ../yajl
mkdir build
cd build
cmake ..
make
sudo make install
cd ..
rm -Rf build
cd ../macos

