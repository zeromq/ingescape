#!/bin/bash

pwd=`pwd`

#get git repos
git clone --depth 1 -b stable https://github.com/jedisct1/libsodium.git
git clone git://github.com/zeromq/libzmq.git
git clone git://github.com/zeromq/czmq.git
git clone git://github.com/zeromq/zyre.git

#run autogen (to enable later call to configure)
cd libsodium
./autogen.sh
cd ../libzmq
./autogen.sh
cd ../czmq
./autogen.sh
cd ../zyre
./autogen.sh
cd ..

#compile sodium
./crosscompile.sh $pwd/libsodium/ libsodium.a

#copy libsodium/output in libzmq
cp -R libsodium/output libzmq/
#compile libzmq
./crosscompile-libzmq-with-sodium.sh $pwd/libzmq/ libzmq.a

#copy libzmq/output in czmq
cp -R libzmq/output ./czmq/
#compile czmq
./crosscompile.sh $pwd/czmq/ libczmq.a

#copy czmq/output in zyre
cp -R czmq/output ./zyre/
#compile zyre
./crosscompile.sh $pwd/zyre/ libzyre.a

#archive and clean
mkdir ios
mkdir ios/lib
mkdir ios/include
mv ./libsodium/libsodium.a ./ios/lib/
mv ./libzmq/libzmq.a ./ios/lib/
mv ./czmq/libczmq.a ./ios/lib/
mv ./zyre/libzyre.a ./ios/lib/
cp -R zyre/output/armv7/include/* ./ios/include/
mkdir ios/perPlatform
mv zyre/output/* ios/perPlatform/
#rm -Rf libzmq/output czmq/output zyre/output libsodium/output
rm -Rf libzmq czmq zyre libsodium
