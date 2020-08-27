//
//  json.c
//  ingescape
//
//  Created by Stephane Vales on 08/05/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ingescape_advanced.h"
#include "yajl_parse.h"
#include "yajl_gen.h"
#include "yajl_tree.h"

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
    if (value == NULL)
        value = "";
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
char* igs_JSONcompactDump(igsJSON_t json){
    igsJSON_t dup = igs_JSONinit();
    igsyajl_gen_config((igsyajl_gen) dup, igsyajl_gen_beautify, 0);
    igs_JSONaddTree(dup, igs_JSONgetTree(json));
    char* prettyDump = igs_JSONdump(dup);
    igs_JSONfree(&dup);
    return prettyDump;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
// parse a JSON string or file "sax style" with a callback
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
    e->cb(IGS_JSON_MAP, NULL, 0, e->myData);
    return 1;
}

static int json_end_map(void * ctx){
    json_parsingElements_t *e = (json_parsingElements_t *) ctx;
    e->cb(IGS_JSON_MAP_END, NULL, 0, e->myData);
    return 1;
}

static int json_start_array(void * ctx){
    json_parsingElements_t *e = (json_parsingElements_t *) ctx;
    e->cb(IGS_JSON_ARRAY, NULL, 0, e->myData);
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


////////////////////////////////////////////////////////////////////////////////////////////////////////
// parse a JSON string or file "DOM style" with a tree
void igs_JSONTreeFree(igsJSONTreeNode_t **node){
    if (node != NULL && *node != NULL){
        igsyajl_tree_free((igsyajl_val)*node);
        *node = NULL;
    }
}

igsJSONTreeNode_t* igs_JSONTreeParseFromFile(const char *path){
    zfile_t *file = zfile_new (NULL, path);
    if (file == NULL || !zfile_is_regular(file) || !zfile_is_readable(file) || zfile_input(file) != 0){
        if (!zfile_is_regular(file))
            igs_error("not a regular file : %s", path);
        if (!zfile_is_readable(file))
            igs_error("not readable file : %s", path);
        if (zfile_input(file) != 0)
            igs_error("could not open %s", path);
        return NULL;
    }
    char errbuf[1024] = "unknown error";
    zchunk_t *data = zfile_read (file, zfile_size(path), 0);
    igsJSONTreeNode_t* node = calloc(1, sizeof(igsJSONTreeNode_t));
    node = (igsJSONTreeNode_t*)igsyajl_tree_parse((const char *)zchunk_data(data), errbuf, sizeof(errbuf));
    if (node == NULL){
        igs_error("parsing error (%s) : %s", path, errbuf);
    }
    zchunk_destroy(&data);
    zfile_destroy(&file);
    return node;
}

igsJSONTreeNode_t* igs_JSONTreeParseFromString(const char *content){
    char errbuf[1024] = "unknown error";
    igsJSONTreeNode_t* node = calloc(1, sizeof(igsJSONTreeNode_t));
    node = (igsJSONTreeNode_t*)igsyajl_tree_parse(content, errbuf, sizeof(errbuf));
    if (node == NULL){
        igs_error("parsing error (%s) : %s", content, errbuf);
    }
    return node;
}

igsJSONTreeNode_t* igs_JSONTreeGetNodeAtPath(igsJSONTreeNode_t* node, const char **path){
    if (node == NULL){
        igs_warn("passed node is NULL");
        return NULL;
    }
    igsyajl_val v = igsyajl_tree_get((igsyajl_val)node, path, igsyajl_t_any);
    return (igsJSONTreeNode_t *)v;
}

bool igs_JSONTreeIsValueAnInteger(igsJSONTreeNode_t *value){
    return IGSYAJL_IS_INTEGER((igsyajl_val)value);
}

bool igs_JSONTreeIsValueADouble(igsJSONTreeNode_t *value){
    return IGSYAJL_IS_DOUBLE((igsyajl_val)value);
}

void igs_JSONTreedumpIterate(igsJSON_t json, igsJSONTreeNode_t *value){
    if (value == NULL)
        return;
    switch (value->type) {
        case IGS_JSON_STRING:
            igs_JSONaddString(json, value->u.string);
            break;
        case IGS_JSON_NUMBER:
            if (igs_JSONTreeIsValueAnInteger(value)){
                igs_JSONaddInt(json, value->u.number.i);
            }else{
                igs_JSONaddDouble(json, value->u.number.d);
            }
            break;
        case IGS_JSON_MAP: //igsyajl_t_object
            igs_JSONopenMap(json);
            for (size_t i = 0; i < value->u.object.len; i++){
                igs_JSONaddString(json, value->u.object.keys[i]);
                igs_JSONTreedumpIterate(json, value->u.object.values[i]);
            }
            igs_JSONcloseMap(json);
            break;
        case IGS_JSON_ARRAY: //igsyajl_t_array
            igs_JSONopenArray(json);
            for (size_t i = 0; i < value->u.array.len; i++){
                igs_JSONTreedumpIterate(json, value->u.array.values[i]);
            }
            igs_JSONcloseArray(json);
            break;
        case IGS_JSON_TRUE:
            igs_JSONaddBool(json, true);
            break;
        case IGS_JSON_FALSE:
            igs_JSONaddBool(json, false);
            break;
        case IGS_JSON_NULL:
            igs_JSONaddNULL(json);
            break;
        default:
            break;
    }
}

char* igs_JSONTreeDump(igsJSONTreeNode_t *value){
    igsJSON_t json = igs_JSONinit();
    igs_JSONTreedumpIterate(json, value);
    //igs_JSONprint(json);
    char *res = igs_JSONdump(json);
    igs_JSONfree(&json);
    return res;
}

igsJSONTreeNode_t* igs_JSONTreeClone(igsJSONTreeNode_t *root){
    if (root == NULL)
        return NULL;
    igsJSONTreeNode_t *result = calloc(1, sizeof(igsJSONTreeNode_t));
    result->type = root->type;
    
    switch (root->type) {
        case IGS_JSON_STRING:
            result->u.string = strdup(root->u.string);
            break;
        case IGS_JSON_NUMBER:
            if (igs_JSONTreeIsValueAnInteger(root)){
                result->u.number.i = root->u.number.i;
            }else{
                result->u.number.d = root->u.number.d;
            }
            if (root->u.number.r != NULL)
                result->u.number.r = strdup(root->u.number.r);
            result->u.number.flags = root->u.number.flags;
            break;
        case IGS_JSON_MAP: //igsyajl_t_object
            result->u.object.len = root->u.object.len;
            result->u.object.keys = calloc(root->u.object.len, sizeof(char*));
            result->u.object.values = calloc(root->u.object.len, sizeof(igsJSONTreeNode_t*));
            for (size_t i = 0; i < root->u.object.len; i++){
                result->u.object.keys[i] = strdup(root->u.object.keys[i]);
                result->u.object.values[i] = igs_JSONTreeClone(root->u.object.values[i]);
            }
            break;
        case IGS_JSON_ARRAY: //igsyajl_t_array
            result->u.array.len = root->u.array.len;
            result->u.array.values = calloc(root->u.array.len, sizeof(igsJSONTreeNode_t*));
            for (size_t i = 0; i < root->u.array.len; i++){
                result->u.array.values[i] = igs_JSONTreeClone(root->u.array.values[i]);
            }
            break;
        case IGS_JSON_TRUE:
            result->u.number.i = true;
            if (root->u.number.r != NULL)
                result->u.number.r = strdup(root->u.number.r);
            result->u.number.flags = root->u.number.flags;
            break;
        case IGS_JSON_FALSE:
            result->u.number.i = false;
            if (root->u.number.r != NULL)
                result->u.number.r = strdup(root->u.number.r);
            result->u.number.flags = root->u.number.flags;
            break;
        case IGS_JSON_NULL:
            break;
        default:
            break;
    }
    return result;
}

void igs_JSONaddTree(igsJSON_t json, igsJSONTreeNode_t *tree){
    if (tree == NULL || json == NULL)
        return;
    switch (tree->type) {
        case IGS_JSON_STRING:
            igs_JSONaddString(json, tree->u.string);
            break;
        case IGS_JSON_NUMBER:
            if (igs_JSONTreeIsValueAnInteger(tree)){
                igs_JSONaddInt(json, tree->u.number.i);
            }else{
                igs_JSONaddDouble(json, tree->u.number.d);
            }
            break;
        case IGS_JSON_MAP: //igsyajl_t_object
            igs_JSONopenMap(json);
            for (size_t i = 0; i < tree->u.object.len; i++){
                igs_JSONaddString(json, tree->u.object.keys[i]);
                igs_JSONaddTree(json, tree->u.object.values[i]);
            }
            igs_JSONcloseMap(json);
            break;
        case IGS_JSON_ARRAY: //igsyajl_t_array
            igs_JSONopenArray(json);
            for (size_t i = 0; i < tree->u.array.len; i++){
                igs_JSONaddTree(json, tree->u.array.values[i]);
            }
            igs_JSONcloseArray(json);
            break;
        case IGS_JSON_TRUE:
            igs_JSONaddBool(json, true);
            break;
        case IGS_JSON_FALSE:
            igs_JSONaddBool(json, false);
            break;
        case IGS_JSON_NULL:
            igs_JSONaddNULL(json);
            break;
        default:
            break;
    }
}

igsJSONTreeNode_t* igs_JSONgetTree(igsJSON_t json){
    char *dump = igs_JSONdump(json);
    igsJSONTreeNode_t *res = igs_JSONTreeParseFromString(dump);
    free(dump);
    return res;
}

void igs_JSONTreeInsertInArray(igsJSONTreeNode_t *array, igsJSONTreeNode_t *nodeToInsert){
    if (array == NULL || array->type != IGS_JSON_ARRAY){
        igs_error("target node must be an array");
        return;
    }
    if (nodeToInsert == NULL){
        igs_error("node to insert cannot be NULL");
        return;
    }
    
    size_t size = array->u.array.len;
    array->u.array.values = realloc(array->u.array.values, size + 1);
    assert(array->u.array.values);
    array->u.array.values[size] = igs_JSONTreeClone(nodeToInsert);
    array->u.array.len += 1;
}

void igs_JSONTreeInsertInMap(igsJSONTreeNode_t *map, const char *key, igsJSONTreeNode_t *nodeToInsert){
    if (map == NULL || map->type != IGS_JSON_MAP){
        igs_error("target node must be a map");
        return;
    }
    if (key == NULL){
        igs_error("key cannot be NULL");
        return;
    }
    if (nodeToInsert == NULL){
        igs_error("node to insert cannot be NULL");
        return;
    }
    size_t size = map->u.object.len;
    bool knownKey = false;
    size_t index = size;
    for (size_t i = 0; i < size; i++){
        if (streq(map->u.object.keys[i], key)){
            knownKey = true;
            index = i;
            break;
        }
    }
    if (knownKey){
        igs_JSONTreeFree(&(map->u.object.values[index]));
    }else{
        map->u.object.values = realloc(map->u.object.values, size + 1);
        map->u.object.keys = (const char **) realloc(map->u.object.keys, size + 1);
        assert(map->u.object.values);
        assert(map->u.object.keys);
        map->u.object.keys[index] = strdup(key);
        map->u.object.len += 1;
    }
    map->u.object.values[index] = igs_JSONTreeClone(nodeToInsert);
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
//        case IGS_JSON_MAP:
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
//        case IGS_JSON_ARRAY:
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
//            igs_JSONaddString(json, "calls"); igs_JSONopenArray(json);{
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
//    igsJSONTreeNode_t* node = igs_JSONTreeParseFromFile("/Users/steph/Documents/IngeScape/agents/philips.json");
//
//    //const char *path[] = {NULL}; //from the root of the node
//    //const char *path[] = {"lights", NULL};
//    //const char *path[] = {"lights", "1", NULL};
//    const char *path[] = {"lights", "1", "state", "xy", NULL};
//    //const char *path[] = {"sensors", "1", "config", "on", NULL};
//    igsJSONTreeNode_t *value = igs_JSONTreeGetNodeAtPath(node, path);
//    bool b1 = igs_JSONTreeIsValueADouble(value);
//    bool b2 = igs_JSONTreeIsValueAnInteger(value);
//    igs_JSONTreeFree(&tree);
//
//    return 0;
//}
