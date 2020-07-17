//
//  admin.c
//  ingescape
//
//  Created by Stephane Vales on 01/12/2017.
//  Copyright © 2017 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <czmq.h>

#if defined(__unix__) || defined(__linux__) || \
(defined(__APPLE__) && defined(__MACH__))
#include <pthread.h>
#endif
#if (defined WIN32 || defined _WIN32)
#include "unixfunctions.h"
#endif

#include "ingescape.h"
#include "ingescape_private.h"

#define INGESCAPE_MAJOR 2
#define INGESCAPE_MINOR 0
#define INGESCAPE_MICRO 0 //replaced by gitlab-ci build number
#define INGESCAPE_VERSION ((INGESCAPE_MAJOR * 10000) + (INGESCAPE_MINOR * 100) + INGESCAPE_MICRO)

#define INGESCAPE_PROTOCOL 2
#define NUMBER_OF_LOGS_FOR_FFLUSH 0

#if defined(__unix__) || defined(__linux__) || \
(defined(__APPLE__) && defined(__MACH__))
pthread_mutex_t *lock = NULL;
#else
#define W_OK 02
pthread_mutex_t lock = NULL;
#endif

static const char *log_levels[] = {
    "TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "FATAL", "LICENSE"
};
static const char *log_colors[] = {
    "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m", "\x1b[35m"
};

#define LOG_TIME_LENGTH 128
char logContent[IGS_MAX_LOG_LENGTH] = "";
char logContentForFile[2*IGS_MAX_LOG_LENGTH] = "";
char logTime[LOG_TIME_LENGTH] = "";

////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////

void admin_computeTime(char *dest){
#if (defined WIN32 || defined _WIN32)
    struct timeval tick;
    gettimeofday(&tick, NULL);
    time_t t = tick.tv_sec;
    struct tm *tm = localtime(&t);
    snprintf(dest,LOG_TIME_LENGTH,"%02d/%02d/%d;%02d:%02d:%02d.%06ld",
             tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec, tick.tv_usec);
#else
    struct timeval tick;
    gettimeofday(&tick, NULL);
    struct tm *tm = localtime(&tick.tv_sec);
    snprintf(dest,LOG_TIME_LENGTH,"%02d/%02d/%d;%02d:%02d:%02d.%06d",
             tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec, (int)tick.tv_usec);
#endif
    }

void admin_makeFilePath(const char *from, char *to, size_t size_of_to){
    if (from[0] == '~') {
        from++;
#ifdef _WIN32
        char *home = getenv("USERPROFILE");
#else
        char *home = getenv("HOME");
#endif
        if (home == NULL) {
            igs_error("could not find path for home directory");
        } else {
            strncpy(to, home, size_of_to);
            strncat(to, from, size_of_to);
        }
    }
    else {
        strncpy(to, from, size_of_to);
    }
}

void admin_lock(void){
#if defined(__unix__) || defined(__linux__) || \
(defined(__APPLE__) && defined(__MACH__))
    if (lock == NULL){
        lock = calloc(1, sizeof(pthread_mutex_t));
        if (pthread_mutex_init(lock, NULL) != 0){
            printf("error: mutex init failed\n");
            return;
        }
    }
#elif (defined WIN32 || defined _WIN32)
    if (lock == NULL){
        if (pthread_mutex_init(&lock) != 0){
            printf("error: mutex init failed\n");
            return;
        }
    }
#endif
    pthread_mutex_lock(lock);
}

