#!/bin/sh


SCRIPT_DIR=$(cd `dirname $0`; pwd)
SCRIPT_NAME=$(basename $0)

(
    cd $SCRIPT_DIR

    IGS=$PWD
    cd ..
    CODEROOT=$PWD
    echo "This script will compile Ingescape and its dependencies in a local sysroot directory at $CODEROOT/sysroot and create Xcode projects for each dependency"
    cd $IGS
    read -p "Do you want to continue? [yN] " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]
    then
        [[ "$0" = "$BASH_SOURCE" ]] && exit 1 || return 1
    fi

    git submodule update --init --recursive #make sure all submodules are here
    mkdir -p $CODEROOT/sysroot

    echo "Deploying ingescape library in $CODEROOT/sysroot"
    mkdir build
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$CODEROOT/sysroot/usr/local/
    make -j8 -C build install
    cp src/include/ingescape_private.h $CODEROOT/sysroot/usr/local/include/ingescape/
    rm -Rf build

    cd dependencies

    if [ -e sodium ]; then
        echo "Building libsodium Xcode project"
        cd sodium
        rm -Rf builds/xcode
        mkdir -p builds/xcode
        cmake -S . -B builds/xcode -DCMAKE_BUILD_TYPE=Debug -G "Xcode"
        cd ..
    fi

    if [ -e libzmq ]; then
        echo "Building libzmq Xcode project"
        cd libzmq
        rm -Rf builds/xcode
        mkdir -p builds/xcode
        cmake -S . -B builds/xcode -DCMAKE_BUILD_TYPE=Debug -DWITH_LIBSODIUM=ON -DENABLE_DRAFTS=ON -DWITH_TLS=OFF -DCMAKE_PREFIX_PATH=$CODEROOT/sysroot/usr/local/ -G "Xcode"
        cd ..
    fi

    if [ -e czmq ]; then
        echo "Building czmq Xcode project"
        cd czmq
        rm -Rf builds/xcode
        mkdir -p builds/xcode
        cmake -S . -B builds/xcode -DCMAKE_BUILD_TYPE=Debug -DENABLE_DRAFTS=ON -DCMAKE_PREFIX_PATH=$CODEROOT/sysroot/usr/local/ -G "Xcode"
        #Hack to enable xcode project embedding
        #because scripts acompanying the xcode project do not manage
        #target folder properly:
        mkdir -p builds/xcode/Debug
        cp $CODEROOT/sysroot/usr/local/lib/libczmq.?.?.?.dylib builds/xcode/Debug
        cd ..
    fi

    if [ -e zyre ]; then
        echo "Building zyre Xcode project"
        cd zyre
        rm -Rf builds/xcode
        mkdir -p builds/xcode
        cmake -S . -B builds/xcode -DCMAKE_BUILD_TYPE=Debug -DENABLE_DRAFTS=ON -DCMAKE_PREFIX_PATH=$CODEROOT/sysroot/usr/local/ -G "Xcode"
        #Hack to enable xcode project embedding
        #because scripts acompanying the xcode project do not manage
        #target folder properly:
        mkdir -p builds/xcode/Debug
        cp $CODEROOT/sysroot/usr/local/lib/libzyre.?.?.?.dylib builds/xcode/Debug
        cd ..
    fi
    cd ..
)
