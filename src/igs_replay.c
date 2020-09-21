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
#include "unixfunctions.h"
#endif
#include "ingescape_private.h"
#include <czmq.h>

zactor_t *replay_actor = NULL;
zloop_t *replay_loop = NULL;
zfile_t *replay_file = NULL;
bool replay_canStart = false;
int replay_timer = 0;
size_t replay_speed = 0;
uint replay_mode = 0;
char replay_startTime[9] = "";
bool replay_isPaused = false;
bool replay_shallStop = false;

int replayRunThroughLogFile(zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(timer_id);
    IGS_UNUSED(arg);
    
    time_t start = zclock_mono();
    time_t previous = 0;
    long nbLines = 0;
    const char *line = zfile_readln(replay_file);
    struct tm currentTime = {0};
    int microsec = 0;
    time_t unixTime = 0;
    time_t unixMsecTime = 0;
    
    //IOP scan
    char agent[IGS_MAX_AGENT_NAME_LENGTH] = "";
    char iopType[16] = "";
    char iopName[IGS_MAX_IOP_NAME_LENGTH] = "";
    char iopData[8192] = "";

    bool crossedStartTime = true;
    struct tm requestedStartTime = {0};
    time_t requestedStartTimeUnix = 0;
    if (strlen(replay_startTime) > 0){
        int nb = sscanf(replay_startTime, "%d:%d:%d", &requestedStartTime.tm_hour, &requestedStartTime.tm_min, &requestedStartTime.tm_sec);
        if (nb == 3){
            //we have a valid start time : we activate flag
            crossedStartTime = false;
        }
    }

    while (line != NULL){
        if (replay_shallStop)
            break;
        if (replay_isPaused){
            zclock_sleep(250);
            continue;
        }
        nbLines++;
        //Try to find an IOP write
        int res = sscanf((char *)line, "%1023s;%d/%d/%d;%d:%d:%d.%d;DEBUG;model_writeIOP;set %15s %1023s to %8191s",
                         agent,
                         &currentTime.tm_mday, &currentTime.tm_mon, &currentTime.tm_year,
                         &currentTime.tm_hour, &currentTime.tm_min, &currentTime.tm_sec, &microsec,
                         iopType, iopName, iopData);
        if (res == 10){
            if (!crossedStartTime && requestedStartTime.tm_year == 0){
                //we need to init day, month and year for our requestedStartTime
                //based on replay file.
                requestedStartTime.tm_year = currentTime.tm_year - 1900;
                requestedStartTime.tm_mon = currentTime.tm_mon - 1;
                requestedStartTime.tm_mday = currentTime.tm_mday;
                requestedStartTime.tm_isdst = 0;
                requestedStartTimeUnix = mktime(&requestedStartTime);
            }
            if (replay_speed > 0){
                currentTime.tm_year -= 1900;
                currentTime.tm_mon -= 1;
                currentTime.tm_isdst = 0;
                unixTime = mktime(&currentTime);
                unixMsecTime = unixTime * 1000 + (int)(microsec / 1000);
            }
            if (crossedStartTime && replay_speed > 0 && previous != 0){
                //int delay = (int)((unixMsecTime - previous) / replaySpeed);
                //igs_info("sleeping %d milliseconds", delay);
                zclock_sleep((int)((unixMsecTime - previous) / replay_speed));
            } else if (!crossedStartTime && unixTime >= requestedStartTimeUnix){
                //we have reached requested start time
                igs_info("reached start time : %s", replay_startTime);
                crossedStartTime = true;
            }
            
            //TODO: achieve replay depending on log entry and mode
            
            if (replay_speed > 0)
                previous = unixMsecTime;
            
        } else {
            //TODO: handle calls
        }
        line = zfile_readln(replay_file);
    }
    
    time_t end = zclock_mono();
    if (replay_speed == 0)
        igs_info("full throttle replay achieved in %ld milliseconds (%ld lines parsed)",
                 end - start, nbLines);
    zfile_destroy(&replay_file);
    replay_shallStop = false;
    replay_isPaused = false;
    replay_timer = 0;
    return -1;
}

