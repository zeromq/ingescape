//
//  igs_replay.c
//  ingescape
//
//  Created by Stephane Vales on 18/09/2020.
//  Copyright © 2020 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#if (defined WIN32 || defined _WIN32)
#define timegm _mkgmtime
#include "unixfunctions.h"
#endif
#include "ingescape_private.h"
#include <czmq.h>

//general variables
zactor_t *replay_actor = NULL;
zloop_t *replay_loop = NULL;
zfile_t *replay_file = NULL;
bool replay_canStart = false;
size_t replay_speed = 0;
uint replay_mode = 0;
char replay_startTime[9] = "";
bool replay_isPaused = false;
bool replay_shallStop = false;
bool replay_isBeyondStartTime = true;
time_t replay_requestedStartTime = 0;
const char *replay_currentLine = NULL;
char replay_agent[IGS_MAX_AGENT_NAME_LENGTH] = "";
time_t replay_start = 0;
time_t replay_end = 0;
long replay_nbLines = 0;

//current action to execute
char current_agent[IGS_MAX_AGENT_NAME_LENGTH] = "";
int current_microsec = 0;
char current_actionTypeS[16] = "";
igs_replay_mode_t current_actionType = 0;
char current_iopName[IGS_MAX_IOP_NAME_LENGTH] = "";
char current_dataTypeS[64] = "";
iopType_t current_dataType = IGS_UNKNOWN_T;
char current_iopData[8192] = "";
struct tm current_time = {0};
time_t current_unixTime = 0;
time_t current_unixMsecTime = 0;
time_t previous_unixMsecTime = 0;


