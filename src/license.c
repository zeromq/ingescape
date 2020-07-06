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
            igs_fatal("mutex init failed");
            assert(false);
            return;
        }
    }
#elif (defined WIN32 || defined _WIN32)
    if (license_readWriteMutex == NULL){
        if (pthread_mutex_init(&license_readWriteMutex) != 0){
            igs_fatal("mutex init failed");
            assert(false);
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
        igs_fatal("mutex was NULL");
        assert(false);
    }
}

int decryptLicenseFromData(char **target_string, void *data, size_t size,
                           const unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]){
    //decrypt signed string
    unsigned char  buf_in[CHUNK_SIZE + crypto_secretstream_xchacha20poly1305_ABYTES];
    unsigned char  buf_out[CHUNK_SIZE];
    unsigned char  header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
    crypto_secretstream_xchacha20poly1305_state st;
    unsigned long long out_len;
    size_t         remainingSize;
    size_t         readSize;
    int            ret = -1;
    unsigned char  tag;
    
    size_t targetSize = 0;
    char *target_index = NULL;
    char *signed_target_string = NULL;
    
    bool decryptionWentOK = true;
    
    if (data == NULL || size == 0){
        igs_license("license data is incomplete : data will be ignored");
        decryptionWentOK = false;
        goto ret;
    }
    
    if (size < sizeof(header)){
        igs_license("license data is incomplete : data will be ignored");
        decryptionWentOK = false;
        goto ret;
    }
    
    //read header and move data head
    memcpy(header, data, sizeof(header));
    data = (char *) data + sizeof(header);
    remainingSize = size - sizeof(header);
    
    if (crypto_secretstream_xchacha20poly1305_init_pull(&st, header, key) != 0) {
        igs_license("license data is incomplete : data will be ignored");
        decryptionWentOK = false;
        goto ret;
    }
    do {
        if (remainingSize < sizeof(buf_in)){
            //reaching end of data
            memcpy(buf_in, data, remainingSize);
            readSize = remainingSize;
            remainingSize = 0;
        }else{
            memcpy(buf_in, data, sizeof(buf_in));
            data = (char *) data + sizeof(buf_in);
            readSize = sizeof(buf_in);
            remainingSize -= sizeof(buf_in);
        }
        if (crypto_secretstream_xchacha20poly1305_pull(&st, buf_out, &out_len, &tag,
                                                       buf_in, readSize, NULL, 0) != 0) {
            igs_license("license data is corrupted and will be ignored");
            decryptionWentOK = false;
            goto ret;
        }
        if (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && remainingSize > 0) {
            igs_license("license data size is too short : data will be ignored");
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
    } while (remainingSize > 0);
    ret = 0;
ret:
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
void license_parseLine(const char *command, const char *data, igs_license_t *myLicense){
    if (strcmp(command, "id") == 0 && data != NULL){
        if (myLicense->id != NULL){
            free(myLicense->id);
        }
        myLicense->id = strdup(data);
    }else if (strcmp(command, "customer") == 0  && data != NULL){
        if (myLicense->customer != NULL){
            free(myLicense->customer);
        }
        myLicense->customer = strdup(data);
    }else if (strcmp(command, "order") == 0  && data != NULL){
        if (myLicense->order != NULL){
            free(myLicense->order);
        }
        myLicense->order = strdup(data);
    }else if (strcmp(command, "expiration") == 0){
        long licenseExpirationDate = atol(data);
        myLicense->licenseExpirationDate = licenseExpirationDate;
        long t = (long)time(NULL);
        if (myLicense->licenseExpirationDate < t){
            myLicense->isLicenseValid = false;
        }else{
            myLicense->isLicenseValid = true;
        }
    }else if (strcmp(command, "platform") == 0){
        int platformNbAgents = 0;
        int platformNbIOPs = 0;
        sscanf(data, "%d %d", &platformNbAgents, &platformNbIOPs);
        myLicense->platformNbAgents = platformNbAgents;
        myLicense->platformNbIOPs = platformNbIOPs;
    }else if (strcmp(command, "editorOwner") == 0  && data != NULL){
        if (myLicense->editorOwner != NULL){
            free(myLicense->editorOwner);
        }
        myLicense->editorOwner = strdup(data);
    }else if (strcmp(command, "editorExpiration") == 0){
        long editorExpirationDate = atol(data);
        myLicense->editorExpirationDate = editorExpirationDate;
        time_t t = time(NULL);
        if (myLicense->editorExpirationDate < t){
            myLicense->isEditorLicenseValid = false;
        }else{
            myLicense->isEditorLicenseValid = true;
        }
    }else if (strcmp(command, "feature") == 0){
        zhash_insert(myLicense->features, data, NULL);
    }else if (strcmp(command, "agent") == 0){
        igs_license_for_agent_t *l = calloc(1, sizeof(igs_license_for_agent_t));
        char agentId[256] = "";
        char agentName[256] = "";
        sscanf(data, "%255s %255s", agentId, agentName);
        l->agentId = strdup(agentId);
        l->agentName = strdup(agentName);
        zhash_insert(myLicense->agents, agentId, l);
    }
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
void license_cleanLicense(){
    license_readWriteLock();
    if (coreContext->license != NULL){
        if (coreContext->license->id != NULL){
            free(coreContext->license->id);
        }
        if (coreContext->license->customer != NULL){
            free(coreContext->license->customer);
        }
        if (coreContext->license->order != NULL){
            free(coreContext->license->order);
        }
        if (coreContext->license->editorOwner != NULL){
            free(coreContext->license->editorOwner);
        }
        if (coreContext->license->features != NULL){
            zhash_destroy(&coreContext->license->features);
        }
        if (coreContext->license->agents != NULL){
            igs_license_for_agent_t *l = zhash_first(coreContext->license->agents);
            while (l != NULL){
                if (l->agentId != NULL)
                    free(l->agentId);
                if (l->agentName)
                    free(l->agentName);
                free(l);
                l = zhash_next(coreContext->license->agents);
            }
            zhash_destroy(&coreContext->license->agents);
        }
        if (coreContext->license->licenseDetails != NULL){
            igs_license_t *detail = zlist_first(coreContext->license->licenseDetails);
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
                    zhash_destroy(&detail->features);
                }
                if (detail->agents != NULL){
                    igs_license_for_agent_t *l = zhash_first(detail->agents);
                    while (l != NULL){
                        if (l->agentId != NULL)
                            free(l->agentId);
                        if (l->agentName)
                            free(l->agentName);
                        free(l);
                        l = zhash_next(detail->agents);
                    }
                    zhash_destroy(&detail->agents);
                }
                free(detail);
                detail = zlist_next(coreContext->license->licenseDetails);
            }
            zlist_destroy(&coreContext->license->licenseDetails);
        }
        free(coreContext->license);
        coreContext->license = NULL;
    }
    license_readWriteUnlock();
}

void license_readLicense(){
    if (coreContext->license == NULL){
        if (sodium_init() != 0) {
            //nothing to do ?
            //NB: sodium returns -1 when trying to initialize more than once
        }
    }
    license_cleanLicense();
    license_readWriteLock();
    
    if (coreContext->licenseData != NULL){
        // USE LICENSE DATA (and not files)
        coreContext->license = calloc(1, sizeof(igs_license_t));
        coreContext->license->features = zhash_new();
        coreContext->license->agents = zhash_new();
        coreContext->license->licenseDetails = zlist_new();
        igs_license_t *detail = calloc(1, sizeof(igs_license_t));
        zlist_append(coreContext->license->licenseDetails, detail);
        detail->features = zhash_new();
        detail->agents = zhash_new();
        detail->fileName = calloc(128, sizeof(char));
        sprintf(detail->fileName, "internal data");
        
        igs_debug("parsing license data");
        //decrypt file
        char *licenseText = NULL;
        decryptLicenseFromData(&licenseText, coreContext->licenseData, coreContext->licenseDataSize, secretEncryptionKey);
        //igs_license("raw license data:\n%s", licenseText);
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
                    //parse into detail to keep trace of each file
                    license_parseLine(type, data, detail);
                }
            }
            if (nextLine)
                *nextLine = '\n';  // then restore newline-char, just to be tidy
            curLine = nextLine ? (nextLine + 1) : NULL;
        }
        free(licenseText);
        
    }else{
        // LOOK FOR LICENSE FILES
        if (coreContext->licensePath == NULL){
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

            // Use GetModuleFileName() to get exec path
            char pathbuf[IGS_MAX_PATH];
        #ifdef UNICODE
            WCHAR temp[IGS_MAX_PATH];
            GetModuleFileName(NULL,temp,IGS_MAX_PATH);
            wcstombs_s(NULL,pathbuf,sizeof(pathbuf),temp,sizeof(temp));
        #else
            GetModuleFileName(NULL,pathbuf,IGS_MAX_PATH);
        #endif
        #endif
            //remove exec name from exec path
            coreContext->licensePath = strdup(pathbuf);
            char *index = coreContext->licensePath + strlen(coreContext->licensePath);
            while (*index != '/' && *index != '\\' && index > coreContext->licensePath) {
                index--;
            }
            *index = '\0';
        #if defined __APPLE__
            switchToBundlePath(&coreContext->licensePath);
        #endif
        }
            
        if (!zsys_file_exists(coreContext->licensePath)){
            igs_error("%s could not be opened properly : no license found", coreContext->licensePath);
        }else{
            //NB: zdir provides a function to scan folder contents but
            //this function scans all subdirectories which may take a very
            //long time, e.g. in /Applications/.
            //That's why we use dirent here.
            igs_debug("scan for licenses in %s", coreContext->licensePath);
            zlist_t *filesList = zlist_new();
            DIR *dir;
            struct dirent *ent;
            if ((dir = opendir (coreContext->licensePath)) != NULL) {
                ent = readdir (dir);
                while (ent != NULL) {
                    zfile_t *new = zfile_new(coreContext->licensePath, ent->d_name);
                    const char *name = zfile_filename(new, coreContext->licensePath);
                    igs_debug("scan %s", name);
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
                igs_error("could not open %s", coreContext->licensePath);
            }
            igs_debug("%zu license(s) found", zlist_size(filesList));
            if (zlist_size(filesList) > 0){
                coreContext->license = calloc(1, sizeof(igs_license_t));
                coreContext->license->features = zhash_new();
                coreContext->license->agents = zhash_new();
                coreContext->license->licenseDetails = zlist_new();
            }
            
            //iterate on license files in folder
            zfile_t *file = zlist_first(filesList);
            while (file != NULL) {
                igs_license_t *detail = calloc(1, sizeof(igs_license_t));
                zlist_append(coreContext->license->licenseDetails, detail);
                detail->features = zhash_new();
                detail->agents = zhash_new();
                
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
                            //parse into detail to keep trace of each file
                            license_parseLine(type, data, detail);
                        }
                    }
                    if (nextLine)
                        *nextLine = '\n';  // then restore newline-char, just to be tidy
                    curLine = nextLine ? (nextLine + 1) : NULL;
                }
                free(licenseText);
                zfile_destroy(&file);
                file = zlist_next(filesList);
            }
            zlist_destroy(&filesList);
        }
    }
    
    
    //go through details to apply least contraining values in main license struct
    //NB: in case of using license raw data, there will only be one detail whereas
    //when using files, there will be one detail by file.
    igs_license_t *detail = NULL;
    if (coreContext->license != NULL && coreContext->license->licenseDetails != NULL){
        detail = zlist_first(coreContext->license->licenseDetails);
    }
    while (detail != NULL){
        if (detail->isLicenseValid){
            if (detail->id != NULL){
                if (coreContext->license->id == NULL){
                    coreContext->license->id = strdup(detail->id);
                }else{
                    char *tempId = strdup(coreContext->license->id);
                    coreContext->license->id = realloc(coreContext->license->id, strlen(coreContext->license->id) + strlen(detail->id) + 3);
                    sprintf(coreContext->license->id, "%s, %s", tempId, detail->id);
                    free(tempId);
                }
            }
            if (detail->customer != NULL){
                if (coreContext->license->customer == NULL){
                    coreContext->license->customer = strdup(detail->customer);
                }else{
                    char *tempCustomer = strdup(coreContext->license->customer);
                    coreContext->license->customer = realloc(coreContext->license->customer, strlen(coreContext->license->customer) + strlen(detail->customer) + 3);
                    sprintf(coreContext->license->customer, "%s, %s", tempCustomer, detail->customer);
                    free(tempCustomer);
                }
            }
            if (detail->order != NULL){
                if (coreContext->license->order == NULL){
                    coreContext->license->order = strdup(detail->order);
                }else{
                    char *tempOrder = strdup(coreContext->license->order);
                    coreContext->license->order = realloc(coreContext->license->order, strlen(coreContext->license->order) + strlen(detail->order) + 3);
                    sprintf(coreContext->license->order, "%s, %s", tempOrder, detail->order);
                    free(tempOrder);
                }
            }
            if (coreContext->license->licenseExpirationDate < detail->licenseExpirationDate){
                coreContext->license->licenseExpirationDate = detail->licenseExpirationDate;
                long t = (long)time(NULL);
                if (coreContext->license->licenseExpirationDate >= t){
                    coreContext->license->isLicenseValid = true;
                }
            }
            
            if (coreContext->license->platformNbAgents < detail->platformNbAgents){
                coreContext->license->platformNbAgents = detail->platformNbAgents;
            }
            if (coreContext->license->platformNbIOPs < detail->platformNbIOPs){
                coreContext->license->platformNbIOPs = detail->platformNbIOPs;
            }
            
            //add new features that are not in the list yet
            zlist_t *features = zhash_keys(detail->features);
            char *k = zlist_first(features);
            while (k != NULL){
                if (zhash_lookup(coreContext->license->features, k) == NULL){
                    zhash_insert(coreContext->license->features, k, NULL);
                }
                k = zlist_next(features);
            }
            zlist_destroy(&features);
            
            //add new agents that are not in the list yet
            zlist_t *agents = zhash_keys(detail->agents);
            k = zlist_first(agents);
            while (k != NULL){
                if (zhash_lookup(coreContext->license->agents, k) == NULL){
                    igs_license_for_agent_t *l = zhash_lookup(detail->agents, k);
                    igs_license_for_agent_t *l_dup = calloc(1, sizeof(igs_license_for_agent_t));
                    l_dup->agentId = strdup(l->agentId);
                    l_dup->agentName = strdup(l->agentName);
                    zhash_insert(coreContext->license->agents, k, l_dup);
                }
                k = zlist_next(agents);
            }
            zlist_destroy(&agents);
        }
        if (detail->isEditorLicenseValid){
            if (detail->editorOwner != NULL){
                if (coreContext->license->editorOwner == NULL){
                    coreContext->license->editorOwner = strdup(detail->editorOwner);
                }else{
                    char *tempOwner = strdup(coreContext->license->editorOwner);
                    coreContext->license->editorOwner = realloc(coreContext->license->editorOwner, strlen(coreContext->license->editorOwner) + strlen(detail->editorOwner) + 3);
                    sprintf(coreContext->license->editorOwner, "%s, %s", tempOwner, detail->editorOwner);
                    free(tempOwner);
                }
            }
            if (coreContext->license->editorExpirationDate < detail->editorExpirationDate){
                coreContext->license->editorExpirationDate = detail->editorExpirationDate;
                time_t t = time(NULL);
                if (coreContext->license->editorExpirationDate >= t){
                    coreContext->license->isEditorLicenseValid = true;
                }
            }
        }
        detail = zlist_next(coreContext->license->licenseDetails);
    }
    
    ///////////////////////////////////////////////////////////
    // SET LICENSE DEFAULTS WHERE NEEDED
    if (coreContext->license == NULL){
        igs_license("no license found in %s : switching to demonstration mode", coreContext->licensePath);
        coreContext->license = calloc(1, sizeof(igs_license_t));
        coreContext->license->features = zhash_new();
        coreContext->license->agents = zhash_new();
    }
    //set license parameters to default for uninitialized values
    if (coreContext->license->id == NULL)
        coreContext->license->id = strdup("Unregistered");
    if (coreContext->license->customer == NULL)
        coreContext->license->customer = strdup("Unregistered");
    if (coreContext->license->order == NULL)
        coreContext->license->order = strdup("none");
    if (coreContext->license->licenseExpirationDate == 0){
        coreContext->license->licenseExpirationDate = -1;
    }
    if (coreContext->license->platformNbAgents == 0)
        coreContext->license->platformNbAgents = MAX_NB_OF_AGENTS;
    if (coreContext->license->platformNbIOPs == 0)
        coreContext->license->platformNbIOPs = MAX_NB_OF_IOP;
    if (coreContext->license->editorOwner == NULL)
        coreContext->license->editorOwner = strdup("Unregistered");
    if (coreContext->license->editorExpirationDate == 0){
        coreContext->license->editorExpirationDate = -1;
    }
    license_readWriteUnlock();
}


