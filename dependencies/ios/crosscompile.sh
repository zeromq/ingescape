#!/bin/bash
#
#	CROSSCOMPILE
#
#  Copyright (c) 2016 Ingenuity i/o. All rights reserved.
#
#	See license terms for the rights and conditions
#	defined by copyright holders.
#
#
#	Contributors:
#      Stephane Vales <vales@ingenuity.io>
#
#
#
#This script is used to cross compile libraries on various platforms around IOS. The compilation
#is thus producing static libraries useful for all platform but also for the IOS simulators.
#The final result is the aggregation of all the selected platforms in a single library using lipo.
#The libraries to be compiled shall be compliant with a configure/make/make install pattern. This
#is the case for most open source libraries.
#It is often necessary to run autogen.sh first before running the script.
#Then just call the script with the proper library name (e.g. libsodium.a).
#It is easy to change the target IOS distribution in the script. Current is IOS 9.3.
#It is also easy to avoid compilation for some of the architectures (armv7, armv7s,
#arm64, i386, x86_64) by commenting the call to build it below for the corresponding
#architecture.

#just in case:
#codesign -s 'iPhone Developer' ./path/to/binary

if [ $# -ne 2 ]; then
echo usage: $0 /Users/steph/tmp/ libmylib.a
exit 1
fi

LIBNAME=$2

PLATFORMPATH="/Applications/Xcode.app/Contents/Developer/Platforms"
TOOLSPATH="/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin"
export IPHONEOS_DEPLOYMENT_TARGET="10.3"
COMPILE_DESTINATION=$1

findLatestSDKVersion()
{
sdks=`ls $PLATFORMPATH/$1.platform/Developer/SDKs`
arr=()
for sdk in $sdks
do
arr[${#arr[@]}]=$sdk
done

# Last item will be the current SDK, since it is alpha ordered
count=${#arr[@]}
if [ $count -gt 0 ]; then
sdk=${arr[$count-1]:${#1}}
num=`expr ${#sdk}-4`
SDKVERSION=${sdk:0:$num}
else
SDKVERSION="10.3"
fi
echo "SDK version is $SDKVERSION"
}

buildit()
{
target=$1
hosttarget=$1
platform=$2

if [[ $hosttarget == "arm64" ]]; then
hosttarget="arm"
fi

export CC="$(xcrun -sdk iphoneos -find clang) -I${COMPILE_DESTINATION}/output/${target}/include/" #-I${COMPILE_DESTINATION}/../curl/include/"
export CPP="$CC -E"
export CFLAGS="-arch ${target} -isysroot $PLATFORMPATH/$platform.platform/Developer/SDKs/$platform$SDKVERSION.sdk -miphoneos-version-min=$SDKVERSION"
export AR=$(xcrun -sdk iphoneos -find ar)
export RANLIB=$(xcrun -sdk iphoneos -find ranlib)
#export CPPFLAGS="-arch ${target}  -isysroot $(xcrun --sdk iphoneos --show-sdk-path)"
#export LDFLAGS="-arch ${target} -isysroot $(xcrun --sdk iphoneos --show-sdk-path) -L${COMPILE_DESTINATION}"
export CPPFLAGS="-arch ${target}  -isysroot $PLATFORMPATH/$platform.platform/Developer/SDKs/$platform$SDKVERSION.sdk -miphoneos-version-min=$SDKVERSION"
export LDFLAGS="-arch ${target} -isysroot $PLATFORMPATH/$platform.platform/Developer/SDKs/$platform$SDKVERSION.sdk -L${COMPILE_DESTINATION}/output/${target}/lib/"

mkdir -p $COMPILE_DESTINATION/output/$target

./configure --prefix="$COMPILE_DESTINATION/output/$target" --disable-shared --disable-sqlite --host=$hosttarget-apple-darwin

make clean
make
make install
}

cd $1

findLatestSDKVersion iPhoneOS

#buildit armv7 iPhoneOS
#buildit armv7s iPhoneOS
buildit arm64 iPhoneOS
#buildit i386 iPhoneSimulator
buildit x86_64 iPhoneSimulator

LIPO=$(xcrun -sdk iphoneos -find lipo)
#$LIPO -create $COMPILE_DESTINATION/output/armv7/lib/${LIBNAME}  $COMPILE_DESTINATION/output/armv7s/lib/${LIBNAME} $COMPILE_DESTINATION/output/arm64/lib/${LIBNAME} $COMPILE_DESTINATION/output/x86_64/lib/${LIBNAME} $COMPILE_DESTINATION/output/i386/lib/${LIBNAME} -output $COMPILE_DESTINATION/${LIBNAME}
$LIPO -create $COMPILE_DESTINATION/output/arm64/lib/${LIBNAME} $COMPILE_DESTINATION/output/x86_64/lib/${LIBNAME} -output $COMPILE_DESTINATION/${LIBNAME}
