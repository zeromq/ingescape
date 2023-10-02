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

(
  cd $SCRIPT_DIR

  IGS_MAJOR=$(grep "#define INGESCAPE_VERSION_MAJOR" ../../include/ingescape.h | cut -d' ' -f3)
  IGS_MINOR=$(grep "#define INGESCAPE_VERSION_MINOR" ../../include/ingescape.h | cut -d' ' -f3)
  IGS_PATCH=$(grep "#define INGESCAPE_VERSION_PATCH" ../../include/ingescape.h | cut -d' ' -f3)

  sed -i.orig "s/\"version\": \".*\",/\"version\": \"$IGS_MAJOR.$IGS_MINOR.$IGS_PATCH\",/" package.json
  cat package.json
)

exit 0

