#!/bin/sh
if [ ! -e ../../../libsodium ]; then
    echo "Clone the libsodium repo at the same level as this one"
    exit
fi
cd ../../../libsodium
./autogen.sh && ./configure && make
sudo make install
cp -R ./src/libsodium/.libs ./src/libsodium/libs

if [ ! -e ../libzmq ]; then
    echo "Clone the libzmq repo at the same level as this one"
    cd ../ingescape/dependencies/macos
    exit
fi
cd ../libzmq
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DWITH_LIBSODIUM=ON -DENABLE_DRAFTS=ON
sudo make -C build install
sudo rm -Rf build
mkdir -p builds/xcode
cmake -S . -B builds/xcode -DCMAKE_BUILD_TYPE=Debug -DWITH_LIBSODIUM=ON -DENABLE_DRAFTS=ON -G "Xcode"

if [ ! -e ../czmq ]; then
    echo "Clone the czmq repo at the same level as this one"
    cd ../ingescape/dependencies/macos
    exit
fi
cd ../czmq
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_DRAFTS=ON
sudo make -C build install
sudo rm -Rf build
mkdir -p builds/xcode
cmake -S . -B builds/xcode -DCMAKE_BUILD_TYPE=Debug -DENABLE_DRAFTS=ON -G "Xcode"

if [ ! -e ../zyre ]; then
    echo "Clone the zyre repo at the same level as this one"
    cd ../ingescape/dependencies/macos
    exit
fi
cd ../zyre
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_DRAFTS=ON
sudo make -C build install
sudo rm -Rf build
mkdir -p builds/xcode
cmake -S . -B builds/xcode -DCMAKE_BUILD_TYPE=Debug -DENABLE_DRAFTS=ON -G "Xcode"

cd ../ingescape/dependencies/macos

