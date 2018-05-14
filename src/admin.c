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
#ifdef _WIN32
#include "unixfunctions.h"
#endif

#if defined(__unix__) || defined(__unix) || \
(defined(__APPLE__) && defined(__MACH__))
#include <pthread.h>
#endif

#include "ingescape.h"
#include "ingescape_private.h"

#define INGESCAPE_MAJOR 0
#define INGESCAPE_MINOR 8
#define INGESCAPE_MICRO 1
#define INGESCAPE_VERSION ((INGESCAPE_MAJOR * 10000) + (INGESCAPE_MINOR * 100) + INGESCAPE_MICRO)

FILE *fp = NULL;
bool admin_logInStream = false;
bool logInFile = false;
bool logInConsole = false;
bool useColorInConsole = false;
igs_logLevel_t logLevel = IGS_LOG_INFO;
char logFile[1024] = "";
char logContent[2048] = "";
char logTime[128] = "";
#define NUMBER_OF_LOGS_FOR_FFLUSH 0
static int nb_of_entries = 0; //for fflush rotation

#if defined(__unix__) || defined(__unix) || \
(defined(__APPLE__) && defined(__MACH__))
pthread_mutex_t *lock = NULL;
#else
#define W_OK 02
#define pthread_mutex_t HANDLE
pthread_mutex_t *lock = NULL;
#endif

static const char *log_levels[] = {
    "TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "FATAL"
};
static const char *log_colors[] = {
    "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
};

////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////

#if defined(_WIN32)
    int gettimeofday(struct timeval* p, void* tz) {
        ULARGE_INTEGER ul; // As specified on MSDN.
        FILETIME ft;

        // Returns a 64-bit value representing the number of
        // 100-nanosecond intervals since January 1, 1601 (UTC).
        GetSystemTimeAsFileTime(&ft);

        // Fill ULARGE_INTEGER low and high parts.
        ul.LowPart = ft.dwLowDateTime;
        ul.HighPart = ft.dwHighDateTime;
        // Convert to microseconds.
        ul.QuadPart /= 10ULL;
        // Remove Windows to UNIX Epoch delta.
        ul.QuadPart -= 11644473600000000ULL;
        // Modulo to retrieve the microseconds.
        p->tv_usec = (long) (ul.QuadPart % 1000000LL);
        // Divide to retrieve the seconds.
        p->tv_sec = (long) (ul.QuadPart / 1000000LL);

        return 0;
    }

    int pthread_mutex_init(pthread_mutex_t *mutex)
    {
        mutex=CreateMutex(NULL,FALSE, NULL);
        if (mutex==NULL) return 1; else return 0;
    }
    int pthread_mutex_lock(pthread_mutex_t *mutex)
    {
        while (OpenMutex(MUTEX_ALL_ACCESS,FALSE,NULL)==NULL) {
                WaitForSingleObject(mutex,INFINITE);
        }
        return 0;
    }
    int pthread_mutex_unlock(pthread_mutex_t *mutex)
    {
        ReleaseMutex(*mutex);
        return 0;
    }
#endif

void admin_computeTime(char *dest){
#if defined(_WIN32)
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
             tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec, tick.tv_usec);
#endif
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
#else
    if (lock == NULL){
        if (pthread_mutex_init(lock) != 0)
        {
            printf("error: mutex init failed\n");
            return;
        }
    }
#endif
}

void admin_unlock(void) {
    //TODO: see what to do for Windows
#if defined(__unix__) || defined(__unix) || \
(defined(__APPLE__) && defined(__MACH__))
    if (lock != NULL){
        pthread_mutex_unlock(lock);
    }
#else
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

int igs_version(void){
    igs_info("IngeScape version : %d.%d.%d\n", INGESCAPE_MAJOR, INGESCAPE_MINOR, INGESCAPE_MICRO);
    return INGESCAPE_VERSION;
}

void igs_log(igs_logLevel_t level, const char *function, const char *fmt, ...){
    admin_lock();

    va_list list;
    va_start(list, fmt);
    vsnprintf(logContent, 2047, fmt, list);
    va_end(list);
    
    //remove final \n if needed
    //TODO: scan the whole string to remove unallowed characters
    if (logContent[strlen(logContent) - 1] == '\n'){
        logContent[strlen(logContent) - 1] = '\0';
    }

    if (logInFile){
        //create default path if current is empty
        if (strlen(logFile) == 0){
            char *name = igs_getAgentName();
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
#if defined(__unix__) || defined(__unix) || \
    (defined(__APPLE__) && defined(__MACH__))
            fclose(fp);
            fp = NULL;
#endif
        }
        if (fp == NULL){
            fp = fopen (logFile,"a");
            if (fp == NULL){
                printf("error when trying to initiate log file: %s\n", logFile);
            }
        }
        admin_computeTime(logTime);
        fprintf(fp,"%s;%s;%s;%s\n", logTime, log_levels[level], function, logContent);
        if (++nb_of_entries > NUMBER_OF_LOGS_FOR_FFLUSH){
            nb_of_entries = 0;
            fflush(fp);
        }
    }
    if ((logInConsole && level >= logLevel) || level >= IGS_LOG_ERROR){
        if (useColorInConsole){
            fprintf(stderr,"%s%s\x1b[0m;%s;%s\n", log_colors[level], log_levels[level], function, logContent);
        }else{
            fprintf(stderr,"%s;%s;%s\n", log_levels[level], function, logContent);
        }
    }
    if (admin_logInStream && agentElements != NULL && agentElements->logger != NULL){
        zstr_sendf(agentElements->logger, "%s;%s;%s\n", log_levels[level], function, logContent);
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
    logInFile = allow;
}

bool igs_getLogInFile (void) {
    return logInFile;
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
    if (agentElements != NULL){
        if (stream){
            igs_warn("agent is already started, log stream cannot be created anymore");
        }else{
            igs_warn("agent is already started, log stream cannot be disabled anymore");
        }
        return;
    }
    admin_logInStream = stream;
}

bool igs_getLogStream (void) {
    return admin_logInStream;
}

void igs_setLogPath(const char *path){
    if ((path != NULL) && (strlen(path) > 0)){
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
                igs_info("creating new log file: %s", logFile);
            }
            fp = fopen (logFile,"a");
            if (fp == NULL){
                igs_error("error when trying to initiate log file at path %s", logFile);
            }else{
                igs_info("switching to new log file: %s", logFile);
            }
            admin_unlock();
        }
    }else{
        igs_error("passed path cannot be NULL or with length equal to zero");
    }
}

char* igs_getLogPath (void) {
    return strdup(logFile);
}
