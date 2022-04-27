/*  =========================================================================
    index.d.ts - Provide types support for developpers.

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

//enums
export type LogLevels = {
    IGS_LOG_TRACE: number;
    IGS_LOG_DEBUG: number;
    IGS_LOG_INFO: number;
    IGS_LOG_WARN: number;
    IGS_LOG_ERROR: number;
    IGS_LOG_FATAL: number;
};
export function logLevels(): LogLevels;

export type IOPValueTypes = {
    IGS_INTEGER_T: number;
    IGS_DOUBLE_T: number;
    IGS_STRING_T: number;
    IGS_BOOL_T: number;
    IGS_IMPULSION_T: number;
    IGS_DATA_T: number;
    IGS_UNKNOWN_T: number;
};
export function iopValueTypes(): IOPValueTypes;

export type IOPTypes = {
    IGS_INPUT_T: number;
    IGS_OUTPUT_T: number;
    IGS_PARAMETER_T: number;
};
export function iopValueTypes(): IOPTypes;

export type MonitorEventTypes = {
    IGS_NETWORK_OK: number;
    IGS_NETWORK_DEVICE_NOT_AVAILABLE: number;
    IGS_NETWORK_ADDRESS_CHANGED: number;
    IGS_NETWORK_OK_AFTER_MANUAL_RESTART: number;
};
export function monitorEventTypes(): MonitorEventTypes;

export type ResultTypes = {
    IGS_SUCCESS: number;
    IGS_FAILURE: number;
};
export function resultTypes(): ResultTypes;

export type AgentEventTypes = {
    IGS_PEER_ENTERED: number;
    IGS_PEER_EXITED: number;
    IGS_AGENT_ENTERED: number;
    IGS_AGENT_UPDATED_DEFINITION: number;
    IGS_AGENT_KNOWS_US: number;
    IGS_AGENT_EXITED: number;
    IGS_AGENT_UPDATED_MAPPING: number;
    IGS_AGENT_WON_ELECTION: number;
    IGS_AGENT_LOST_ELECTION: number;
};
export function agentEventTypes(): AgentEventTypes;

export type ReplayModes = {
    IGS_REPLAY_INPUT: number;
    IGS_REPLAY_OUTPUT: number;
    IGS_REPLAY_PARAMETER: number;
    IGS_REPLAY_EXECUTE_SERVICE: number;
    IGS_REPLAY_CALL_SERVICE: number;
};
export function replayModes(): ReplayModes;

//admin_config_utils
export function version(): number;
export function protocol(): number;
export function netDevicesList(): string[];
export function netAddressesList(): string[];
export function commandLine(): string;
export function setCommandLine(commandLine:string): undefined;
export function mappingSetOutputsRequest(outputsRequest:boolean): undefined;
export function mappingOutputsRequest(): boolean;
export function logSetConsole(logConsole:boolean): undefined;
export function logConsole(): boolean;
export function logSetConsoleLevel(consoleLevel:number): undefined;
export function logConsoleLevel(): number;
export function logSetConsoleColor(consoleColor:boolean): undefined;
export function logConsoleColor(): boolean;
export function logSetStream(logStream:boolean): undefined;
export function logStream(): boolean;
export function logSetFile(logInFile:boolean, logPath:undefined|null|string): undefined;
export function logFile(): boolean;
export function logSetFilePath(filePath:string): undefined;
export function logFilePath(): string;
export function logSetFileLevel(fileLevel:number): undefined;
export function logSetFileMaxLineLength(maxLineLength:number): undefined;
export function logIncludeServices(includeServices:boolean): undefined;
export function logIncludeData(includeData:boolean): undefined;
export function trace(log:string): undefined;
export function debug(log:string): undefined;
export function info(log:string): undefined;
export function warn(log:string): undefined;
export function error(log:string): undefined;
export function fatal(log:string): undefined;
export function definitionSetPath(path:string): undefined;
export function mappingSetPath(path:string): undefined;
export function definitionSave(): undefined;
export function mappingSave(): undefined;
export function setIpcDir(path:string): undefined;
export function ipcDir(): string;
export function setIpc(ipc:boolean): undefined;
export function hasIpc(): boolean;
export function timerStart(delay:number, times:number, cb:(timerID:number, myData:any)=>undefined, myData:any): number;
export function timerStop(timerID:number): undefined;

//advanced
export function brokerAdd(brokerEndpoint:string): number;
export function clearBrokers(): undefined;
export function brokerEnableWithEndpoint(ourBrokerEndpoint:string): undefined;
export function brokerSetAdvertizedEndpoint(advertizedEndpoint:string|null): undefined;
export function startWithBrokers(agentEndpoint:string): number;
export function enableSecurity(privateCertificateFile:string|null, publicCertificatesDirectory:string|null): number;
export function disableSecurity(): undefined; 
export function brokerAddSecure(brokerEndpoint:string, pathToPublicCertificateForBroker:string): number;
export function electionJoin(electionName:string): number;
export function electionLeave(electionName:string): number;
export function netSetPublishingPort(port:number): undefined;
export function netSetLogStreamPort(port:number): undefined;
export function netSetDiscoveryInterval(intervalMs:number): undefined;
export function netSetTimeout(durationMs:number): undefined;
export function netRaiseSocketsLimit(): undefined;
export function netSetHighWaterMarks(hwmValue:number): undefined;
export function netPerformanceCheck(peerID:string, msgSize:number, msgsNbr:number): undefined;
export function agentSetFamily(family:string): undefined;
export function agentFamily(): string;
export function monitorStart(period:number): undefined;
export function monitorStartWithNetwork(period:number, networkDevice:string, port:number): undefined;
export function monitorStop(): undefined;
export function monitorIsRunning(): bool;
export function monitorSetStartStop(flag:bool): undefined;
export function observeMonitor(cb:(event:number, device:string, ipAddress:string, myData:any)=>undefined, myData:any): number;
export function replayInit(logFilePath:string, speed:number, startTime:string, waitForStart:boolean, replayMode:number, agentName:string): undefined;
export function replayStart(): undefined;
export function replayPause(pause:bool): undefined;
export function replayTerminate(): undefined;
export function clearContext(): undefined;

//agent
export class Agent {
    constructor (name:string, activateImmediately:boolean);

    destroy():undefined;
    activate():undefined;
    deactivate():undefined;
    isActivated():boolean;
    observe(cb:(agent:Agent, isActivated:boolean, myData:any) => undefined, myData:any):undefined;

    trace(log:string):undefined;
    debug(log:string):undefined;
    info(log:string):undefined;
    warn(log:string):undefined;
    error(log:string):undefined;
    fatal(log:string):undefined;

    name():string;
    setName(name:string):undefined;
    family():string;
    setFamily(family:string):undefined;
    uuid():string;
    state():string;
    setState(state:string):undefined;

    mute():undefined;
    unmute():undefined;
    isMuted():boolean;
    observeMute(cb:(agent:Agent, mute:boolean, myData:any) => undefined, myData:any):undefined;
    observeAgentEvents(cb:(agent:Agent, event:number, uuid:string, name:string, eventData:string|null, myData:any) => undefined, myData:any):undefined;

    definitionLoadStr(jsonDefinition:string):number;
    definitionLoadFile(filePath:string):number;
    clearDefinition():undefined;
    definitionJson():string;
    definitionDescription():string;
    definitionVersion():string;
    definitionSetDescription(description:string):undefined;
    definitionSetVersion(version:string):undefined;

    inputCreate(name:string, type:number, value:number|boolean|string|null|ArrayBuffer):number;
    outputCreate(name:string, type:number, value:number|boolean|string|null|ArrayBuffer):number;
    parameterCreate(name:string, type:number, value:number|boolean|string|null|ArrayBuffer):number;
    inputRemove(name:string):number;
    outputRemove(name:string):number;
    parameterRemove(name:string):number;

    inputType(name:string):number;
    outputType(name:string):number;
    parameterType(name:string):number;
    inputCount():number;
    outputCount():number;
    parameterCount():number;
    inputList():string[];
    outputList():string[];
    parameterList():string[];
    inputExists(name:string):boolean;
    outputExists(name:string):boolean;
    parameterExists(name:string):boolean;

    inputBool(name:string):boolean;
    inputInt(name:string):number;
    inputDouble(name:string):number;
    inputString(name:string):string;
    inputData(name:string):null|ArrayBuffer;

    outputBool(name:string):boolean;
    outputInt(name:string):number;
    outputDouble(name:string):number;
    outputString(name:string):string;
    outputData(name:string):null|ArrayBuffer;

    parameterBool(name:string):boolean;
    parameterInt(name:string):number;
    parameterDouble(name:string):number;
    parameterString(name:string):string;
    parameterData(name:string):null|ArrayBuffer;

    inputSetBool(name:string, value:boolean):number;
    inputSetInt(name:string, value:number):number;
    inputSetDouble(name:string, value:number):number;
    inputSetString(name:string, value:string):number;
    inputSetImpulsion(name:string):number;
    inputSetData(name:string, value:null|ArrayBuffer):number;

    outputSetBool(name:string, value:boolean):number;
    outputSetInt(name:string, value:number):number;
    outputSetDouble(name:string, value:number):number;
    outputSetString(name:string, value:string):number;
    outputSetImpulsion(name:string):number;
    outputSetData(name:string, value:null|ArrayBuffer):number;

    parameterSetBool(name:string, value:boolean):number;
    parameterSetInt(name:string, value:number):number;
    parameterSetDouble(name:string, value:number):number;
    parameterSetString(name:string, value:string):number;
    parameterSetData(name:string, value:null|ArrayBuffer):number;

    constraintsEnforce(enforce:boolean):undefined;
    inputAddConstraint(name:string, constraint:string):number;
    outputAddConstraint(name:string, constraint:string):number;
    parameterAddConstraint(name:string, constraint:string):number;
    inputSetDescription(name:string, description:string):undefined;
    outputSetDescription(name:string, description:string):undefined;
    parameterSetDescription(name:string, description:string):undefined;
    
    clearInput(name:string):undefined;
    clearOutput(name:string):undefined;
    clearParameter(name:string):undefined;

    observeInput(name:string, cb:(agent:Agent, iopType:number, name:string, valueType:number, value:number|boolean|string|null|ArrayBuffer, myData:any) => undefined, myData:any):undefined;
    observeOutput(name:string, cb:(agent:Agent, iopType:number, name:string, valueType:number, value:number|boolean|string|null|ArrayBuffer, myData:any) => undefined, myData:any):undefined;
    observeParameter(name:string, cb:(agent:Agent, iopType:number, name:string, valueType:number, value:number|boolean|string|null|ArrayBuffer, myData:any) => undefined, myData:any):undefined;

    outputMute(name:string):undefined;
    outputUnmute(name:string):undefined;
    outputIsMuted(name:string):boolean;

    mappingLoadStr(jsonMapping:string):number;
    mappingLoadFile(filePath:string):number;
    mappingJson():string;
    mappingCount():number;
    clearMappings():undefined;
    clearMappingsWithAgent(agentName:string):undefined;
    mappingAdd(fromOurInput:string, toAgent:string, withOutput:string):BigInt;
    mappingRemoveWithId(id:BigInt):number;
    mappingRemoveWithName(fromOurInput:string, toAgent:string, withOutput:string): number;

    splitCount():number;
    splitAdd(fromOurInput:string, toAgent:string, withOutput:string):BigInt;
    splitRemoveWithId(id:BigInt):number;
    splitRemoveWithName(fromOurInput:string, toAgent:string, withOutput:string):number;

    mappingOutputsRequest():boolean;
    mappingSetOutputsRequest(outputsRequest:boolean):undefined;

    serviceCall(agentNameOrUUID:string, serviceName:string, arguments:null|Object[], token:string):number;
    serviceInit(name:string, cb: (agent:Agent, senderAgentName:string, senderAgentUUID:string, serviceName:string, arguments:null|Object[], token:string, myData:any) => undefined, myData:any):number
    serviceRemove(name:string):number;
    serviceArgAdd(serviceName:string, argName:string, type:number):number;
    serviceArgRemove(serviceName:string, argName:string):number;
    serviceCount():number;
    serviceExists(name:string):boolean;
    serviceList():string[];
    serviceArgsList(serviceName:string):Object[];
    serviceArgsCount(serviceName:string):number;
    serviceArgExists(serviceName:string, argName:string):boolean;

    electionJoin(electionName:string):number;
    electionLeave(electionName:string):number;

    definitionSetPath(path:string):undefined;
    definitionSave():undefined;
    mappingSetPath(path:string):undefined;
    mappingSave():undefined;
}

//definition
export function definitionLoadStr(jsonDefinition:string):number;
export function definitionLoadFile(filePath:string):number;
export function clearDefinition():undefined;
export function definitionJson():string;
export function definitionDescription():string;
export function definitionVersion():string;
export function definitionSetDescription(description:string):undefined;
export function definitionSetVersion(version:string):undefined;
export function inputCreate(name:string, type:number, value:number|boolean|string|null|ArrayBuffer):number;
export function outputCreate(name:string, type:number, value:number|boolean|string|null|ArrayBuffer):number;
export function parameterCreate(name:string, type:number, value:number|boolean|string|null|ArrayBuffer):number;
export function inputRemove(name:string):number;
export function outputRemove(name:string):number;
export function parameterRemove(name:string):number;

//init_control
export function startWithDevice(device:string, port:number):number;
export function startWithIp(ipAddress:string, port:number):number;
export function stop():undefined;
export function isStarted():boolean;
export function observeForcedStop(cb:(myData:any) => undefined, myData:any):undefined;
export function agentSetName(name:string):undefined;
export function agentName():string;
export function agentUuid():string;
export function agentSetState(state:string):undefined;
export function agentState():string;
export function agentMute():undefined;
export function agentUnmute():undefined;
export function agentIsMuted():boolean;
export function observeMute(cb:(isMuted:boolean, myData:any) => undefined, myData:any):undefined;
export function freeze():undefined;
export function isFrozen():boolean;
export function unfreeze():undefined;
export function observeFreeze(cb:(isPaused:boolean, myData:any) => undefined, myData:any):undefined;
export function observeAgentEvents(cb:(event:number, uuid:string, name:string, eventData:string|null, myData:any) => undefined, myData:any):undefined;

//iop
export function inputBool(name:string):boolean;
export function inputInt(name:string):number;
export function inputDouble(name:string):number;
export function inputString(name:string):string;
export function inputData(name:string):null|ArrayBuffer;
export function outputBool(name:string):boolean;
export function outputInt(name:string):number;
export function outputDouble(name:string):number;
export function outputString(name:string):string;
export function outputData(name:string):null|ArrayBuffer;
export function parameterBool(name:string):boolean;
export function parameterInt(name:string):number;
export function parameterDouble(name:string):number;
export function parameterString(name:string):string;
export function parameterData(name:string):null|ArrayBuffer;

export function inputSetBool(name:string, value:boolean):number;
export function inputSetInt(name:string, value:number):number;
export function inputSetDouble(name:string, value:number):number;
export function inputSetString(name:string, value:string):number;
export function inputSetImpulsion(name:string):number;
export function inputSetData(name:string, value:null|ArrayBuffer):number;
export function outputSetBool(name:string, value:boolean):number;
export function outputSetInt(name:string, value:number):number;
export function outputSetDouble(name:string, value:number):number;
export function outputSetString(name:string, value:string):number;
export function outputSetImpulsion(name:string):number;
export function outputSetData(name:string, value:null|ArrayBuffer):number;
export function parameterSetBool(name:string, value:boolean):number;
export function parameterSetInt(name:string, value:number):number;
export function parameterSetDouble(name:string, value:number):number;
export function parameterSetString(name:string, value:string):number;
export function parameterSetData(name:string, value:null|ArrayBuffer):number;

export function clearInput(name:string):undefined;
export function clearOutput(name:string):undefined;
export function clearParameter(name:string):undefined;
export function observeInput(name:string, cb:(iopType:number, name:string, valueType:number, value:number|boolean|string|null|ArrayBuffer, myData:any) => undefined, myData:any):undefined;
export function observeOutput(name:string, cb:(iopType:number, name:string, valueType:number, value:number|boolean|string|null|ArrayBuffer, myData:any) => undefined, myData:any):undefined;
export function observeParameter(name:string, cb:(iopType:number, name:string, valueType:number, value:number|boolean|string|null|ArrayBuffer, myData:any) => undefined, myData:any):undefined;
export function outputMute(name:string):undefined;
export function outputUnmute(name:string):undefined;
export function outputIsMuted(name:string):boolean;
export function inputType(name:string):number;
export function outputType(name:string):number;
export function parameterType(name:string):number;
export function inputCount():number;
export function outputCount():number;
export function parameterCount():number;
export function inputList():string[];
export function outputList():string[];
export function parameterList():string[];
export function inputExists(name:string):boolean;
export function outputExists(name:string):boolean;
export function parameterExists(name:string):boolean;

export function constraintsEnforce(enforce:boolean):undefined;
export function inputAddConstraint(name:string, constraint:string):number;
export function outputAddConstraint(name:string, constraint:string):number;
export function parameterAddConstraint(name:string, constraint:string):number;
export function inputSetDescription(name:string, description:string):undefined;
export function outputSetDescription(name:string, description:string):undefined;
export function parameterSetDescription(name:string, description:string):undefined;

//mapping
export function mappingLoadStr(jsonMapping:string):number;
export function mappingLoadFile(path:string):number;
export function clearMappings():undefined;
export function clearMappingsWithAgent(name:string):undefined;
export function mappingJson():string;
export function mappingCount():number;
export function mappingAdd(fromOurInput:string, toAgent:string, withOutput:string):BigInt;
export function mappingRemoveWithId(id:BigInt):number;
export function mappingRemoveWithName(fromOurInput:string, toAgent:string, withOutput:string):number;
export function splitCount():number;
export function splitAdd(fromOurInput:string, toAgent:string, withOutput:string):BigInt;
export function splitRemoveWithId(id:BigInt):number;
export function splitRemoveWithName(fromOurInput:string, toAgent:string, withOutput:string):number;

//service
export function serviceArgsAddInt(serviceArgs:Object[], value:number):Object[];
export function serviceArgsAddBool(serviceArgs:Object[], value:boolean):Object[];
export function serviceArgsAddDouble(serviceArgs:Object[], value:number):Object[];
export function serviceArgsAddString(serviceArgs:Object[], value:string):Object[];
export function serviceArgsAddData(serviceArgs:Object[], value:null|ArrayBuffer):Object[];
export function serviceCall(agentNameOrUUID:string, serviceName:string, arguments:null|Object[], token:string):number;
export function serviceInit(name:string, cb: (senderAgentName:string, senderAgentUUID:string, serviceName:string, arguments:null|Object[], token:string, myData:any) => undefined, myData:any):number;
export function serviceRemove(name:string):number;
export function serviceArgAdd(serviceName:string, argName:string, type:number):number;
export function serviceArgRemove(serviceName:string, argName:string):number;
export function serviceCount():number;
export function serviceExists(name:string):boolean;
export function serviceList():string[];
export function serviceArgsList(serviceName:string):Object[];
export function serviceArgsCount(serviceName:string):number;
export function serviceArgExists(serviceName:string):boolean;
