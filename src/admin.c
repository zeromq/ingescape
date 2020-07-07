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
#define INGESCAPE_MICRO 0 //replaced by gitlab-ci build number
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
    igs_debug("IngeScape protocol version : %d\n", INGESCAPE_PROTOCOL);
    return INGESCAPE_PROTOCOL;
}

void admin_log(igs_agent_t *agent, igs_logLevel_t level, const char *function, const char *fmt, ...){
    admin_lock();
    core_initCoreAgent();
    
    va_list list;
    va_start(list, fmt);
    vsnprintf(coreContext->logContent, 2047, fmt, list);
    va_end(list);
    
    //remove final \n if needed
    //TODO: scan the whole string to remove unallowed characters
    if (coreContext->logContent[strlen(coreContext->logContent) - 1] == '\n'){
        coreContext->logContent[strlen(coreContext->logContent) - 1] = '\0';
    }

    if (coreContext->logInFile){
        //create default path if current is empty
        if (strlen(coreContext->logFilePath) == 0){
            char buff[4097] = "";
            snprintf(coreContext->logFilePath, 4095, "~/Documents/IngeScape/logs/");
            strncpy(buff, coreContext->logFilePath, 4096);
            admin_makeFilePath(buff, coreContext->logFilePath, 4096);
            if (!zsys_file_exists(coreContext->logFilePath)){
                printf("creating log path %s\n", coreContext->logFilePath);
                if(zsys_dir_create(coreContext->logFilePath) != 0){
                    printf("error while creating log path %s\n", coreContext->logFilePath);
                }
            }
            char *name = igsAgent_getAgentName(agent);
            strncat(coreContext->logFilePath, name, 4095);
            strncat(coreContext->logFilePath, ".log", 4095);
            printf("using log file %s\n", coreContext->logFilePath);
            free(name);
            if (coreContext != NULL && coreContext->node != NULL){
                bus_zyreLock();
                zyre_shouts(coreContext->node, IGS_PRIVATE_CHANNEL, "LOG_FILE_PATH=%s", coreContext->logFilePath);
                bus_zyreUnlock();
            }
        }
        if (coreContext->logFile == NULL || !zsys_file_exists(coreContext->logFilePath)){
            if (coreContext->logFile != NULL)
                fclose(coreContext->logFile);
            coreContext->logFile = fopen (coreContext->logFilePath,"a");
            if (coreContext->logFile == NULL){
                printf("error while trying to create/open log file: %s\n", coreContext->logFilePath);
            }
        }
        if (coreContext->logFile != NULL){
            admin_computeTime(coreContext->logTime);
            if (fprintf(coreContext->logFile,"%s;%s;%s;%s;%s\n", agent->name, coreContext->logTime, log_levels[level], function, coreContext->logContent) > 0){
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
                fprintf(stderr,"%s;%s%s\x1b[0m;%s;%s\n", agent->name, log_colors[level], log_levels[level], function, coreContext->logContent);
            }else{
                fprintf(stderr,"%s;%s;%s;%s\n", agent->name, log_levels[level], function, coreContext->logContent);
            }
        }else{
            if (coreContext->useColorInConsole){
                fprintf(stdout,"%s;%s%s\x1b[0m;%s;%s\n", agent->name, log_colors[level], log_levels[level], function, coreContext->logContent);
            }else{
                fprintf(stdout,"%s;%s;%s;%s\n", agent->name, log_levels[level], function, coreContext->logContent);
            }
        }
        
    }
    if (coreContext->logInStream && coreContext != NULL && coreContext->logger != NULL){
        zstr_sendf(coreContext->logger, "%s;%s;%s;%s\n", agent->name, log_levels[level], function, coreContext->logContent);
    }
    admin_unlock();

}

void igs_setLogLevel (igs_logLevel_t level){
    coreContext->logLevel = level;
}

igs_logLevel_t igs_getLogLevel () {
    return coreContext->logLevel;
}

void igs_setLogInFile (bool allow){
    if (allow != coreContext->logInFile){
        core_initCoreAgent();
        coreContext->logInFile = allow;
        if (coreContext != NULL && coreContext->node != NULL){
            bus_zyreLock();
            if (allow){
                zyre_shouts(coreContext->node, IGS_PRIVATE_CHANNEL, "LOG_IN_FILE=1");
            }else{
                zyre_shouts(coreContext->node, IGS_PRIVATE_CHANNEL, "LOG_IN_FILE=0");
            }
            bus_zyreUnlock();
        }
    }
}

bool igs_getLogInFile () {
    return coreContext->logInFile;
}

void igs_setVerbose (bool allow){
    coreContext->logInConsole = allow;
}

bool igs_isVerbose () {
    return coreContext->logInConsole;
}

void igs_setUseColorVerbose (bool allow){
    coreContext->useColorInConsole = allow;
}

bool igs_getUseColorVerbose() {
    return coreContext->useColorInConsole;
}

void igs_setLogStream(bool stream){
    if (stream != coreContext->logInStream){
        core_initCoreAgent();
        if (coreContext != NULL){
            if (stream){
                igs_warn("agent is already started, log stream cannot be created anymore");
            }else{
                igs_warn("agent is already started, log stream cannot be disabled anymore");
            }
            return;
        }
        coreContext->logInStream = stream;
        if (coreContext != NULL && coreContext->node != NULL){
            bus_zyreLock();
            if (stream){
                zyre_shouts(coreContext->node, IGS_PRIVATE_CHANNEL, "LOG_IN_STREAM=1");
            }else{
                zyre_shouts(coreContext->node, IGS_PRIVATE_CHANNEL, "LOG_IN_STREAM=0");
            }
            bus_zyreUnlock();
        }
    }
}

bool igs_getLogStream () {
    return coreContext->logInStream;
}

void igs_setLogPath(const char *path){
    if ((path != NULL) && (strlen(path) > 0)){
        core_initCoreAgent();
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
            zyre_shouts(coreContext->node, IGS_PRIVATE_CHANNEL, "LOG_FILE_PATH=%s", coreContext->logFilePath);
            bus_zyreUnlock();
        }
        admin_unlock();
    }else{
        printf("passed path cannot be NULL or zero length\n");
    }
}

char* igs_getLogPath () {
    return strdup(coreContext->logFilePath);
}
