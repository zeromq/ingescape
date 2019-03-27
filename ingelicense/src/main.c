//
//  main.c
//  ingelicense
//
//  Created by Stephane Vales on 25/03/2019.
//  Copyright © 2019 Ingescape. All rights reserved.
//

//Check here for sodium:
//https://libsodium.gitbook.io/doc/secret-key_cryptography/secretstream

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define NOMINMAX
#include <windows.h>
#include <winsock2.h>
#endif

#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <czmq.h>
#include <sodium.h>

#define BUFFER 1024
#define CHUNK_SIZE 4096

#define DEFAULT_CUSTOMER "Ingescape"
#define DEFAULT_ORDER "for internal use only"
#define DEFAULT_EXPIRATION 2524608000 //January 1st 2050
#define DEFAULT_AGENTS_NB 500
#define DEFAULT_IOP_NB 5000
#define DEFAULT_EDITOR_OWNER "Ingescape"
#define DEFAULT_EDITOR_EXPIRATION 2524608000 //January 1st 2050
#define DEFAULT_FEATURES_FILE "features.txt"
#define DEFAULT_AGENTS_FILE "agents.txt"
#define DEFAULT_OUTPUT_FILE "new.igslicense"

uint8_t secretKey[crypto_secretstream_xchacha20poly1305_KEYBYTES] = {1,255,34,41,58,63,47,183,134,223,33,41,25,16,87,38,211,27,183,124,185,196,107,128,34,92,83,54,35,60,37,28};

char customer[BUFFER] = DEFAULT_CUSTOMER;
char order[BUFFER] = DEFAULT_ORDER;
char editorOwner[BUFFER] = DEFAULT_EDITOR_OWNER;
char features[BUFFER] = DEFAULT_FEATURES_FILE;
char agents[BUFFER] = DEFAULT_AGENTS_FILE;
char output[BUFFER*8] = DEFAULT_OUTPUT_FILE;
long expiration = DEFAULT_EXPIRATION;
int maxAgents = DEFAULT_AGENTS_NB;
int maxIOPs = DEFAULT_IOP_NB;
long editorExpiration = DEFAULT_EDITOR_EXPIRATION;

void encryptLicenseToFile(const char *target_file, const char *source_string,
                         const unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]){
    unsigned char  buf_in[CHUNK_SIZE];
    unsigned char  buf_out[CHUNK_SIZE + crypto_secretstream_xchacha20poly1305_ABYTES];
    unsigned char  header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
    crypto_secretstream_xchacha20poly1305_state st;
    FILE          *fp_t;
    unsigned long long out_len;
    unsigned char  tag;
    
    fp_t = fopen(target_file, "wb");
    crypto_secretstream_xchacha20poly1305_init_push(&st, header, key);
    fwrite(header, 1, sizeof header, fp_t);
    
    const char *source_index = source_string;
    while (source_index < source_string + strlen(source_string)){
        size_t sizeToCopy = MIN(CHUNK_SIZE, strlen(source_index));
        memcpy(buf_in, source_index, sizeToCopy);
        source_index += sizeToCopy;
        tag = (*source_index == '\0') ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;
        crypto_secretstream_xchacha20poly1305_push(&st, buf_out, &out_len, buf_in, sizeToCopy,
                                                   NULL, 0, tag);
        fwrite(buf_out, 1, (size_t) out_len, fp_t);
    }
    fclose(fp_t);
}

int decryptLicenseFromFile(char **target_string, const char *source_file,
                           const unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES]){
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
    
    fp_s = fopen(source_file, "rb");
    if (fp_s == NULL){
        printf("%s file not found\n", source_file);
        goto ret;
    }
    fread(header, 1, sizeof header, fp_s);
    if (crypto_secretstream_xchacha20poly1305_init_pull(&st, header, key) != 0) {
        goto ret; /* incomplete header */
    }
    char *target_index = NULL;
    do {
        rlen = fread(buf_in, 1, sizeof buf_in, fp_s);
        eof = feof(fp_s);
        if (crypto_secretstream_xchacha20poly1305_pull(&st, buf_out, &out_len, &tag,
                                                       buf_in, rlen, NULL, 0) != 0) {
            goto ret; /* corrupted chunk */
        }
        if (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && ! eof) {
            goto ret; /* premature end (end of file reached before the end of the stream) */
        }
        if (targetSize == 0){
            targetSize = out_len + 1; //+1 to reserve space for final \0
            *target_string = calloc(1, targetSize);
            target_index = *target_string;
        }else{
            targetSize += out_len;
            *target_string = realloc(*target_string, targetSize);
        }
        memcpy(target_index, buf_out, out_len);
        target_index += out_len;
    } while (! eof);
    ret = 0;
ret:
    fclose(fp_s);
    return ret;
}


