#!/bin/bash

set -e
set -x

function wget_failed {
    remote_url=$1
    echo "The 'wget' failed to retrieve file at $remote_url"
    echo "The URL you are trying to reach may be incorrect or you may not have 'wget' installed."
    echo "First, try installing wget using"
    echo ""
    echo "    brew install wget"
    echo ""
    echo "and try relaunching the script."
    echo "You may also want to try accessing the URL directly from a web browser to see if it's correct."
    exit 1
}

function _wget {
    remote_file=$1
    local_file=$2

    remote_url=http://rafale/ingescape/I2Quick/$remote_file
    wget $remote_url --quiet --output-document=applications/frameworks/I2Quick/$local_file || wget_failed $remote_url
}

echo "### Update I2Quick ###"
echo ""
echo " -- macOS (only release. no debug)"
    mkdir -p applications/frameworks/I2Quick/Mac
    _wget macos/release/libI2Quick.dylib Mac/libI2Quick.dylib
    echo ""
    echo " -- win32 (release and debug)"
    mkdir -p applications/frameworks/I2Quick/Win32
    file_list_release=(
        "I2Quick.dll"
        "I2Quick.exp"
        "I2Quick.lib"
    )
    file_list_debug=(
        "I2Quickd.dll"
        "I2Quickd.exp"
        "I2Quickd.lib"
        "I2Quickd.pdb"
    )
    for f in ${file_list_release[*]}
    do
        _wget win32/release/$f Win32/$f
    done
    for f in ${file_list_debug[*]}
    do
        _wget win32/debug/$f Win32/$f
    done
echo ""
echo " -- win64 (release and debug)"
    mkdir -p applications/frameworks/I2Quick/Win64
    file_list_release=(
        "I2Quick.dll"
        "I2Quick.exp"
        "I2Quick.lib"
    )
    file_list_debug=(
        "I2Quickd.dll"
        "I2Quickd.exp"
        "I2Quickd.lib"
        "I2Quickd.pdb"
    )
    for f in ${file_list_release[*]}
    do
        _wget win64/release/$f Win64/$f
    done
    for f in ${file_list_debug[*]}
    do
        _wget win64/debug/$f Win64/$f
    done
echo ""
echo " -- Unix (only release. no debug)"
    mkdir -p applications/frameworks/I2Quick/Unix
    _wget linux/libI2Quick.so Unix/libI2Quick.so
echo ""
echo " -- Generic files"
    _wget generic/include.tar.gz include.tar.gz
    tar xf applications/frameworks/I2Quick/include.tar.gz -C applications/frameworks/I2Quick/ 
    rm applications/frameworks/I2Quick/include.tar.gz
    _wget generic/I2Quick.qmltypes I2Quick.qmltypes
    _wget generic/qmldir qmldir
echo ""
echo " -- Write README.md"
    cat > applications/frameworks/I2Quick/README.md <<EOF
I2Quick
=======

Last updater:         $(whoami)
Last recorded update: $(date)

_Updated using the update_i2quick.sh script at the root of the git repository._

EOF
echo ""
echo "### All done ###"

