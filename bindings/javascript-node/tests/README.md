# Ingescape Node JS TEST

Your ingescape package must be in ../prebuilds/{platform}-{arch}/node.napi. N.B: by using 'prebuildify --napi' (or 'npm run build') in the JS binding directory will place the generated binding in the right directory automatically.

Launch test.js to execute tests, assertions will fail if the script encounters an error. Be sure to specify correct values for port and network device variables inside the script test.js.