void admin_unlock(void) {
    if (lock != NULL){
        pthread_mutex_unlock(lock);
    }
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////

int igs_version(void){
    //igs_debug("IngeScape version : %d.%d.%d\n", INGESCAPE_MAJOR, INGESCAPE_MINOR, INGESCAPE_MICRO);
    return INGESCAPE_VERSION;
}

int igs_protocol(void){
    //igs_debug("IngeScape protocol version : %d\n", INGESCAPE_PROTOCOL);
    return INGESCAPE_PROTOCOL;
}

void admin_log(igs_agent_t *agent, igs_logLevel_t level, const char *function, const char *fmt, ...){
    assert(agent);
    assert(function);
    assert(fmt);
    admin_lock();
    
    va_list list;
    va_start(list, fmt);
    vsnprintf(logContent, IGS_MAX_LOG_LENGTH, fmt, list);
    va_end(list);
    
    //scan the whole string to transform unallowed characters
    int j = 0;
    for (int i = 0; i < strlen(logContent); i++){
        if (logContent[i] == '\n'){
            logContentForFile[j] = '\\';
            logContentForFile[j+1] = 'n';
            j++;
        }else{
            logContentForFile[j] = logContent[i];
        }
        j++;
    }
    logContentForFile[j] = '\0';

    if (coreContext->logInFile){
        //create default path if current is empty
        if (coreContext->logFile == NULL && strlen(coreContext->logFilePath) == 0){
            char buff[IGS_MAX_PATH_LENGTH] = "";
            snprintf(coreContext->logFilePath, IGS_MAX_PATH_LENGTH, "~/Documents/IngeScape/logs/");
            strncpy(buff, coreContext->logFilePath, IGS_MAX_PATH_LENGTH);
            admin_makeFilePath(buff, coreContext->logFilePath, IGS_MAX_PATH_LENGTH);
            if (!zsys_file_exists(coreContext->logFilePath)){
                printf("creating log path %s\n", coreContext->logFilePath);
                if(zsys_dir_create(coreContext->logFilePath) != 0){
                    printf("error while creating log path %s\n", coreContext->logFilePath);
                }
            }
            strncat(coreContext->logFilePath, agent->name, IGS_MAX_PATH_LENGTH);
            strncat(coreContext->logFilePath, ".log", IGS_MAX_PATH_LENGTH);
            printf("using log file %s\n", coreContext->logFilePath);
            if (coreContext != NULL && coreContext->node != NULL){
                bus_zyreLock();
                zmsg_t *msg = zmsg_new();
                zmsg_addstr(msg, "LOG_FILE_PATH");
                zmsg_addstr(msg, coreContext->logFilePath);
                zmsg_addstr(msg, agent->uuid);
                zyre_shout(coreContext->node, IGS_PRIVATE_CHANNEL, &msg);
                bus_zyreUnlock();
            }
        }
        if (coreContext->logFile == NULL || !zsys_file_exists(coreContext->logFilePath)){
            coreContext->logFile = fopen (coreContext->logFilePath,"a");
            if (coreContext->logFile == NULL){
                printf("error while trying to create/open log file: %s\n", coreContext->logFilePath);
            }
        }
        if (coreContext->logFile != NULL){
            admin_computeTime(logTime);
            if (fprintf(coreContext->logFile,"%s;%s;%s;%s;%s\n", agent->name, logTime, log_levels[level], function, logContentForFile) > 0){
                if (++coreContext->logNbOfEntries > NUMBER_OF_LOGS_FOR_FFLUSH){
                    coreContext->logNbOfEntries = 0;
                    fflush(coreContext->logFile);
                }
            }else{
                printf("error while writing logs in %s\n", coreContext->logFilePath);
            }
        }
    }
    if ((coreContext->logInConsole && level >= coreContext->logLevel) || level >= IGS_LOG_ERROR){
        if (level >= IGS_LOG_WARN){
            if (coreContext->useColorInConsole){
                fprintf(stderr,"%s;%s%s\x1b[0m;%s;%s\n", agent->name, log_colors[level], log_levels[level], function, logContent);
            }else{
                fprintf(stderr,"%s;%s;%s;%s\n", agent->name, log_levels[level], function, logContent);
            }
        }else{
            if (coreContext->useColorInConsole){
                fprintf(stdout,"%s;%s%s\x1b[0m;%s;%s\n", agent->name, log_colors[level], log_levels[level], function, logContent);
            }else{
                fprintf(stdout,"%s;%s;%s;%s\n", agent->name, log_levels[level], function, logContent);
            }
        }
        
    }
    if (coreContext->logInStream && coreContext != NULL && coreContext->logger != NULL){
        zstr_sendf(coreContext->logger, "%s;%s;%s;%s\n", agent->name, log_levels[level], function, logContentForFile);
    }
    admin_unlock();

}

void igs_setLogLevel (igs_logLevel_t level){
    core_initContext();
    coreContext->logLevel = level;
}

igs_logLevel_t igs_getLogLevel () {
    core_initContext();
    return coreContext->logLevel;
}

void igs_setLogInFile (bool allow){
    core_initContext();
    if (allow != coreContext->logInFile){
        coreContext->logInFile = allow;
        if (coreContext->networkActor != NULL && coreContext->node != NULL){
            bus_zyreLock();
            igs_agent_t *agent, *tmp;
            HASH_ITER(hh, coreContext->agents, agent, tmp){
                zmsg_t *msg = zmsg_new();
                zmsg_addstr(msg, "LOG_IN_FILE");
                if (allow){
                    zmsg_addstr(msg, "1");
                }else{
                    zmsg_addstr(msg, "0");
                }
                zmsg_addstr(msg, agent->uuid);
                zyre_shout(coreContext->node, IGS_PRIVATE_CHANNEL, &msg);
            }
            bus_zyreUnlock();
        }
    }
}

bool igs_getLogInFile () {
    core_initContext();
    return coreContext->logInFile;
}

void igs_setVerbose (bool allow){
    core_initContext();
    coreContext->logInConsole = allow;
}

bool igs_isVerbose () {
    core_initContext();
    return coreContext->logInConsole;
}

void igs_setUseColorVerbose (bool allow){
    core_initContext();
    coreContext->useColorInConsole = allow;
}

bool igs_getUseColorVerbose() {
    core_initContext();
    return coreContext->useColorInConsole;
}

void igs_setLogStream(bool stream){
    core_initContext();
    if (stream != coreContext->logInStream){
        if (coreContext->networkActor != NULL){
            if (stream){
                igs_error("agent is already started, log stream cannot be created anymore");
            }else{
                igs_error("agent is already started, log stream cannot be disabled anymore");
            }
            return;
        }
        coreContext->logInStream = stream;
        if (coreContext->networkActor != NULL && coreContext->node != NULL){
            bus_zyreLock();
            igs_agent_t *agent, *tmp;
            HASH_ITER(hh, coreContext->agents, agent, tmp){
                zmsg_t *msg = zmsg_new();
                zmsg_addstr(msg, "LOG_IN_STREAM");
                if (stream){
                    zmsg_addstr(msg, "1");
                }else{
                    zmsg_addstr(msg, "0");
                }
                zmsg_addstr(msg, agent->uuid);
                zyre_shout(coreContext->node, IGS_PRIVATE_CHANNEL, &msg);
            }
            bus_zyreUnlock();
        }
    }
}

bool igs_getLogStream () {
    core_initContext();
    return coreContext->logInStream;
}

void igs_setLogPath(const char *path){
    core_initContext();
    if ((path != NULL) && (strlen(path) > 0)){
        char tmpPath[4096] = "";
        admin_lock();
        admin_makeFilePath(path, tmpPath, 4095);
        if (!zsys_file_exists(tmpPath)){
            zfile_t *newF = zfile_new(NULL, tmpPath);
            if (newF != NULL){
                zfile_output(newF);
            }
            zfile_destroy(&newF);
        }
        if (access(tmpPath, W_OK) == -1){
            printf("'%s' is not writable and will not be used\n", tmpPath);
            admin_unlock();
            return;
        }
        if (strcmp(coreContext->logFilePath, tmpPath) == 0){
            printf("'%s' is already the log path\n", coreContext->logFilePath);
            admin_unlock();
            return;
        }else{
            strncpy(coreContext->logFilePath, tmpPath, 4096);
        }
        if (coreContext->logFile != NULL){
            fflush(coreContext->logFile);
            fclose(coreContext->logFile);
            coreContext->logFile = NULL;
        }
        coreContext->logFile = fopen(coreContext->logFilePath,"a");
        if (coreContext->logFile == NULL){
            printf("could NOT create log file at path %s\n", coreContext->logFilePath);
        }else{
            printf("switching to new log file: %s\n", coreContext->logFilePath);
        }
        if (coreContext->logFile != NULL && coreContext != NULL && coreContext->node != NULL){
            bus_zyreLock();
            
            igs_agent_t *agent, *tmp;
            HASH_ITER(hh, coreContext->agents, agent, tmp){
                zmsg_t *msg = zmsg_new();
                zmsg_addstr(msg, "LOG_FILE_PATH");
                zmsg_addstr(msg, coreContext->logFilePath);
                zmsg_addstr(msg, agent->uuid);
                zyre_shout(coreContext->node, IGS_PRIVATE_CHANNEL, &msg);
            }
            bus_zyreUnlock();
        }
        admin_unlock();
    }else{
        printf("passed path cannot be NULL or zero length\n");
    }
}

char* igs_getLogPath () {
    core_initContext();
    return strdup(coreContext->logFilePath);
}