////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////
void igs_setLicensePath(const char *path){
    char reviewedPath[4096] = "";
    admin_makeFilePath(path, reviewedPath, 4096);
    if (zsys_file_exists(reviewedPath)){
        if (coreContext->licensePath != NULL){
            free(coreContext->licensePath);
        }
        coreContext->licensePath = strdup(reviewedPath);
    }else{
        igs_license("%s does not exist", reviewedPath);
    }
}
char *igs_getLicensePath(){
    return strdup(coreContext->licensePath);
}

bool igs_checkLicense(const char *agentId){
    license_readLicense();
    if (agentId == NULL)
        return false;
    igs_license_for_agent_t *l = zhash_first(coreContext->license->agents);
    while (l != NULL) {
        if (l->agentId != NULL
            && strcmp(agentId, l->agentId) == 0){
            return true;
        }
        l = zhash_next(coreContext->license->agents);
    }
    return false;
}

int igs_observeLicense(igs_licenseCallback cb, void *myData){
    igs_license_callback_t *l = (igs_license_callback_t *)calloc(1, sizeof(igs_license_callback_t));
    l->callback_ptr = cb;
    l->data = myData;
    DL_APPEND(coreContext->licenseCallbacks, l);
    return 1;
}

void igs_loadLicenseData(const void *data, size_t size){
    if (data == NULL){
        igs_license("license data cannot be NULL");
        return;
    }
    if (coreContext->licenseData != NULL)
        free(coreContext->licenseData);
    coreContext->licenseData = calloc(1, size);
    memcpy(coreContext->licenseData, data, size);
    coreContext->licenseDataSize = size;
}
