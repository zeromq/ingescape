/*  =========================================================================
    test - Binding Node.JS tests

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

const IGS = require(__dirname + "/../prebuilds/" + process.platform + "-" + process.arch + "/node.napi"); // igs js wrapper

const assert = require('assert');
const commands = require('command-line-args');

const logLevelEnum = IGS.logLevels();
const iopEnum = IGS.iopTypes();
const iopTypeEnum = IGS.iopValueTypes();
const igsResultEnum = IGS.resultTypes();
const agentEventEnum = IGS.agentEventTypes();

let port = 5670;
let agentName = "nodeTester";
let networkDevice = "en1";
let verbose = false;
let staticTests = false; 

const optionDefinitions = [
    { name : 'verbose', alias: 'v', type: Boolean},
    { name : 'device', alias: 'd', type: String},
    { name : 'port', alias: 'p', type: Number},
    { name : 'name', alias: 'n', type: String},
    { name : 'static', alias: 's', type: Boolean},
    { name : 'help', alias: 'h', type: Boolean},
];

function printUsage() {
    console.log("Usage example: node testNodeAgent.js --verbose --port " + port + " --device device_name ");
    console.log()
    console.log("these parameters have default value (indicated here above):");
    console.log("--verbose : enable verbose mode in the application (default is disabled)");
    console.log("--device device_name : name of the network device to be used (useful if several devices available)");
    console.log("--port port_number : port used for autodiscovery between agents (default: " + port + ")");
    console.log("--name agent_name : published name for this agent (default: " + agentName + ")");
    console.log("--static : execute static tests");
}

const options = commands(optionDefinitions);
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
if (options.static)
    staticTests = options.static;

global.firstAgent;
global.secondAgent;

//callbacks for services
function testerServiceCallback(
    senderAgentName, 
    senderAgentUUID, 
    serviceName, 
    argumentsArray, 
    token, 
    myData) {
    console.log("received service %s from %s(%s) (", serviceName, senderAgentName, senderAgentUUID);
    argumentsArray.forEach(function(argument) {
        switch (argument.type) {
            case iopTypeEnum.IGS_BOOL_T:
                console.log(" let " + argument.value);
                break;
            case iopTypeEnum.IGS_INTEGER_T:
                console.log(" integer " + argument.value);
                break;
            case iopTypeEnum.IGS_DOUBLE_T:
                console.log(" double " + argument.value);
                break;
            case iopTypeEnum.IGS_STRING_T:
                console.log(" string " + argument.value);
                break;
            case iopTypeEnum.IGS_DATA_T:
                console.log(" data - " + argument.value.byteLength+ " bytes");
                break;
            default:
                break;
            }
    });
    console.log(" )\n");
}

function agentServiceCallback(
    agent,
    senderAgentName, 
    senderAgentUUID, 
    serviceName, 
    argumentsArray, 
    token, 
    myData) {
    let thisAgentName = agent.name();
    console.log(thisAgentName + " received service " + serviceName + " from " + senderAgentName + "(" + senderAgentUUID + ")");
    argumentsArray.forEach(function(argument) {
        switch (argument.type) {
            case iopTypeEnum.IGS_BOOL_T:
                console.log(" bool " + argument.value);
                break;
            case iopTypeEnum.IGS_INTEGER_T:
                console.log(" integer " + argument.value);
                break;
            case iopTypeEnum.IGS_DOUBLE_T:
                console.log(" double " + argument.value);
                break;
            case iopTypeEnum.IGS_STRING_T:
                console.log(" string " + argument.value);
                break;
            case iopTypeEnum.IGS_DATA_T:
                console.log(" data - " + argument.value.byteLength+ " bytes");
                break;
            default:
                break;
            }
    });
    console.log(" )\n");
}

//callbacks for iops
function testerIOPCallback(type, name, valueType, value, data) {
    console.log("input " + name + " changed");
    switch (valueType) {
        case iopTypeEnum.IGS_IMPULSION_T:
            console.log(" (impulsion)\n");
            break;
        case iopTypeEnum.IGS_BOOL_T:
            console.log(" to bool ", IGS.inputBool(name) + "\n");
            break;
        case iopTypeEnum.IGS_INTEGER_T:
            console.log(" to int " + IGS.inputInt(name) + "\n");
            break;
        case iopTypeEnum.IGS_DOUBLE_T:
            console.log(" to double " + IGS.inputDouble(name) + "\n");
            break;
        case iopTypeEnum.IGS_STRING_T:
        {
            console.log(" to string " + IGS.inputString(name) + "\n");
            break;
        }
        case iopTypeEnum.IGS_DATA_T:
            console.log(" to data with size ", IGS.inputData(name).byteLength + "\n");
            break;
        default:
            break;
    }
}

function agentIOPCallback(agent, type, name, valueType, value, data) {
    let thisAgentName = agent.name();
    console.log("input " + name + " changed on " + thisAgentName);
    switch (valueType) {
        case iopTypeEnum.IGS_IMPULSION_T:
            console.log(" (impulsion)\n");
            break;
        case iopTypeEnum.IGS_BOOL_T:
            console.log(" to bool ", IGS.inputBool(name) + "\n");
            break;
        case iopTypeEnum.IGS_INTEGER_T:
            console.log(" to int " + IGS.inputInt(name) + "\n");
            break;
        case iopTypeEnum.IGS_DOUBLE_T:
            console.log(" to double " + IGS.inputDouble(name) + "\n");
            break;
        case iopTypeEnum.IGS_STRING_T:
        {
            console.log(" to string " + IGS.inputString(name) + "\n");
            break;
        }
        case iopTypeEnum.IGS_DATA_T:
            console.log(" to data with size ", IGS.inputData(name).byteLength + "\n");
            break;
        default:
            break;
    }
}

IGS.clearContext();

//agent name and uuid
assert(IGS.agentName() === "no_name");
IGS.agentSetName("agent 007");
assert(IGS.agentName() === "agent_007");
IGS.agentSetName(agentName);
assert(IGS.agentUuid(), "IGS.agentUuid()");

//agent family
assert(IGS.agentFamily() === "");
IGS.agentSetFamily("family_test");
assert(IGS.agentFamily() === "family_test");

//Logs
IGS.logIncludeData(true);
IGS.logIncludeServices(true);
assert(!IGS.logConsole());
assert(!IGS.logConsoleColor());
assert(!IGS.logStream());
assert(!IGS.logFile());
assert(IGS.logFilePath() === "");
IGS.logSetConsole(true);
assert(IGS.logConsole());
IGS.logSetStream(true);
assert(IGS.logStream());
IGS.logSetFilePath("/tmp/log.txt");
assert(IGS.logFilePath() === "/tmp/log.txt");
IGS.logSetFile(true, null);
console.log(IGS.logFilePath())
assert(IGS.logFilePath() === "");
assert(IGS.logFile());
IGS.logSetConsoleLevel(logLevelEnum.IGS_LOG_TRACE);
assert(IGS.logConsoleLevel() === logLevelEnum.IGS_LOG_TRACE);
IGS.logSetFileLevel( logLevelEnum.IGS_LOG_TRACE);
IGS.logSetFileMaxLineLength(1024);
IGS.trace("trace example 1");
IGS.debug("debug  example 2");
IGS.info("info example 3");
IGS.warn("warn example 4");
IGS.error("error example 5");
IGS.fatal("fatal example 6");
IGS.info("multi-line log \n second line");

//try to write uninitialized definition and mapping (generates errors)
IGS.definitionSave();
IGS.mappingSave();

// Utils
assert(IGS.version() >= 3, "IGS.version() >= 3");
assert(IGS.protocol() >= 4, "IGS.protocol() >= 4");
let devices = IGS.netDevicesList();
devices.forEach(function(device) {
    console.log("device : " + device);
});
let addresses = IGS.netAddressesList();
addresses.forEach(function(address) {
    console.log("ip : " + address);
});
assert(IGS.commandLine() === "", "IGS.commandLine() === \"\"");
IGS.setCommandLine("test command line");
assert(IGS.commandLine() === "test command line", "IGS.commandLine() === \"test command line\"");
assert(!IGS.mappingOutputsRequest(), "!IGS.mappingOutputsRequest()");
IGS.mappingSetOutputsRequest(true);
assert(IGS.mappingOutputsRequest(), "IGS.mappingOutputsRequest()");

// Control
assert(!IGS.isStarted(), "!IGS.isStarted()");
assert(IGS.agentState() === "", "IGS.agentState() === \"\"");
IGS.agentSetState("my state");
assert(IGS.agentState() ===  "my state", "IGS.agentState() ===  \"my state\"");
assert(!IGS.agentIsMuted(), "!IGS.agentIsMuted()");
IGS.agentMute();
assert(IGS.agentIsMuted(), "IGS.agentIsMuted()");
IGS.agentUnmute();
assert(!IGS.agentIsMuted(), "!IGS.agentIsMuted()");
assert(!IGS.isFrozen(), "!IGS.isFrozen()");
IGS.freeze();
assert(IGS.isFrozen(), "IGS.isFrozen()");
IGS.unfreeze();
assert(!IGS.isFrozen(), "!IGS.isFrozen()");
 
// IOPs with null definition
assert(IGS.inputCount() === 0);
assert(IGS.outputCount() === 0);
assert(IGS.parameterCount() === 0);
assert(!IGS.inputExists("toto"));
assert(!IGS.outputExists("toto"));
assert(!IGS.parameterExists("toto"));
assert(IGS.inputList().length === 0);
assert(IGS.outputList().length === 0);
assert(IGS.parameterList().length === 0);

assert(!IGS.outputIsMuted(""));
assert(!IGS.outputIsMuted("toto"));
IGS.outputMute("toto");
IGS.outputUnmute("toto");

assert(!IGS.inputBool("toto"));
assert(IGS.inputInt("toto") === 0);
assert(IGS.inputDouble("toto") < 0.000001);
assert(IGS.inputString("toto").length === 0);
assert(IGS.inputData("toto") === null);
IGS.clearInput("toto");
assert(!IGS.outputBool("toto"));
assert(IGS.outputInt("toto") === 0);
assert(IGS.outputDouble("toto") < 0.000001);
assert(IGS.outputString("toto").length === 0);
assert(IGS.outputData("toto") === null);
assert(!IGS.parameterBool("toto"));
assert(IGS.parameterInt("toto") === 0);
assert(IGS.parameterDouble("toto") < 0.000001);
assert(IGS.parameterString("toto").length === 0);
assert(IGS.parameterData("toto") === null);

// Definition - part 1
assert(IGS.definitionLoadStr("invalid json") === igsResultEnum.IGS_FAILURE);
assert(IGS.definitionLoadFile("/does not exist") === igsResultEnum.IGS_FAILURE);
assert(IGS.definitionJson()); 
assert(IGS.agentName() === agentName);
assert(IGS.definitionDescription() === "");
assert(IGS.definitionVersion() === "");
//TODO: test loading valid string and file definitions
IGS.definitionSetDescription("my description");
assert(IGS.definitionDescription());
IGS.definitionSetVersion("version");
assert(IGS.definitionVersion() === "version");

assert(IGS.inputCreate("toto", iopTypeEnum.IGS_BOOL_T, false) === igsResultEnum.IGS_SUCCESS);
assert(IGS.outputCreate("toto", iopTypeEnum.IGS_BOOL_T, false) === igsResultEnum.IGS_SUCCESS);
assert(IGS.parameterCreate("toto", iopTypeEnum.IGS_BOOL_T, false) === igsResultEnum.IGS_SUCCESS);
assert(IGS.inputCreate("toto", iopTypeEnum.IGS_BOOL_T, false) === igsResultEnum.IGS_FAILURE);
assert(IGS.outputCreate("toto", iopTypeEnum.IGS_BOOL_T, false) === igsResultEnum.IGS_FAILURE);
assert(IGS.parameterCreate("toto", iopTypeEnum.IGS_BOOL_T, false) === igsResultEnum.IGS_FAILURE);
IGS.outputMute("toto");
assert(IGS.outputIsMuted("toto"));
IGS.outputUnmute("toto");
assert(!IGS.outputIsMuted("toto"));
assert(IGS.inputRemove("toto") === igsResultEnum.IGS_SUCCESS);
assert(IGS.outputRemove("toto") === igsResultEnum.IGS_SUCCESS);
assert(IGS.parameterRemove("toto") === igsResultEnum.IGS_SUCCESS);
assert(IGS.inputRemove("toto") === igsResultEnum.IGS_FAILURE);
assert(IGS.outputRemove("toto") === igsResultEnum.IGS_FAILURE);
assert(IGS.parameterRemove("toto") === igsResultEnum.IGS_FAILURE);

 // Inputs
assert(IGS.inputCreate("my impulsion", iopTypeEnum.IGS_IMPULSION_T, null) === igsResultEnum.IGS_SUCCESS);
assert(IGS.inputCreate("my impulsion", iopTypeEnum.IGS_IMPULSION_T, null) === igsResultEnum.IGS_FAILURE);
assert(IGS.inputCreate("my bool", iopTypeEnum.IGS_BOOL_T, true) === igsResultEnum.IGS_SUCCESS);
assert(IGS.inputCreate("my bool", iopTypeEnum.IGS_BOOL_T, true) === igsResultEnum.IGS_FAILURE);
assert(IGS.inputCreate("my int", iopTypeEnum.IGS_INTEGER_T, 1) === igsResultEnum.IGS_SUCCESS);
assert(IGS.inputCreate("my int", iopTypeEnum.IGS_INTEGER_T, 1) === igsResultEnum.IGS_FAILURE);
assert(IGS.inputCreate("my double", iopTypeEnum.IGS_DOUBLE_T, 1) === igsResultEnum.IGS_SUCCESS);
assert(IGS.inputCreate("my double", iopTypeEnum.IGS_DOUBLE_T, 1) === igsResultEnum.IGS_FAILURE);
assert(IGS.inputCreate("my string", iopTypeEnum.IGS_STRING_T, "my string") === igsResultEnum.IGS_SUCCESS);
assert(IGS.inputCreate("my string", iopTypeEnum.IGS_STRING_T, "my string") === igsResultEnum.IGS_FAILURE);
assert(IGS.inputCreate("my data", iopTypeEnum.IGS_DATA_T, new ArrayBuffer(32)) === igsResultEnum.IGS_SUCCESS);
assert(IGS.inputCreate("my data", iopTypeEnum.IGS_DATA_T, new ArrayBuffer(32)) === igsResultEnum.IGS_FAILURE);
assert(IGS.inputList().length === 6);
assert(IGS.inputCount() === 6);
assert(IGS.inputType("my_impulsion") === iopTypeEnum.IGS_IMPULSION_T);
assert(IGS.inputExists("my_impulsion"));
assert(IGS.inputType("my_bool") === iopTypeEnum.IGS_BOOL_T);
assert(IGS.inputExists("my_bool"));
assert(IGS.inputType("my_int") === iopTypeEnum.IGS_INTEGER_T);
assert(IGS.inputExists("my_int"));
assert(IGS.inputType("my_double") === iopTypeEnum.IGS_DOUBLE_T);
assert(IGS.inputExists("my_double"));
assert(IGS.inputType("my_string") === iopTypeEnum.IGS_STRING_T);
assert(IGS.inputExists("my_string"));
assert(IGS.inputType("my_data") === iopTypeEnum.IGS_DATA_T);
assert(IGS.inputExists("my_data"));

assert(IGS.inputBool("my_bool"));
assert(IGS.inputInt("my_int") === 1);
assert(IGS.inputDouble("my_double") - 1.0 < 0.000001);
assert(IGS.inputString("my_string") === "my string");
assert(IGS.inputData("my_data").byteLength === 32);
assert(IGS.inputData("my_impulsion") === null);
assert(IGS.inputSetBool("", false) === igsResultEnum.IGS_FAILURE);
assert(IGS.inputSetBool("my_bool", false) === igsResultEnum.IGS_SUCCESS);
assert(IGS.inputData("my_bool").byteLength === 1); // 1 = bool c size
assert(!IGS.inputBool("my_bool"));
assert(IGS.inputSetInt("", 2) === igsResultEnum.IGS_FAILURE);
assert(IGS.inputSetInt("my_int", 2) === igsResultEnum.IGS_SUCCESS);
assert(IGS.inputData("my_int").byteLength === 4);
assert(IGS.inputInt("my_int") === 2);
assert(IGS.inputSetDouble("", 2) === igsResultEnum.IGS_FAILURE);
assert(IGS.inputSetDouble("my_double", 2) === igsResultEnum.IGS_SUCCESS);
assert(IGS.inputData("my_double").byteLength === 8);
assert(IGS.inputDouble("my_double") - 2 < 0.000001);
assert(IGS.inputSetString("", "new string") === igsResultEnum.IGS_FAILURE);
assert(IGS.inputSetString("my_string", "new string") === igsResultEnum.IGS_SUCCESS);
assert(IGS.inputData("my_string").byteLength === 11)
assert(IGS.inputString("my_string") === "new string");
assert(IGS.inputSetData("",  new ArrayBuffer(16)) === igsResultEnum.IGS_FAILURE);
assert(IGS.inputSetData("my_data",  new ArrayBuffer(16)) === igsResultEnum.IGS_SUCCESS);
assert(IGS.inputData("my_data").byteLength === 16, 'dataData.byteLength === 16');
IGS.clearInput("my_data");
assert(IGS.inputData("my_data") === null);


// Outputs
assert(IGS.outputCreate("my impulsion", iopTypeEnum.IGS_IMPULSION_T, null) === igsResultEnum.IGS_SUCCESS);
assert(IGS.outputCreate("my impulsion", iopTypeEnum.IGS_IMPULSION_T, null) === igsResultEnum.IGS_FAILURE);
assert(IGS.outputCreate("my bool", iopTypeEnum.IGS_BOOL_T, true) === igsResultEnum.IGS_SUCCESS);
assert(IGS.outputCreate("my bool", iopTypeEnum.IGS_BOOL_T, true) === igsResultEnum.IGS_FAILURE);
assert(IGS.outputCreate("my int", iopTypeEnum.IGS_INTEGER_T, 1) === igsResultEnum.IGS_SUCCESS);
assert(IGS.outputCreate("my int", iopTypeEnum.IGS_INTEGER_T, 1) === igsResultEnum.IGS_FAILURE);
assert(IGS.outputCreate("my double", iopTypeEnum.IGS_DOUBLE_T, 1) === igsResultEnum.IGS_SUCCESS);
assert(IGS.outputCreate("my double", iopTypeEnum.IGS_DOUBLE_T, 1) === igsResultEnum.IGS_FAILURE);
assert(IGS.outputCreate("my string", iopTypeEnum.IGS_STRING_T, "my string") === igsResultEnum.IGS_SUCCESS);
assert(IGS.outputCreate("my string", iopTypeEnum.IGS_STRING_T, "my string") === igsResultEnum.IGS_FAILURE);
assert(IGS.outputCreate("my data", iopTypeEnum.IGS_DATA_T, new ArrayBuffer(32)) === igsResultEnum.IGS_SUCCESS);
assert(IGS.outputCreate("my data", iopTypeEnum.IGS_DATA_T, new ArrayBuffer(32)) === igsResultEnum.IGS_FAILURE);
assert(IGS.outputList().length === 6);
assert(IGS.outputCount() === 6);
assert(IGS.outputType("my_impulsion") === iopTypeEnum.IGS_IMPULSION_T);
assert(IGS.outputExists("my_impulsion"));
assert(IGS.outputType("my_bool") === iopTypeEnum.IGS_BOOL_T);
assert(IGS.outputExists("my_bool"));
assert(IGS.outputType("my_int") === iopTypeEnum.IGS_INTEGER_T);
assert(IGS.outputExists("my_int"));
assert(IGS.outputType("my_double") === iopTypeEnum.IGS_DOUBLE_T);
assert(IGS.outputExists("my_double"));
assert(IGS.outputType("my_string") === iopTypeEnum.IGS_STRING_T);
assert(IGS.outputExists("my_string"));
assert(IGS.outputType("my_data") === iopTypeEnum.IGS_DATA_T);
assert(IGS.outputExists("my_data"));
assert(IGS.outputBool("my_bool"));
assert(IGS.outputInt("my_int") === 1);
assert(IGS.outputDouble("my_double") - 1.0 < 0.000001);
assert(IGS.outputString("my_string") === "my string");
assert(IGS.outputData("my_data").byteLength === 32);
assert(IGS.outputSetBool("", false) === igsResultEnum.IGS_FAILURE);
assert(IGS.outputSetBool("my_bool", false) === igsResultEnum.IGS_SUCCESS);
assert(!IGS.outputBool("my_bool"));
assert(IGS.outputSetInt("", 2) === igsResultEnum.IGS_FAILURE);
assert(IGS.outputSetInt("my_int", 2) === igsResultEnum.IGS_SUCCESS);
assert(IGS.outputInt("my_int") === 2);
assert(IGS.outputSetDouble("", 2) === igsResultEnum.IGS_FAILURE);
assert(IGS.outputSetDouble("my_double", 2) === igsResultEnum.IGS_SUCCESS);
assert(IGS.outputDouble("my_double") - 2 < 0.000001);
assert(IGS.outputSetString("", "new string") === igsResultEnum.IGS_FAILURE);
assert(IGS.outputSetString("my_string", "new string") === igsResultEnum.IGS_SUCCESS);
assert(IGS.outputString("my_string") === "new string");
assert(IGS.outputSetData("", new ArrayBuffer(64)) === igsResultEnum.IGS_FAILURE);
assert(IGS.outputSetData("my_data", new ArrayBuffer(64)) === igsResultEnum.IGS_SUCCESS);
assert(IGS.outputData("my_data").byteLength === 64);
IGS.clearOutput("my_data");
assert(IGS.outputData("my_data") === null);

// Parameters
assert(IGS.parameterCreate("my impulsion", iopTypeEnum.IGS_IMPULSION_T, null) === igsResultEnum.IGS_SUCCESS);
assert(IGS.parameterCreate("my impulsion", iopTypeEnum.IGS_IMPULSION_T, null) === igsResultEnum.IGS_FAILURE);
assert(IGS.parameterCreate("my bool", iopTypeEnum.IGS_BOOL_T, true) === igsResultEnum.IGS_SUCCESS);
assert(IGS.parameterCreate("my bool", iopTypeEnum.IGS_BOOL_T, true) === igsResultEnum.IGS_FAILURE);
assert(IGS.parameterCreate("my int", iopTypeEnum.IGS_INTEGER_T, 1) === igsResultEnum.IGS_SUCCESS);
assert(IGS.parameterCreate("my int", iopTypeEnum.IGS_INTEGER_T, 1) === igsResultEnum.IGS_FAILURE);
assert(IGS.parameterCreate("my double", iopTypeEnum.IGS_DOUBLE_T, 1) === igsResultEnum.IGS_SUCCESS);
assert(IGS.parameterCreate("my double", iopTypeEnum.IGS_DOUBLE_T, 1) === igsResultEnum.IGS_FAILURE);
assert(IGS.parameterCreate("my string", iopTypeEnum.IGS_STRING_T,"my string") === igsResultEnum.IGS_SUCCESS);
assert(IGS.parameterCreate("my string", iopTypeEnum.IGS_STRING_T, "my string") === igsResultEnum.IGS_FAILURE);
assert(IGS.parameterCreate("my data", iopTypeEnum.IGS_DATA_T, new ArrayBuffer(32)) === igsResultEnum.IGS_SUCCESS);
assert(IGS.parameterCreate("my data", iopTypeEnum.IGS_DATA_T, new ArrayBuffer(32)) === igsResultEnum.IGS_FAILURE);
assert(IGS.parameterList().length === 6);
assert(IGS.parameterCount() === 6);
assert(IGS.parameterType("my_impulsion") === iopTypeEnum.IGS_IMPULSION_T);
assert(IGS.parameterExists("my_impulsion"));
assert(IGS.parameterType("my_bool") === iopTypeEnum.IGS_BOOL_T);
assert(IGS.parameterExists("my_bool"));
assert(IGS.parameterType("my_int") === iopTypeEnum.IGS_INTEGER_T);
assert(IGS.parameterExists("my_int"));
assert(IGS.parameterType("my_double") === iopTypeEnum.IGS_DOUBLE_T);
assert(IGS.parameterExists("my_double"));
assert(IGS.parameterType("my_string") === iopTypeEnum.IGS_STRING_T);
assert(IGS.parameterExists("my_string"));
assert(IGS.parameterType("my_data") === iopTypeEnum.IGS_DATA_T);
assert(IGS.parameterExists("my_data"));
assert(IGS.parameterBool("my_bool"));
assert(IGS.parameterInt("my_int") === 1);
assert(IGS.parameterDouble("my_double") - 1.0 < 0.000001);
assert(IGS.parameterString("my_string") === "my string");
assert(IGS.parameterData("my_data").byteLength === 32);
assert(IGS.parameterSetBool("", false) === igsResultEnum.IGS_FAILURE);
assert(IGS.parameterSetBool("my_bool", false) === igsResultEnum.IGS_SUCCESS);
assert(!IGS.parameterBool("my_bool"));
assert(IGS.parameterSetInt("", 2) === igsResultEnum.IGS_FAILURE);
assert(IGS.parameterSetInt("my_int", 2) === igsResultEnum.IGS_SUCCESS);
assert(IGS.parameterInt("my_int") === 2);
assert(IGS.parameterSetDouble("", 2) === igsResultEnum.IGS_FAILURE);
assert(IGS.parameterSetDouble("my_double", 2) === igsResultEnum.IGS_SUCCESS);
assert(IGS.parameterDouble("my_double") - 2 < 0.000001);
assert(IGS.parameterSetString("", "new string") === igsResultEnum.IGS_FAILURE);
assert(IGS.parameterSetString("my_string", "new string") === igsResultEnum.IGS_SUCCESS);
assert(IGS.parameterString("my_string") === "new string");
assert(IGS.parameterSetData("", new ArrayBuffer(64)) === igsResultEnum.IGS_FAILURE);
assert(IGS.parameterSetData("my_data", new ArrayBuffer(64)) === igsResultEnum.IGS_SUCCESS);
assert(IGS.parameterData("my_data").byteLength === 64);
IGS.clearParameter("my_data");
assert(IGS.parameterData("my_data") === null);

// N.B.: no way to assert, just call method 
IGS.inputSetDescription("my_impulsion", "my iop description here");
IGS.outputSetDescription("my_impulsion", "my iop description here");
IGS.parameterSetDescription("my_impulsion", "my iop description here");


// Definition - part 2
// TODO: compare exported def, saved file and reference file
let exportedDefinition = IGS.definitionJson()
assert(exportedDefinition);
IGS.definitionSetPath("/tmp/simple Demo Agent.json");
IGS.definitionSave();
IGS.clearDefinition();
assert(IGS.agentName() === agentName);
assert(IGS.definitionDescription() === "");
assert(IGS.definitionVersion() === "");
assert(IGS.inputList().length === 0);
assert(IGS.outputList().length === 0);
assert(IGS.parameterList().length === 0);
assert(IGS.serviceList().length === 0);

IGS.definitionLoadStr(exportedDefinition);
assert(IGS.inputList().length === 6);
assert(IGS.inputCount() === 6);
assert(IGS.inputType("my_impulsion") === iopTypeEnum.IGS_IMPULSION_T);
assert(IGS.inputExists("my_impulsion"));
assert(IGS.inputType("my_bool") === iopTypeEnum.IGS_BOOL_T);
assert(IGS.inputExists("my_bool"));
assert(IGS.inputType("my_int") === iopTypeEnum.IGS_INTEGER_T);
assert(IGS.inputExists("my_int"));
assert(IGS.inputType("my_double") === iopTypeEnum.IGS_DOUBLE_T);
assert(IGS.inputExists("my_double"));
assert(IGS.inputType("my_string") === iopTypeEnum.IGS_STRING_T);
assert(IGS.inputExists("my_string"));
assert(IGS.inputType("my_data") === iopTypeEnum.IGS_DATA_T);
assert(IGS.inputExists("my_data"));
assert(IGS.outputList().length === 6);
assert(IGS.outputCount() === 6);
assert(IGS.outputType("my_impulsion") === iopTypeEnum.IGS_IMPULSION_T);
assert(IGS.outputExists("my_impulsion"));
assert(IGS.outputType("my_bool") === iopTypeEnum.IGS_BOOL_T);
assert(IGS.outputExists("my_bool"));
assert(IGS.outputType("my_int") === iopTypeEnum.IGS_INTEGER_T);
assert(IGS.outputExists("my_int"));
assert(IGS.outputType("my_double") === iopTypeEnum.IGS_DOUBLE_T);
assert(IGS.outputExists("my_double"));
assert(IGS.outputType("my_string") === iopTypeEnum.IGS_STRING_T);
assert(IGS.outputExists("my_string"));
assert(IGS.outputType("my_data") === iopTypeEnum.IGS_DATA_T);
assert(IGS.outputExists("my_data"));
assert(!IGS.outputBool("my_bool"));
assert(IGS.outputInt("my_int") === 2);
assert(IGS.outputDouble("my_double") - 2.0 < 0.000001);
assert(IGS.outputString("my_string") === "new string");
assert(IGS.outputData("my_data") === null);
assert(IGS.parameterList().length === 6);
assert(IGS.parameterCount() === 6);
assert(IGS.parameterType("my_impulsion") === iopTypeEnum.IGS_IMPULSION_T);
assert(IGS.parameterExists("my_impulsion"));
assert(IGS.parameterType("my_bool") === iopTypeEnum.IGS_BOOL_T);
assert(IGS.parameterExists("my_bool"));
assert(IGS.parameterType("my_int") === iopTypeEnum.IGS_INTEGER_T);
assert(IGS.parameterExists("my_int"));
assert(IGS.parameterType("my_double") === iopTypeEnum.IGS_DOUBLE_T);
assert(IGS.parameterExists("my_double"));
assert(IGS.parameterType("my_string") === iopTypeEnum.IGS_STRING_T);
assert(IGS.parameterExists("my_string"));
assert(IGS.parameterType("my_data") === iopTypeEnum.IGS_DATA_T);
assert(IGS.parameterExists("my_data"));
assert(!IGS.parameterBool("my_bool"));
assert(IGS.parameterInt("my_int") === 2);
assert(IGS.parameterDouble("my_double") - 2.0 < 0.000001);
assert(IGS.parameterString("my_string") === "new string");
assert(IGS.parameterData("my_data") === null);
IGS.clearDefinition();

IGS.definitionLoadFile("/tmp/simple Demo Agent.json");
assert(IGS.inputList().length === 6);
assert(IGS.inputCount() === 6);
assert(IGS.inputType("my_impulsion") === iopTypeEnum.IGS_IMPULSION_T);
assert(IGS.inputExists("my_impulsion"));
assert(IGS.inputType("my_bool") === iopTypeEnum.IGS_BOOL_T);
assert(IGS.inputExists("my_bool"));
assert(IGS.inputType("my_int") === iopTypeEnum.IGS_INTEGER_T);
assert(IGS.inputExists("my_int"));
assert(IGS.inputType("my_double") === iopTypeEnum.IGS_DOUBLE_T);
assert(IGS.inputExists("my_double"));
assert(IGS.inputType("my_string") === iopTypeEnum.IGS_STRING_T);
assert(IGS.inputExists("my_string"));
assert(IGS.inputType("my_data") === iopTypeEnum.IGS_DATA_T);
assert(IGS.inputExists("my_data"));
assert(IGS.outputList().length === 6);
assert(IGS.outputCount() === 6);
assert(IGS.outputType("my_impulsion") === iopTypeEnum.IGS_IMPULSION_T);
assert(IGS.outputExists("my_impulsion"));
assert(IGS.outputType("my_bool") === iopTypeEnum.IGS_BOOL_T);
assert(IGS.outputExists("my_bool"));
assert(IGS.outputType("my_int") === iopTypeEnum.IGS_INTEGER_T);
assert(IGS.outputExists("my_int"));
assert(IGS.outputType("my_double") === iopTypeEnum.IGS_DOUBLE_T);
assert(IGS.outputExists("my_double"));
assert(IGS.outputType("my_string") === iopTypeEnum.IGS_STRING_T);
assert(IGS.outputExists("my_string"));
assert(IGS.outputType("my_data") === iopTypeEnum.IGS_DATA_T);
assert(IGS.outputExists("my_data"));
assert(!IGS.outputBool("my_bool"));
assert(IGS.outputInt("my_int") === 2);
assert(IGS.outputDouble("my_double") - 2.0 < 0.000001);
assert(IGS.outputString("my_string") === "new string");
assert(IGS.outputData("my_data") === null);
assert(IGS.parameterList().length === 6);
assert(IGS.parameterCount() === 6);
assert(IGS.parameterType("my_impulsion") === iopTypeEnum.IGS_IMPULSION_T);
assert(IGS.parameterExists("my_impulsion"));
assert(IGS.parameterType("my_bool") === iopTypeEnum.IGS_BOOL_T);
assert(IGS.parameterExists("my_bool"));
assert(IGS.parameterType("my_int") === iopTypeEnum.IGS_INTEGER_T);
assert(IGS.parameterExists("my_int"));
assert(IGS.parameterType("my_double") === iopTypeEnum.IGS_DOUBLE_T);
assert(IGS.parameterExists("my_double"));
assert(IGS.parameterType("my_string") === iopTypeEnum.IGS_STRING_T);
assert(IGS.parameterExists("my_string"));
assert(IGS.parameterType("my_data") === iopTypeEnum.IGS_DATA_T);
assert(IGS.parameterExists("my_data"));
assert(!IGS.parameterBool("my_bool"));
assert(IGS.parameterInt("my_int") === 2);
assert(IGS.parameterDouble("my_double") - 2.0 < 0.000001);
assert(IGS.parameterString("my_string") === "new string");
assert(IGS.parameterData("my_data") === null);

IGS.clearDefinition();

//inputs constraints
IGS.inputCreate("constraint_impulsion", iopTypeEnum.IGS_IMPULSION_T, null);
IGS.inputCreate("constraint_bool", iopTypeEnum.IGS_BOOL_T, false);
IGS.inputCreate("constraint_int", iopTypeEnum.IGS_INTEGER_T, 0);
IGS.inputCreate("constraint_double", iopTypeEnum.IGS_DOUBLE_T, 0);
IGS.inputCreate("constraint_string", iopTypeEnum.IGS_STRING_T, "");
IGS.inputCreate("constraint_data", iopTypeEnum.IGS_DATA_T, null);

assert(IGS.inputAddConstraint("constraint_int", "min 10.12") == igsResultEnum.IGS_SUCCESS); //will set 10 as min constraint
assert(IGS.inputAddConstraint("constraint_int", "max 10.12") == igsResultEnum.IGS_SUCCESS); //will set 10 as max constraint
assert(IGS.inputAddConstraint("constraint_int", "[-.1, +10.13]") == igsResultEnum.IGS_SUCCESS);
assert(IGS.inputAddConstraint("constraint_int", "[-.1  ,  +10.13]") == igsResultEnum.IGS_SUCCESS);
assert(IGS.inputAddConstraint("constraint_int", "[-.1,+10.13]") == igsResultEnum.IGS_SUCCESS);
assert(IGS.inputAddConstraint("constraint_int", "[1,-10.13]") == igsResultEnum.IGS_FAILURE);

assert(IGS.inputAddConstraint("constraint_double", "min 10.12") == igsResultEnum.IGS_SUCCESS);
assert(IGS.inputAddConstraint("constraint_double", "max 10.12") == igsResultEnum.IGS_SUCCESS);
assert(IGS.inputAddConstraint("constraint_double", "[-.1, +10.13]") == igsResultEnum.IGS_SUCCESS);
assert(IGS.inputAddConstraint("constraint_double", "[12.12,12.12]") == igsResultEnum.IGS_SUCCESS);

assert(IGS.inputAddConstraint("constraint_bool", "min 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.inputAddConstraint("constraint_bool", "max 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.inputAddConstraint("constraint_bool", "[1,10.13]") == igsResultEnum.IGS_FAILURE);

assert(IGS.inputAddConstraint("constraint_impulsion", "min 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.inputAddConstraint("constraint_impulsion", "max 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.inputAddConstraint("constraint_impulsion", "[1,10.13]") == igsResultEnum.IGS_FAILURE);

assert(IGS.inputAddConstraint("constraint_string", "min 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.inputAddConstraint("constraint_string", "max 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.inputAddConstraint("constraint_string", "[1,10.13]") == igsResultEnum.IGS_FAILURE);

assert(IGS.inputAddConstraint("constraint_data", "min 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.inputAddConstraint("constraint_data", "max 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.inputAddConstraint("constraint_data", "[1,-10.13]") == igsResultEnum.IGS_FAILURE);

assert(IGS.inputAddConstraint("constraint_string", "~ [^ +") == igsResultEnum.IGS_FAILURE); //bad format for regex
assert(IGS.inputAddConstraint("constraint_string", "~ (\\d+)") == igsResultEnum.IGS_SUCCESS);
assert(IGS.inputAddConstraint("constraint_string", "~ (\\d+)") == igsResultEnum.IGS_SUCCESS);
assert(IGS.inputAddConstraint("constraint_impulsion", "~ (\\d+)") == igsResultEnum.IGS_FAILURE);
assert(IGS.inputAddConstraint("constraint_int", "~ (\\d+)") == igsResultEnum.IGS_FAILURE);
assert(IGS.inputAddConstraint("constraint_double", "~ (\\d+)") == igsResultEnum.IGS_FAILURE);
assert(IGS.inputAddConstraint("constraint_bool", "~ (\\d+)") == igsResultEnum.IGS_FAILURE);
assert(IGS.inputAddConstraint("constraint_data", "~ (\\d+)") == igsResultEnum.IGS_FAILURE);

IGS.inputRemove("constraint_impulsion");
IGS.inputRemove("constraint_int");
IGS.inputRemove("constraint_bool");
IGS.inputRemove("constraint_double");
IGS.inputRemove("constraint_string");
IGS.inputRemove("constraint_data");

//output constraints
IGS.outputCreate("constraint_impulsion", iopTypeEnum.IGS_IMPULSION_T, null);
IGS.outputCreate("constraint_bool", iopTypeEnum.IGS_BOOL_T, false);
IGS.outputCreate("constraint_int", iopTypeEnum.IGS_INTEGER_T, 0);
IGS.outputCreate("constraint_double", iopTypeEnum.IGS_DOUBLE_T, 0);
IGS.outputCreate("constraint_string", iopTypeEnum.IGS_STRING_T, "");
IGS.outputCreate("constraint_data", iopTypeEnum.IGS_DATA_T, null);

assert(IGS.outputAddConstraint("constraint_int", "min 10.12") == igsResultEnum.IGS_SUCCESS); //will set 10 as min constraint
assert(IGS.outputAddConstraint("constraint_int", "max 10.12") == igsResultEnum.IGS_SUCCESS); //will set 10 as max constraint
assert(IGS.outputAddConstraint("constraint_int", "[-.1, +10.13]") == igsResultEnum.IGS_SUCCESS);
assert(IGS.outputAddConstraint("constraint_int", "[-.1  ,  +10.13]") == igsResultEnum.IGS_SUCCESS);
assert(IGS.outputAddConstraint("constraint_int", "[-.1,+10.13]") == igsResultEnum.IGS_SUCCESS);
assert(IGS.outputAddConstraint("constraint_int", "[1,-10.13]") == igsResultEnum.IGS_FAILURE);

assert(IGS.outputAddConstraint("constraint_double", "min 10.12") == igsResultEnum.IGS_SUCCESS);
assert(IGS.outputAddConstraint("constraint_double", "max 10.12") == igsResultEnum.IGS_SUCCESS);
assert(IGS.outputAddConstraint("constraint_double", "[-.1, +10.13]") == igsResultEnum.IGS_SUCCESS);
assert(IGS.outputAddConstraint("constraint_double", "[12.12,12.12]") == igsResultEnum.IGS_SUCCESS);

assert(IGS.outputAddConstraint("constraint_bool", "min 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.outputAddConstraint("constraint_bool", "max 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.outputAddConstraint("constraint_bool", "[1,10.13]") == igsResultEnum.IGS_FAILURE);

assert(IGS.outputAddConstraint("constraint_impulsion", "min 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.outputAddConstraint("constraint_impulsion", "max 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.outputAddConstraint("constraint_impulsion", "[1,10.13]") == igsResultEnum.IGS_FAILURE);

assert(IGS.outputAddConstraint("constraint_string", "min 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.outputAddConstraint("constraint_string", "max 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.outputAddConstraint("constraint_string", "[1,10.13]") == igsResultEnum.IGS_FAILURE);

assert(IGS.outputAddConstraint("constraint_data", "min 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.outputAddConstraint("constraint_data", "max 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.outputAddConstraint("constraint_data", "[1,-10.13]") == igsResultEnum.IGS_FAILURE);

assert(IGS.outputAddConstraint("constraint_string", "~ [^ +") == igsResultEnum.IGS_FAILURE); //bad format for regex
assert(IGS.outputAddConstraint("constraint_string", "~ (\\d+)") == igsResultEnum.IGS_SUCCESS);
assert(IGS.outputAddConstraint("constraint_string", "~ (\\d+)") == igsResultEnum.IGS_SUCCESS);
assert(IGS.outputAddConstraint("constraint_impulsion", "~ (\\d+)") == igsResultEnum.IGS_FAILURE);
assert(IGS.outputAddConstraint("constraint_int", "~ (\\d+)") == igsResultEnum.IGS_FAILURE);
assert(IGS.outputAddConstraint("constraint_double", "~ (\\d+)") == igsResultEnum.IGS_FAILURE);
assert(IGS.outputAddConstraint("constraint_bool", "~ (\\d+)") == igsResultEnum.IGS_FAILURE);
assert(IGS.outputAddConstraint("constraint_data", "~ (\\d+)") == igsResultEnum.IGS_FAILURE);

IGS.outputRemove("constraint_impulsion");
IGS.outputRemove("constraint_int");
IGS.outputRemove("constraint_bool");
IGS.outputRemove("constraint_double");
IGS.outputRemove("constraint_string");
IGS.outputRemove("constraint_data");

//parameter constraints
IGS.parameterCreate("constraint_impulsion", iopTypeEnum.IGS_IMPULSION_T, null);
IGS.parameterCreate("constraint_bool", iopTypeEnum.IGS_BOOL_T, false);
IGS.parameterCreate("constraint_int", iopTypeEnum.IGS_INTEGER_T, 0);
IGS.parameterCreate("constraint_double", iopTypeEnum.IGS_DOUBLE_T, 0);
IGS.parameterCreate("constraint_string", iopTypeEnum.IGS_STRING_T, "");
IGS.parameterCreate("constraint_data", iopTypeEnum.IGS_DATA_T, null);

assert(IGS.parameterAddConstraint("constraint_int", "min 10.12") == igsResultEnum.IGS_SUCCESS); //will set 10 as min constraint
assert(IGS.parameterAddConstraint("constraint_int", "max 10.12") == igsResultEnum.IGS_SUCCESS); //will set 10 as max constraint
assert(IGS.parameterAddConstraint("constraint_int", "[-.1, +10.13]") == igsResultEnum.IGS_SUCCESS);
assert(IGS.parameterAddConstraint("constraint_int", "[-.1  ,  +10.13]") == igsResultEnum.IGS_SUCCESS);
assert(IGS.parameterAddConstraint("constraint_int", "[-.1,+10.13]") == igsResultEnum.IGS_SUCCESS);
assert(IGS.parameterAddConstraint("constraint_int", "[1,-10.13]") == igsResultEnum.IGS_FAILURE);

assert(IGS.parameterAddConstraint("constraint_double", "min 10.12") == igsResultEnum.IGS_SUCCESS);
assert(IGS.parameterAddConstraint("constraint_double", "max 10.12") == igsResultEnum.IGS_SUCCESS);
assert(IGS.parameterAddConstraint("constraint_double", "[-.1, +10.13]") == igsResultEnum.IGS_SUCCESS);
assert(IGS.parameterAddConstraint("constraint_double", "[12.12,12.12]") == igsResultEnum.IGS_SUCCESS);

assert(IGS.parameterAddConstraint("constraint_bool", "min 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.parameterAddConstraint("constraint_bool", "max 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.parameterAddConstraint("constraint_bool", "[1,10.13]") == igsResultEnum.IGS_FAILURE);

assert(IGS.parameterAddConstraint("constraint_impulsion", "min 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.parameterAddConstraint("constraint_impulsion", "max 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.parameterAddConstraint("constraint_impulsion", "[1,10.13]") == igsResultEnum.IGS_FAILURE);

assert(IGS.parameterAddConstraint("constraint_string", "min 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.parameterAddConstraint("constraint_string", "max 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.parameterAddConstraint("constraint_string", "[1,10.13]") == igsResultEnum.IGS_FAILURE);

assert(IGS.parameterAddConstraint("constraint_data", "min 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.parameterAddConstraint("constraint_data", "max 10.12") == igsResultEnum.IGS_FAILURE);
assert(IGS.parameterAddConstraint("constraint_data", "[1,-10.13]") == igsResultEnum.IGS_FAILURE);

assert(IGS.parameterAddConstraint("constraint_string", "~ [^ +") == igsResultEnum.IGS_FAILURE); //bad format for regex
assert(IGS.parameterAddConstraint("constraint_string", "~ (\\d+)") == igsResultEnum.IGS_SUCCESS);
assert(IGS.parameterAddConstraint("constraint_string", "~ (\\d+)") == igsResultEnum.IGS_SUCCESS);
assert(IGS.parameterAddConstraint("constraint_impulsion", "~ (\\d+)") == igsResultEnum.IGS_FAILURE);
assert(IGS.parameterAddConstraint("constraint_int", "~ (\\d+)") == igsResultEnum.IGS_FAILURE);
assert(IGS.parameterAddConstraint("constraint_double", "~ (\\d+)") == igsResultEnum.IGS_FAILURE);
assert(IGS.parameterAddConstraint("constraint_bool", "~ (\\d+)") == igsResultEnum.IGS_FAILURE);
assert(IGS.parameterAddConstraint("constraint_data", "~ (\\d+)") == igsResultEnum.IGS_FAILURE);

IGS.parameterRemove("constraint_impulsion");
IGS.parameterRemove("constraint_int");
IGS.parameterRemove("constraint_bool");
IGS.parameterRemove("constraint_double");
IGS.parameterRemove("constraint_string");
IGS.parameterRemove("constraint_data");


// Mapping & split
assert(IGS.mappingLoadStr("invalid json") === igsResultEnum.IGS_FAILURE);
assert(IGS.mappingLoadFile("/does not exist") === igsResultEnum.IGS_FAILURE);
assert(IGS.mappingJson());
assert(IGS.mappingCount() === 0);

assert(IGS.mappingAdd("toto", "other_agent", "tata") !== "");
let mapId = IGS.mappingAdd("toto", "other_agent", "tata");
assert(IGS.mappingAdd("toto", "other_agent", "tata") === mapId);
assert(mapId !== "");
assert(IGS.mappingRemoveWithId(BigInt("12345")) === igsResultEnum.IGS_FAILURE);
assert(IGS.mappingRemoveWithId(mapId) === igsResultEnum.IGS_SUCCESS);
assert(IGS.mappingAdd("toto", "other_agent", "tata") > 0);
assert(IGS.mappingCount() === 1);
assert(IGS.mappingRemoveWithName("toto", "other_agent", "tata") === igsResultEnum.IGS_SUCCESS);
assert(IGS.mappingRemoveWithName("toto", "other_agent", "tata") === igsResultEnum.IGS_FAILURE);
assert(IGS.mappingCount() === 0);
assert(IGS.mappingAdd("toto", "other_agent", "tata") > 0);
assert(IGS.mappingCount() === 1);
IGS.clearMappingsWithAgent("other_agent");
assert(IGS.mappingCount() === 0);
assert(IGS.mappingRemoveWithName("toto", "other_agent", "tata") === igsResultEnum.IGS_FAILURE);

assert(IGS.splitCount() === 0);
assert(IGS.splitAdd("toto", "other_agent", "tata") != 0);
let splitId = IGS.splitAdd("toto", "other_agent", "tata");
assert(splitId !== "");
assert(IGS.splitCount() === 1);
assert(IGS.splitRemoveWithId(BigInt("12345")) === igsResultEnum.IGS_FAILURE);
assert(IGS.splitRemoveWithId(splitId) === igsResultEnum.IGS_SUCCESS);
assert(IGS.splitCount() === 0);
assert(IGS.splitAdd("toto", "other_agent", "tata") != 0);
assert(IGS.splitCount() === 1);
assert(IGS.splitRemoveWithName("toto", "other_agent", "tata") === igsResultEnum.IGS_SUCCESS);
assert(IGS.splitCount() === 0);
assert(IGS.splitRemoveWithName("toto", "other_agent", "tata") === igsResultEnum.IGS_FAILURE);

IGS.clearMappings();
IGS.mappingAdd("toto", "other_agent", "tata");
IGS.splitAdd("toto", "other_agent", "tata");
let exportedMapping = IGS.mappingJson();
assert(exportedMapping);
IGS.mappingSetPath("/tmp/simple Demo Agent mapping.json");
IGS.mappingSave();
IGS.clearMappings();
IGS.mappingLoadStr(exportedMapping);
assert(IGS.mappingRemoveWithName("toto", "other_agent", "tata") === igsResultEnum.IGS_SUCCESS);
assert(IGS.splitRemoveWithName("toto", "other_agent", "tata") === igsResultEnum.IGS_SUCCESS);
IGS.clearMappings();
IGS.mappingLoadFile("/tmp/simple Demo Agent mapping.json");
assert(IGS.mappingRemoveWithName("toto", "other_agent", "tata") === igsResultEnum.IGS_SUCCESS);
assert(IGS.splitRemoveWithName("toto", "other_agent", "tata") === igsResultEnum.IGS_SUCCESS);
IGS.clearMappings();


// Services
let argsList = IGS.serviceArgsAddBool([], true);
argsList = IGS.serviceArgsAddInt(argsList, 1);
argsList = IGS.serviceArgsAddDouble(argsList, 1);
argsList = IGS.serviceArgsAddString(argsList, "my string");
argsList = IGS.serviceArgsAddData(argsList, new ArrayBuffer(32));
assert(argsList[0].type === iopTypeEnum.IGS_BOOL_T);
assert(argsList[0].value === true);
assert(argsList[1].type === iopTypeEnum.IGS_INTEGER_T);
assert(argsList[1].value === 1);
assert(argsList[2].type === iopTypeEnum.IGS_DOUBLE_T);
assert(argsList[2].value - 1 < 0.000001);
assert(argsList[3].type === iopTypeEnum.IGS_STRING_T);
assert(argsList[3].value === "my string");
assert(argsList[4].type === iopTypeEnum.IGS_DATA_T);
assert(argsList[4].value.byteLength === 32);

assert(IGS.serviceCount() === 0);
assert(!IGS.serviceExists(""));
assert(!IGS.serviceExists("toto"));
assert(IGS.serviceList().length === 0);
assert(IGS.serviceArgsList("").length === 0);
assert(IGS.serviceArgsList("toto").length === 0);
assert(IGS.serviceArgsCount("") === 0);
assert(IGS.serviceArgsCount("toto") === 0);
assert(IGS.serviceArgExists("", "") === false);
assert(IGS.serviceArgExists("toto", "") === false);
assert(IGS.serviceArgExists("", "toto") === false);
assert(IGS.serviceArgExists("toto", "toto") === false);
assert(IGS.serviceInit("myService", testerServiceCallback, null) === igsResultEnum.IGS_SUCCESS);
assert(IGS.serviceRemove("myService") === igsResultEnum.IGS_SUCCESS);
assert(IGS.serviceRemove("myService") === igsResultEnum.IGS_FAILURE);
assert(IGS.serviceArgAdd("myService", "myArg", iopTypeEnum.IGS_BOOL_T) === igsResultEnum.IGS_FAILURE);
assert(IGS.serviceArgRemove("myService", "myArg") === igsResultEnum.IGS_FAILURE);

assert(IGS.serviceInit("myService", testerServiceCallback, null) === igsResultEnum.IGS_SUCCESS);
assert(IGS.serviceArgAdd("myService", "myBool", iopTypeEnum.IGS_BOOL_T) === igsResultEnum.IGS_SUCCESS);
assert(IGS.serviceArgAdd("myService", "myInt", iopTypeEnum.IGS_INTEGER_T) === igsResultEnum.IGS_SUCCESS);
assert(IGS.serviceArgAdd("myService", "myDouble", iopTypeEnum.IGS_DOUBLE_T) === igsResultEnum.IGS_SUCCESS);
assert(IGS.serviceArgAdd("myService", "myString", iopTypeEnum.IGS_STRING_T) === igsResultEnum.IGS_SUCCESS);
assert(IGS.serviceArgAdd("myService", "myData", iopTypeEnum.IGS_DATA_T) === igsResultEnum.IGS_SUCCESS);
assert(IGS.serviceCount() === 1);
assert(IGS.serviceExists("myService"));
assert(IGS.serviceList().length === 1 && IGS.serviceList()[0] === "myService");
assert(IGS.serviceRemove("myService") === igsResultEnum.IGS_SUCCESS);

assert(IGS.serviceInit("myService", testerServiceCallback, null) === igsResultEnum.IGS_SUCCESS);
assert(IGS.serviceArgAdd("myService", "myBool", iopTypeEnum.IGS_BOOL_T) === igsResultEnum.IGS_SUCCESS);
assert(IGS.serviceArgAdd("myService", "myInt", iopTypeEnum.IGS_INTEGER_T) === igsResultEnum.IGS_SUCCESS);
assert(IGS.serviceArgAdd("myService", "myDouble", iopTypeEnum.IGS_DOUBLE_T) === igsResultEnum.IGS_SUCCESS);
assert(IGS.serviceArgAdd("myService", "myString", iopTypeEnum.IGS_STRING_T) === igsResultEnum.IGS_SUCCESS);
assert(IGS.serviceArgAdd("myService", "myData", iopTypeEnum.IGS_DATA_T) === igsResultEnum.IGS_SUCCESS);
assert(IGS.serviceArgsCount("myService") === 5);
assert(IGS.serviceArgExists("myService", "myBool"));
assert(IGS.serviceArgExists("myService", "myInt"));
assert(IGS.serviceArgExists("myService", "myDouble"));
assert(IGS.serviceArgExists("myService", "myString"));
assert(IGS.serviceArgExists("myService", "myData"));
argsList = IGS.serviceArgsList("myService");
assert(argsList);
assert(argsList[0].name === "myBool");
assert(argsList[0].type === iopTypeEnum.IGS_BOOL_T);
assert(argsList[1].name === "myInt");
assert(argsList[1].type === iopTypeEnum.IGS_INTEGER_T);
assert(argsList[2].name === "myDouble");
assert(argsList[2].type === iopTypeEnum.IGS_DOUBLE_T);
assert(argsList[3].name === "myString");
assert(argsList[3].type === iopTypeEnum.IGS_STRING_T);
assert(argsList[4].name === "myData");
assert(argsList[4].type === iopTypeEnum.IGS_DATA_T);
IGS.definitionSave();
assert(IGS.serviceRemove("myService") === igsResultEnum.IGS_SUCCESS);
IGS.clearDefinition();

IGS.definitionLoadFile("/tmp/simple Demo Agent.json");
assert(IGS.serviceArgsCount("myService") === 5);
assert(IGS.serviceArgExists("myService", "myBool"));
assert(IGS.serviceArgExists("myService", "myInt"));
assert(IGS.serviceArgExists("myService", "myDouble"));
assert(IGS.serviceArgExists("myService", "myString"));
assert(IGS.serviceArgExists("myService", "myData"));
argsList = IGS.serviceArgsList("myService");
assert(argsList);
assert(argsList[0].name === "myBool");
assert(argsList[0].type === iopTypeEnum.IGS_BOOL_T);
assert(argsList[1].name === "myInt");
assert(argsList[1].type === iopTypeEnum.IGS_INTEGER_T);
assert(argsList[2].name === "myDouble");
assert(argsList[2].type === iopTypeEnum.IGS_DOUBLE_T);
assert(argsList[3].name === "myString");
assert(argsList[3].type === iopTypeEnum.IGS_STRING_T);
assert(argsList[4].name === "myData");
assert(argsList[4].type === iopTypeEnum.IGS_DATA_T);


//prepare agent for dynamic tests by adding proper complete definitions
IGS.agentSetName(agentName);
IGS.logSetConsole(verbose);

IGS.definitionSetDescription("One example for each type of IOP and call");
IGS.definitionSetVersion("1.0");
IGS.inputCreate("my_impulsion", iopTypeEnum.IGS_IMPULSION_T, null);
IGS.inputCreate("my_bool", iopTypeEnum.IGS_BOOL_T, true);
IGS.inputCreate("my_int", iopTypeEnum.IGS_INTEGER_T, 1);
IGS.inputCreate("my_double", iopTypeEnum.IGS_DOUBLE_T, 1);
IGS.inputCreate("my_string", iopTypeEnum.IGS_STRING_T, "my string");
IGS.inputCreate("my_data", iopTypeEnum.IGS_DATA_T, new ArrayBuffer(32));
IGS.inputCreate("my_impulsion_split", iopTypeEnum.IGS_IMPULSION_T, null);
IGS.inputCreate("my_bool_split", iopTypeEnum.IGS_BOOL_T, true);
IGS.inputCreate("my_int_split", iopTypeEnum.IGS_INTEGER_T, 1);
IGS.inputCreate("my_double_split", iopTypeEnum.IGS_DOUBLE_T, 1);
IGS.inputCreate("my_string_split", iopTypeEnum.IGS_STRING_T, "my string");
IGS.inputCreate("my_data_split", iopTypeEnum.IGS_DATA_T, new ArrayBuffer(32));
IGS.outputCreate("my_impulsion", iopTypeEnum.IGS_IMPULSION_T, null);
IGS.outputCreate("my_bool", iopTypeEnum.IGS_BOOL_T, true);
IGS.outputCreate("my_int", iopTypeEnum.IGS_INTEGER_T, 1);
IGS.outputCreate("my_double", iopTypeEnum.IGS_DOUBLE_T, 1);
IGS.outputCreate("my_string", iopTypeEnum.IGS_STRING_T, "my string");
IGS.outputCreate("my_data", iopTypeEnum.IGS_DATA_T, new ArrayBuffer(32));
IGS.parameterCreate("my_impulsion", iopTypeEnum.IGS_IMPULSION_T, null);
IGS.parameterCreate("my_bool", iopTypeEnum.IGS_BOOL_T, true);
IGS.parameterCreate("my_int", iopTypeEnum.IGS_INTEGER_T, 1);
IGS.parameterCreate("my_double", iopTypeEnum.IGS_DOUBLE_T, 1);
IGS.parameterCreate("my_string", iopTypeEnum.IGS_STRING_T, "my string");
IGS.parameterCreate("my_data", iopTypeEnum.IGS_DATA_T, new ArrayBuffer(32));
IGS.serviceInit("myService", testerServiceCallback, null);
IGS.serviceArgAdd("myService", "myBool", iopTypeEnum.IGS_BOOL_T);
IGS.serviceArgAdd("myService", "myInt", iopTypeEnum.IGS_INTEGER_T);
IGS.serviceArgAdd("myService", "myDouble", iopTypeEnum.IGS_DOUBLE_T);
IGS.serviceArgAdd("myService", "myString", iopTypeEnum.IGS_STRING_T);
IGS.serviceArgAdd("myService", "myData", iopTypeEnum.IGS_DATA_T);

IGS.observeInput("my_impulsion", testerIOPCallback, null);
IGS.observeInput("my_bool", testerIOPCallback, null);
IGS.observeInput("my_int", testerIOPCallback, null);
IGS.observeInput("my_double", testerIOPCallback, null);
IGS.observeInput("my_string", testerIOPCallback, null);
IGS.observeInput("my_data", testerIOPCallback, null);
IGS.observeInput("my_impulsion_split", testerIOPCallback, null);
IGS.observeInput("my_bool_split", testerIOPCallback, null);
IGS.observeInput("my_int_split", testerIOPCallback, null);
IGS.observeInput("my_double_split", testerIOPCallback, null);
IGS.observeInput("my_string_split", testerIOPCallback, null);
IGS.observeInput("my_data_split", testerIOPCallback, null);

IGS.mappingAdd("my_impulsion", "partner", "sparing_impulsion");
IGS.mappingAdd("my_bool", "partner", "sparing_bool");
IGS.mappingAdd("my_int", "partner", "sparing_int");
IGS.mappingAdd("my_double", "partner", "sparing_double");
IGS.mappingAdd("my_string", "partner", "sparing_string");
IGS.mappingAdd("my_data", "partner", "sparing_data");

IGS.splitAdd("my_impulsion_split", "partner", "sparing_impulsion");
IGS.splitAdd("my_bool_split", "partner", "sparing_bool");
IGS.splitAdd("my_int_split", "partner", "sparing_int");
IGS.splitAdd("my_double_split", "partner", "sparing_double");
IGS.splitAdd("my_string_split", "partner", "sparing_string");
IGS.splitAdd("my_data_split", "partner", "sparing_data");

// IOP writing and types conversions
IGS.inputSetImpulsion("my_impulsion");
IGS.inputSetImpulsion("my_bool");
assert(!IGS.inputBool("my_bool"));
IGS.inputSetImpulsion("my_int");
assert(IGS.inputInt("my_int") === 0);
IGS.inputSetImpulsion("my_double");
assert(IGS.inputDouble("my_double")  < 0.000001);
IGS.inputSetImpulsion("my_string");
assert(IGS.inputString("my_string").length === 0);
IGS.inputSetImpulsion("my_data");
assert(IGS.inputData("my_data") === null);

IGS.inputSetBool("my_impulsion", true);
IGS.inputSetBool("my_bool", true);
assert(IGS.inputBool("my_bool"));
IGS.inputSetBool("my_int", true);
assert(IGS.inputInt("my_int") === 1);
IGS.inputSetBool("my_double", true);
assert(IGS.inputDouble("my_double") - 1.0 < 0.000001);
IGS.inputSetBool("my_string", true);
assert(IGS.inputString("my_string") === "1");
IGS.inputSetBool("my_data", true);
assert(IGS.inputData("my_data").byteLength === 1);

IGS.inputSetInt("my_impulsion", 3);
IGS.inputSetInt("my_bool", 3);
assert(IGS.inputBool("my_bool"));
IGS.inputSetInt("my_int", 3);
assert(IGS.inputInt("my_int") === 3);
IGS.inputSetInt("my_double", 3);
assert(IGS.inputDouble("my_double") - 3.0 < 0.000001);
IGS.inputSetInt("my_string", 3);
assert(IGS.inputString("my_string") === "3");
IGS.inputSetInt("my_data", 3);
assert(IGS.inputData("my_data").byteLength === 4);

IGS.inputSetDouble("my_impulsion", 3.3);
IGS.inputSetDouble("my_bool", 3.3);
assert(IGS.inputBool("my_bool"));
IGS.inputSetDouble("my_int", 3.3);
assert(IGS.inputInt("my_int") === 3);
IGS.inputSetDouble("my_double", 3.3);
assert(IGS.inputDouble("my_double") - 3.3 < 0.000001);
IGS.inputSetDouble("my_string", 3.3);
assert(IGS.inputString("my_string") === "3.300000");
IGS.inputSetDouble("my_data", 3.3);
assert(IGS.inputData("my_data").byteLength === 8);

IGS.inputSetString("my_impulsion", "true");
IGS.inputSetString("my_bool", "true");
assert(IGS.inputBool("my_bool"));
IGS.inputSetString("my_int", "3.3");
assert(IGS.inputInt("my_int") === 3);
IGS.inputSetString("my_double", "3.3");
assert(IGS.inputDouble("my_double") - 3.3 < 0.000001);
IGS.inputSetString("my_string", "3.3");
assert(IGS.inputString("my_string") === "3.3");
assert(IGS.inputSetString("my_data", "toto") === igsResultEnum.IGS_FAILURE);
assert(IGS.inputSetString("my_data", "0123456789abcdef") === igsResultEnum.IGS_SUCCESS);
assert(IGS.inputData("my_data").byteLength === 8);


IGS.inputSetData("my_impulsion", new ArrayBuffer(32));
IGS.inputSetData("my_bool", new ArrayBuffer(32));
IGS.inputSetData("my_int", new ArrayBuffer(32));
IGS.inputSetData("my_double", new ArrayBuffer(32));
IGS.inputSetData("my_string", new ArrayBuffer(32));
IGS.inputSetData("my_data", new ArrayBuffer(32));
assert(IGS.inputData("my_data").byteLength === 32);

IGS.inputSetBool("my_bool", true);
assert(IGS.inputBool("my_bool"));
assert(IGS.inputInt("my_bool") === 1);
assert(IGS.inputDouble("my_bool") - 1 < 0.000001);
assert(IGS.inputString("my_bool") === "true");
IGS.inputSetBool("my_bool", false);
assert(!IGS.inputBool("my_bool"));
assert(IGS.inputInt("my_bool") === 0);
assert(IGS.inputDouble("my_bool") < 0.000001);
assert(IGS.inputString("my_bool") === "false");
assert(IGS.inputData("my_bool").byteLength === 1)

IGS.inputSetInt("my_int", 3);
assert(IGS.inputBool("my_int"));
assert(IGS.inputInt("my_int") === 3);
assert(IGS.inputDouble("my_int") - 3 < 0.000001);
assert(IGS.inputString("my_int") === "3");
IGS.inputSetInt("my_int", 0);
assert(!IGS.inputBool("my_int"));
assert(IGS.inputInt("my_int") === 0);
assert(IGS.inputDouble("my_int") < 0.000001);
assert(IGS.inputString("my_int") === "0");
assert(IGS.inputData("my_int").byteLength === 4);

IGS.inputSetDouble("my_double", 3.3);
assert(IGS.inputBool("my_double"));
assert(IGS.inputInt("my_double") === 3);
assert(IGS.inputDouble("my_double") - 3.3 < 0.000001);
assert(IGS.inputString("my_double") === "3.300000");
IGS.inputSetDouble("my_double", 0.0);
assert(!IGS.inputBool("my_double"));
assert(IGS.inputInt("my_double") === 0);
assert(IGS.inputDouble("my_double") < 0.000001);
assert(IGS.inputString("my_double") === "0.000000");
assert(IGS.inputData("my_double").byteLength === 8);

IGS.inputSetString("my_string", "true");
assert(IGS.inputBool("my_string"));
IGS.inputSetString("my_string", "false");
assert(!IGS.inputBool("my_string"));
IGS.inputSetString("my_string", "10.1");
assert(IGS.inputInt("my_string") === 10);
assert(IGS.inputDouble("my_string") - 10.1 < 0.000001);
assert(IGS.inputString("my_string") === "10.1");
assert(IGS.inputData("my_string").byteLength === 5);

IGS.inputSetData("my_data", null);
assert(!IGS.inputBool("my_data"));
IGS.inputSetData("my_data", new ArrayBuffer(16));
assert(!IGS.inputBool("my_data"));
assert(IGS.inputInt("my_data") === 0);
assert(IGS.inputDouble("my_data") < 0.000001);
assert(IGS.inputString("my_data") === "");
assert(IGS.inputData("my_data").byteLength === 16);

//add multiple agents to be enabled and disabled on demand
//first additional agent is activated immediately
global.firstAgent = new IGS.Agent("firstAgent_bogus", true);
assert(firstAgent.name() === "firstAgent_bogus");
firstAgent.setName("firstAgent");
assert(firstAgent.name() === "firstAgent");
firstAgent.definitionSetDescription("First virtual agent");
firstAgent.definitionSetVersion("1.0");
firstAgent.inputCreate("first_impulsion", iopTypeEnum.IGS_IMPULSION_T, null);
firstAgent.inputCreate("first_bool", iopTypeEnum.IGS_BOOL_T, true);
firstAgent.inputCreate("first_int", iopTypeEnum.IGS_INTEGER_T, 1);
firstAgent.inputCreate("first_double", iopTypeEnum.IGS_DOUBLE_T, 1);
firstAgent.inputCreate("first_string", iopTypeEnum.IGS_STRING_T, "my string");
firstAgent.inputCreate("first_data", iopTypeEnum.IGS_DATA_T, new ArrayBuffer(32));
firstAgent.inputCreate("first_impulsion_split", iopTypeEnum.IGS_IMPULSION_T, null);
firstAgent.inputCreate("first_bool_split", iopTypeEnum.IGS_BOOL_T, true);
firstAgent.inputCreate("first_int_split", iopTypeEnum.IGS_INTEGER_T, 1);
firstAgent.inputCreate("first_double_split", iopTypeEnum.IGS_DOUBLE_T, 1);
firstAgent.inputCreate("first_string_split", iopTypeEnum.IGS_STRING_T, "my string");
firstAgent.inputCreate("first_data_split", iopTypeEnum.IGS_DATA_T, new ArrayBuffer(32));
firstAgent.outputCreate("first_impulsion", iopTypeEnum.IGS_IMPULSION_T, null);
firstAgent.outputCreate("first_bool", iopTypeEnum.IGS_BOOL_T, true);
firstAgent.outputCreate("first_int", iopTypeEnum.IGS_INTEGER_T, 1);
firstAgent.outputCreate("first_double", iopTypeEnum.IGS_DOUBLE_T, 1);
firstAgent.outputCreate("first_string", iopTypeEnum.IGS_STRING_T, "my string");
firstAgent.outputCreate("first_data", iopTypeEnum.IGS_DATA_T, new ArrayBuffer(32));
firstAgent.parameterCreate("first_impulsion", iopTypeEnum.IGS_IMPULSION_T, null);
firstAgent.parameterCreate("first_bool", iopTypeEnum.IGS_BOOL_T, true);
firstAgent.parameterCreate("first_int", iopTypeEnum.IGS_INTEGER_T, 1);
firstAgent.parameterCreate("first_double", iopTypeEnum.IGS_DOUBLE_T, 1);
firstAgent.parameterCreate("first_string", iopTypeEnum.IGS_STRING_T, "my string");
firstAgent.parameterCreate("first_data", iopTypeEnum.IGS_DATA_T, new ArrayBuffer(32));

firstAgent.serviceInit("firstService", agentServiceCallback, null);
firstAgent.serviceArgAdd("firstService", "firstBool", iopTypeEnum.IGS_BOOL_T);
firstAgent.serviceArgAdd("firstService", "firstInt", iopTypeEnum.IGS_INTEGER_T);
firstAgent.serviceArgAdd("firstService", "firstDouble", iopTypeEnum.IGS_DOUBLE_T);
firstAgent.serviceArgAdd("firstService", "firstString", iopTypeEnum.IGS_STRING_T);
firstAgent.serviceArgAdd("firstService", "firstData", iopTypeEnum.IGS_DATA_T);

firstAgent.observeInput("first_impulsion", agentIOPCallback, null);
firstAgent.observeInput("first_bool", agentIOPCallback, null);
firstAgent.observeInput("first_int", agentIOPCallback, null);
firstAgent.observeInput("first_double", agentIOPCallback, null);
firstAgent.observeInput("first_string", agentIOPCallback, null);
firstAgent.observeInput("first_data", agentIOPCallback, null);

firstAgent.observeInput("first_impulsion_split", agentIOPCallback, null);
firstAgent.observeInput("first_bool_split", agentIOPCallback, null);
firstAgent.observeInput("first_int_split", agentIOPCallback, null);
firstAgent.observeInput("first_double_split", agentIOPCallback, null);
firstAgent.observeInput("first_string_split", agentIOPCallback, null);
firstAgent.observeInput("first_data_split", agentIOPCallback, null);

firstAgent.mappingAdd("first_impulsion", "partner", "sparing_impulsion");
firstAgent.mappingAdd("first_bool", "partner", "sparing_bool");
firstAgent.mappingAdd("first_int", "partner", "sparing_int");
firstAgent.mappingAdd("first_double", "partner", "sparing_double");
firstAgent.mappingAdd("first_string", "partner", "sparing_string");
firstAgent.mappingAdd("first_data", "partner", "sparing_data");

firstAgent.splitAdd("first_impulsion_split", "partner", "sparing_impulsion");
firstAgent.splitAdd("first_bool_split", "partner", "sparing_bool");
firstAgent.splitAdd("first_int_split", "partner", "sparing_int");
firstAgent.splitAdd("first_double_split", "partner", "sparing_double");
firstAgent.splitAdd("first_string_split", "partner", "sparing_string");
firstAgent.splitAdd("first_data_split", "partner", "sparing_data");

 //second additional agent is NOT activated immediately
global.secondAgent = new IGS.Agent("secondAgent", false);
secondAgent.definitionSetDescription("Second virtual agent");
secondAgent.definitionSetVersion("1.0");
secondAgent.inputCreate("second_impulsion", iopTypeEnum.IGS_IMPULSION_T, null);
secondAgent.inputCreate("second_bool", iopTypeEnum.IGS_BOOL_T, true);
secondAgent.inputCreate("second_int", iopTypeEnum.IGS_INTEGER_T, 1);
secondAgent.inputCreate("second_double", iopTypeEnum.IGS_DOUBLE_T, 1);
secondAgent.inputCreate("second_string", iopTypeEnum.IGS_STRING_T, "my string");
secondAgent.inputCreate("second_data", iopTypeEnum.IGS_DATA_T, new ArrayBuffer(32));
secondAgent.inputCreate("second_impulsion_split", iopTypeEnum.IGS_IMPULSION_T, null);
secondAgent.inputCreate("second_bool_split", iopTypeEnum.IGS_BOOL_T, true);
secondAgent.inputCreate("second_int_split", iopTypeEnum.IGS_INTEGER_T, 1);
secondAgent.inputCreate("second_double_split", iopTypeEnum.IGS_DOUBLE_T, 1);
secondAgent.inputCreate("second_string_split", iopTypeEnum.IGS_STRING_T, "my string");
secondAgent.inputCreate("second_data_split", iopTypeEnum.IGS_DATA_T, new ArrayBuffer(32));
secondAgent.outputCreate("second_impulsion", iopTypeEnum.IGS_IMPULSION_T, null);
secondAgent.outputCreate("second_bool", iopTypeEnum.IGS_BOOL_T, true);
secondAgent.outputCreate("second_int", iopTypeEnum.IGS_INTEGER_T, 1);
secondAgent.outputCreate("second_double", iopTypeEnum.IGS_DOUBLE_T, 1);
secondAgent.outputCreate("second_string", iopTypeEnum.IGS_STRING_T, "my string");
secondAgent.outputCreate("second_data", iopTypeEnum.IGS_DATA_T, new ArrayBuffer(32));
secondAgent.parameterCreate("second_impulsion", iopTypeEnum.IGS_IMPULSION_T, null);
secondAgent.parameterCreate("second_bool", iopTypeEnum.IGS_BOOL_T, true);
secondAgent.parameterCreate("second_int", iopTypeEnum.IGS_INTEGER_T, 1);
secondAgent.parameterCreate("second_double", iopTypeEnum.IGS_DOUBLE_T, 1);
secondAgent.parameterCreate("second_string", iopTypeEnum.IGS_STRING_T, "my string");
secondAgent.parameterCreate("second_data", iopTypeEnum.IGS_DATA_T, new ArrayBuffer(32));
secondAgent.serviceInit("secondService", agentServiceCallback, null);
secondAgent.serviceArgAdd("secondService", "secondBool", iopTypeEnum.IGS_BOOL_T);
secondAgent.serviceArgAdd("secondService", "secondInt", iopTypeEnum.IGS_INTEGER_T);
secondAgent.serviceArgAdd("secondService", "secondDouble", iopTypeEnum.IGS_DOUBLE_T);
secondAgent.serviceArgAdd("secondService", "secondString", iopTypeEnum.IGS_STRING_T);
secondAgent.serviceArgAdd("secondService", "secondData", iopTypeEnum.IGS_DATA_T);

secondAgent.observeInput("second_impulsion", agentIOPCallback, null);
secondAgent.observeInput("second_bool", agentIOPCallback, null);
secondAgent.observeInput("second_int", agentIOPCallback, null);
secondAgent.observeInput("second_double", agentIOPCallback, null);
secondAgent.observeInput("second_string", agentIOPCallback, null);
secondAgent.observeInput("second_data", agentIOPCallback, null);

secondAgent.observeInput("second_impulsion_split", agentIOPCallback, null);
secondAgent.observeInput("second_bool_split", agentIOPCallback, null);
secondAgent.observeInput("second_int_split", agentIOPCallback, null);
secondAgent.observeInput("second_double_split", agentIOPCallback, null);
secondAgent.observeInput("second_string_split", agentIOPCallback, null);
secondAgent.observeInput("second_data_split", agentIOPCallback, null);

secondAgent.mappingAdd("second_impulsion", "partner", "sparing_impulsion");
secondAgent.mappingAdd("second_bool", "partner", "sparing_bool");
secondAgent.mappingAdd("second_int", "partner", "sparing_int");
secondAgent.mappingAdd("second_double", "partner", "sparing_double");
secondAgent.mappingAdd("second_string", "partner", "sparing_string");
secondAgent.mappingAdd("second_data", "partner", "sparing_data");

secondAgent.mappingAdd("second_impulsion", "tester", "my_impulsion");
secondAgent.mappingAdd("second_bool", "tester", "my_bool");
secondAgent.mappingAdd("second_int", "tester", "my_int");
secondAgent.mappingAdd("second_double", "tester", "my_double");
secondAgent.mappingAdd("second_string", "tester", "my_string");
secondAgent.mappingAdd("second_data", "tester", "my_data");

secondAgent.mappingAdd("second_impulsion", "firstAgent", "first_impulsion");
secondAgent.mappingAdd("second_bool", "firstAgent", "first_bool");
secondAgent.mappingAdd("second_int", "firstAgent", "first_int");
secondAgent.mappingAdd("second_double", "firstAgent", "first_double");
secondAgent.mappingAdd("second_string", "firstAgent", "first_string");
secondAgent.mappingAdd("second_data", "firstAgent", "first_data");

secondAgent.splitAdd("second_impulsion_split", "partner", "sparing_impulsion");
secondAgent.splitAdd("second_bool_split", "partner", "sparing_bool");
secondAgent.splitAdd("second_int_split", "partner", "sparing_int");
secondAgent.splitAdd("second_double_split", "partner", "sparing_double");
secondAgent.splitAdd("second_string_split", "partner", "sparing_string");
secondAgent.splitAdd("second_data_split", "partner", "sparing_data");

secondAgent.splitAdd("second_impulsion_split", "tester", "my_impulsion");
secondAgent.splitAdd("second_bool_split", "tester", "my_bool");
secondAgent.splitAdd("second_int_split", "tester", "my_int");
secondAgent.splitAdd("second_double_split", "tester", "my_double");
secondAgent.splitAdd("second_string_split", "tester", "my_string");
secondAgent.splitAdd("second_data_split", "tester", "my_data");

secondAgent.splitAdd("second_impulsion_split", "firstAgent", "first_impulsion");
secondAgent.splitAdd("second_bool_split", "firstAgent", "first_bool");
secondAgent.splitAdd("second_int_split", "firstAgent", "first_int");
secondAgent.splitAdd("second_double_split", "firstAgent", "first_double");
secondAgent.splitAdd("second_string_split", "firstAgent", "first_string");
secondAgent.splitAdd("second_data_split", "firstAgent", "first_data");

//test mapping in same process between second_agent and first_agent
secondAgent.activate();
firstAgent.outputSetBool("first_bool", true);
assert(secondAgent.inputBool("second_bool"));
firstAgent.outputSetBool("first_bool", false);
assert(!secondAgent.inputBool("second_bool"));
firstAgent.outputSetInt("first_int", 5);
assert(secondAgent.inputInt("second_int") === 5);
firstAgent.outputSetDouble("first_double", 5.5);
assert(secondAgent.inputDouble("second_double") - 5.5 < 0.000001);
firstAgent.outputSetString("first_string", "test string mapping");
assert(secondAgent.inputString("second_string") === "test string mapping");
firstAgent.outputSetData("first_data", new ArrayBuffer(16));
assert(secondAgent.inputData("second_data").byteLength === 16);

//test service in the same process
argsList = [];
argsList = IGS.serviceArgsAddBool(argsList, true);
argsList = IGS.serviceArgsAddInt(argsList, 13);
argsList = IGS.serviceArgsAddDouble(argsList, 13.3);
argsList = IGS.serviceArgsAddString(argsList, "my string arg");
argsList = IGS.serviceArgsAddData(argsList, new ArrayBuffer(16));
firstAgent.serviceCall("secondAgent", "secondService", argsList, "token");

//elections
assert(IGS.electionLeave("my election") === igsResultEnum.IGS_FAILURE);
assert(IGS.electionJoin("my election") === igsResultEnum.IGS_SUCCESS);
assert(IGS.electionJoin("my election") === igsResultEnum.IGS_FAILURE);
assert(IGS.electionJoin("INGESCAPE_PRIVATE") === igsResultEnum.IGS_FAILURE);
assert(IGS.electionLeave("my election") === igsResultEnum.IGS_SUCCESS);
assert(IGS.electionLeave("my election") === igsResultEnum.IGS_FAILURE);
assert(IGS.electionLeave("my other election") === igsResultEnum.IGS_FAILURE);
assert(IGS.electionJoin("my other election") === igsResultEnum.IGS_SUCCESS);
assert(IGS.electionJoin("my other election") === igsResultEnum.IGS_FAILURE);
assert(IGS.electionLeave("my other election") === igsResultEnum.IGS_SUCCESS);
assert(IGS.electionLeave("my other election") === igsResultEnum.IGS_FAILURE);

IGS.agentSetFamily("family_test");

//TODO : test agent events in same process

if (staticTests) {
    //we terminate now after passing the static tests
    secondAgent.destroy();
    delete secondAgent;
    firstAgent.destroy();
    delete firstAgent;
    process.exit();
}
else {
    //we run normally
    if (networkDevice === undefined) {
        // we have no device to start with: try to find one
        let devices = IGS.netDevicesList();
        let nb = devices.length;
        if (nb === 1) {
            // we have exactly one compliant network device available: we use it
            networkDevice = devices[0];
            IGS.info("using " + networkDevice + " as default network device (this is the only one available)");
        } else {
            if (nb === 0) {
                IGS.error("No network device found: aborting.");
                process.exit(1);
            }
            else {
                IGS.error("No network device passed as command line parameter and several are available.");
                console.log("Please use one of these network devices:");
                for (let i = 0; i < nb; i++){
                    console.log("\t" + devices[i]);
                }
                printUsage();
                process.exit(1);
            }
        }
    }
    firstAgent.activate();
}

// Stop the agent when server receive SIGINT signal
process.on('SIGINT', function() {
    process.exit();
});

// Stop the agent when Node process exit 
process.on('exit', (code) => {
    IGS.stop();
});
