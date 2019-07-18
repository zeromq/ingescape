//
//  license.c
//  ingescape
//
//  Created by Stephane Vales on 24/03/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <czmq.h>
#include <sodium.h>
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
#include <dirent.h>
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

uint8_t secretEncryptionKey[crypto_secretstream_xchacha20poly1305_KEYBYTES] = {1,255,34,41,58,63,47,183,134,223,33,41,25,16,87,38,211,27,183,124,185,196,107,128,34,92,83,54,35,60,37,28};
unsigned char publicSignKey[crypto_sign_PUBLICKEYBYTES] = {47,20,1,206,112,73,169,19,31,67,21,116,192,151,109,34,215,117,250,86,247,235,53,159,208,126,234,177,133,49,103,111};
#define CHUNK_SIZE 4096

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

int decryptLicenseFromFile(char **target_string, const char *source_file,
                           const unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]){
    //decrypt signed string
    unsigned char  buf_in[CHUNK_SIZE + crypto_secretstream_xchacha20poly1305_ABYTES];
    unsigned char  buf_out[CHUNK_SIZE];
    unsigned char  header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
    crypto_secretstream_xchacha20poly1305_state st;
    FILE          *fp_s;
    unsigned long long out_len;
    size_t         rlen;
    int            eof;
    int            ret = -1;
    unsigned char  tag;
    
    size_t targetSize = 0;
    char *target_index = NULL;
    char *signed_target_string = NULL;
    
    fp_s = fopen(source_file, "rb");
    if (fp_s == NULL){
        printf("%s file not found\n", source_file);
        return -1;
    }
    bool decryptionWentOK = true;
    fread(header, 1, sizeof header, fp_s);
    if (crypto_secretstream_xchacha20poly1305_init_pull(&st, header, key) != 0) {
        igs_license("license file %s header is incomplete : file will be ignored", source_file);
        decryptionWentOK = false;
        goto ret;
    }
    do {
        rlen = fread(buf_in, 1, sizeof buf_in, fp_s);
        eof = feof(fp_s);
        if (crypto_secretstream_xchacha20poly1305_pull(&st, buf_out, &out_len, &tag,
                                                       buf_in, rlen, NULL, 0) != 0) {
            igs_license("license file %s is corrupted and will be ignored", source_file);
            decryptionWentOK = false;
            goto ret;
        }
        if (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && ! eof) {
            igs_license("license file %s size is too short : file will be ignored", source_file);
            decryptionWentOK = false;
            goto ret;
        }
        if (targetSize == 0){
            targetSize = out_len + 1; //+1 to reserve space for final \0
            signed_target_string = calloc(1, targetSize);
            target_index = signed_target_string;
        }else{
            targetSize += out_len;
            signed_target_string = realloc(signed_target_string, targetSize);
        }
        memcpy(target_index, buf_out, out_len);
        target_index += out_len;
    } while (! eof);
    ret = 0;
ret:
    if (fp_s != NULL)
        fclose(fp_s);
    
    if (decryptionWentOK){//verify signed string
        //    printf("decrypted string is %zu bytes long:\n%s***\n", targetSize - 1, signed_target_string);
        *target_string = calloc(targetSize, sizeof(char));
        unsigned long long target_string_len;
        if (crypto_sign_open((unsigned char*)(*target_string), &target_string_len,
                             (const unsigned char *)signed_target_string, targetSize - 1, publicSignKey) != 0) {
            igs_license("license signature is incorrect");
            ret = -1;
        }
    }
    
    if (signed_target_string != NULL)
        free(signed_target_string);
    return ret;
}

