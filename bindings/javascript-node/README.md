# Binding Node.JS for Ingescape

This set of files is intended to provide a Node JS module for Ingescape.

## General information

### Description

Various C files allow us to wrap Ingescape functions in JavaScript language. 

Once the addon is compiled, we can use it as any other Node module. We just have to import it in our Node JS programs :
```
const igs = require(/path_to/ingescape.node); 
```

### Files

* headers/ : .h headers to build the module
* src/ : .c source files to build the module
* binding.gyp : contains information for npm to create the module 
* build/Release/ingescape.node : the actual Ingescape module produced after compilation

### Prerequisites

* Ingescape library installed in /usr/local
* NodeJS : Javascript runtime (https://nodejs.org/api/n-api.html#n_api_n_api)

## Compiling and installing

### Install Node JS (if it is not already)
https://nodejs.org/en/

### Build module

To build the module, run on command line into project root dir :
```
$ npm run build
```
