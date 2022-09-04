#!/bin/sh

set -e
#sudo xcode-select -switch /Applications/Xcode.app #adapt path if needed, according to 'xcode-select --print-path'
rm -Rf build; rm -Rf lib
mkdir -p lib/static
cmake -S../../ -B./build -G Xcode -DCMAKE_TOOLCHAIN_FILE=./builds/ios/ios.toolchain.cmake -DPLATFORM=OS64 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=./sysroot -DOSX_UNIVERSAL=ON -DWITH_DEPS=ON -T buildsystem=1 -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM=56DRT9MG9S
cd build
xcodebuild -list -project ingescape.xcodeproj
xcodebuild -project ingescape.xcodeproj -target ingescape-static -configuration Debug build
xcodebuild -project ingescape.xcodeproj -target ingescape -configuration Release build
cp Release-iphoneos/libingescape.dylib ../lib
cp dependencies/sodium/Release-iphoneos/libsodium.dylib ../lib
cp dependencies/libzmq/lib/Release/libzmq.dylib ../lib
cp dependencies/czmq/Release-iphoneos/libczmq.dylib ../lib
cp dependencies/zyre/Release-iphoneos/libzyre.dylib ../lib
libtool -static -o ../lib/static/libingescape.a Debug-iphoneos/libingescape.a dependencies/sodium/Debug-iphoneos/libsodium.a dependencies/libzmq/lib/Debug/libzmq.a dependencies/czmq/Debug-iphoneos/libczmq.a dependencies/zyre/Debug-iphoneos/libzyre.a
cd ..
rm -Rf build
