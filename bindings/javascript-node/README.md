# Ingescape NodeJS

Ingescape NodeJS embeds the Ingescape C library (statically linked) and uses n-api to wrap to it.
`prebuildify` and `node-gyp-build` are used to prebuild the ingescape.node binary for windows, macos and linux, and to deliver the right package for all the supported operating systems.


## Prerequisites

* NodeJS (https://nodejs.org/en/download/)
* Python 3.8 to 3.11, **only if you plan to build the native addon yourself** (see below). This is not required to simply `npm install ingescape`, since prebuilt binaries are downloaded/used automatically via `node-gyp-build`.
  Newer Python versions (3.12+) removed the `distutils` module from the standard library, which `node-gyp` (used internally by `prebuildify`) still depends on.


## Installing

You can install ingescape with npm.
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

If the build fails with a `ModuleNotFoundError: No module named 'distutils'` error, see the Python note under Prerequisites above.

Your package will be built in `./prebuilds/{platform}-{arch}/node.napi.node`.


### Testing your own ingescape Node package

Once built, you can test your ingescape package thanks to the ./tests/test.js script. Be sure to specify correct values for network device (default is 'en0') and port (default is 5670) variables inside the script.

```bash
npm run test
```
