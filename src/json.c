//
//  json.c
//  ingescape
//
//  Created by Stephane Vales on 08/05/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include "ingescape_advanced.h"
#include "yajl_parse.h"
#include "yajl_gen.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
// generate a JSON string
void igs_JSONerror(igsyajl_gen_status status, const char *function, void *value){
    IGS_UNUSED(value)
    const char *output = NULL;
    switch (status) {
        case igsyajl_gen_keys_must_be_strings:
            output = "a JSON key must be a string";
            break;
        case igsyajl_max_depth_exceeded:
            output = "maximum generation depth was exceeded";
            break;
        case igsyajl_gen_in_error_state:
            output = "impossible to add new elements while in error state (correct error first)";
            break;
        case igsyajl_gen_generation_complete:
            output = "a complete JSON document has been generated";
            break;
        case igsyajl_gen_invalid_number:
            output = "an invalid floating point value has been passed";
            break;
        case igsyajl_gen_no_buf:
            output = "a print callback was passed in, so there is no internal buffer to get from";
            break;
        case igsyajl_gen_invalid_string:
            output = "an invalid string was passed";
            break;
            
        default:
            break;
    }
    igs_error("%s - %s", function, output);
}

void igs_JSONfree(igsJSON_t *json){
    igsyajl_gen *g = (igsyajl_gen *) json;
    igsyajl_gen_free (*g);
    *json = NULL;
}

