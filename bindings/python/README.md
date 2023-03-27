# Ingescape Python binding

## Dependencies

The Ingescape Python binding relies on the following libraries:

- Python 3.6+ 
- setuptools, wheel


## Installing

Ingescape python binding is designed to be installed through pip :

    python3 -m pip install ingescape


## Using Ingescape with Python

Two examples are provided in the 'examples' directory.


## Compiling the Ingescape Python binding

Run the following commands from the Ingescape Python directory :

    python3 -m pip install setuptools wheel
    ./prepare_setup.sh (or prepare_setup.ps1 for Windows Powershell)
    python3 -m pip install .
