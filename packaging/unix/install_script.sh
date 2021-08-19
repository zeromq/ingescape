#!/usr/bin/env bash

# Checks whether the current user is root or not and make use of 'sudo' if needed.
function _check_sudo {
    if [[ $EUID = 0 ]]
    then
        $@
    else
        echo "WARNING: You need an elevated prompt to install the ingescape library"
        sudo $@
    fi
}

## Initialisation

# Exit on error
set -o errexit

# Exit on unknown variable used
set -o nounset

# Return code for piped sequences is the last command that returned non-zero (we don't have pipes for now)
set -o pipefail

echo "Installing ingescape and its dependencies..."
echo "-----------------------"
_check_sudo cp -rv . /usr/local/

echo ""
echo "Installation completed with success!"

## EOF ##
