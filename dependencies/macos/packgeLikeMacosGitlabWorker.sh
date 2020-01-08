#!/bin/sh
cd ../../..
mkdir -p packagingAll/packages

cd libsodium
./autogen.sh && ./configure && make
make prefix=$PWD/../packagingAll/ingescape/usr/local/ install
make distclean
cd ..

cd libzmq
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DWITH_LIBSODIUM=ON -DENABLE_DRAFTS=ON -DCMAKE_PREFIX_PATH=$PWD/../packagingAll/ingescape/usr/local/
make -C build
make -C build DESTDIR=$PWD/../packagingAll/ingescape/ install
sudo rm -Rf build
cd ..

cd czmq
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DENABLE_DRAFTS=ON -DCMAKE_PREFIX_PATH=$PWD/../packagingAll/ingescape/usr/local/
make -C build
make -C build DESTDIR=$PWD/../packagingAll/ingescape/ install
sudo rm -Rf build
cd ..

cd zyre
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DENABLE_DRAFTS=ON -DCMAKE_PREFIX_PATH=$PWD/../packagingAll/ingescape/usr/local/
make -C build
make -C build DESTDIR=$PWD/../packagingAll/ingescape/ install
sudo rm -Rf build
cd ..

cd ingescape
mkdir build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$PWD/../packagingAll/ingescape/usr/local/
make -C build
make -C build DESTDIR=$PWD/../packagingAll/ingescape/ install
cp src/include/ingescape_private.h $PWD/../packagingAll/ingescape/usr/local/include/ingescape/
make -C build package
cp ./build/ingescape*.pkg $PWD/../packagingAll/packages/
sudo rm -Rf build
cd ..
