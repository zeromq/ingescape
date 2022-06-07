# echo_example
This is a full industrial-grade example of a NodeJS Ingescape agent using the Node JS binding.
It provides inputs and outputs of each type. The behavior copies every data received on each input to the output of the same type.

## Prerequisites

* NodeJS (https://nodejs.org/en/download/)

## Install dependencies
```bash
npm install
```

## Run
```bash
node index.js --verbose --port 5670 --device "en0"
```