long long executeCurrentAndFindNextAction(void){
    //execute current action
    if (current_actionType && (streq(replay_agent,"") || streq(current_agent, replay_agent))){
        if (current_dataType == IGS_DATA_T && current_iopData[0] == '|'){
            //ignore this data entry because it is a size and not actual binary data
        }else{
            igs_agent_t *agent, *tmp;
            HASH_ITER(hh, coreContext->agents, agent, tmp){
                if (streq(agent->name, current_agent)){
                    switch (current_actionType) {
                        case IGS_REPLAY_INPUT:
                            if(replay_mode & IGS_REPLAY_INPUT){
                                igs_info("replaying %s.%s.%s = (%s) %s", agent->name, "input", current_iopName, current_dataTypeS, current_iopData);
                                igsAgent_writeInputAsString(agent, current_iopName, current_iopData);
                            }
                            break;
                        case IGS_REPLAY_OUTPUT:
                            if(replay_mode & IGS_REPLAY_OUTPUT){
                                igs_info("replaying %s.%s.%s = (%s) %s", agent->name, "output", current_iopName, current_dataTypeS, current_iopData);
                                igsAgent_writeOutputAsString(agent, current_iopName, current_iopData);
                            }
                            break;
                        case IGS_REPLAY_PARAMETER:
                            if(replay_mode & IGS_REPLAY_PARAMETER){
                                igs_info("replaying %s.%s.%s = (%s) %s", agent->name, "parameter", current_iopName, current_dataTypeS, current_iopData);
                                igsAgent_writeParameterAsString(agent, current_iopName, current_iopData);
                            }
                            break;
                        case IGS_REPLAY_SENT_CALL:
                            if(replay_mode & IGS_REPLAY_SENT_CALL){
                                //TODO:
                            }
                            break;
                        case IGS_REPLAY_RECEIVED_CALL:
                            if(replay_mode & IGS_REPLAY_RECEIVED_CALL){
                                //TODO:
                            }
                            break;
                            
                        default:{
                            break;
                        }
                    }
                }
            }
        }
    }
    current_actionType = 0;
    
    //find next action
    replay_currentLine = zfile_readln(replay_file);
    replay_nbLines++;
    while (replay_currentLine) {
        bool foundData = false;
        //Try to find an IOP or call log entry
        int res = sscanf((char *)replay_currentLine, "%1023[^;];%d/%d/%d;%d:%d:%d.%d;DEBUG;model_writeIOP;set %15s %1023s to %63s %8191[^\0]",
                         current_agent,
                         &current_time.tm_mday, &current_time.tm_mon, &current_time.tm_year,
                         &current_time.tm_hour, &current_time.tm_min, &current_time.tm_sec, &current_microsec,
                         current_actionTypeS, current_iopName, current_dataTypeS, current_iopData);
        if (res == 12){
            //printf("line: %s\n", replay_currentLine);
            if (streq(current_actionTypeS, "input")){
                current_actionType = IGS_REPLAY_INPUT;
            } else if (streq(current_actionTypeS, "output")){
                current_actionType = IGS_REPLAY_OUTPUT;
            } else if (streq(current_actionTypeS, "parameter")){
                current_actionType = IGS_REPLAY_PARAMETER;
            } else {
                current_actionType = 0;
            }
            
            if (streq(current_dataTypeS, "impulsion")){
                current_dataType = IGS_IMPULSION_T;
            } else if (streq(current_dataTypeS, "bool")){
                current_dataType = IGS_BOOL_T;
            } else if (streq(current_dataTypeS, "int")){
                current_dataType = IGS_INTEGER_T;
            } else if (streq(current_dataTypeS, "double")){
                current_dataType = IGS_DOUBLE_T;
            } else if (streq(current_dataTypeS, "string")){
                current_dataType = IGS_STRING_T;
            } else if (streq(current_dataTypeS, "data")){
                current_dataType = IGS_DATA_T;
            } else {
                current_dataType = IGS_UNKNOWN_T;
            }
            
            foundData = true;
        } else {
            //TODO: handle call logs
        }
        
        if (foundData){
            //compute current time
            if (replay_speed > 0 || (replay_requestedStartTime == 0 && strlen(replay_startTime) > 0)){
                current_time.tm_year -= 1900;
                current_time.tm_mon -= 1;
                current_time.tm_isdst = 0;
                current_unixTime = timegm(&current_time);
                current_unixMsecTime = current_unixTime * 1000 + (int)(current_microsec / 1000);
            }
            
            //compute start time if needed
            if (replay_requestedStartTime == 0 && strlen(replay_startTime) > 0){
                //we need to init day, month and year for our replay_requestedStartTime
                //based on the replay file.
                struct tm requestedStartTime = {0};
                requestedStartTime.tm_year = current_time.tm_year;
                requestedStartTime.tm_mon = current_time.tm_mon;
                requestedStartTime.tm_mday = current_time.tm_mday;
                requestedStartTime.tm_isdst = 0;
                int nb = sscanf(replay_startTime, "%d:%d:%d",
                                &requestedStartTime.tm_hour, &requestedStartTime.tm_min, &requestedStartTime.tm_sec);
                if(nb == 3){ //we have a valid start time : we activate flag
                    replay_isBeyondStartTime = false;
                    replay_requestedStartTime = timegm(&requestedStartTime);
                }else{
                    igs_error("invalid start time : '%s'", replay_startTime);
                }
            }
            
            //Compute return value depending on situation
            if (replay_isBeyondStartTime && replay_speed > 0 && previous_unixMsecTime != 0){
                //replay is active with non-null speed, startTime has not been reached
                long long delta = (long long)((current_unixMsecTime - previous_unixMsecTime) / replay_speed);
                previous_unixMsecTime = current_unixMsecTime;
                return delta;
            } else if (!replay_isBeyondStartTime && current_unixTime >= replay_requestedStartTime){
                //we have reached requested start time
                igs_info("reached start time : %s", replay_startTime);
                replay_isBeyondStartTime = true;
                //full-throttle replay (certainly until next entry because we just reached requested start time)
                previous_unixMsecTime = current_unixMsecTime;
                return 0;
            } else {
                //full-throttle replay
                previous_unixMsecTime = current_unixMsecTime;
                return 0;
            }
        }else{
            //no usefull data found : continue reading file
            replay_currentLine = zfile_readln(replay_file);
            replay_nbLines++;
        }
    }
    //reached end of file
    return -1;
}

