# ingescape NodeJS

Ingescape NodeJS comes with the Ingescape C library embedded (statically linked) and uses n-api to bind to it.
`prebuildify` and `node-gyp-build` are used to respectively prebuild ingescape.node binary for windows, macos and linux, and to deliver the right package for your OS.


## Prerequisites

* NodeJS (https://nodejs.org/en/download/)


## Install 

Once your Node.JS project is initiated, you can install ingescape with npm. 
```
npm install ingescape
```


## Usefull tips for this package developpers

### Build
```bash
npm run build
```

### Test
```bash
npm run test
```