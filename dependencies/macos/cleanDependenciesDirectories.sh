#!/bin/sh
cd ../../../libsodium
rm -Rf src/libsodium/libs
cd ../libzmq
rm -Rf builds/xcode
cd ../czmq
rm -Rf builds/xcode
cd ../zyre
rm -Rf builds/xcode

