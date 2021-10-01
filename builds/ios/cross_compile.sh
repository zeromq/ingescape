#!/bin/sh

set -e

cmake -S../../ -B./build -GXcode \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_PREFIX_PATH=./sysroot/ \
    "-DCMAKE_OSX_ARCHITECTURES=x86_64 arm64" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
    "-DCMAKE_OSX_SYSROOT=iphoneos" \
    -DCMAKE_INSTALL_PREFIX=`pwd`/sysroot \
    -DCMAKE_XCODE_GENERATE_SCHEME=YES \
    -DCMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH=NO \
    -DCMAKE_IOS_INSTALL_COMBINED=NO \
    -DWITH_DEPS=ON \
    -DWITH_PERF_TOOL=OFF \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM=56DRT9MG9S

cmake --build build --config Debug --target sodium
cmake --build build --config Debug --target sodium-static
cmake --build build --config Debug --target libzmq
cmake --build build --config Debug --target libzmq-static
cmake --build build --config Debug --target czmq
cmake --build build --config Debug --target czmq-static
cmake --build build --config Debug --target zyre
cmake --build build --config Debug --target zyre-static
cmake --build build --config Debug --target ingescape
cmake --build build --config Debug --target ingescape-static
cmake --install build --config Debug
