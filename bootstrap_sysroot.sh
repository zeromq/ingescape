#!/bin/sh

BUILD_TYPE="Debug"
SCRIPT_DIR=$(cd `dirname $0`; pwd)

(
    cd $SCRIPT_DIR
    IGS=$PWD
    cd ..
    CODEROOT=$PWD
    
    echo "This script will compile ingescape and its dependencies in a local sysroot directory at $CODEROOT/sysroot and create xcode projects for each dependency using cmake. This is a nice way to have everything you need to debug/contribute to Ingescape without installing anything on your system."
    cd $IGS
    read -p "Do you want to continue? [yN] " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]
    then
        [[ "$0" = "$BASH_SOURCE" ]] && exit 1 || return 1
    fi

    git submodule update --init --recursive #make sure all dependecies are here
    mkdir -p $CODEROOT/sysroot

    echo "Deploying ingescape library and its dependencies in $CODEROOT/sysroot..."
    mkdir build
    cmake -S . -B build -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_INSTALL_PREFIX=$CODEROOT/sysroot/usr/local/ -DOSX_UNIVERSAL=ON -DCMAKE_OSX_DEPLOYMENT_TARGET=10.11 -DWITH_DEPS=ON
    make -j8 -C build install
    rm -Rf build

    if [ -e dependencies/sodium ] && [[ $OSTYPE == "darwin"* ]]; then
        echo "Building libsodium xcode project..."
        cd dependencies/sodium
        rm -Rf builds/xcode
        mkdir -p builds/xcode
        cmake -S . -B builds/xcode -DCMAKE_BUILD_TYPE=$BUILD_TYPE -G "Xcode" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.11
        #Hack to enable xcode project embedding because scripts coming with
        #the xcode project do not manage the target folder properly:
        mkdir -p builds/xcode/Debug
        cp $CODEROOT/sysroot/usr/local/lib/libsodium.* builds/xcode/Debug
        mkdir -p builds/xcode/Release
        cp $CODEROOT/sysroot/usr/local/lib/libsodium.* builds/xcode/Release
        cd ../..
    fi

    if [ -e dependencies/libzmq ] && [[ $OSTYPE == "darwin"* ]]; then
        echo "Building libzmq xcode project..."
        cd dependencies/libzmq
        rm -Rf builds/xcode
        mkdir -p builds/xcode
        cmake -S . -B builds/xcode -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DWITH_DOCS=OFF -DENABLE_CURVE=ON -DWITH_LIBSODIUM=ON -DWITH_TLS=OFF -DCMAKE_PREFIX_PATH=$CODEROOT/sysroot/usr/local/ -G "Xcode" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.11
        #Hack to enable xcode project embedding because scripts coming with
        #the xcode project do not manage the target folder properly:
        mkdir -p builds/xcode/lib/Debug
        cp $CODEROOT/sysroot/usr/local/lib/libzmq.* builds/xcode/lib/Debug
        mkdir -p builds/xcode/lib/Release
        cp $CODEROOT/sysroot/usr/local/lib/libzmq.* builds/xcode/lib/Release
        cd ../..
    fi

    if [ -e dependencies/czmq ] && [[ $OSTYPE == "darwin"* ]]; then
        echo "Building czmq xcode project..."
        cd dependencies/czmq
        rm -Rf builds/xcode
        mkdir -p builds/xcode
        cmake -S . -B builds/xcode -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_PREFIX_PATH=$CODEROOT/sysroot/usr/local/ -G "Xcode" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.11 -DCZMQ_WITH_LZ4=OFF
        #Hack to enable xcode project embedding because scripts coming with
        #the xcode project do not manage the target folder properly:
        mkdir -p builds/xcode/Debug
        cp $CODEROOT/sysroot/usr/local/lib/libczmq.* builds/xcode/Debug
        mkdir -p builds/xcode/Release
        cp $CODEROOT/sysroot/usr/local/lib/libczmq.* builds/xcode/Release
        cd ../..
    fi

    if [ -e dependencies/zyre ] && [[ $OSTYPE == "darwin"* ]]; then
        echo "Building zyre xcode project..."
        cd dependencies/zyre
        rm -Rf builds/xcode
        mkdir -p builds/xcode
        cmake -S . -B builds/xcode -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_PREFIX_PATH=$CODEROOT/sysroot/usr/local/ -G "Xcode" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.11
        #Hack to enable xcode project embedding because scripts coming with
        #the xcode project do not manage the target folder properly:
        mkdir -p builds/xcode/Debug
        cp $CODEROOT/sysroot/usr/local/lib/libzyre.* builds/xcode/Debug
        mkdir -p builds/xcode/Release
        cp $CODEROOT/sysroot/usr/local/lib/libzyre.* builds/xcode/Release
        cd ../..
    fi
    cd ..
)
