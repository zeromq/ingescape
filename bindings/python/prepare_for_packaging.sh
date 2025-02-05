#!/bin/bash

set -e

SCRIPT_DIR=$(cd `dirname $0`; pwd)
SCRIPT_NAME=$(basename $0)

function print_usage {
    echo "Usage: $SCRIPT_NAME [-h]"
    echo "  -h               : Show this help message and exit."
}

while getopts ":h" opt
do
    case "${opt}" in
        h) # Help option
            print_usage
            exit 0
            ;;
        :) # Option with missing argument
            echo "ERROR: -${OPTARG} requires an argument"
            print_usage
            exit 100
            ;;
        *) # Unknown option
            echo "ERROR: unknown parameter -${OPTARG}"
            print_usage
            exit 110
            ;;
    esac
done

[[ -d "$SCRIPT_DIR/../../build/" ]] || (echo "The library must be built before building the python binding." ; echo "Its expected here $(cd $SCRIPT_DIR/../../; pwd)/build" ; false)
[[ -n "$PLATFORM" ]] || (echo "The PLATFORM env variable must be defined to one of the following: 'linux', 'macos', 'windows'" ; echo "Defined to PLATFORM=[$PLATFORM]" ; false)

(
  cd $SCRIPT_DIR

  IGS_MAJOR=$(grep "#define INGESCAPE_VERSION_MAJOR" ../../include/ingescape.h | cut -d' ' -f3)
  IGS_MINOR=$(grep "#define INGESCAPE_VERSION_MINOR" ../../include/ingescape.h | cut -d' ' -f3)
  IGS_PATCH=$(grep "#define INGESCAPE_VERSION_PATCH" ../../include/ingescape.h | cut -d' ' -f3)

  echo "Prepare pyproject.toml from pyproject.toml.in"
  sed "s/@IGS_VERSION@/\"$IGS_MAJOR.$IGS_MINOR.$IGS_PATCH\"/" pyproject.toml.in > pyproject.toml
  echo "Prepare setup.py from setup.py.in"
  sed "s/@IGS_VERSION@/\"$IGS_MAJOR.$IGS_MINOR.$IGS_PATCH\"/" setup.py.in > setup.py
  echo "Copy library files"
  mkdir -p ./dependencies/$PLATFORM/
  mkdir -p ./dependencies/include/ingescape
  cp ../../build/libingescape.a "./dependencies/$PLATFORM"
  cp ../../build/dependencies/czmq/libczmq.a "./dependencies/$PLATFORM"
  cp ../../build/dependencies/libzmq/lib/libzmq.a "./dependencies/$PLATFORM"
  cp ../../build/dependencies/zyre/libzyre.a "./dependencies/$PLATFORM"
  cp ../../build/dependencies/sodium/libsodium.a "./dependencies/$PLATFORM"
  cp -r ../../include/* ./dependencies/include/ingescape
  cp -r ../../dependencies/czmq/include/* ./dependencies/include
  cp -r ../../dependencies/libzmq/include/* ./dependencies/include
  cp -r ../../dependencies/sodium/src/libsodium/include/* ./dependencies/include
  cp -r ../../dependencies/zyre/include/* ./dependencies/include
)

exit 0