//main function for parsing and using the log files
int replayRunThroughLogFile(zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(timer_id);
    IGS_UNUSED(arg);
    
    if (!replay_start)
        replay_start = zclock_mono();
    if (replay_shallStop)
        return -1;
    if (replay_isPaused) {
        zloop_timer(loop, 250, 1, replayRunThroughLogFile, NULL);
    }
    
    long long timeToWait = executeCurrentAndFindNextAction();
    while (timeToWait == 0 && !replay_shallStop && !replay_isPaused){
        //if timeToWait is zero, we continue as fast as we can
        timeToWait = executeCurrentAndFindNextAction();
    }
    if (!replay_shallStop && !replay_isPaused){
        if (timeToWait == -1)
            return -1; //stop
        if (timeToWait > 250) {
            //use timer
            zloop_timer(loop, timeToWait, 1, replayRunThroughLogFile, NULL);
        } else { //value is between 0 and 250
            //use sleep
            zclock_sleep((int)timeToWait);
            replayRunThroughLogFile(loop, timer_id, arg);
        }
    }
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// CALLS

void igs_replayInitCB(const char *senderAgentName, const char *senderAgentUUID,
                     const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
                     const char *token, void* myData){
    IGS_UNUSED(senderAgentName);
    IGS_UNUSED(senderAgentUUID);
    IGS_UNUSED(callName);
    IGS_UNUSED(nbArgs);
    IGS_UNUSED(token);
    IGS_UNUSED(myData);
    igs_replayTerminate();
    char *logFilePath = firstArgument->c;
    size_t speed = firstArgument->next->i;
    char *startTime = firstArgument->next->next->c;
    bool waitForStart = firstArgument->next->next->next->b;
    uint replayMode = firstArgument->next->next->next->next->i;
    char *agent = firstArgument->next->next->next->next->next->c;
    igs_replayInit(logFilePath, speed, startTime, waitForStart, replayMode, agent);
}

void igs_replayStartCB(const char *senderAgentName, const char *senderAgentUUID,
                     const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
                     const char *token, void* myData){
    IGS_UNUSED(senderAgentName);
    IGS_UNUSED(senderAgentUUID);
    IGS_UNUSED(callName);
    IGS_UNUSED(firstArgument);
    IGS_UNUSED(nbArgs);
    IGS_UNUSED(token);
    IGS_UNUSED(myData);
    igs_replayStart();
}

void igs_replayPauseCB(const char *senderAgentName, const char *senderAgentUUID,
                       const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
                       const char *token, void* myData){
    IGS_UNUSED(senderAgentName);
    IGS_UNUSED(senderAgentUUID);
    IGS_UNUSED(callName);
    IGS_UNUSED(nbArgs);
    IGS_UNUSED(token);
    IGS_UNUSED(myData);
    bool pause = firstArgument->b;
    igs_replayPause(pause);
}

void igs_replayTerminateCB(const char *senderAgentName, const char *senderAgentUUID,
                      const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
                      const char *token, void* myData){
    IGS_UNUSED(senderAgentName);
    IGS_UNUSED(senderAgentUUID);
    IGS_UNUSED(callName);
    IGS_UNUSED(firstArgument);
    IGS_UNUSED(nbArgs);
    IGS_UNUSED(token);
    IGS_UNUSED(myData);
    igs_replayTerminate();
}


//////////////////////////////////////////////////////////////////////////////////
// REPLAY THREAD CONTROL AND LOOP

//messages from other threads to replay thread
int pipeReadFromOtherThreads(zloop_t *loop, zsock_t *socket, void *arg){
    IGS_UNUSED(loop);
    IGS_UNUSED(arg);
    char *msg = zstr_recv(socket);
    if (streq(msg, "START_REPLAY") && !replay_canStart && !replay_shallStop){
        replay_canStart = true;
        zloop_timer(replay_loop, 0, 1, replayRunThroughLogFile, NULL);
    } else if(streq(msg, "STOP_REPLAY")){
        free(msg);
        return -1;
    }
    free(msg);
    return 0;
}


//loop for replay trhead
void replayRunLoop(zsock_t *pipe, void *args){
    IGS_UNUSED(args);
    replay_loop = zloop_new();
    if (replay_canStart){
        zloop_timer(replay_loop, 1500, 1, replayRunThroughLogFile, NULL); //1500 ms gives time for network init
    }
    zloop_reader(replay_loop, pipe, pipeReadFromOtherThreads, NULL);
    zloop_reader_set_tolerant(replay_loop, pipe);
    zsock_signal (pipe, 0);
    
    zloop_start(replay_loop);
    
    replay_end = zclock_mono();
    if (replay_speed == 0){
        igs_info("full throttle replay achieved in %ld milliseconds (%ld lines parsed)",
                 replay_end - replay_start, replay_nbLines);
        printf("full throttle replay achieved in %ld milliseconds (%ld lines parsed)\n",
               replay_end - replay_start, replay_nbLines);
    }
    replay_start = 0;
    zloop_destroy(&replay_loop);
}

//////////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS

void igs_replayInit(const char *logFilePath, size_t speed, const char *startTime,
                    bool waitForStart, uint replayMode, const char *agent){
    assert(logFilePath || waitForStart);
    assert(speed >= 0);
    assert(!startTime || strlen(startTime) < 9);
    igs_replayTerminate();
    char logFile[IGS_MAX_PATH_LENGTH] = "";
    if (logFilePath){
        admin_makeFilePath(logFilePath, logFile, IGS_MAX_PATH_LENGTH);
        if (!zsys_file_exists(logFile)){
            igs_error("file %s does not exist", logFilePath);
        }
        if (replay_file){
            igs_error("replay already active with file %s", zfile_filename(replay_file, NULL));
            return;
        }
        replay_file = zfile_new(NULL, logFile);
        if (!replay_file || zfile_input(replay_file)){
            igs_error("could not read %s", zfile_filename(replay_file, NULL));
            zfile_destroy(&replay_file);
            return;
        }
    }
    replay_speed = speed;
    
    if (startTime)
        strncpy(replay_startTime, startTime, 8);
    else
        strcpy(replay_startTime, "");
    
    if (agent)
        strncpy(replay_agent, agent, IGS_MAX_AGENT_NAME_LENGTH);
    else
        strcpy(replay_agent, "");
    
    if (!replayMode)
        replay_mode = IGS_REPLAY_INPUT + IGS_REPLAY_OUTPUT + IGS_REPLAY_PARAMETER + IGS_REPLAY_RECEIVED_CALL + IGS_REPLAY_SENT_CALL;
    else
        replay_mode = replayMode;
    
    replay_canStart = !waitForStart;
    replay_start = replay_end = 0;
    replay_isPaused = false;
    replay_nbLines = 0;
    replay_shallStop = false;
    replay_isBeyondStartTime = true;
    
    if (waitForStart && !igs_checkCallExistence("igs_replayInit")){
        igs_initCall("igs_replayInit", igs_replayInitCB, NULL);
        igs_addArgumentToCall("igs_replayInit", "logFilePath", IGS_STRING_T);
        igs_addArgumentToCall("igs_replayInit", "speed", IGS_DOUBLE_T);
        igs_addArgumentToCall("igs_replayInit", "startTime", IGS_STRING_T);
        igs_addArgumentToCall("igs_replayInit", "waitForStart", IGS_BOOL_T);
        igs_addArgumentToCall("igs_replayInit", "replayMode", IGS_INTEGER_T);
        igs_addArgumentToCall("igs_replayInit", "agent", IGS_STRING_T);
        
        igs_initCall("igs_replayStart", igs_replayStartCB, NULL);
        
        igs_initCall("igs_replayPause", igs_replayPauseCB, NULL);
        igs_addArgumentToCall("igs_replayPause", "pause", IGS_BOOL_T);
        
        igs_initCall("igs_replayTerminate", igs_replayTerminateCB, NULL);
    }
    replay_actor = zactor_new(replayRunLoop, NULL);
}

void igs_replayStart(void){
    if (replay_actor)
        zstr_send(zactor_sock(replay_actor), "START_REPLAY");
    else
        igs_error("init replay before starting it");
}

void igs_replayPause(bool pause){
    replay_isPaused = pause;
}

void igs_replayTerminate(void){
    if (replay_actor){
        replay_shallStop = true;
        zstr_send(zactor_sock(replay_actor), "STOP_REPLAY");
        zactor_destroy(&replay_actor);
    }
    if (replay_file)
        zfile_destroy(&replay_file);
    
    if (igs_checkCallExistence("igs_replayInit")){
        igs_removeCall("igs_replayInit");
        igs_removeCall("igs_replayStart");
        igs_removeCall("igs_replayPause");
        igs_removeCall("igs_replayTerminate");
    }
}
