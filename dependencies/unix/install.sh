#!/bin/sh

git clone --depth 1 -b stable https://github.com/jedisct1/libsodium.git
cd libsodium
./autogen.sh && ./configure && make check
sudo make install
cd ..

git clone git://github.com/zeromq/libzmq.git
cd libzmq
./autogen.sh
# do not specify "--with-libsodium" if you prefer to use internal tweetnacl
# security implementation (recommended for development)
./configure
make check
sudo make install
sudo ldconfig
cd ..

git clone git://github.com/zeromq/czmq.git
cd czmq
./autogen.sh && ./configure && make check
sudo make install
sudo ldconfig
cd ..

git clone git://github.com/zeromq/zyre.git
cd zyre
./autogen.sh && ./configure && make check
sudo make install
sudo ldconfig
cd ..

#The following lines will install the Yajl JSON parser
#based on the source code in ../yajl (patched version)
#Warning: cmake is required for the compilation
cd ../yajl
mkdir build
cd build
cmake ..
make
sudo make install
cd ..
rm -Rf build
cd ../macos

