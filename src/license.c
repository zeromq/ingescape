//
//  license.c
//  ingescape
//
//  Created by Stephane Vales on 24/03/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <czmq.h>
#include "ingescape_private.h"

#ifdef __APPLE__
#include <TargetConditionals.h>
#include <CoreFoundation/CFBundle.h>
#if TARGET_OS_OSX
#include <libproc.h>
#endif
#endif

#if defined(__unix__) || defined(__linux__) || \
(defined(__APPLE__) && defined(__MACH__))
#include <pthread.h>
#endif
#if (defined WIN32 || defined _WIN32)
#include "unixfunctions.h"
#endif

#if defined(__unix__) || defined(__linux__) || \
(defined(__APPLE__) && defined(__MACH__))
pthread_mutex_t *license_readWriteMutex = NULL;
#else
#define W_OK 02
pthread_mutex_t license_readWriteMutex = NULL;
#endif

license_t *license = NULL;
char *licensePath = NULL;

////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////
void license_readWriteLock(void)   {
#if defined(__unix__) || defined(__linux__) || \
(defined(__APPLE__) && defined(__MACH__))
    if (license_readWriteMutex == NULL){
        license_readWriteMutex = calloc(1, sizeof(pthread_mutex_t));
        if (pthread_mutex_init(license_readWriteMutex, NULL) != 0){
            igs_error("mutex init failed");
            return;
        }
    }
#elif (defined WIN32 || defined _WIN32)
    if (license_readWriteMutex == NULL){
        if (pthread_mutex_init(&license_readWriteMutex) != 0){
            igs_error("mutex init failed");
            return;
        }
    }
#endif
    pthread_mutex_lock(license_readWriteMutex);
}

void license_readWriteUnlock(void) {
    if (license_readWriteMutex != NULL){
        pthread_mutex_unlock(license_readWriteMutex);
    }else{
        igs_error("mutex was NULL");
    }
}


