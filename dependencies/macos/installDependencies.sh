#!/bin/sh
cd ../../../libsodium
./autogen.sh && ./configure && make
sudo make install
make distclean

cd ../libzmq
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DWITH_LIBSODIUM=ON -DENABLE_DRAFTS=ON
sudo make -C build install
sudo rm -Rf build
mkdir -p builds/xcode
cmake -S . -B builds/xcode -DCMAKE_BUILD_TYPE=Debug -DWITH_LIBSODIUM=ON -DENABLE_DRAFTS=ON -G "Xcode"

cd ../czmq
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_DRAFTS=ON
sudo make -C build install
sudo rm -Rf build
mkdir -p builds/xcode
cmake -S . -B builds/xcode -DCMAKE_BUILD_TYPE=Debug -DENABLE_DRAFTS=ON -G "Xcode"

cd ../zyre
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_DRAFTS=ON
sudo make -C build install
sudo rm -Rf build
mkdir -p builds/xcode
cmake -S . -B builds/xcode -DCMAKE_BUILD_TYPE=Debug -DENABLE_DRAFTS=ON -G "Xcode"

cd ../ingescape/dependencies/macos