const char* license_getFilenameExt(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

#ifdef __APPLE__
void switchToBundlePath(char **path){
    assert(path);
    assert(*path);
    CFBundleRef b = CFBundleGetMainBundle();
    if (b != NULL){
        CFURLRef newPathURL = CFBundleCopyBundleURL(b);
        CFStringRef newPath = CFURLCopyPath(newPathURL);
        free (*path);
        *path = strdup(CFStringGetCStringPtr(newPath, kCFStringEncodingUTF8));
        char *index = *path + strlen(*path) - 2; //start just before final '/' in URL
        //we change license path only if bundle path ends with '.app'
        if (*index == 'p' &&
            *(index - 1) == 'p'&&
            *(index - 2) == 'a'&&
            *(index - 3) == '.'){
            while (*index != '/' && index > *path){
                index--;
            }
            *index = '\0';
        }
    }
}
#endif

//parse license file content
//NB: because several license files can be parsed, this function
//takes the least restricting values found in the files
void license_parseLine(const char *command, const char *data, license_t *myLicense){
    if (strcmp(command, "id") == 0 && data != NULL){
        if (myLicense->id == NULL){
            myLicense->id = strdup(data);
        }else{
            myLicense->id = realloc(myLicense->id, strlen(myLicense->id) + strlen(data) + 3);
            sprintf(myLicense->id, "%s, %s", myLicense->id, data);
        }
    }else if (strcmp(command, "customer") == 0  && data != NULL){
        if (myLicense->customer == NULL){
            myLicense->customer = strdup(data);
        }else{
            myLicense->customer = realloc(myLicense->customer, strlen(myLicense->customer) + strlen(data) + 3);
            sprintf(myLicense->customer, "%s, %s", myLicense->customer, data);
        }
    }else if (strcmp(command, "order") == 0  && data != NULL){
        if (myLicense->order == NULL){
            myLicense->order = strdup(data);
        }else{
            myLicense->order = realloc(myLicense->order, strlen(myLicense->order) + strlen(data) + 3);
            sprintf(myLicense->order, "%s, %s", myLicense->order, data);
        }
    }else if (strcmp(command, "expiration") == 0){
        long licenseExpirationDate = atol(data);
        if (myLicense->licenseExpirationDate < licenseExpirationDate){
            myLicense->licenseExpirationDate = licenseExpirationDate;
        }
        long t = (long)time(NULL);
        if (myLicense->licenseExpirationDate < t){
            myLicense->isLicenseExpired = true;
        }
    }else if (strcmp(command, "platform") == 0){
        int platformNbAgents = 0;
        int platformNbIOPs = 0;
        sscanf(data, "%d %d", &platformNbAgents, &platformNbIOPs);
        if (myLicense->platformNbAgents < platformNbAgents){
            myLicense->platformNbAgents = platformNbAgents;
        }
        if (myLicense->platformNbIOPs < platformNbIOPs){
            myLicense->platformNbIOPs = platformNbIOPs;
        }
    }else if (strcmp(command, "editorOwner") == 0  && data != NULL){
        if (myLicense->editorOwner == NULL){
            myLicense->editorOwner = strdup(data);
        }else{
            myLicense->editorOwner = realloc(myLicense->editorOwner, strlen(myLicense->editorOwner) + strlen(data) + 3);
            sprintf(myLicense->editorOwner, "%s, %s", myLicense->editorOwner, data);
        }
    }else if (strcmp(command, "editorExpiration") == 0){
        long editorExpirationDate = atol(data);
        if (myLicense->editorExpirationDate < editorExpirationDate){
            myLicense->editorExpirationDate = editorExpirationDate;
        }
        time_t t = time(NULL);
        if (myLicense->editorExpirationDate < t){
            myLicense->isEditorLicenseExpired = true;
        }
    }else if (strcmp(command, "feature") == 0){
        zlist_append(myLicense->features, strdup(data));
    }else if (strcmp(command, "agent") == 0){
        licenseForAgent_t *l = calloc(1, sizeof(licenseForAgent_t));
        char agentId[256] = "";
        char agentName[256] = "";
        sscanf(data, "%255s %255s", agentId, agentName);
        l->agentId = strdup(agentId);
        l->agentName = strdup(agentName);
        zlist_append(myLicense->agents, l);
    }
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
void license_cleanLicense(void){
    license_readWriteLock();
    if (license != NULL){
        if (license->id != NULL){
            free(license->id);
        }
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
        if (license->licenceDetails != NULL){
            license_t *detail = zlist_first(license->licenceDetails);
            while (detail != NULL){
                if (detail->id != NULL){
                    free(detail->id);
                }
                if (detail->fileName != NULL){
                    free(detail->fileName);
                }
                if (detail->customer != NULL){
                    free(detail->customer);
                }
                if (detail->order != NULL){
                    free(detail->order);
                }
                if (detail->editorOwner != NULL){
                    free(detail->editorOwner);
                }
                if (detail->features != NULL){
                    char *feat = zlist_first(detail->features);
                    while (feat != NULL){
                        free(feat);
                        feat = zlist_next(detail->features);
                    }
                    zlist_destroy(&detail->features);
                }
                if (detail->agents != NULL){
                    licenseForAgent_t *l = zlist_first(detail->agents);
                    while (l != NULL){
                        if (l->agentId != NULL)
                            free(l->agentId);
                        if (l->agentName)
                            free(l->agentName);
                        l = zlist_next(detail->agents);
                    }
                    zlist_destroy(&detail->agents);
                }
                detail = zlist_next(license->licenceDetails);
            }
            zlist_destroy(&license->licenceDetails);
        }
        license = NULL;
    }
    license_readWriteUnlock();
}

void license_readLicense(void){
    if (license == NULL){
        if (sodium_init() != 0) {
            //nothing to do ?
            //NB: sodium returns -1 when trying to initialize more than once
        }
    }
    license_cleanLicense();
    license_readWriteLock();
    if (licensePath == NULL){
        //use agent executable file as license path
#if defined __unix__ || defined __APPLE__ || defined __linux__
#ifdef __APPLE__
#if TARGET_OS_IOS
        char pathbuf[64] = "no_path";
#elif TARGET_OS_OSX
        pid_t pid = getpid();
        char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
        proc_pidpath (pid, pathbuf, sizeof(pathbuf));
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
    
    if (!zsys_file_exists(licensePath)){
        igs_error("%s could not be opened properly : no license found", licensePath);
    }else{
        //NB: zdir provides a function to scan folder contents but
        //this function scans all subdirectories which may time a very
        //long time to do, e.g. in /Applications/.
        //That's why we use dirent here.
        igs_license("scan for licenses in %s", licensePath);
        zlist_t *filesList = zlist_new();
        DIR *dir;
        struct dirent *ent;
        if ((dir = opendir (licensePath)) != NULL) {
            ent = readdir (dir);
            while (ent != NULL) {
                zfile_t *new = zfile_new(licensePath, ent->d_name);
                const char *name = zfile_filename(new, licensePath);
                const char *extension = license_getFilenameExt(name);
                if (strcmp(extension, "igslicense") == 0
                    && !zfile_is_directory(new)
                    && zfile_is_readable(new)){
                    zlist_append(filesList, new);
                }else{
                    zfile_destroy(&new);
                }
                ent = readdir (dir);
            }
            closedir (dir);
        } else {
            // could not open directory
        }
        //iterate on license files in folder
        zfile_t *file = zlist_first(filesList);
        while (file != NULL) {
            if (license == NULL){
                license = calloc(1, sizeof(license_t));
                license->features = zlist_new();
                license->agents = zlist_new();
                license->licenceDetails = zlist_new();
            }
            license_t *detail = calloc(1, sizeof(license_t));
            zlist_append(license->licenceDetails, detail);
            detail->features = zlist_new();
            detail->agents = zlist_new();
            
            const char *name = zfile_filename(file, NULL);
            detail->fileName = strdup(name);
            igs_debug("parsing license file %s", name);
            //decrypt file
            char *licenseText = NULL;
            decryptLicenseFromFile(&licenseText, zfile_filename(file, NULL), secretEncryptionKey);
            //igs_license("raw license file:\n%s", licenseText);
            //parse file
            char * curLine = licenseText;
            while(curLine){
                char * nextLine = strchr(curLine, '\n');
                if (nextLine) *nextLine = '\0';  // temporarily terminate the current line
                if (curLine[0] == ':'){
                    //we have a line with something to parse
                    char type[256] = "";
                    char data[256] = "";
                    if (sscanf(curLine, ":%s %[\001-\377]", type, data) == 2){
                        //parse into main license to define least restricting conditions
                        license_parseLine(type, data, license);
                        //parse into detail to keep trace of each file detail
                        license_parseLine(type, data, detail);
                    }
                }
                if (nextLine)
                    *nextLine = '\n';  // then restore newline-char, just to be tidy
                curLine = nextLine ? (nextLine + 1) : NULL;
            }
            zfile_destroy(&file);
            file = zlist_next(filesList);
        }
        zlist_destroy(&filesList);
    }
    if (license == NULL){
        igs_license("no license found in %s : switching to demo mode", licensePath);
        license = calloc(1, sizeof(license_t));
        license->features = zlist_new();
        license->agents = zlist_new();
        license->licenceDetails = NULL;
    }
    //set license parameters to default for uninitialized values
    if (license->id == NULL)
        license->id = strdup("Unregistered");
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
        igs_license("%s does not exist", path);
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

int igs_observeLicense(igs_licenseCallback cb, void *myData){
    license_callback_t *l = (license_callback_t *)calloc(1, sizeof(license_callback_t));
    l->callback_ptr = cb;
    l->data = myData;
    DL_APPEND(licenseCallbacks, l);
    return 1;
}