void printLicenseLine(const char *line){
    char command[256] = "";
    char data[256] = "";
    if (sscanf(line, ":%s %[\001-\377]", command, data) == 2){
        if (strcmp(command, "customer") == 0){
            printf("customer: %s\n", data);
        }else if (strcmp(command, "order") == 0){
            printf("order: %s\n", data);
        }else if (strcmp(command, "expiration") == 0){
            long licenseExpirationDate = atol(data);
            struct tm  ts = *localtime(&licenseExpirationDate);
            char       buf[80];
            strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
            printf("license expriation date : %s\n", buf);
        }else if (strcmp(command, "platform") == 0){
            int platformNbAgents = 0;
            int platformNbIOPs = 0;
            sscanf(data, "%d %d", &platformNbAgents, &platformNbIOPs);
            printf("platform limited to %d agents and %d iops\n", platformNbAgents, platformNbIOPs);
        }else if (strcmp(command, "editorOwner") == 0){
            printf("editor owner: %s\n", data);
        }else if (strcmp(command, "editorExpiration") == 0){
            long editorExpirationDate = atol(data);
            struct tm  ts = *localtime(&editorExpirationDate);
            char       buf[80];
            strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
            printf("license expriation date : %s\n", buf);
        }else if (strcmp(command, "feature") == 0){
            printf("allowed feature: %s\n", data);
        }else if (strcmp(command, "agent") == 0){
            char agentId[256] = "";
            char agentName[256] = "";
            sscanf(data, "%255s %255s", agentId, agentName);
            printf("allowed agent : %s with id %s\n", agentName, agentId);
        }
    }else{
        printf("%s\n", line);
    }
    
}

void readLicense(char *license){
    char * curLine = license;
    while(curLine){
        char * nextLine = strchr(curLine, '\n');
        if (nextLine) *nextLine = '\0';  // temporarily terminate the current line
        printLicenseLine(curLine);
        if (nextLine) *nextLine = '\n';  // then restore newline-char, just to be tidy
        curLine = nextLine ? (nextLine+1) : NULL;
    }
}

char* generateLicense(void){
    char line[BUFFER] = "";
    snprintf(line, BUFFER, ":customer %s\n", customer);
    char *license = calloc(1, strlen(line)+1);
    memcpy(license, line, strlen(line)+1);
    
    snprintf(line, BUFFER, ":order %s\n", order);
    license = realloc(license, strlen(license) + strlen(line) + 1);
    strcat(license, line);
    
    snprintf(line, BUFFER, ":expiration %ld\n", expiration);
    license = realloc(license, strlen(license) + strlen(line) + 1);
    strcat(license, line);
    
    snprintf(line, BUFFER, ":platform %d %d\n", maxAgents, maxIOPs);
    license = realloc(license, strlen(license) + strlen(line) + 1);
    strcat(license, line);
    
    snprintf(line, BUFFER, ":editorOwner %s\n", editorOwner);
    license = realloc(license, strlen(license) + strlen(line) + 1);
    strcat(license, line);
    
    snprintf(line, BUFFER, ":editorExpiration %ld\n", editorExpiration);
    license = realloc(license, strlen(license) + strlen(line) + 1);
    strcat(license, line);
    
    //manage features and agents
    if (zsys_file_exists(features)){
        zfile_t *f = zfile_new(NULL, features);
        if (f != NULL
            && !zfile_is_directory(f)
            && zfile_is_readable(f)
            && zfile_input(f) == 0) {
            const char *l;
            while ((l = zfile_readln(f))){
                char word[BUFFER] = "";
                if (sscanf(l, "%s", word) == 1 && strlen(word) > 0){
                    snprintf(line, BUFFER, ":feature %s\n", word);
                    license = realloc(license, strlen(license) + strlen(line) + 1);
                    strcat(license, line);
                }
            }
            zfile_close(f);
            zfile_destroy(&f);
        }
    }else{
        printf("file %s does not exist : features won't be added\n", features);
    }
    
    if (zsys_file_exists(agents)){
        zfile_t *f = zfile_new(NULL, agents);
        if (f != NULL
            && !zfile_is_directory(f)
            && zfile_is_readable(f)
            && zfile_input(f) == 0) {
            const char *l;
            while ((l = zfile_readln(f))){
                char uuid[BUFFER] = "";
                char name[BUFFER] = "";
                if (sscanf(l, "%s %s", uuid, name) == 2){
                    snprintf(line, BUFFER, ":agent %s %s\n", uuid, name);
                    license = realloc(license, strlen(license) + strlen(line) + 1);
                    strcat(license, line);
                }
            }
            zfile_close(f);
            zfile_destroy(&f);
        }
    }else{
        printf("file %s does not exist : agents won't be added\n", features);
    }
    
    printf("generated license file:\n%s\n", license);
    return license;
}

