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

FILE *fp = NULL;
bool admin_logInStream = false;
bool admin_logInFile = false;
bool logInConsole = false;
bool useColorInConsole = false;
igs_logLevel_t logLevel = IGS_LOG_INFO;
char admin_logFile[4096] = "";
char logContent[2048] = "";
char logTime[128] = "";
static int nb_of_entries = 0; //for fflush rotation

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
    igs_debug("IngeScape version : %d.%d.%d\n", INGESCAPE_MAJOR, INGESCAPE_MINOR, INGESCAPE_MICRO);
    return INGESCAPE_VERSION;
}

int igs_protocol(void){
    igs_debug("IngeScape protocol version : %d\n", INGESCAPE_PROTOCOL);
    return INGESCAPE_PROTOCOL;
}

void igs_log(char *name, igs_logLevel_t level, const char *function, const char *fmt, ...){
    admin_lock();
    initInternalAgentIfNeeded();
    
    va_list list;
    va_start(list, fmt);
    vsnprintf(logContent, 2047, fmt, list);
    va_end(list);
    
    //remove final \n if needed
    //TODO: scan the whole string to remove unallowed characters
    if (logContent[strlen(logContent) - 1] == '\n'){
        logContent[strlen(logContent) - 1] = '\0';
    }

    if (admin_logInFile){
        //create default path if current is empty
        if (strlen(admin_logFile) == 0){
            char buff[4097] = "";
            snprintf(admin_logFile, 4095, "~/Documents/IngeScape/logs/");
            strncpy(buff, admin_logFile, 4096);
            admin_makeFilePath(buff, admin_logFile, 4096);
            if (!zsys_file_exists(admin_logFile)){
                printf("creating log path %s\n", admin_logFile);
                if(zsys_dir_create(admin_logFile) != 0){
                    printf("error while creating log path %s\n", admin_logFile);
                }
            }
            char *name = igs_getAgentName();
            strncat(admin_logFile, name, 4095);
            strncat(admin_logFile, ".log", 4095);
            printf("creating default log file %s\n", admin_logFile);
            free(name);
            if (internalAgent->agentElements != NULL && internalAgent->agentElements->node != NULL){
                bus_zyreLock();
                zyre_shouts(internalAgent->agentElements->node, CHANNEL, "LOG_FILE_PATH=%s", admin_logFile);
                bus_zyreUnlock();
            }
        }
        if (fp == NULL){
            fp = fopen (admin_logFile,"a");
            if (fp == NULL){
                printf("error while trying to create/open log file: %s\n", admin_logFile);
            }
        }
        if (fp != NULL){
            admin_computeTime(logTime);
            if (fprintf(fp,"%s;%s;%s;%s;%s\n", name, logTime, log_levels[level], function, logContent) >= 0){
                if (++nb_of_entries > NUMBER_OF_LOGS_FOR_FFLUSH){
                    nb_of_entries = 0;
                    fflush(fp);
                }
            }else{
                printf("error while writing logs in %s\n", admin_logFile);
            }
        }
    }
    if ((logInConsole && level >= logLevel) || level >= IGS_LOG_ERROR){
        if (level >= IGS_LOG_WARN){
            if (useColorInConsole){
                fprintf(stderr,"%s;%s%s\x1b[0m;%s;%s\n", name, log_colors[level], log_levels[level], function, logContent);
            }else{
                fprintf(stderr,"%s;%s;%s;%s\n", name, log_levels[level], function, logContent);
            }
        }else{
            if (useColorInConsole){
                fprintf(stdout,"%s;%s%s\x1b[0m;%s;%s\n", name, log_colors[level], log_levels[level], function, logContent);
            }else{
                fprintf(stdout,"%s;%s;%s;%s\n", name, log_levels[level], function, logContent);
            }
        }
        
    }
    if (admin_logInStream && internalAgent->agentElements != NULL && internalAgent->agentElements->logger != NULL){
        zstr_sendf(internalAgent->agentElements->logger, "%s;%s;%s;%s\n", name, log_levels[level], function, logContent);
    }
    admin_unlock();

}

void igs_setLogLevel (igs_logLevel_t level){
    logLevel = level;
}

igs_logLevel_t igs_getLogLevel (void) {
    return logLevel;
}

void igs_setLogInFile (bool allow){
    if (allow != admin_logInFile){
        initInternalAgentIfNeeded();
        admin_logInFile = allow;
        if (internalAgent->agentElements != NULL && internalAgent->agentElements->node != NULL){
            bus_zyreLock();
            if (allow){
                zyre_shouts(internalAgent->agentElements->node, CHANNEL, "LOG_IN_FILE=1");
            }else{
                zyre_shouts(internalAgent->agentElements->node, CHANNEL, "LOG_IN_FILE=0");
            }
            bus_zyreUnlock();
        }
    }
}

bool igs_getLogInFile (void) {
    return admin_logInFile;
}

void igs_setVerbose (bool allow){
    logInConsole = allow;
}

bool igs_isVerbose (void) {
    return logInConsole;
}

void igs_setUseColorVerbose (bool allow){
    useColorInConsole = allow;
}

bool igs_getUseColorVerbose (void) {
    return useColorInConsole;
}

void igs_setLogStream(bool stream){
    if (stream != admin_logInStream){
        initInternalAgentIfNeeded();
        if (internalAgent->agentElements != NULL){
            if (stream){
                igs_warn("agent is already started, log stream cannot be created anymore");
            }else{
                igs_warn("agent is already started, log stream cannot be disabled anymore");
            }
            return;
        }
        admin_logInStream = stream;
        if (internalAgent->agentElements != NULL && internalAgent->agentElements->node != NULL){
            bus_zyreLock();
            if (stream){
                zyre_shouts(internalAgent->agentElements->node, CHANNEL, "LOG_IN_STREAM=1");
            }else{
                zyre_shouts(internalAgent->agentElements->node, CHANNEL, "LOG_IN_STREAM=0");
            }
            bus_zyreUnlock();
        }
    }
}

bool igs_getLogStream (void) {
    return admin_logInStream;
}

void igs_setLogPath(const char *path){
    if ((path != NULL) && (strlen(path) > 0)){
        initInternalAgentIfNeeded();
        char tmpPath[4096] = "";
        admin_lock();
        admin_makeFilePath(path, tmpPath, 4095);
        if (access(tmpPath, W_OK) == -1){
            printf("'%s' is not writable and will not be used\n", tmpPath);
            admin_unlock();
            return;
        }
        if (strcmp(admin_logFile, tmpPath) == 0){
            printf("'%s' is already the log path\n", admin_logFile);
            admin_unlock();
            return;
        }else{
            strncpy(admin_logFile, tmpPath, 4096);
        }
        if (fp != NULL){
            fflush(fp);
            fclose(fp);
            fp = NULL;
        }
        fp = fopen (admin_logFile,"a");
        if (fp == NULL){
            printf("could NOT create log file at path %s\n", admin_logFile);
        }else{
            printf("switching to new log file: %s\n", admin_logFile);
        }
        if (fp != NULL && internalAgent->agentElements != NULL && internalAgent->agentElements->node != NULL){
            bus_zyreLock();
            zyre_shouts(internalAgent->agentElements->node, CHANNEL, "LOG_FILE_PATH=%s", admin_logFile);
            bus_zyreUnlock();
        }
        admin_unlock();
    }else{
        printf("passed path cannot be NULL or zero length\n");
    }
}

char* igs_getLogPath (void) {
    return strdup(admin_logFile);
}
