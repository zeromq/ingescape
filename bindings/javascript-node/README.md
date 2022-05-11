# Ingescape NodeJS

Ingescape NodeJS comes with the Ingescape C library embedded (statically linked) and uses n-api to bind to it.
`prebuildify` and `node-gyp-build` are used to respectively prebuild ingescape.node binary for windows, macos and linux, and to deliver the right package for your OS.


## Prerequisites

* NodeJS (https://nodejs.org/en/download/)


## Installing

Once your Node.JS project is initiated, you can install ingescape with npm. 
```
npm install ingescape
```

Two examples of ingescape agents are provided in the directory ./examples. The multiply_example is a minimal example and the echo_example is a full example.


## Building your own ingescape Node package

Install `prebuildify` to build the package : 

```bash
npm install prebuildify
```

Replace the paths to the static ingescape library with your owns in the binding.gyp file.

Then, build your own ingescape package : 

```bash
npm run build
```

Your package will be built in `./prebuilds/{platform}-{arch}/node.napi.node`.


### Testing your own ingescape Node package

Once built, you can test your ingescape package thanks to the ./tests/test.js script : assertions will fail if the script encounters an error. Be sure to specify correct values for port and network device variables inside the script.

```bash
npm run test
```