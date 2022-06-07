/*  =========================================================================
    multiply.js 

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

const igs = require('ingescape');
const iopValueTypes = igs.iopValueTypes();

/*
 This is a callback function that runs everytime the
 input "integer" is written. The callback is linked to
 the input by the use of igs.observeInput below.
 The behavior of this agent is very simple:
 - reading the changed input
 - printing the new value
 - multiplying the value by 2 and writing the result to the output
 */
function myIOPCallback(iopType, name, valueType, value, myData) {
  /*
    This callback is fed with all the information necessary:
    - the IOP type : input, output or parameter
    - the IOP name
    - the IOP value type (impulsion, boolean, etc.)
    - a pointer to the value
    - the actual memory size of the value
    - optional data passed from the call to observeInput
  */
  let r = igs.inputInt(name);
  console.info(name + " changed to " + r);
  igs.outputSetInt("integer", 2*r);
}

/*
 This is a callback function that runs everytime the
 service "multiply" is called by another agent. 
 The callback is linked to the service by the use 
 of igs.serviceInit below.
 The behavior of this agent is very simple:
 - retrieve service arguments a & b
 - printing the result of a*b and writing the result to the output
 */
function myServiceCallback(senderAgentName, senderAgentUUID, serviceName, serviceArgs, token, myData) {
  igs.info(senderAgentName + "(" + senderAgentUUID + ") called " + serviceName);
  
  if (serviceArgs.length < 2)
  {
      igs.warn("Invalid number of arguments (2 expected, " + serviceArgs.length   +  " received). Service will not be executed.");
      return;
  }
  if ((serviceArgs[0] == null) || (serviceArgs[0].value == null))
  {
      igs.warn("Unable to retrieve service argument at index 0. Service will not be executed.");
      return;
  }
  if (serviceArgs[0].type !== iopValueTypes.IGS_INTEGER_T)
  {
      igs.warn("Argument at index 0 is not of type IGS_INTEGER_T. Service will not be executed.");
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

  let a = serviceArgs[0].value;
  let b = serviceArgs[1].value;
  let result = a*b;
  console.info("Result is : " + result);
  igs.outputSetInt("integer", result);
}

/*
  Use ingeScape function to find compatible network devices
  on your computer.
  You need to use one of the returned results in the
  igs.startWithDevice call below for your agent to start
  on the network.
*/
let devices = igs.netDevicesList();
for (let i = 0; i < devices.length; i++)
  console.info(devices[i]);

//set the agent name
if (process.argc === 2) 
  //either by using the argument on the command line
  igs.agentSetName(process.argv[1]);
else 
  //or by giving a default name if no argument was passed
  igs.agentSetName("multiply");

//Create an input and an ouput: both are of integer type
//with default value set to zero.
igs.inputCreate("integer", iopValueTypes.IGS_INTEGER_T, 0);
igs.outputCreate("integer", iopValueTypes.IGS_INTEGER_T, 0);

//Set the definition information (optional)
igs.definitionSetVersion("1.0");
igs.definitionSetDescription("Definition for our first agent.");

//Attach the callback to our agent's input
//NB: callbacks can be attached to any IOP
igs.observeInput("integer", myIOPCallback, null);

//Create a service with 2 arguments: both are of integer type
//And attach the callback to our agent's service
//NB: callbacks can be attached to any service
igs.serviceInit("multiply", myServiceCallback, null);
igs.serviceArgAdd("multiply", "a", iopValueTypes.IGS_INTEGER_T);
igs.serviceArgAdd("multiply", "b", iopValueTypes.IGS_INTEGER_T);

//Actually and finally start the agent
//First argument is the network device name to use
//Second argument is the network port used by all the agents of your system
igs.startWithDevice("en0", 5670);

//Stop the agent properly before terminating the program
process.on('SIGINT', function() {
    process.exit();
});

process.on('exit', (code) => {
    igs.stop();
});
