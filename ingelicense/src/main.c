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
#define CHUNK_SIZE 4096 //must be at least 256 for encryption to work

#define DEFAULT_ID "IngescapeInternalID"
#define DEFAULT_CUSTOMER "Ingescape"
#define DEFAULT_ORDER "for internal use only"
#define DEFAULT_AGENTS_NB 500
#define DEFAULT_IOP_NB 5000
#define DEFAULT_EDITOR_OWNER "Ingescape"
#define DEFAULT_FEATURES_FILE "features.txt"
#define DEFAULT_AGENTS_FILE "agents.txt"
#define DEFAULT_OUTPUT_FILE "new.igslicense"

uint8_t secretEncryptionKey[crypto_secretstream_xchacha20poly1305_KEYBYTES] = {1,255,34,41,58,63,47,183,134,223,33,41,25,16,87,38,211,27,183,124,185,196,107,128,34,92,83,54,35,60,37,28};

//here is how to create and display new public and private keys:
//crypto_sign_keypair(publicSignKey, privateSignKey);
//printf("public key : ");
//for (int i = 0; i<32; i++){
//    printf("%d,", publicSignKey[i]);
//}
//printf("\n");
//printf("private key : ");
//for (int i = 0; i<64; i++){
//    printf("%d,", privateSignKey[i]);
//}
//printf("\n");
unsigned char publicSignKey[crypto_sign_PUBLICKEYBYTES] = {47,20,1,206,112,73,169,19,31,67,21,116,192,151,109,34,215,117,250,86,247,235,53,159,208,126,234,177,133,49,103,111};
unsigned char privateSignKey[crypto_sign_SECRETKEYBYTES] = {140,185,190,199,185,123,212,117,31,178,102,139,248,172,108,215,177,185,214,240,241,228,181,187,87,169,65,180,215,169,66,90,47,20,1,206,112,73,169,19,31,67,21,116,192,151,109,34,215,117,250,86,247,235,53,159,208,126,234,177,133,49,103,111};

char id[BUFFER] = DEFAULT_ID;
char customer[BUFFER] = DEFAULT_CUSTOMER;
char order[BUFFER] = DEFAULT_ORDER;
char editorOwner[BUFFER] = DEFAULT_EDITOR_OWNER;
char features[BUFFER] = DEFAULT_FEATURES_FILE;
char agents[BUFFER] = DEFAULT_AGENTS_FILE;
char output[BUFFER*8] = DEFAULT_OUTPUT_FILE;
int maxAgents = DEFAULT_AGENTS_NB;
int maxIOPs = DEFAULT_IOP_NB;
char expirationString[BUFFER] = "";
long expiration = 0;
char editorExpirationString[BUFFER] = "";
long editorExpiration = 0;

void encryptLicenseToFile(const char *target_file, const char *source_string,
                         const unsigned char secretEncryptionKey[crypto_secretstream_xchacha20poly1305_KEYBYTES]){
    //sign source string
//    printf("source string: %s***\n%lu bytes\n", source_string, strlen(source_string));
    unsigned char *signed_string = calloc(strlen(source_string) + 1 + crypto_sign_BYTES, sizeof(char));
    unsigned long long signed_string_length = 0;
    crypto_sign(signed_string, &signed_string_length, (const unsigned char*)source_string, strlen(source_string), privateSignKey);
//    printf("signed string (%lu -> %llu bytes):\n%s***\n", strlen(source_string), signed_string_length, signed_string);
    
    //encrypt signed string to file
    unsigned char  buf_in[CHUNK_SIZE];
    unsigned char  buf_out[CHUNK_SIZE + crypto_secretstream_xchacha20poly1305_ABYTES];
    unsigned char  header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
    crypto_secretstream_xchacha20poly1305_state st;
    FILE          *fp;
    unsigned long long out_len;
    unsigned char  tag;
    
    fp = fopen(target_file, "wb");
    crypto_secretstream_xchacha20poly1305_init_push(&st, header, secretEncryptionKey);
    fwrite(header, 1, sizeof header, fp);
    
    const unsigned char *source_index = signed_string;
    size_t remaining_string_size = signed_string_length;
    while (source_index < signed_string + signed_string_length){
        size_t sizeToCopy = MIN(CHUNK_SIZE, remaining_string_size);
        remaining_string_size -= sizeToCopy;
//        printf("encoding %zu bytes (remaining %zu bytes)\n", sizeToCopy, remaining_string_size);
        memcpy(buf_in, source_index, sizeToCopy);
        source_index += sizeToCopy;
        tag = (*source_index == '\0') ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;
        crypto_secretstream_xchacha20poly1305_push(&st, buf_out, &out_len, buf_in, sizeToCopy,
                                                   NULL, 0, tag);
        fwrite(buf_out, 1, (size_t) out_len, fp);
    }
    fclose(fp);
    
    free(signed_string);
}

