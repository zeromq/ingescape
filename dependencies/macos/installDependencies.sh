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

cd ../czmq
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_DRAFTS=ON
sudo make -C build install
sudo rm -Rf build

cd ../zyre
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_DRAFTS=ON
sudo make -C build install
sudo rm -Rf build

cd ../ingescape/dependencies/macos

