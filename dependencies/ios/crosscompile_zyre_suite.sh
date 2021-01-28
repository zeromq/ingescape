#!/bin/bash

#configure xcode path
sudo xcode-select --switch /Applications/Xcode.app
cd ..

#get git repos
git clone ssh://git@gitlab.ingescape.com:22222/third-party/curl.git

#run autoconf (to enable later call to configure in script)
cd curl
./buildconf
cd ../sodium
./autogen.sh
cd ../libzmq
./autogen.sh
cd ../czmq
./autogen.sh
cd ../zyre
./autogen.sh
cd ..

#compile curl required by CZMQ
./ios/crosscompile.sh $PWD/curl/ libcurl.a

#copy curl/output in libsodium
cp -R curl/output sodium/
#compile sodium
./ios/crosscompile.sh $PWD/sodium/ libsodium.a

#copy libsodium/output in libzmq
cp -R sodium/output libzmq/
#compile libzmq
./ios/crosscompile-libzmq-with-sodium.sh $PWD/libzmq/ libzmq.a

#copy libzmq/output in czmq
cp -R libzmq/output ./czmq/
#compile czmq
./ios/crosscompile.sh $PWD/czmq/ libczmq.a

#copy czmq/output in zyre
cp -R czmq/output ./zyre/
#compile zyre
./ios/crosscompile.sh $PWD/zyre/ libzyre.a

#archive and clean
mkdir -p ios/lib
mkdir -p ios/include
mv ./curl/libcurl.a ./ios/lib/
mv ./sodium/libsodium.a ./ios/lib/
mv ./libzmq/libzmq.a ./ios/lib/
mv ./czmq/libczmq.a ./ios/lib/
mv ./zyre/libzyre.a ./ios/lib/
cp -R zyre/output/arm64/include/* ./ios/include/
mkdir -p ios/perPlatform
cp -R zyre/output/* ios/perPlatform/

rm -Rf curl
rm -Rf sodium/output
rm -Rf libzmq/output
rm -Rf czmq/output
rm -Rf zyre/output

cd ios