int decryptLicenseFromFile(char **target_string, const char *source_file,
                           const unsigned char secretEncryptionKey[crypto_secretstream_xchacha20poly1305_KEYBYTES]){
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
    
    printf("opening %s\n", source_file);
    fp_s = fopen(source_file, "rb");
    if (fp_s == NULL){
        printf("%s file not found : aborting\n", source_file);
        return -1;
    }
    bool decryptionWentOK = true;
    fread(header, 1, sizeof header, fp_s);
    if (crypto_secretstream_xchacha20poly1305_init_pull(&st, header, secretEncryptionKey) != 0) {
        printf("incomplete header\n");
        decryptionWentOK = false;
        goto ret;
    }
    do {
        rlen = fread(buf_in, 1, sizeof buf_in, fp_s);
        eof = feof(fp_s);
        if (crypto_secretstream_xchacha20poly1305_pull(&st, buf_out, &out_len, &tag,
                                                       buf_in, rlen, NULL, 0) != 0) {
            printf("corrupted chunk in file\n");
            decryptionWentOK = false;
            goto ret;
        }
        if (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && ! eof) {
            printf("premature end (end of file reached before the end of the stream)\n");
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
//        printf("decrypting %llu bytes\n", out_len);
        target_index += out_len;
    } while (! eof);
    ret = 0;
ret:
    if (fp_s != NULL)
        fclose(fp_s);
    
    if (decryptionWentOK){
        //verify signed string
        //    printf("decrypted string is %zu bytes long:\n%s***\n", targetSize - 1, signed_target_string);
        *target_string = calloc(targetSize, sizeof(char));
        unsigned long long target_string_len = 0;
        if (crypto_sign_open((unsigned char*)(*target_string), &target_string_len,
                             (const unsigned char *)signed_target_string, targetSize - 1, publicSignKey) != 0) {
            printf("license signature is incorrect\n");
            ret = -1;
        }
    }
    
    if (signed_target_string != NULL)
        free(signed_target_string);
    return ret;
}


void printLicenseLine(const char *line){
    char command[256] = "";
    char data[256] = "";
    if (sscanf(line, ":%s %[\001-\377]", command, data) == 2){
        if (strcmp(command, "id") == 0){
            printf("id: %s\n", data);
        }else if (strcmp(command, "customer") == 0){
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
            printf("license expiration date : %s\n", buf);
        }else if (strcmp(command, "feature") == 0){
            printf("allowed feature: %s\n", data);
        }else if (strcmp(command, "agent") == 0){
            char agentId[256] = "";
            char agentName[256] = "";
            sscanf(data, "%255s %255s", agentId, agentName);
            printf("allowed agent : %s with id %s\n", agentName, agentId);
        }else{
            printf("unknown entry -> %s\n", line);
        }
    }else{
        printf("unformatted line -> %s\n", line);
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
    snprintf(line, BUFFER, ":id %s\n", id);
    char *license = calloc(1, strlen(line)+1);
    memcpy(license, line, strlen(line)+1);
    
    snprintf(line, BUFFER, ":customer %s\n", customer);
    license = realloc(license, strlen(license) + strlen(line) + 1);
    strcat(license, line);
    
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
    license[strlen(license)-1] = '\0'; //remove last return char
    printf("generated license file:\n%s\n", license);
    return license;
}

///////////////////////////////////////////////////////////////////////////////
// COMMAND LINE AND INTERPRETER OPTIONS
//
void print_usage() {
    printf("Usage example: ingelicense \n");
    printf("\nthese parameters have default value (indicated here above):\n");
    printf("--id : license id (default: %s)\n", DEFAULT_ID);
    printf("--customer : customer name (default: %s)\n", DEFAULT_CUSTOMER);
    printf("--order : order identification (default: %s)\n", DEFAULT_ORDER);
    printf("--expiration : license expiration date expressed as yyyy/mm/dd (default: 4 weeks from now)\n");
    printf("--maxAgents : maximum number of agents in the platform (default: %d)\n", DEFAULT_AGENTS_NB);
    printf("--maxIOPs : maximum number of IOPs for all the agents in the platform (default: %d)\n", DEFAULT_IOP_NB);
    printf("--editorOwner : name of the license owner (default: %s)\n", DEFAULT_EDITOR_OWNER);
    printf("--editorExpiration : editor license expiration date expressed as yyyy/mm/dd (default: 4 weeks from now)\n");
    printf("--features : path to file listing the allowed features (default: %s)\n", DEFAULT_FEATURES_FILE);
    printf("--agents : path to file listing the allowed agents (default: %s)\n", DEFAULT_AGENTS_FILE);
    printf("--output : path to the generated license file (default: %s)\n", DEFAULT_OUTPUT_FILE);
    printf("\n");
    printf("--read : path to license file to be read and displayed\n");
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
    char *readFile = NULL;
    
    static struct option long_options[] = {
        {"help",     no_argument, 0,  'h' },
        {"id",     required_argument, 0,  'd' },
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
            case 'd':
                strncpy(id, optarg, BUFFER - 1);
                break;
            case 'c':
                strncpy(customer, optarg, BUFFER - 1);
                break;
            case 'r':
                strncpy(order, optarg, BUFFER - 1);
                break;
            case 'w':
                strncpy(editorOwner, optarg, BUFFER - 1);
                break;
            case 'f':
                strncpy(features, optarg, BUFFER - 1);
                break;
            case 'a':
                strncpy(agents, optarg, BUFFER - 1);
                break;
            case 'o':
                strncpy(output, optarg, BUFFER - 1);
                break;
            case 'e':
                strncpy(expirationString, optarg, BUFFER - 1);
                break;
            case 'x':
                strncpy(editorExpirationString, optarg, BUFFER - 1);
                break;
            case 't':
                maxAgents = atoi(optarg);
                break;
            case 'i':
                maxIOPs = atoi(optarg);
                break;
            case 'l':
                readFile = optarg;
                break;
            case 'h':
                print_usage();
                printf("plop\n");
                exit(0);
            default:
                print_usage();
                printf("plip : %c\n", opt);
                exit(1);
        }
    }
    
    if (sodium_init() != 0) {
        printf("could not initiate encryption : aborting\n");
        return 1;
    }
    
    if (strlen(expirationString) > 0){
        int year = 0;
        int month = 0;
        int day = 0;
        int res = sscanf(expirationString, "%d/%d/%d", &year, &month, &day);
        if (res == 3){
            struct tm t;
            t.tm_year = year - 1900;
            t.tm_mon = month - 1; // Month, 0 - jan
            t.tm_mday = day;
            t.tm_hour = 23;
            t.tm_min = 59;
            t.tm_sec = 59;
            t.tm_isdst = -1; // Is DST on? 1 = yes, 0 = no, -1 = unknown
            expiration = mktime(&t);
        }
    }
    if (expiration == 0){
        time_t     now;
        time(&now);
        expiration = now + 4 * 7 * 24 * 60 * 60; //add 4 weeks to now
    }
    
    
    
    if (strlen(editorExpirationString) > 0){
        int year = 0;
        int month = 0;
        int day = 0;
        int res = sscanf(editorExpirationString, "%d/%d/%d", &year, &month, &day);
        if (res == 3){
            struct tm t;
            t.tm_year = year - 1900;
            t.tm_mon = month - 1; // Month, 0 - jan
            t.tm_mday = day;
            t.tm_hour = 23;
            t.tm_min = 59;
            t.tm_sec = 59;
            t.tm_isdst = -1; // Is DST on? 1 = yes, 0 = no, -1 = unknown
            editorExpiration = mktime(&t);
        }
    }
    if (editorExpiration == 0){
        time_t     now;
        time(&now);
        editorExpiration = now + 4 * 7 * 24 * 60 * 60; //add 4 weeks to now
    }
    
    if (readFile != NULL){
        char *license = NULL;
        decryptLicenseFromFile(&license, readFile, secretEncryptionKey);
//        printf("raw license file:\n%s\n\n", license);
        printf("License file contains:\n");
        readLicense(license);
    }else{
        char *license = generateLicense();
        encryptLicenseToFile(output, license, secretEncryptionKey);
    }
    
    return 0;
}