void igs_startReplayCB(const char *senderAgentName, const char *senderAgentUUID,
                    const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
                    const char *token, void* myData)
{
    IGS_UNUSED(senderAgentName);
    IGS_UNUSED(senderAgentUUID);
    IGS_UNUSED(callName);
    IGS_UNUSED(nbArgs);
    IGS_UNUSED(token);
    IGS_UNUSED(myData);
    char *replayFileBis = firstArgument->c;
    char *startTimeBis = firstArgument->next->c;
    double replaySpeedBis = firstArgument->next->next->d;

    char logFile[IGS_MAX_PATH_LENGTH] = "";
    admin_makeFilePath(replayFileBis, logFile, IGS_MAX_PATH_LENGTH);
    if (!zsys_file_exists(logFile)){
        igs_error("file %s does not exist", replayFileBis);
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
    if (startTimeBis && strlen(startTimeBis) > 0)
        strncpy(replay_startTime, startTimeBis, 8);
    
    if (replaySpeedBis >= 0)
        replay_speed = replaySpeedBis;
    else
        igs_error("replay speed '%f' is invalid : set to zero", replaySpeedBis);
    
    //send message to replay thread from ingescape thread
    zstr_sendf(zsock_resolve(replay_actor), "START_REPLAY");
}

void igs_pauseReplayCB(const char *senderAgentName, const char *senderAgentUUID,
                       const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
                       const char *token, void* myData)
{
    IGS_UNUSED(senderAgentName);
    IGS_UNUSED(senderAgentUUID);
    IGS_UNUSED(callName);
    IGS_UNUSED(token);
    IGS_UNUSED(myData);
    if(nbArgs == 1 && firstArgument){
        igs_info("pause replay : %d", firstArgument->b);
        replay_isPaused = firstArgument->b;
    }
}

void igs_stopReplayCB(const char *senderAgentName, const char *senderAgentUUID,
                      const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
                      const char *token, void* myData)
{
    IGS_UNUSED(senderAgentName);
    IGS_UNUSED(senderAgentUUID);
    IGS_UNUSED(callName);
    IGS_UNUSED(nbArgs);
    IGS_UNUSED(firstArgument);
    IGS_UNUSED(token);
    IGS_UNUSED(myData);
    igs_info("stop replay");
    if (replay_timer) //replay is ongoing : loop is not reactive
        replay_shallStop = true;
    else //replay is not started : loop is reactive
        zstr_send(zactor_sock(replay_actor), "STOP_REPLAY");
}

int pipeReadFromParent(zloop_t *loop, zsock_t *socket, void *arg){
    IGS_UNUSED(loop);
    char *msg = zstr_recv(socket);
    if (streq(msg, "START_REPLAY") && !replay_canStart){
        replay_canStart = true;
        replay_timer = zloop_timer(replay_loop, 0, 1, replayRunThroughLogFile, NULL);
    } else if(streq(msg, "STOP_REPLAY")){
        free(msg);
        return -1;
    }
    free(msg);
    return 0;
}

int pipeReadToParent(zloop_t *loop, zsock_t *socket, void *arg){
    IGS_UNUSED(loop);
    char *msg = zstr_recv(socket);
    if (streq(msg, "LOOP_STOPPED") && replay_actor){
        zactor_destroy(&replay_actor);
    }
    free(msg);
    return -1;
}

void replayRunLoop(zsock_t *pipe, void *args){
    replay_loop = zloop_new();
    if (replay_canStart){
        replay_timer = zloop_timer(replay_loop, 1500, 1, replayRunThroughLogFile, NULL); //1500 ms gives time for network init
    }
    zloop_reader(replay_loop, pipe, pipeReadFromParent, NULL);
    zsock_signal (pipe, 0);
    zloop_start(replay_loop);
    zloop_destroy(&replay_loop);
    zstr_send(pipe, "LOOP_STOPPED");
}

void replayFromLogFile(const char *logFilePath, size_t speed, const char *startTime, bool waitForSignal, uint replayMode){
    assert(logFilePath || waitForSignal);
    assert(speed >= 0);
    assert(!startTime || strlen(startTime) < 9);
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
    strncpy(replay_startTime, startTime, 8);
    replay_canStart = !waitForSignal;
    replay_mode = replayMode;
    
    if (waitForSignal && !igs_checkCallExistence("igs_startReplay")){
        igs_initCall("igs_startReplay", igs_startReplayCB, NULL);
        igs_addArgumentToCall("igs_startReplay", "replayFile", IGS_STRING_T);
        igs_addArgumentToCall("igs_startReplay", "startTime", IGS_STRING_T);
        igs_addArgumentToCall("igs_startReplay", "replaySpeed", IGS_DOUBLE_T);
        
        igs_initCall("igs_pauseReplay", igs_pauseReplayCB, NULL);
        igs_addArgumentToCall("igs_pauseReplay", "pause", IGS_BOOL_T);
        
        igs_initCall("igs_stopReplay", igs_stopReplayCB, NULL);
    }
    
    replay_actor = zactor_new(replayRunLoop, NULL);
    zloop_t *mainLoop = zloop_new();
    zloop_reader(mainLoop, zactor_sock(replay_actor), pipeReadToParent, NULL);
    zloop_start(mainLoop);
    zloop_destroy(&mainLoop);
}

void igs_enableDataLogging(bool enable){
    core_initContext();
    coreContext->enableDataLogging = enable;
}

void igs_replayStop(void){
}
