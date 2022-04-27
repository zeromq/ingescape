/*  =========================================================================
    index.js 

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

const igs = require('ingescape');
const commandLineArgs = require('command-line-args')
const fs = require('fs');
const EchoExample = require("./src/echoExample").EchoExample;

const iopTypes = igs.iopTypes();
const iopValueTypes = igs.iopValueTypes();

let verbose = false;
let networkDevice = "";
let port = 5670;
let agentName = "echo_example";
let igsCertPath = "";
let publicCertsDir = "";

let echoExample = new EchoExample();

//inputs
function impulsionInputCallback(iopType, name, valueType, value, myData) {
  igs.info(name + " changed (impulsion)");
  echoExample.setImpulsionI();
}

function boolInputCallback(iopType, name, valueType, value, myData) {
  igs.info(name + " changed to " + value);
  echoExample.setBoolI(value);
}

function integerInputCallback(iopType, name, valueType, value, myData) {
  igs.info(name + " changed to " + value);
  echoExample.setIntegerI(value);
}

function doubleInputCallback(iopType, name, valueType, value, myData) {
  igs.info(name + " changed to " + value);
  echoExample.setDoubleI(value);
}

function stringInputCallback(iopType, name, valueType, value, myData) {
  igs.info(name + " changed to " + value);
  echoExample.setStringI(value);
}

function dataInputCallback(iopType, name, valueType, value, myData) {
  igs.info(name + " changed (" + (value ? value.byteLength : "0") +" bytes)");
  echoExample.setDataI(value);
}

//services
function receiveValuesCallback(senderAgentName, senderAgentUUID, serviceName, serviceArgs, token, myData) {
  igs.info(senderAgentName + "(" + senderAgentUUID + ") called " + serviceName);

  if (serviceArgs.length < 5)
  {
      igs.warn("Invalid number of arguments (5 expected, " + serviceArgs.length   +  " received). Service will not be executed.");
      return;
  }

  if ((serviceArgs[0] == null) || (serviceArgs[0].value == null))
  {
      igs.warn("Unable to retrieve service argument at index 0. Service will not be executed.");
      return;
  }
  if (serviceArgs[0].type !== iopValueTypes.IGS_BOOL_T)
  {
      igs.warn("Argument at index 0 is not of type IGS_BOOL_T. Service will not be executed.");
      return;
  }

  if ((serviceArgs[1] == null) || (serviceArgs[1].value == null))
  {
      igs.warn("Unable to retrieve service argument at index 1. Service will not be executed.");
      return;
  }
  if (serviceArgs[1].type !== iopValueTypes.IGS_INTEGER_T)
  {
      igs.warn("Argument at index 1 is not of type IGS_INTEGER_T. Service will not be executed.");
      return;
  }

  if ((serviceArgs[2] == null) || (serviceArgs[2].value == null))
  {
      igs.warn("Unable to retrieve service argument at index 2. Service will not be executed.");
      return;
  }
  if (serviceArgs[2].type !== iopValueTypes.IGS_DOUBLE_T)
  {
      igs.warn("Argument at index 2 is not of type IGS_DOUBLE_T. Service will not be executed.");
      return;
  }

  if ((serviceArgs[3] == null) || (serviceArgs[3].value == null))
  {
      igs.warn("Unable to retrieve service argument at index 3. Service will not be executed.");
      return;
  }
  if (serviceArgs[3].type !== iopValueTypes.IGS_STRING_T)
  {
      igs.warn("Argument at index 3 is not of type IGS_STRING_T. Service will not be executed.");
      return;
  }

  if ((serviceArgs[4] == null) || (serviceArgs[4].value == null))
  {
      igs.warn("Unable to retrieve service argument at index 4. Service will not be executed.");
      return;
  }
  if (serviceArgs[4].type !== iopValueTypes.IGS_DATA_T)
  {
      igs.warn("Argument at index 4 is not of type IGS_DATA_T. Service will not be executed.");
      return;
  }

  echoExample.receiveValues(serviceArgs[0].value, serviceArgs[1].value, serviceArgs[2].value, serviceArgs[3].value, serviceArgs[4].value);
}

function sendValuesCallback(senderAgentName, senderAgentUUID, serviceName, serviceArgs, token, myData) {
  igs.info(senderAgentName + "(" + senderAgentUUID + ") called " + serviceName);

  echoExample.sendValues();
}


// Command line interpreter
function printUsage() {
    console.info("Usage examples:");
    console.info("    node index.js --verbose --device en0 --port 5670");
    console.info("\nIngescape parameters:");
    console.info("--verbose : enable verbose mode in the application (default is disabled)");
    console.info("--device device_name : name of the network device to be used (useful if several devices are available)");
    console.info("--port port_number : port used for autodiscovery between agents (default: " + port + ")");
    console.info("--name agent_name : published name of this agent (default: " + agentName + ")");
    console.info("Security:");
    console.info("--igsCert filePath : path to a private certificate used to connect to a secure platform");
    console.info("--publicCerts directoryPath : path to a directory providing public certificates usable by ingescape");
}

const optionDefinitions = [
    { name : 'verbose', alias: 'v', type: Boolean},
    { name : 'device', alias: 'd', type: String},
    { name : 'port', alias: 'p', type: Number},
    { name : 'name', alias: 'n', type: String},
    { name : 'igsCert', type: String},
    { name : 'publicCerts', type: String},
    { name : 'help', alias: 'h', type: Boolean}
];

const options = commandLineArgs(optionDefinitions);
if (options.help) {
    printUsage();
    process.exit(0);
}
if (options.verbose)
    verbose = true;
if (options.device)
    networkDevice = options.device;
if (options.port)
    port = options.port;
if (options.name)
    agentName = options.name;
if (options.igsCert)
    igsCertPath = options.igsCert
if (options.publicCerts)
    publicCertsDir = options.publicCerts


igs.agentSetName(agentName);
igs.logSetConsole(verbose);
igs.logSetFile(true, null);
igs.logSetStream(verbose);
igs.definitionSetVersion("1.0");
igs.setCommandLine(process.argv.join(" "));

//security
if (igsCertPath.length > 0) {
    if (fs.existsSync(igsCertPath)) {
        igs.enableSecurity(igsCertPath, publicCertsDir);
    }
    else {
        igs.error("Could not find Ingescape certificate file '" + igsCertPath + "': exiting");
        process.exit();
    }
}

//network device if not set
if (networkDevice.length === 0) {
    //we have no device to start with: try to find one
    let devices = igs.netDevicesList();
    let addresses = igs.netAddressesList();
    if (devices.length === 1) {
        //we have exactly one compliant network device available: we use it
        networkDevice = devices[0];
        igs.info("using " + networkDevice + " as default network device (this is the only one available)");
    }
    else if ((devices.length === 2) && ((addresses[0] === "127.0.0.1") || (addresses[1] === "127.0.0.1"))) {
        //we have two devices, one of which is the loopback
        //pick the device that is NOT the loopback
        if (addresses[0] === "127.0.0.1")
            networkDevice = devices[1];
        else
            networkDevice = devices[0];
        igs.info("using " + networkDevice + " as default network device (this is the only one available that is not the loopback)");
    }
    else {
        if (devices.length === 0)
            igs.error("No network device found: aborting.");
        else {
            igs.error("No network device passed as command line parameter and several are available.");
            console.info("Please use one of these network devices:");
            for (let i = 0; i < devices.length; i++)
                console.info(devices[i]);
            console.info();
            printUsage();
        }
        process.exit();
    }
}

igs.inputCreate("impulsion", iopValueTypes.IGS_IMPULSION_T, null);
igs.inputCreate("bool", iopValueTypes.IGS_BOOL_T, false);
igs.inputCreate("integer", iopValueTypes.IGS_INTEGER_T, 0);
igs.inputCreate("double", iopValueTypes.IGS_DOUBLE_T, 0);
igs.inputCreate("string", iopValueTypes.IGS_STRING_T, "");
igs.inputCreate("data", iopValueTypes.IGS_DATA_T, new ArrayBuffer());
igs.outputCreate("impulsion", iopValueTypes.IGS_IMPULSION_T, null);
igs.outputCreate("bool", iopValueTypes.IGS_BOOL_T, false);
igs.outputCreate("integer", iopValueTypes.IGS_INTEGER_T, 0);
igs.outputCreate("double", iopValueTypes.IGS_DOUBLE_T, 0);
igs.outputCreate("string", iopValueTypes.IGS_STRING_T, "");
igs.outputCreate("data", iopValueTypes.IGS_DATA_T, new ArrayBuffer());
igs.observeInput("impulsion", impulsionInputCallback, null);
igs.observeInput("bool", boolInputCallback, null);
igs.observeInput("integer", integerInputCallback, null);
igs.observeInput("double", doubleInputCallback, null);
igs.observeInput("string", stringInputCallback, null);
igs.observeInput("data", dataInputCallback, null);
igs.serviceInit("receive_values", receiveValuesCallback, null);
igs.serviceArgAdd("receive_values", "bool", iopValueTypes.IGS_BOOL_T);
igs.serviceArgAdd("receive_values", "integer", iopValueTypes.IGS_INTEGER_T);
igs.serviceArgAdd("receive_values", "double", iopValueTypes.IGS_DOUBLE_T);
igs.serviceArgAdd("receive_values", "string", iopValueTypes.IGS_STRING_T);
igs.serviceArgAdd("receive_values", "data", iopValueTypes.IGS_DATA_T);
igs.serviceInit("send_values", sendValuesCallback, null);

//actually start ingescape
igs.startWithDevice(networkDevice, port);

// Properly stop our agent/process
function forcedStopCallback(myData) {
    process.exit();
}
igs.observeForcedStop(forcedStopCallback, null);

process.on('SIGINT', function() {
    process.exit();
});

process.on('exit', (code) => {
    igs.stop();
});

