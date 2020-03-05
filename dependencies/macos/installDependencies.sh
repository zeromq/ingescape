#!/bin/sh

currentLocation=$PWD

if [ -e /usr/local/bin/brew ]; then
    echo "Removing zeromq stack from homebrew if needed"
    brew remove -f zyre czmq zeromq libsodium
else
    echo "Installing homebrew"
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
fi
echo "Checking and installing system dependencies"
brew install autoconf automake cmake libtool pkgconfig

if [ ! -e ../../../libsodium ]; then
    echo "Cloning libsodium"
    cd ../../..
    git clone -b stable ssh://git@gitlab.ingescape.com:22222/third-party/libsodium.git
    cd $currentLocation
fi
cd ../../../libsodium
./autogen.sh && ./configure && make
sudo make install
make distclean

if [ ! -e ../libzmq ]; then
    echo "Cloning libzmq"
    cd ..
    git clone ssh://git@gitlab.ingescape.com:22222/third-party/libzmq.git
    cd libzmq
fi
cd ../libzmq
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DWITH_LIBSODIUM=ON -DENABLE_DRAFTS=ON
sudo make -C build install
sudo rm -Rf build

if [ ! -e ../czmq ]; then
    echo "Cloning czmq"
    cd ..
    git clone ssh://git@gitlab.ingescape.com:22222/third-party/czmq.git
    cd czmq
fi
cd ../czmq
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_DRAFTS=ON
sudo make -C build install
sudo rm -Rf build

if [ ! -e ../zyre ]; then
    echo "Cloning zyre"
    cd ..
    git clone ssh://git@gitlab.ingescape.com:22222/third-party/zyre.git
    cd zyre
fi
cd ../zyre
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_DRAFTS=ON
sudo make -C build install
sudo rm -Rf build

cd ../ingescape/dependencies/macos

