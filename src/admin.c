//
//  admin.c
//  mastic
//
//  Created by Stephane Vales on 01/12/2017.
//  Copyright © 2017 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#if defined(__unix__) || defined(__unix) || \
(defined(__APPLE__) && defined(__MACH__))
#include <pthread.h>
#endif

#include "mastic.h"
#include "mastic_private.h"

FILE *fp = NULL;
bool admin_logInStream = false;
bool logInFile = false;
bool logInConsole = false;
bool useColorInConsole = false;
mtic_logLevel_t logLevel = MTIC_LOG_TRACE;
char logFile[1024] = "";
char logContent[2048] = "";
char logTime[128] = "";
#define NUMBER_OF_LOGS_FOR_FFLUSH 0
static int nb_of_entries = 0; //for fflush rotation

#if defined(__unix__) || defined(__unix) || \
(defined(__APPLE__) && defined(__MACH__))
pthread_mutex_t *lock = NULL;
#endif

static const char *log_levels[] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};
static const char *log_colors[] = {
    "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
};

////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////
void admin_computeTime(char *dest)
{
    struct timeval tick;
    gettimeofday(&tick, NULL);
    struct tm *tm = localtime(&tick.tv_sec);
    snprintf(dest,128,"%02d/%02d/%d;%02d:%02d:%02d.%06d",
             tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec, tick.tv_usec);
}

void admin_makeFilePath(const char *from, char *to, size_t size_of_to)
{
#if defined(__unix__) || defined(__unix) || \
(defined(__APPLE__) && defined(__MACH__))
    if (from[0] == '~'){
        from++;
        char *home = getenv("HOME");
        strncpy(to, home, size_of_to);
        strncat(to, from, size_of_to - strlen(home));
    }else{
        strncpy(to, from, size_of_to);
    }
#else
    //on Windows, just copy from to to
    strncpy(to, from, size_of_to);
#endif
}

void admin_lock(void)   {
    //TODO: see what to do for Windows
#if defined(__unix__) || defined(__unix) || \
(defined(__APPLE__) && defined(__MACH__))
    if (lock == NULL){
        lock = calloc(1, sizeof(pthread_mutex_t));
        if (pthread_mutex_init(lock, NULL) != 0)
        {
            printf("error: mutex init failed\n");
            return;
        }
    }
    pthread_mutex_lock(lock);
#endif
}

void admin_unlock(void) {
    //TODO: see what to do for Windows
#if defined(__unix__) || defined(__unix) || \
(defined(__APPLE__) && defined(__MACH__))
    if (lock != NULL){
        pthread_mutex_unlock(lock);
    }
#endif
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////

void mtic_log(mtic_logLevel_t level, const char *fmt, ...){
    admin_lock();
    
    va_list list;
    va_start(list, fmt);
    vsnprintf(logContent, 2047, fmt, list);
    va_end(list);
    
    if (logInFile){
        //create default path if current is empty
        if (strlen(logFile) == 0){
            char *name = mtic_getAgentName();
#if defined(__unix__) || defined(__unix) || \
(defined(__APPLE__) && defined(__MACH__))
            snprintf(logFile, 1023, "~/%s_log.csv", name);
#else
            //default path for Windows is current PATH
            snprintf(logFile, 1023, "%s_log.csv", name);
#endif
            free(name);
        }
        if (logFile[0] == '~'){
            char buff[1024] = "";
            strncpy(buff, logFile, 1023);
            admin_makeFilePath(buff, logFile, 1023);
        }
        if (access(logFile, W_OK) == -1){
            printf("creating log file: %s\n", logFile);
            fclose(fp);
            fp = NULL;
        }
        if (fp == NULL){
            fp = fopen (logFile,"a");
            if (fp == NULL){
                printf("error when trying to initiate log file: %s\n", logFile);
            }
        }
        admin_computeTime(logTime);
        fprintf(fp,"%s;%s;%s", logTime, log_levels[level], logContent);
        if (++nb_of_entries > NUMBER_OF_LOGS_FOR_FFLUSH){
            nb_of_entries = 0;
            fflush(fp);
        }
    }
    if (logInConsole && level >= logLevel){
        if (useColorInConsole){
            fprintf(stderr,"%s%-5s\x1b[0m;%s", log_colors[logLevel], log_levels[logLevel], logContent);
        }else{
            fprintf(stderr,"%-5s;%s", log_levels[logLevel], logContent);
        }
    }
    if (admin_logInStream && agentElements != NULL && agentElements->logger != NULL){
        zstr_sendf(agentElements->logger, "%-5s;%s", log_levels[logLevel], logContent);
    }
    admin_unlock();
    
}

void mtic_setLogLevel (mtic_logLevel_t level){
    logLevel = level;
}
void mtic_setLogInFile (bool allow){
    logInFile = allow;
}
void mtic_setVerbose (bool allow){
    logInConsole = allow;
}
void mtic_setUseColorVerbose (bool allow){
    useColorInConsole = allow;
}

void mtic_setLogStream(bool stream){
    if (agentElements != NULL){
        if (stream){
            mtic_warn("mtic_setLogStream: agent is already started, log stream cannot be created anymore\n");
        }else{
            mtic_warn("mtic_setLogStream: agent is already started, log stream cannot be destroyed anymore\n");
        }
        return;
    }
    admin_logInStream = stream;
}

void mtic_setLogPath(const char *path){
    if (path != NULL && strlen(path) > 0){
        bool needToResetFile = false;
        if (fp != NULL){
            //we need to close previous and initiate new one
            needToResetFile = true;
        }
        admin_makeFilePath(path, logFile, 1023);
        if (needToResetFile){
            admin_lock();
            fflush(fp);
            fclose(fp);
            fp = NULL;
            if (access(logFile, W_OK) == -1){
                printf("creating log file: %s\n", logFile);
            }
            fp = fopen (logFile,"a");
            if (fp == NULL){
                printf("error when trying to initiate log file: %s\n", logFile);
            }else{
                printf("switching to new log file: %s\n", logFile);
            }
            admin_unlock();
        }
    }else{
        printf("mtic_setLogPath : passed path cannot be NULL or with length equal to zero\n");
    }
}
