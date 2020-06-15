#!/bin/bash

#configure xcode path
sudo xcode-select --switch /Applications/Xcode.app

#get git repos
git clone ssh://git@gitlab.ingescape.com:2222/third-party/curl.git
git clone -b stable ssh://git@gitlab.ingescape.com:2222/third-party/libsodium.git
git clone ssh://git@gitlab.ingescape.com:2222/third-party/libzmq.git
git clone ssh://git@gitlab.ingescape.com:2222/third-party/czmq.git
git clone ssh://git@gitlab.ingescape.com:2222/third-party/zyre.git

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
./crosscompile.sh $PWD/curl/ libcurl.a

#copy curl/output in libsodium
cp -R curl/output libsodium/
#compile sodium
./crosscompile.sh $PWD/libsodium/ libsodium.a

#copy libsodium/output in libzmq
cp -R libsodium/output libzmq/
#compile libzmq
./crosscompile-libzmq-with-sodium.sh $PWD/libzmq/ libzmq.a

#copy libzmq/output in czmq
cp -R libzmq/output ./czmq/
#compile czmq
./crosscompile.sh $PWD/czmq/ libczmq.a

#copy czmq/output in zyre
cp -R czmq/output ./zyre/
#compile zyre
./crosscompile.sh $PWD/zyre/ libzyre.a

#archive and clean
mkdir ios
mkdir ios/lib
mkdir ios/include
cp ./curl/libcurl.a ./ios/lib/
cp ./libsodium/libsodium.a ./ios/lib/
cp ./libzmq/libzmq.a ./ios/lib/
cp ./czmq/libczmq.a ./ios/lib/
cp ./zyre/libzyre.a ./ios/lib/
cp -R zyre/output/arm64/include/* ./ios/include/
mkdir ios/perPlatform
cp -R zyre/output/* ios/perPlatform/
rm -Rf curl libzmq czmq zyre libsodium