igsJSON_t igs_JSONinit(void){
    igsyajl_gen g = igsyajl_gen_alloc(NULL);
    igsyajl_gen_config(g, igsyajl_gen_beautify, 1);
    igsyajl_gen_config(g, igsyajl_gen_validate_utf8, 1);
    return (igsJSON_t) g;
}
void igs_JSONopenMap(igsJSON_t json){
    igsyajl_gen g = (igsyajl_gen) json;
    igsyajl_gen_status status = igsyajl_gen_map_open(g);
    if (status != igsyajl_gen_status_ok){
        igs_JSONerror(status, __func__, NULL);
    }
}
void igs_JSONcloseMap(igsJSON_t json){
    igsyajl_gen g = (igsyajl_gen) json;
    igsyajl_gen_status status = igsyajl_gen_map_close(g);
    if (status != igsyajl_gen_status_ok){
        igs_JSONerror(status, __func__, NULL);
    }
}
void igs_JSONopenArray(igsJSON_t json){
    igsyajl_gen g = (igsyajl_gen) json;
    igsyajl_gen_status status = igsyajl_gen_array_open(g);
    if (status != igsyajl_gen_status_ok){
        igs_JSONerror(status, __func__, NULL);
    }
}
void igs_JSONcloseArray(igsJSON_t json){
    igsyajl_gen g = (igsyajl_gen) json;
    igsyajl_gen_status status = igsyajl_gen_array_close(g);
    if (status != igsyajl_gen_status_ok){
        igs_JSONerror(status, __func__, NULL);
    }
}
void igs_JSONaddNULL(igsJSON_t json){
    igsyajl_gen g = (igsyajl_gen) json;
    igsyajl_gen_status status = igsyajl_gen_null(g);
    if (status != igsyajl_gen_status_ok){
        igs_JSONerror(status, __func__, NULL);
    }
}
void igs_JSONaddBool(igsJSON_t json, bool value){
    igsyajl_gen g = (igsyajl_gen) json;
    igsyajl_gen_status status = igsyajl_gen_bool(g, value);
    if (status != igsyajl_gen_status_ok){
        igs_JSONerror(status, __func__, &value);
    }
}
void igs_JSONaddInt(igsJSON_t json, long long value){
    igsyajl_gen g = (igsyajl_gen) json;
    igsyajl_gen_status status = igsyajl_gen_integer(g, value);
    if (status != igsyajl_gen_status_ok){
        igs_JSONerror(status, __func__, &value);
    }
}
void igs_JSONaddDouble(igsJSON_t json, double value){
    igsyajl_gen g = (igsyajl_gen) json;
    igsyajl_gen_status status = igsyajl_gen_double(g, value);
    if (status != igsyajl_gen_status_ok){
        igs_JSONerror(status, __func__, &value);
    }
}
void igs_JSONaddString(igsJSON_t json, const char *value){
    igsyajl_gen g = (igsyajl_gen) json;
    igsyajl_gen_status status = igsyajl_gen_string(g, (unsigned const char *)value, strlen(value));
    if (status != igsyajl_gen_status_ok){
        igs_JSONerror(status, __func__, &value);
    }
}
void igs_JSONprint(igsJSON_t json){
    igsyajl_gen g = (igsyajl_gen) json;
    const unsigned char *buffer = NULL;
    size_t bufferSize = 0;
    igsyajl_gen_status status = igsyajl_gen_get_buf(g, &buffer, &bufferSize);
    if (status != igsyajl_gen_status_ok){
        igs_JSONerror(status, __func__, NULL);
    }else{
        printf("%s\n", buffer);
    }
}
char* igs_JSONdump(igsJSON_t json){
    igsyajl_gen g = (igsyajl_gen) json;
    const unsigned char *buffer = NULL;
    size_t bufferSize = 0;
    igsyajl_gen_status status = igsyajl_gen_get_buf(g, &buffer, &bufferSize);
    if (status != igsyajl_gen_status_ok){
        igs_JSONerror(status, __func__, NULL);
        return NULL;
    }else{
        return strdup((char *)buffer);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
// parse a JSON string or file
typedef struct json_parsingElements {
    igsyajl_handle handle;
    void *myData;
    igs_JSONCallback cb;
    unsigned char buffer[65536];
} json_parsingElements_t;

static int json_null(void * ctx){
    json_parsingElements_t *e = (json_parsingElements_t *) ctx;
    e->cb(IGS_JSON_NULL, NULL, 0, e->myData);
    return 1;
}

static int json_boolean(void * ctx, int boolean){
    json_parsingElements_t *e = (json_parsingElements_t *) ctx;
    e->cb(IGS_JSON_BOOL, &boolean, sizeof(int), e->myData);
    return 1;
}

static int json_number(void * ctx, const char * stringVal,
                       size_t stringLen){
    json_parsingElements_t *e = (json_parsingElements_t *) ctx;
    char *tmp = calloc(1, stringLen+1);
    strncpy(tmp, (char *)stringVal, stringLen);
    e->cb(IGS_JSON_NUMBER, tmp, stringLen, e->myData);
    free(tmp);
    return 1;
}

static int json_string(void * ctx, const unsigned char * stringVal,
                       size_t stringLen){
    json_parsingElements_t *e = (json_parsingElements_t *) ctx;
    char *tmp = calloc(1, stringLen+1);
    strncpy(tmp, (char *)stringVal, stringLen);
    e->cb(IGS_JSON_STRING, tmp, stringLen, e->myData);
    free(tmp);
    return 1;
}

static int json_map_key(void * ctx, const unsigned char * stringVal,
                        size_t stringLen){
    json_parsingElements_t *e = (json_parsingElements_t *) ctx;
    char *tmp = calloc(1, stringLen+1);
    strncpy(tmp, (char *)stringVal, stringLen);
    e->cb(IGS_JSON_KEY, tmp, stringLen, e->myData);
    free(tmp);
    return 1;
}

static int json_start_map(void * ctx){
    json_parsingElements_t *e = (json_parsingElements_t *) ctx;
    e->cb(IGS_JSON_MAP_START, NULL, 0, e->myData);
    return 1;
}

static int json_end_map(void * ctx){
    json_parsingElements_t *e = (json_parsingElements_t *) ctx;
    e->cb(IGS_JSON_MAP_END, NULL, 0, e->myData);
    return 1;
}

static int json_start_array(void * ctx){
    json_parsingElements_t *e = (json_parsingElements_t *) ctx;
    e->cb(IGS_JSON_ARRAY_START, NULL, 0, e->myData);
    return 1;
}

static int json_end_array(void * ctx){
    json_parsingElements_t *e = (json_parsingElements_t *) ctx;
    e->cb(IGS_JSON_ARRAY_END, NULL, 0, e->myData);
    return 1;
}

static igsyajl_callbacks json_callbacks = {
    json_null,
    json_boolean,
    NULL,
    NULL,
    json_number,
    json_string,
    json_start_map,
    json_map_key,
    json_end_map,
    json_start_array,
    json_end_array
};

void json_freeParsingElements(json_parsingElements_t **e){
    if (*e != NULL){
        if ((*e)->handle != NULL){
            igsyajl_free((*e)->handle);
        }
        free(*e);
        *e = NULL;
    }
}

void igs_JSONparseFromFile(const char *path, igs_JSONCallback cb, void *myData){
    FILE *fp = NULL;
    
    if (cb == NULL){
        igs_error("callback cannot be NULL");
        return;
    }
    if (path != NULL)
        fp = fopen(path,"r");
    if (fp == NULL){
        igs_error("could not open %s", path);
        return;
    }
    
    json_parsingElements_t *elements = calloc(1, sizeof(json_parsingElements_t));
    igsyajl_handle handle = igsyajl_alloc(&json_callbacks, NULL, elements);
    igsyajl_config(handle, igsyajl_allow_trailing_garbage, 1);
    elements->handle = handle;
    elements->myData = myData;
    elements->cb = cb;
    
    igsyajl_status status;
    while (1) {
        elements->buffer[0] = 0;
        fgets((char *)elements->buffer, sizeof(elements->buffer) - 1, fp);
        if (!feof(fp) && strlen((char *)elements->buffer) == 0){
            igs_error("could not read %s", path);
            break;
        }
        //printf("reading: %s\n", elements->buffer);
        status = igsyajl_parse(handle, elements->buffer, strlen((char *)(elements->buffer)));
        if (status != igsyajl_status_ok || feof(fp))
            break;
    }
    status = igsyajl_complete_parse(handle);
    
    if (status != igsyajl_status_ok) {
        unsigned char * str = igsyajl_get_error(handle, 1, elements->buffer, strlen((char *)elements->buffer));
        igs_error("%s", str);
        igsyajl_free_error(handle, str);
    }
    
    fclose(fp);
    json_freeParsingElements(&elements);
}

void igs_JSONparseFromString(const char *content, igs_JSONCallback cb, void *myData){
    if (cb == NULL){
        igs_error("callback cannot be NULL");
        return;
    }
    if (content == NULL){
        igs_error("content is NULL, nothing to parse");
        return;
    }
    
    json_parsingElements_t *elements = calloc(1, sizeof(json_parsingElements_t));
    igsyajl_handle handle = igsyajl_alloc(&json_callbacks, NULL, elements);
    igsyajl_config(handle, igsyajl_allow_trailing_garbage, 1);
    elements->handle = handle;
    elements->myData = myData;
    elements->cb = cb;
    
    igsyajl_status status;
    igsyajl_parse(handle, (const unsigned char *)content, strlen(content));
    status = igsyajl_complete_parse(handle);
    
    if (status != igsyajl_status_ok) {
        unsigned char * str = igsyajl_get_error(handle, 1, elements->buffer, strlen((char *)elements->buffer));
        igs_error("%s", str);
        igsyajl_free_error(handle, str);
    }
    json_freeParsingElements(&elements);
}

// TEST SCRIPT
// to be copied and compiled as a main.c file
//
//#include <stdio.h>
//#include <ingescape/ingescape.h>
//#include <ingescape/ingescape_advanced.h>
//
//int indent = 0;
//void json_testParsingCallback(igs_JSONValueType_t type, void *value, size_t size, void *myData){
//    char *str = NULL;
//    switch (type) {
//        case IGS_JSON_NULL:
//            printf("null\n");
//            break;
//        case IGS_JSON_BOOL:
//            printf("%s\n", (*(bool*)value)?"true":"false");
//            break;
//        case IGS_JSON_NUMBER:
//            printf("%s\n", (char *)value);
//            break;
//        case IGS_JSON_STRING:
//            str = (char *)value;
//            printf("%s\n", (char *)value);
//            break;
//        case IGS_JSON_KEY:
//            for (int i = 0; i < indent; i++){
//                printf("  ");
//            }
//            str = (char *)value;
//            printf("K:%s : ", (char *)value);
//            break;
//        case IGS_JSON_MAP_START:
//            printf("\n");
//            for (int i = 0; i < indent; i++){
//                printf("  ");
//            }
//            printf("{\n");
//            indent++;
//            break;
//        case IGS_JSON_MAP_END:
//            indent--;
//            for (int i = 0; i < indent; i++){
//                printf("  ");
//            }
//            printf("}\n");
//            break;
//        case IGS_JSON_ARRAY_START:
//            printf("\n");
//            for (int i = 0; i < indent; i++){
//                printf("  ");
//            }
//            printf("[\n");
//            indent++;
//            break;
//        case IGS_JSON_ARRAY_END:
//            indent--;
//            for (int i = 0; i < indent; i++){
//                printf("  ");
//            }
//            printf("]\n");
//            break;
//            
//        default:
//            break;
//    }
//}
//int main(int argc, const char * argv[]) {
//    igsJSON_t json = igs_JSONinit();
//    igs_JSONopenMap(json);
//    {
//        igs_JSONaddInt(json, -15.15); igs_JSONaddInt(json, -15.15);
//        igs_JSONaddString(json, "null"); igs_JSONaddNULL(json);
//        igs_JSONaddString(json, "bool"); igs_JSONaddBool(json, -1);
//        igs_JSONaddString(json, "int"); igs_JSONaddInt(json, -15.15);
//        igs_JSONaddString(json, "double"); igs_JSONaddDouble(json, 002.500);
//        igs_JSONaddString(json, "UTF8"); igs_JSONaddString(json, "˻");
//        igs_JSONaddString(json, "definition");
//        igs_JSONopenMap(json);
//        {
//            igs_JSONaddString(json, "name"); igs_JSONaddString(json, "igsDDS");
//            igs_JSONaddString(json, "description"); igs_JSONaddString(json, "dynamic definition for DDS storage agent");
//            igs_JSONaddString(json, "version"); igs_JSONaddString(json, "dynamic");
//            igs_JSONaddString(json, "parameters"); igs_JSONopenArray(json);{
//            }igs_JSONcloseArray(json);
//            igs_JSONaddString(json, "inputs"); igs_JSONopenArray(json);{
//                igs_JSONopenMap(json);
//                {
//                    igs_JSONaddString(json, "name"); igs_JSONaddString(json, "myVariable");
//                    igs_JSONaddString(json, "type"); igs_JSONaddString(json, "INTEGER");
//                    igs_JSONaddString(json, "value"); igs_JSONaddString(json, "");
//                }
//                igs_JSONcloseMap(json);
//            }igs_JSONcloseArray(json);
//            igs_JSONaddString(json, "outputs"); igs_JSONopenArray(json);{
//            }igs_JSONcloseArray(json);
//            igs_JSONaddString(json, "tokens"); igs_JSONopenArray(json);{
//                igs_JSONopenMap(json);
//                {
//                    igs_JSONaddString(json, "name"); igs_JSONaddString(json, "addVariable");
//                    igs_JSONaddString(json, "arguments"); igs_JSONopenArray(json);{
//                        igs_JSONopenMap(json);
//                        {
//                            igs_JSONaddString(json, "name"); igs_JSONaddString(json, "name");
//                            igs_JSONaddString(json, "type"); igs_JSONaddString(json, "STRING");
//                        }
//                        igs_JSONcloseMap(json);
//                        igs_JSONopenMap(json);
//                        {
//                            igs_JSONaddString(json, "name"); igs_JSONaddString(json, "type");
//                            igs_JSONaddString(json, "type"); igs_JSONaddString(json, "STRING");
//                        }
//                        igs_JSONcloseMap(json);
//                    }igs_JSONcloseArray(json);
//                }
//                igs_JSONcloseMap(json);
//                igs_JSONopenMap(json);
//                {
//                    igs_JSONaddString(json, "name"); igs_JSONaddString(json, "removeVariable");
//                    igs_JSONaddString(json, "arguments"); igs_JSONopenArray(json);{
//                        igs_JSONopenMap(json);
//                        {
//                            igs_JSONaddString(json, "name"); igs_JSONaddString(json, "name");
//                            igs_JSONaddString(json, "type"); igs_JSONaddString(json, "STRING");
//                        }
//                        igs_JSONcloseMap(json);
//                    }igs_JSONcloseArray(json);
//                }
//                igs_JSONcloseMap(json);
//            }igs_JSONcloseArray(json);
//        }
//        igs_JSONcloseMap(json);
//    }
//    igs_JSONcloseMap(json);
//    igs_JSONaddString(json, "bool"); igs_JSONaddBool(json, -1);
//    igs_JSONprint(json);
//    printf("\n\n***************\n\n");
//
//    char *res = igs_JSONdump(json);
//    igs_JSONfree(&json);
//    printf("%s\n\n***************\n\n", res);
//    igs_JSONparseFromString(res, json_testParsingCallback, "plop");
//    free(res);
//
//
//    printf("\n\n***************\n\n");
//
//    igs_JSONparseFromFile("/Users/steph/Documents/IngeScape/agents/igsDDS_definition.json", json_testParsingCallback, "plop");
//
//    return 0;
//}
