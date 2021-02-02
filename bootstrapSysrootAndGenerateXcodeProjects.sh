#!/bin/sh


SCRIPT_DIR=$(cd `dirname $0`; pwd)
SCRIPT_NAME=$(basename $0)

(
    cd $SCRIPT_DIR

    IGS=$PWD
    cd ..
    CODEROOT=$PWD
    echo "This script will create Xcode projects and compile Ingescape and its dependencies in a local sysroot directory at $CODEROOT/sysroot"
    cd $IGS
    read -p "Do you want to continue? [yN]" -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]
    then
        [[ "$0" = "$BASH_SOURCE" ]] && exit 1 || return 1
    fi

    git submodule update --init --recursive #make sure all submodules are here
    mkdir -p $CODEROOT/sysroot
    cd dependencies

    if [ -e sodium ]; then
        echo "Deploying libsodium in $CODEROOT/sysroot"
        cd sodium
        ./autogen.sh && ./configure --prefix=$CODEROOT/sysroot/usr/local/ && make -j8 install
        make clean
        install_name_tool -id "@rpath/libsodium.23.dylib" $CODEROOT/sysroot/usr/local/lib/libsodium.23.dylib
        cd ..
    fi

    if [ -e libzmq ]; then
        echo "Deploying libzmq in $CODEROOT/sysroot"
        cd libzmq
        mkdir build
        cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DWITH_LIBSODIUM=ON -DENABLE_DRAFTS=ON -DWITH_TLS=OFF -DCMAKE_PREFIX_PATH=$CODEROOT/sysroot/usr/local/
        make -j8 -C build DESTDIR=$CODEROOT/sysroot/ install
        rm -Rf build
        rm -Rf builds/xcode
        mkdir -p builds/xcode
        echo "Building libzmq Xcode project"
        cmake -S . -B builds/xcode -DCMAKE_BUILD_TYPE=Debug -DWITH_LIBSODIUM=ON -DENABLE_DRAFTS=ON -DWITH_TLS=OFF -DCMAKE_PREFIX_PATH=$CODEROOT/sysroot/usr/local/ -G "Xcode"
        cd ..
    fi

    if [ -e czmq ]; then
        echo "Deploying czmq in $CODEROOT/sysroot"
        cd czmq
        mkdir build
        cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_DRAFTS=ON -DCMAKE_PREFIX_PATH=$CODEROOT/sysroot/usr/local/
        make -j8 -C build DESTDIR=$CODEROOT/sysroot/ install
        rm -Rf builds/xcode
        mkdir -p builds/xcode
        echo "Building czmq Xcode project"
        cmake -S . -B builds/xcode -DCMAKE_BUILD_TYPE=Debug -DENABLE_DRAFTS=ON -DCMAKE_PREFIX_PATH=$CODEROOT/sysroot/usr/local/ -G "Xcode"
        #Hack to enable xcode project embedding
        #because scripts acompanying the xcode project do not manage
        #target folder properly:
        mkdir -p builds/xcode/Debug
        cp build/libczmq.?.?.?.dylib builds/xcode/Debug
        rm -Rf build
        cd ..
    fi

    if [ -e zyre ]; then
        echo "Deploying zyre in $CODEROOT/sysroot"
        cd zyre
        rm -Rf build
        mkdir build
        cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_DRAFTS=ON -DCMAKE_PREFIX_PATH=$CODEROOT/sysroot/usr/local/
        make -j8 -C build DESTDIR=$CODEROOT/sysroot/ install
        rm -Rf builds/xcode
        mkdir -p builds/xcode
        echo "Building zyre Xcode project"
        cmake -S . -B builds/xcode -DCMAKE_BUILD_TYPE=Debug -DENABLE_DRAFTS=ON -DCMAKE_PREFIX_PATH=$CODEROOT/sysroot/usr/local/ -G "Xcode"
        #Hack to enable xcode project embedding
        #because scripts acompanying the xcode project do not manage
        #target folder properly:
        mkdir -p builds/xcode/Debug
        cp build/libzyre.?.?.?.dylib builds/xcode/Debug
        rm -Rf build
        cd ..
    fi
    cd ..

    echo "Deploying ingescape library in $CODEROOT/sysroot"
    mkdir build
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=$CODEROOT/sysroot/usr/local/
    make -j8 -C build DESTDIR=$CODEROOT/sysroot/ install
    cp src/include/ingescape_private.h $CODEROOT/sysroot/usr/local/include/ingescape/
    rm -Rf build
)
