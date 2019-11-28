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

#define INGESCAPE_MAJOR 1
#define INGESCAPE_MINOR 0
#define INGESCAPE_MICRO 0
#define INGESCAPE_VERSION ((INGESCAPE_MAJOR * 10000) + (INGESCAPE_MINOR * 100) + INGESCAPE_MICRO)

#define INGESCAPE_PROTOCOL 1
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

////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////

void admin_computeTime(char *dest){
#if (defined WIN32 || defined _WIN32)
    struct timeval tick;
    gettimeofday(&tick, NULL);
    time_t t = tick.tv_sec;
    struct tm *tm = localtime(&t);
    snprintf(dest,128,"%02d/%02d/%d;%02d:%02d:%02d.%06ld",
             tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec, tick.tv_usec);
#else
    struct timeval tick;
    gettimeofday(&tick, NULL);
    struct tm *tm = localtime(&tick.tv_sec);
    snprintf(dest,128,"%02d/%02d/%d;%02d:%02d:%02d.%06d",
             tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec, (int)tick.tv_usec);
#endif
    }

void admin_makeFilePath(igsAgent_t *agent, const char *from, char *to, size_t size_of_to){
    if (from[0] == '~') {
        from++;
#ifdef _WIN32
        char *home = getenv("USERPROFILE");
#else
        char *home = getenv("HOME");
#endif
        if (home == NULL) {
            igsAgent_error(agent, "could not find path for home directory");
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
    igs_debug("IngeScape version : %d.%d.%d\n", INGESCAPE_MAJOR, INGESCAPE_MINOR, INGESCAPE_MICRO);
    return INGESCAPE_VERSION;
}

int igs_protocol(void){
    igs_debug("IngeScape protocol version : %d\n", INGESCAPE_PROTOCOL);
    return INGESCAPE_PROTOCOL;
}

void admin_log(igsAgent_t *agent, igs_logLevel_t level, const char *function, const char *fmt, ...){
    admin_lock();
    initInternalAgentIfNeeded();
    
    va_list list;
    va_start(list, fmt);
    vsnprintf(agent->logContent, 2047, fmt, list);
    va_end(list);
    
    //remove final \n if needed
    //TODO: scan the whole string to remove unallowed characters
    if (agent->logContent[strlen(agent->logContent) - 1] == '\n'){
        agent->logContent[strlen(agent->logContent) - 1] = '\0';
    }

    if (agent->logInFile){
        //create default path if current is empty
        if (strlen(agent->logFilePath) == 0){
            char buff[4097] = "";
            snprintf(agent->logFilePath, 4095, "~/Documents/IngeScape/logs/");
            strncpy(buff, agent->logFilePath, 4096);
            admin_makeFilePath(agent, buff, agent->logFilePath, 4096);
            if (!zsys_file_exists(agent->logFilePath)){
                printf("creating log path %s\n", agent->logFilePath);
                if(zsys_dir_create(agent->logFilePath) != 0){
                    printf("error while creating log path %s\n", agent->logFilePath);
                }
            }
            char *name = igsAgent_getAgentName(agent);
            strncat(agent->logFilePath, name, 4095);
            strncat(agent->logFilePath, ".log", 4095);
            printf("creating default log file %s\n", agent->logFilePath);
            free(name);
            if (agent->loopElements != NULL && agent->loopElements->node != NULL){
                bus_zyreLock();
                zyre_shouts(agent->loopElements->node, CHANNEL, "LOG_FILE_PATH=%s", agent->logFilePath);
                bus_zyreUnlock();
            }
        }
        if (agent->logFile == NULL){
            agent->logFile = fopen (agent->logFilePath,"a");
            if (agent->logFile == NULL){
                printf("error while trying to create/open log file: %s\n", agent->logFilePath);
            }
        }
        if (agent->logFile != NULL){
            admin_computeTime(agent->logTime);
            if (fprintf(agent->logFile,"%s;%s;%s;%s;%s\n", agent->agentName, agent->logTime, log_levels[level], function, agent->logContent) >= 0){
                if (++agent->logNbOfEntries > NUMBER_OF_LOGS_FOR_FFLUSH){
                    agent->logNbOfEntries = 0;
                    fflush(agent->logFile);
                }
            }else{
                printf("error while writing logs in %s\n", agent->logFilePath);
            }
        }
    }
    if ((agent->logInConsole && level >= agent->logLevel) || level >= IGS_LOG_ERROR){
        if (level >= IGS_LOG_WARN){
            if (agent->useColorInConsole){
                fprintf(stderr,"%s;%s%s\x1b[0m;%s;%s\n", agent->agentName, log_colors[level], log_levels[level], function, agent->logContent);
            }else{
                fprintf(stderr,"%s;%s;%s;%s\n", agent->agentName, log_levels[level], function, agent->logContent);
            }
        }else{
            if (agent->useColorInConsole){
                fprintf(stdout,"%s;%s%s\x1b[0m;%s;%s\n", agent->agentName, log_colors[level], log_levels[level], function, agent->logContent);
            }else{
                fprintf(stdout,"%s;%s;%s;%s\n", agent->agentName, log_levels[level], function, agent->logContent);
            }
        }
        
    }
    if (agent->logInStream && agent->loopElements != NULL && agent->loopElements->logger != NULL){
        zstr_sendf(agent->loopElements->logger, "%s;%s;%s;%s\n", agent->agentName, log_levels[level], function, agent->logContent);
    }
    admin_unlock();

}

void igsAgent_setLogLevel (igsAgent_t *agent, igs_logLevel_t level){
    agent->logLevel = level;
}

igs_logLevel_t igsAgent_getLogLevel (igsAgent_t *agent) {
    return agent->logLevel;
}

void igsAgent_setLogInFile (igsAgent_t *agent, bool allow){
    if (allow != agent->logInFile){
        initInternalAgentIfNeeded();
        agent->logInFile = allow;
        if (agent->loopElements != NULL && agent->loopElements->node != NULL){
            bus_zyreLock();
            if (allow){
                zyre_shouts(agent->loopElements->node, CHANNEL, "LOG_IN_FILE=1");
            }else{
                zyre_shouts(agent->loopElements->node, CHANNEL, "LOG_IN_FILE=0");
            }
            bus_zyreUnlock();
        }
    }
}

bool igsAgent_getLogInFile (igsAgent_t *agent) {
    return agent->logInFile;
}

void igsAgent_setVerbose (igsAgent_t *agent, bool allow){
    agent->logInConsole = allow;
}

bool igsAgent_isVerbose (igsAgent_t *agent) {
    return agent->logInConsole;
}

void igsAgent_setUseColorVerbose (igsAgent_t *agent, bool allow){
    agent->useColorInConsole = allow;
}

bool igsAgent_getUseColorVerbose (igsAgent_t *agent) {
    return agent->useColorInConsole;
}

void igsAgent_setLogStream(igsAgent_t *agent, bool stream){
    if (stream != agent->logInStream){
        initInternalAgentIfNeeded();
        if (agent->loopElements != NULL){
            if (stream){
                igsAgent_warn(agent, "agent is already started, log stream cannot be created anymore");
            }else{
                igsAgent_warn(agent, "agent is already started, log stream cannot be disabled anymore");
            }
            return;
        }
        agent->logInStream = stream;
        if (agent->loopElements != NULL && agent->loopElements->node != NULL){
            bus_zyreLock();
            if (stream){
                zyre_shouts(agent->loopElements->node, CHANNEL, "LOG_IN_STREAM=1");
            }else{
                zyre_shouts(agent->loopElements->node, CHANNEL, "LOG_IN_STREAM=0");
            }
            bus_zyreUnlock();
        }
    }
}

bool igsAgent_getLogStream (igsAgent_t *agent) {
    return agent->logInStream;
}

void igsAgent_setLogPath(igsAgent_t *agent, const char *path){
    if ((path != NULL) && (strlen(path) > 0)){
        initInternalAgentIfNeeded();
        char tmpPath[4096] = "";
        admin_lock();
        admin_makeFilePath(agent, path, tmpPath, 4095);
        if (access(tmpPath, W_OK) == -1){
            printf("'%s' is not writable and will not be used\n", tmpPath);
            admin_unlock();
            return;
        }
        if (strcmp(agent->logFilePath, tmpPath) == 0){
            printf("'%s' is already the log path\n", agent->logFilePath);
            admin_unlock();
            return;
        }else{
            strncpy(agent->logFilePath, tmpPath, 4096);
        }
        if (agent->logFile != NULL){
            fflush(agent->logFile);
            fclose(agent->logFile);
            agent->logFile = NULL;
        }
        agent->logFile = fopen(agent->logFilePath,"a");
        if (agent->logFile == NULL){
            printf("could NOT create log file at path %s\n", agent->logFilePath);
        }else{
            printf("switching to new log file: %s\n", agent->logFilePath);
        }
        if (agent->logFile != NULL && agent->loopElements != NULL && agent->loopElements->node != NULL){
            bus_zyreLock();
            zyre_shouts(agent->loopElements->node, CHANNEL, "LOG_FILE_PATH=%s", agent->logFilePath);
            bus_zyreUnlock();
        }
        admin_unlock();
    }else{
        printf("passed path cannot be NULL or zero length\n");
    }
}

char* igsAgent_getLogPath (igsAgent_t *agent) {
    return strdup(agent->logFilePath);
}