const char* license_getFilenameExt(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

#ifdef __APPLE__
void switchToBundlePath(char **path){
    CFBundleRef b = CFBundleGetMainBundle();
    if (b != NULL){
        CFURLRef newPathURL = CFBundleCopyBundleURL(b);
        CFStringRef newPath = CFURLCopyPath(newPathURL);
        free (*path);
        *path = strdup(CFStringGetCStringPtr(newPath, kCFStringEncodingUTF8));
        char *index = *path + strlen(*path) - 2; //start just before final '/' in URL
        while (*index != '/' && index > *path){
            index--;
        }
        *index = '\0';
    }
}
#endif

//parse license file content
//NB: because several license files can be parsed, this function
//takes the least restricting values found in the files
//NB: license informations are not cumulated : first file found gives customer, owner, etc.
void license_parseCommand(const char *command, const char *data){
    if (strcmp(command, "customer") == 0 && license->customer == NULL){
        license->customer = strdup(data);
    }else if (strcmp(command, "order") == 0 && license->order == NULL){
        license->order = strdup(data);
    }else if (strcmp(command, "expiration") == 0){
        long licenseExpirationDate = atol(data);
        license->editorExpirationDate = MAX(license->licenseExpirationDate, licenseExpirationDate);
        unsigned long t = (unsigned long)time(NULL);
        if (license->licenseExpirationDate < t){
            license->isLicenseExpired = true;
        }
    }else if (strcmp(command, "platform") == 0){
        int platformNbAgents = 0;
        int platformNbIOPs = 0;
        sscanf(data, "%d %d", &platformNbAgents, &platformNbIOPs);
        license->platformNbAgents = MAX(license->platformNbAgents, platformNbAgents);
        license->platformNbIOPs = MAX(license->platformNbIOPs, platformNbIOPs);
    }else if (strcmp(command, "editor_owner") == 0 && license->editorOwner == NULL){
        license->editorOwner = strdup(data);
    }else if (strcmp(command, "editor_expiration") == 0){
        long editorExpirationDate = atol(data);
        license->editorExpirationDate = MAX(license->editorExpirationDate, editorExpirationDate);
        time_t t = time(NULL);
        if (license->editorExpirationDate < t){
            license->isEditorLicenseExpired = true;
        }
    }else if (strcmp(command, "feature") == 0){
        zlist_append(license->features, strdup(data));
    }else if (strcmp(command, "agent") == 0){
        licenseForAgent_t *l = calloc(1, sizeof(licenseForAgent_t));
        char agentId[256] = "";
        char agentName[256] = "";
        sscanf(data, "%255s %255s", agentId, agentName);
        l->agentId = strdup(agentId);
        l->agentName = strdup(agentName);
        zlist_append(license->agents, l);
    }
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
void license_cleanLicense(void){
    license_readWriteLock();
    if (license != NULL){
        if (license->customer != NULL){
            free(license->customer);
        }
        if (license->order != NULL){
            free(license->order);
        }
        if (license->editorOwner != NULL){
            free(license->editorOwner);
        }
        if (license->features != NULL){
            char *feat = zlist_first(license->features);
            while (feat != NULL){
                free(feat);
                feat = zlist_next(license->features);
            }
            zlist_destroy(&license->features);
        }
        if (license->agents != NULL){
            licenseForAgent_t *l = zlist_first(license->agents);
            while (l != NULL){
                if (l->agentId != NULL)
                    free(l->agentId);
                if (l->agentName)
                    free(l->agentName);
                l = zlist_next(license->agents);
            }
            zlist_destroy(&license->agents);
        }
        license = NULL;
    }
    license_readWriteUnlock();
}

void license_readLicense(void){
    if (licensePath == NULL){
        //use agent executable file as license path
#if defined __unix__ || defined __APPLE__ || defined __linux__
        int ret;
        pid_t pid;
        pid = getpid();
#ifdef __APPLE__
#if TARGET_OS_IOS
        char pathbuf[64] = "no_path";
        ret = 1;
#elif TARGET_OS_OSX
        char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
        ret = proc_pidpath (pid, pathbuf, sizeof(pathbuf));
#endif
#else
        char pathbuf[4*1024];
        memset(pathbuf, 0, 4*1024);
        readlink("/proc/self/exe", pathbuf, sizeof(pathbuf));
#endif
#endif
#if (defined WIN32 || defined _WIN32)
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2,2), &wsaData);
        //Use GetModuleFileName() to get exec path
        // See comment on define UNICODE in the top of this file, without define UNICODE This lines return NULL String
        WCHAR temp[MAX_PATH];
        GetModuleFileName(NULL,temp,MAX_PATH);
            
        //Conversion in char *
        char pathbuf[MAX_PATH];
        wcstombs_s(NULL,pathbuf,sizeof(pathbuf),temp,sizeof(temp));
#endif
        //remove exec name from exec path
        licensePath = strdup(pathbuf);
        char *index = licensePath + strlen(licensePath);
        while (*index != '/' && *index != '\\' && index > licensePath) {
               index--;
        }
        *index = '\0';
#if defined __APPLE__
        switchToBundlePath(&licensePath);
#endif
    }
    
    zdir_t *path = NULL;
    license_cleanLicense();
    license_readWriteLock();
    if (!zsys_file_exists(licensePath)){
        igs_error("%s could not be opened properly : no license found", licensePath);
    }else{
        path = zdir_new(licensePath, NULL);
        zlist_t *filesList = zdir_list(path);
        //iterate on files in folder to find a license file
        zfile_t *file = zlist_first(filesList);
        while (file != NULL && !zfile_is_directory(file)) {
            const char *name = zfile_filename(file, licensePath);
            const char *extension = license_getFilenameExt(name);
            if (strcmp(extension, "igslicense") == 0
                && zfile_is_readable(file)
                && zfile_input(file) == 0){
                if (license == NULL){
                    license = calloc(1, sizeof(license_t));
                    license->features = zlist_new();
                    license->agents = zlist_new();
                }
                igs_debug("parsing license file %s/%s", licensePath, name);
                //parse file
                const char *line;
                while ((line = zfile_readln(file))){
                    if (line[0] == ':'){
                        //we have a line with something to parse
                        char command[256] = "";
                        char data[256] = "";
                        if (sscanf(line, ":%s %[\001-\377]", command, data) == 2){
                            license_parseCommand(command, data);
                        }
                    }
                }
                zfile_close(file);
                break;
            }
            file = zlist_next(filesList);
        }
    }
    if (license == NULL){
        igs_info("no license found in %s : switching to demo mode", licensePath);
        license = calloc(1, sizeof(license_t));
        license->features = zlist_new();
        license->agents = zlist_new();
    }
    //set license parameters to default for uninitialized values
    if (license->customer == NULL)
        license->customer = strdup("Unregistered");
    if (license->order == NULL)
        license->order = strdup("none");
    if (license->licenseExpirationDate == 0){
        license->licenseExpirationDate = -1;
        license->isLicenseExpired = true;
    }
    if (license->platformNbAgents == 0)
        license->platformNbAgents = MAX_NB_OF_AGENTS;
    if (license->platformNbIOPs == 0)
        license->platformNbIOPs = MAX_NB_OF_IOP;
    if (license->editorOwner == NULL)
        license->editorOwner = strdup("Unregistered");
    if (license->editorExpirationDate == 0){
        license->editorExpirationDate = -1;
        license->isEditorLicenseExpired = true;
    }
    license_readWriteUnlock();
}


////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////
void igs_setLicensePath(const char *path){
    if (zsys_file_exists(path)){
        if (licensePath != NULL){
            free(licensePath);
        }
        licensePath = strdup(path);
    }else{
        igs_warn("%s does not exist", path);
    }
}
char *igs_getLicensePath(void){
    return strdup(licensePath);
}

bool igs_checkLicenseForAgent(const char *agentId){
    if (license == NULL){
        license_readLicense();
    }
    licenseForAgent_t *l = zlist_first(license->agents);
    while (l != NULL) {
        if (l->agentId != NULL
            && strcmp(agentId, l->agentId) == 0){
            return true;
        }
    }
    return false;
}
