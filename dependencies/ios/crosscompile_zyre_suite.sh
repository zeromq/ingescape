#!/bin/bash

pwd=`pwd`

#configure xcode path
sudo xcode-select --switch /Applications/Xcode.app

#get git repos
git clone https://github.com/curl/curl.git
git clone --depth 1 -b stable https://github.com/jedisct1/libsodium.git
git clone git://github.com/zeromq/libzmq.git
git clone git://github.com/zeromq/czmq.git
git clone git://github.com/zeromq/zyre.git

#run autoconf (to enable later call to configure in script)
cd curl
./buildconf
cd ../libsodium
./autogen.sh
cd ../libzmq
./autogen.sh
cd ../czmq
./autogen.sh
cd ../zyre
./autogen.sh
cd ..

#compile curl required by CZMQ
./crosscompile.sh $pwd/curl/ libcurl.a

#copy curl/output in libsodium
cp -R curl/output libsodium/
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
mv ./curl/libcurl.a ./ios/lib/
mv ./libsodium/libsodium.a ./ios/lib/
mv ./libzmq/libzmq.a ./ios/lib/
mv ./czmq/libczmq.a ./ios/lib/
mv ./zyre/libzyre.a ./ios/lib/
cp -R zyre/output/arm64/include/* ./ios/include/
mkdir ios/perPlatform
mv zyre/output/* ios/perPlatform/
rm -Rf curl libzmq czmq zyre libsodium