///////////////////////////////////////////////////////////////////////////////
// COMMAND LINE AND INTERPRETER OPTIONS
//
void print_usage() {
    printf("Usage example: ingelicense \n");
    printf("\nthese parameters have default value (indicated here above):\n");
    printf("--customer : (default: %s)\n", DEFAULT_CUSTOMER);
    printf("--order : (default: %s)\n", DEFAULT_ORDER);
    printf("--expiration : (default: %ld)\n", DEFAULT_EXPIRATION);
    printf("--maxAgents : (default: %d)\n", DEFAULT_AGENTS_NB);
    printf("--maxIOPs : (default: %d)\n", DEFAULT_IOP_NB);
    printf("--editorOwner : (default: %s)\n", DEFAULT_EDITOR_OWNER);
    printf("--editorExpiration : (default: %ld)\n", DEFAULT_EDITOR_EXPIRATION);
    printf("--features : (default: %s)\n", DEFAULT_FEATURES_FILE);
    printf("--agents : (default: %s)\n", DEFAULT_AGENTS_FILE);
    printf("--output : (default: %s)\n", DEFAULT_OUTPUT_FILE);
    printf("\n");
    printf("--read : read an existing license file\n");
}

//helper to convert paths starting with ~ to absolute paths
void makeFilePath(char *from, char *to, size_t size_of_to) {
    if (from[0] == '~') {
        from++;
#ifdef _WIN32
        char *home = getenv("HOMEPATH");
#else
        char *home = getenv("HOME");
#endif
        if (home == NULL){
            printf("could not find path for home directory\n");
        }else{
            strncpy(to, home, size_of_to);
            strncat(to, from, size_of_to);
        }
    }
    else {
        strncpy(to, from, size_of_to);
    }
}


int main(int argc, const char * argv[]) {
    //manage options
    int opt = 0;
//    char *readFile = NULL;
    char *readFile = NULL;
    
    static struct option long_options[] = {
        {"help",     no_argument, 0,  'h' },
        {"customer",     required_argument, 0,  'c' },
        {"order",     required_argument, 0,  'r' },
        {"expiration",     required_argument, 0,  'e' },
        {"maxAgents",     required_argument, 0,  't' },
        {"maxIOPs",     required_argument, 0,  'i' },
        {"editorOwner",     required_argument, 0,  'w' },
        {"editorExpiration",     required_argument, 0,  'x' },
        {"features",     required_argument, 0,  'f' },
        {"agents",     required_argument, 0,  'a' },
        {"output",     required_argument, 0,  'o' },
        {"read",     required_argument, 0,  'l' },
        {0, 0, 0, 0}
    };
    
    int long_index = 0;
    while ((opt = getopt_long(argc, (char *const *)argv, "p", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'c':
                strncpy(customer, optarg, BUFFER - 1);
            case 'r':
                strncpy(order, optarg, BUFFER - 1);
            case 'w':
                strncpy(editorOwner, optarg, BUFFER - 1);
            case 'f':
                strncpy(features, optarg, BUFFER - 1);
            case 'a':
                strncpy(agents, optarg, BUFFER - 1);
            case 'o':
                strncpy(output, optarg, BUFFER - 1);
            case 'e':
                expiration = atol(optarg);
            case 'x':
                editorExpiration = atol(optarg);
            case 't':
                maxAgents = atoi(optarg);
            case 'i':
                maxIOPs = atoi(optarg);
            case 'l':
                readFile = optarg;
            case 'h':
                print_usage();
                exit(0);
            default:
                print_usage();
                exit(1);
        }
    }
    
    if (sodium_init() != 0) {
        return 1;
    }
    
    if (readFile != NULL){
        char *license = NULL;
        decryptLicenseFromFile(&license, readFile, secretKey);
        printf("raw license file:\n%s\n\n", license);
        readLicense(license);
    }else{
        char *license = generateLicense();
        encryptLicenseToFile(output, license, secretKey);
    }
    
    return 0;
}
