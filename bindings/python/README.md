# Ingescape Python binding

## Dependencies

The Ingescape Python binding relies on the following libraries:

- Python 3.6+
- setuptools, wheel

## Installing

Ingescape python binding is designed to be installed through pip

    python3 -m pip install ingescape

## Using Ingescape with Python

Two examples are provided in the 'examples' directory.

## Compiling the Ingescape Python binding

Ingescape library must be built before compiling the python binding.
The directory in which the library is expected to be built is given by the prepare scripts.
Run the following commands from the Ingescape Python directory :

    python3 -m pip install setuptools wheel
    export FROM_SOURCES="true"
    export PLATFORM="linux" (or "macos", or "windows" according to the platform you are on)
    ./prepare_for_packaging.sh (or prepare_for_packaging.ps1 for Windows Powershell)
    python3 -m pip install .

The last command will build and install the python binding on your environment
