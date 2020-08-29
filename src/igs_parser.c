//
//  igs_parser.c
//
//  Created by Stephane Vales
//  Modified by Patxi Berard
//  Copyright © 2017 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#if (defined WIN32 || defined _WIN32)
#include "unixfunctions.h"
#endif

#include "yajl_tree.h"
#include "yajl_gen.h"
#include "ingescape_private.h"

#define STR_CATEGORY "category"
#define STR_DEFINITION "definition"
#define STR_NAME "name"
#define STR_DESCRIPTION "description"
#define STR_VERSION "version"
#define STR_PARAMETERS "parameters"
#define STR_OUTPUTS "outputs"
#define STR_INPUTS "inputs"
#define STR_CALLS "calls"
#define STR_ARGUMENTS "arguments"
#define STR_REPLY "reply"
#define STR_CATEGORIES "categories"
#define STR_TYPE "type"
#define STR_VALUE "value"

#define DEF_NO_NAME "NO_NAME"
#define DEF_NO_DESCRIPTION "NO_DESCRIPTION"
#define DEF_NO_VERSION "NO_VERSION"

#define MAP_NO_NAME "NO_NAME"
#define MAP_NO_DESCRIPTION "NO_DESCRIPTION"
#define MAP_NO_VERSION "NO_VERSION"

char definition_path[IGS_MAX_PATH_LENGTH] = "";

iopType_t string_to_value_type(const char* str) {
    
    if (str != NULL){
        if (!strcmp(str, "INTEGER"))
            return IGS_INTEGER_T;
        if (!strcmp(str, "DOUBLE"))
            return IGS_DOUBLE_T;
        if (!strcmp(str, "STRING"))
            return IGS_STRING_T;
        if (!strcmp(str, "BOOL"))
            return IGS_BOOL_T;
        if (!strcmp(str, "IMPULSION"))
            return IGS_IMPULSION_T;
        if (!strcmp(str, "DATA"))
            return IGS_DATA_T;
    }
    
    igs_error("unknown value type \"%s\" to convert", str);
    return IGS_UNKNOWN_T;
}

bool string_to_boolean(const char* str) {
    if(str != NULL){
        if (!strcmp(str, "true"))
            return true;
        
        if (!strcmp(str, "false"))
            return false;
    }
    
    igs_warn("unknown string \"%s\" to convert", str);
    return false;
}

const char* value_type_to_string (iopType_t type) {
    switch (type) {
        case IGS_INTEGER_T:
            return "INTEGER";
            break;
        case IGS_DOUBLE_T:
            return "DOUBLE";
            break;
        case IGS_STRING_T:
            return "STRING";
            break;
        case IGS_BOOL_T:
            return "BOOL";
            break;
        case IGS_IMPULSION_T:
            return "IMPULSION";
            break;
        case IGS_DATA_T:
            return "DATA";
            break;
        case IGS_UNKNOWN_T:
            return "UNKNOWN";
            break;
        default:
            igs_error("unknown iopType_t to convert");
            break;
    }
    
    return "";
}

const char* boolean_to_string (bool boolean) {
    return (boolean ? "true" : "false");
}

////////////////////////////////////////
// IOP parsing

//parse an igs_iop_t data and add it to the corresponding hash table
static void json_add_iop_to_hash (igs_iop_t **hasht, iop_t type,
                                   igsyajl_val obj){
    const char *name = NULL;
    iopType_t valType = IGS_UNKNOWN_T;
    igsyajl_val value = NULL;

    if (IGSYAJL_IS_OBJECT(obj)){
        size_t nb = obj->u.object.len;
        size_t i = 0;
        for (i = 0; i < nb; i++){
            const char *key = obj->u.object.keys[i];
            if (strcmp("name", key) == 0){
                name = IGSYAJL_GET_STRING(obj->u.object.values[i]);
            }else if (strcmp("type", key) == 0){
                valType = string_to_value_type (IGSYAJL_GET_STRING(obj->u.object.values[i]));
                if (valType == IGS_UNKNOWN_T){
                    return;
                }
            }else if (strcmp("value", key) == 0){
                value = obj->u.object.values[i];
            }
        }
    }
    
    igs_iop_t *iop = NULL;
    if (name != NULL){
        //handle name, value type and value
        char *n = strndup(name, IGS_MAX_IOP_NAME_LENGTH);
        bool spaceInName = false;
        size_t lengthOfN = strlen(n);
        size_t i = 0;
        for (i = 0; i < lengthOfN; i++){
            if (n[i] == ' '){
                n[i] = '_';
                spaceInName = true;
            }
        }
        if (spaceInName){
            igs_warn("Spaces are not allowed in IOP name: %s has been renamed to %s", name, n);
        }
        HASH_FIND_STR(*hasht, n, iop);
        if (iop == NULL){
            iop = calloc (1, sizeof (igs_iop_t));
            iop->name = n;
            iop->value_type = valType;
            switch (iop->value_type) {
                case IGS_INTEGER_T:
                    iop->value.i =(int) IGSYAJL_GET_INTEGER (value);
                    break;
                case IGS_DOUBLE_T:
                    iop->value.d = IGSYAJL_GET_DOUBLE (value);
                    break;
                case IGS_BOOL_T:
                    iop->value.b = string_to_boolean (IGSYAJL_GET_STRING(value));
                    break;
                case IGS_STRING_T:
                    iop->value.s =  (IGSYAJL_IS_STRING(value) ? strdup(value->u.string) : NULL);
                    break;
                case IGS_IMPULSION_T:
                    //IMPULSION has no value
                    break;
                case IGS_DATA_T:
                    //FIXME : we store data as string but we should check it convert it to hexa
                    //data->value.s = strdup (IGSYAJL_IS_STRING(obj->u.object.values[2]) ? obj->u.object.values[2]->u.string : "");
                    break;
                default:
                    igs_warn("unknown data type to load from json for %s", n);
                    break;
            }
            iop->is_muted = false;
            iop->type = type;
            HASH_ADD_STR(*hasht, name, iop);
        }else{
            igs_warn("%s already exists", n);
        }
    }
}

//parse a tab of igs_iop_t data and add them into the corresponding hash table
static void json_add_iops (igsyajl_val node, const char** path, iop_t type,
                           igs_iop_t **hasht) {
    igsyajl_val v;
    v = igsyajl_tree_get(node, path, igsyajl_t_array);

    if (v && IGSYAJL_IS_ARRAY(v)){
        unsigned int  i;
        for (i = 0; i < v->u.array.len; i++ ){
            igsyajl_val obj = v->u.array.values[i];
            if( obj && IGSYAJL_IS_OBJECT(obj))
                json_add_iop_to_hash (hasht, type, obj);
        }
    }
}

////////////////////////////////////////
// calls parsing

static void json_parse_call_arguments (igs_call_t *call, igsyajl_val arguments){
    if (IGSYAJL_IS_ARRAY(arguments)){
        size_t nbArgs = arguments->u.array.len;
        size_t i = 0;
        for (i = 0; i < nbArgs; i++){
            //iterate on arguments
            igsyajl_val arg = arguments->u.array.values[i];
            
            if (IGSYAJL_IS_OBJECT(arg)){
                size_t nbKeys = arg->u.object.len;
                const char *name = NULL;
                iopType_t valType = IGS_UNKNOWN_T;
                size_t j = 0;
                for (j = 0; j < nbKeys; j++){
                    //iterate on keys for this argument
                    const char *key = arg->u.object.keys[j];
                    if (strcmp("name", key) == 0){
                        name = IGSYAJL_GET_STRING(arg->u.object.values[j]);
                    }else if (strcmp("type", key) == 0){
                        valType = string_to_value_type (IGSYAJL_GET_STRING(arg->u.object.values[j]));
                    }
                }
                
                if (name != NULL){
                    char *n = strndup(name, IGS_MAX_IOP_NAME_LENGTH);
                    bool spaceInName = false;
                    size_t lengthOfN = strlen(n);
                    size_t _i = 0;
                    for (_i = 0; _i < lengthOfN; _i++){
                        if (n[_i] == ' '){
                            n[_i] = '_';
                            spaceInName = true;
                        }
                    }
                    if (spaceInName){
                        igs_warn("Spaces are not allowed in call argument name: %s has been renamed to %s", name, n);
                    }
                    if (valType != IGS_UNKNOWN_T){
                        igs_callArgument_t *callArg = calloc(1, sizeof(igs_callArgument_t));
                        callArg->name = n;
                        callArg->type = valType;
                        LL_APPEND(call->arguments, callArg);
                    }else{
                        free(n);
                    }
                }
            }else{
                igs_error("argument is not passed as an object");
            }
        }
    }else{
        igs_error("arguments are not passed as an array");
    }
}

//parse a call and add it to the corresponding hash table
static void json_add_call_to_hash (igs_call_t **hasht, igsyajl_val obj){
    
    const char *name = NULL;
    const char *description = NULL;
    igsyajl_val arguments = NULL;
    igsyajl_val reply = NULL;
    
    if (IGSYAJL_IS_OBJECT(obj)){
        size_t nb = obj->u.object.len;
        size_t i = 0;
        for (i = 0; i < nb; i++){
            const char *key = obj->u.object.keys[i];
            if (strcmp("name", key) == 0){
                name = IGSYAJL_GET_STRING(obj->u.object.values[i]);
            }else if (strcmp("description", key) == 0){
                description = IGSYAJL_GET_STRING(obj->u.object.values[i]);
            }else if (strcmp("arguments", key) == 0){
                arguments = obj->u.object.values[i];
            }else if (strcmp("reply", key) == 0){
                reply = obj->u.object.values[i];
            }
        }
    }
    char *n = NULL;
    if (name != NULL){
        n = strndup(name, IGS_MAX_IOP_NAME_LENGTH);
        bool spaceInName = false;
        size_t lengthOfN = strlen(n);
        size_t i = 0;
        for (i = 0; i < lengthOfN; i++){
            if (n[i] == ' '){
                n[i] = '_';
                spaceInName = true;
            }
        }
        if (spaceInName){
            igs_warn("Spaces are not allowed in call name: %s has been renamed to %s", name, n);
        }
    }else{
        igs_warn("parsed call with NULL name");
    }
    
    igs_call_t *call = NULL;
    HASH_FIND_STR(*hasht, n, call);
    if (call == NULL){
        call = calloc(1, sizeof(igs_call_t));
        call->name = n;
        if (description != NULL)
            call->description = strndup(description, IGS_MAX_DESCRIPTION_LENGTH);
        if (arguments != NULL){
            json_parse_call_arguments(call, arguments);
        }
        if (reply != NULL && reply->type == igsyajl_t_object
            && reply->u.object.len >= 2){
            igs_call_t *callReply = calloc(1, sizeof(igs_call_t));
            for (size_t i = 0; i < reply->u.object.len; i++){
                const char *key = reply->u.object.keys[i];
                if (strcmp(key, STR_NAME) == 0
                    && reply->u.object.values[i] != NULL
                    && reply->u.object.values[i]->type == igsyajl_t_string){
                    callReply->name = strdup(reply->u.object.values[i]->u.string);
                } else if (strcmp(key, STR_ARGUMENTS) == 0
                           && reply->u.object.values[i] != NULL
                           && reply->u.object.values[i]->type == igsyajl_t_array){
                    json_parse_call_arguments(callReply, reply->u.object.values[i]);
                }
            }
            call->reply = callReply;
        }
        HASH_ADD_STR(*hasht, name, call);
    }else{
        igs_warn("%s already exists", name);
    }
}

//parse a tab of calls and add them into the corresponding hash table
static void json_add_calls (igsyajl_val node, const char **path, igs_call_t **hasht){
    igsyajl_val v;
    v = igsyajl_tree_get(node, path, igsyajl_t_array);
    
    if (v && IGSYAJL_IS_ARRAY(v)){
        unsigned int  i;
        for (i = 0; i < v->u.array.len; i++ ){
            igsyajl_val obj = v->u.array.values[i];
            if( obj && IGSYAJL_IS_OBJECT(obj))
                json_add_call_to_hash (hasht, obj);
        }
    }
}

////////////////////////////////////////
// File reading and parsing

// fetch a JSON file and convert it into string readable by the JSON parser
static char* json_fetch (const char* path) {

    FILE *file;
    char buff[BUFSIZ];
    char *js = NULL;
    int jslen = 0;
    unsigned int rd;

    file = fopen(path, "r");
    if (!file){
        igs_error("file %s not found", path);
        return 0;
    }

    /* read the whole config file */
    do {

        rd = (unsigned int) fread(buff, 1, sizeof(buff) , file);
        /* file read error handling */
        if (rd == 0 && !feof(stdin)) {
            igs_error("could not read %s", path);
            return 0;
        }

        /* rebuild the json string */
        js = realloc(js, jslen + rd + 1);
        if (!js) {
            igs_error("could not realloc parsed string");
            return 0;
        }
        strncpy(js + jslen, buff, rd);
        jslen = jslen + rd;

    }while (rd >= sizeof(buff));

    if (file)
        fclose(file);

    return js;
}

// convert JSON string into DOM
static int json_callize (const char* json_str, igsyajl_val *node) {

    char errbuf[BUFSIZ] = "unknown error";
    /* we have the whole config file in memory.  let's parse it ... */
    *node = igsyajl_tree_parse(json_str, errbuf, sizeof(errbuf));

    /* parse error handling */
    if (!(*node) || strlen(errbuf) > 0) {
        igs_error("could not parse string (%s)", errbuf);
        return 0;
    }

    return 1;
}

// convert a definition json file into a definition structure
static igs_definition_t* json_parse_definition (igsyajl_val node) {
    igs_definition_t *def;
    igsyajl_val v;
    def = (igs_definition_t*) calloc(1, sizeof(igs_definition_t));
    const char * path[] = { STR_DEFINITION, "", (const char *) 0 };

    path[1] = STR_NAME;
    v = igsyajl_tree_get(node, path, igsyajl_t_any);
    if (v){
        def->name = (IGSYAJL_IS_STRING(v) ? strdup((v)->u.string) : NULL);
    }

    path[1] = STR_DESCRIPTION;
    v = igsyajl_tree_get(node, path, igsyajl_t_any);
    if (v){
        def->description = (IGSYAJL_IS_STRING(v) ? strdup((v)->u.string) : NULL);
    }

    path[1] = STR_VERSION;
    v = igsyajl_tree_get(node, path, igsyajl_t_any);
    if (v){
        def->version = (IGSYAJL_IS_STRING(v) ? strdup((v)->u.string) : NULL);
    }

    path[1] = STR_INPUTS;
    json_add_iops (node, path, IGS_INPUT_T, &def->inputs_table);

    path[1] = STR_OUTPUTS;
    json_add_iops (node, path, IGS_OUTPUT_T, &def->outputs_table);

    path[1] = STR_PARAMETERS;
    json_add_iops (node, path, IGS_PARAMETER_T, &def->params_table);
    
    path[1] = STR_CALLS;
    json_add_calls (node, path, &def->calls_table);

    return def;
}

// parse a tab of mapping output type and add them into the corresponding hash table
static void json_add_map_out_to_hash (igs_mapping_element_t** hasht,
                                       igsyajl_val current_map_out){

    const char* input_name = NULL;
    const char* agent_name = NULL;
    const char* output_name = NULL;
    igsyajl_val v;
    const char * path_in_current[] = { "", (const char *) 0 };

    //input_name
    path_in_current[0] = "input_name";
    v = igsyajl_tree_get(current_map_out, path_in_current, igsyajl_t_any);
    if (v){
        input_name = IGSYAJL_GET_STRING(v);
    }
    char *reviewedFromOurInput = strndup(input_name, IGS_MAX_IOP_NAME_LENGTH);
    bool spaceInName = false;
    size_t lengthOfReviewedFromOurInput = strlen(reviewedFromOurInput);
    size_t i = 0;
    for (i = 0; i < lengthOfReviewedFromOurInput; i++){
        if (reviewedFromOurInput[i] == ' '){
            reviewedFromOurInput[i] = '_';
            spaceInName = true;
        }
    }
    if (spaceInName){
        igs_warn("Mapping parser : spaces are not allowed in IOP: %s has been renamed to %s\n", input_name, reviewedFromOurInput);
    }
    
    //agent_name
    path_in_current[0] = "agent_name";
    v = igsyajl_tree_get(current_map_out, path_in_current, igsyajl_t_any);
    if (v){
        agent_name = IGSYAJL_GET_STRING(v);
    }
    char *reviewedToAgent = strndup(agent_name, IGS_MAX_IOP_NAME_LENGTH);
    size_t lengthOfReviewedToAgent = strlen(reviewedToAgent);
    spaceInName = false;
    for (i = 0; i < lengthOfReviewedToAgent; i++){
        if (reviewedToAgent[i] == ' '){
            reviewedToAgent[i] = '_';
            spaceInName = true;
        }
    }
    if (spaceInName){
        igs_warn("Mapping parser : spaces are not allowed in agent name: %s has been renamed to %s\n", agent_name, reviewedToAgent);
    }
    
    //output_name
    path_in_current[0] = "output_name";
    v = igsyajl_tree_get(current_map_out, path_in_current, igsyajl_t_any);
    if (v){
        output_name = IGSYAJL_GET_STRING(v);
    }
    char *reviewedWithOutput = strndup(output_name, IGS_MAX_IOP_NAME_LENGTH);
    size_t lengthOfReviewedWithOutput = strlen(reviewedWithOutput);
    spaceInName = false;
    for (i = 0; i < lengthOfReviewedWithOutput; i++){
        if (reviewedWithOutput[i] == ' '){
            reviewedWithOutput[i] = '_';
            spaceInName = true;
        }
    }
    if (spaceInName){
        igs_warn("Mapping parser : spaces are not allowed in IOP: %s has been renamed to %s\n", output_name, reviewedWithOutput);
    }
    
    size_t len = strlen(reviewedFromOurInput)+strlen(reviewedToAgent)+strlen(reviewedWithOutput)+3+1;
    char *mashup = calloc(1, len*sizeof(char));
    strcpy(mashup, reviewedFromOurInput);
    strcat(mashup, ".");//separator
    strcat(mashup, reviewedToAgent);
    strcat(mashup, ".");//separator
    strcat(mashup, reviewedWithOutput);
    mashup[len -1] = '\0';
    unsigned long h = djb2_hash((unsigned char *)mashup);
    free (mashup);
    
    igs_mapping_element_t *tmp = NULL;
    if (*hasht != NULL){
        HASH_FIND(hh, *hasht, &h, sizeof(unsigned long), tmp);
    }
    if (tmp == NULL){
        //element does not exist yet : create and register it
        igs_mapping_element_t *new = mapping_createMappingElement(reviewedFromOurInput, reviewedToAgent, reviewedWithOutput);
        new->id = h;
        HASH_ADD(hh, *hasht, id, sizeof(unsigned long), new);
    }
    free(reviewedFromOurInput);
    free(reviewedToAgent);
    free(reviewedWithOutput);
}

// parse a tab of mapping category type and add them into the corresponding hash table
static void json_add_map_cat_to_hash (igs_mapping_element_t** hasht,
                                       igsyajl_val current_map_out){
    IGS_UNUSED(hasht)
    IGS_UNUSED(current_map_out)

    return;
//    const char* agent_name;
//    const char* category_name;
//    struct mapping_cat *map_cat = NULL;
//    igsyajl_val v;
//    const char * path_in_current[] = { "", (const char *) 0 };
//
//    path_in_current[0] = "map_cat_id";
//    v = igsyajl_tree_get(current_map_out, path_in_current, igsyajl_t_any);
//    if (v) {
//        /* check if the key already exist */
//        int map_cat_id = (int) IGSYAJL_GET_INTEGER(v);
//
//        HASH_FIND_INT(*hasht, &map_cat_id , map_cat);
//
//        if (map_cat == NULL){
//            map_cat = calloc (1, sizeof (struct mapping_cat));
//            map_cat->map_cat_id = map_cat_id;
//            map_cat->state = OFF;
//
//            //agent_name
//            path_in_current[0] = "agent_name";
//            v = igsyajl_tree_get(current_map_out, path_in_current, igsyajl_t_any);
//            if (v){
//                agent_name = IGSYAJL_GET_STRING(v);
//                map_cat->agent_name = strdup (agent_name);
//            }
//
//            //category_name
//            path_in_current[0] = "category_name";
//            v = igsyajl_tree_get(current_map_out, path_in_current, igsyajl_t_any);
//            if (v){
//                category_name = IGSYAJL_GET_STRING(v);
//                map_cat->category_name = strdup (category_name);
//            }
//
//            HASH_ADD_INT(*hasht , map_cat_id, map_cat);  /* id: name of key field */
//        }
//    }
}

// convert a map.json file into a mapping (output & category) structure
static igs_mapping_t* json_parse_mapping (igsyajl_val node) {

    igs_mapping_t* mapp;
    igsyajl_val v;
    mapp = (igs_mapping_t*) calloc(1, sizeof(igs_mapping_t));
    const char * path[] = { "mapping", "", (const char *) 0 };

    path[1] = STR_NAME;
    v = igsyajl_tree_get(node, path, igsyajl_t_any);
    if (v){
        const char* name = IGSYAJL_GET_STRING(v);
        mapp->name = strdup (name);
    }


    path[1] = STR_DESCRIPTION;
    v = igsyajl_tree_get(node, path, igsyajl_t_any);
    if (v){
        const char* description = IGSYAJL_GET_STRING(v);
        mapp->description = strdup (description);
    }

    path[1] = STR_VERSION;
    v = igsyajl_tree_get(node, path, igsyajl_t_any);
    if (v){
        const char* version = IGSYAJL_GET_STRING(v);
        mapp->version = strdup (version);
    }

    path[1] = "mapping_out";
    v = igsyajl_tree_get(node, path, igsyajl_t_array);
    if (v && IGSYAJL_IS_ARRAY(v)){
        unsigned int  i;
        for (i = 0; i < v->u.array.len; i++ ){
            igsyajl_val obj = v->u.array.values[i];
            if( obj && IGSYAJL_IS_OBJECT(obj))
                json_add_map_out_to_hash (&mapp->map_elements, obj);
        }
    }

    path[1] = "mapping_cat";
    v = igsyajl_tree_get(node, path, igsyajl_t_array);
    if (v && IGSYAJL_IS_ARRAY(v)){
        unsigned int  i;
        for (i = 0; i < v->u.array.len; i++ ){
            igsyajl_val obj = v->u.array.values[i];
            if( obj && IGSYAJL_IS_OBJECT(obj))
                json_add_map_cat_to_hash (&mapp->map_elements, obj);
        }
    }

    return mapp;
}

/////////////////////////
// Dumping functions


// convert a call into json string
static void json_dump_call (igsyajl_gen *g, igs_call_t *call) {
    
    igsyajl_gen_map_open(*g);
    
    igsyajl_gen_string(*g, (const unsigned char *) STR_NAME, strlen(STR_NAME));
    igsyajl_gen_string(*g, (const unsigned char *) call->name, strlen (call->name));
    
    if (call->description != NULL){
        igsyajl_gen_string(*g, (const unsigned char *) STR_DESCRIPTION, strlen(STR_DESCRIPTION));
        igsyajl_gen_string(*g, (const unsigned char *) call->description, strlen (call->description));
    }
    
    igs_callArgument_t *arg = NULL;
    int nbArgs = 0;
    DL_COUNT(call->arguments, arg, nbArgs);
    if ((call->arguments != NULL) && (nbArgs > 0)){
        igsyajl_gen_string(*g, (const unsigned char *) STR_ARGUMENTS, strlen(STR_ARGUMENTS));
        igsyajl_gen_array_open(*g);
        DL_FOREACH(call->arguments, arg){
            igsyajl_gen_map_open(*g);
            igsyajl_gen_string(*g, (const unsigned char *) STR_NAME, strlen(STR_NAME));
            igsyajl_gen_string(*g, (const unsigned char *) arg->name, strlen(arg->name));
            igsyajl_gen_string(*g, (const unsigned char *) STR_TYPE, strlen(STR_TYPE));
            const char *type = value_type_to_string(arg->type);
            igsyajl_gen_string(*g, (const unsigned char *) type, strlen(type));
            igsyajl_gen_map_close(*g);
        }
        igsyajl_gen_array_close(*g);
    }
    //dump reply
    if (call->reply != NULL && call->reply->name != NULL){
        igsyajl_gen_string(*g, (const unsigned char *) STR_REPLY, strlen(STR_REPLY));
        igsyajl_gen_map_open(*g);
        
        igsyajl_gen_string(*g, (const unsigned char *) STR_NAME, strlen(STR_NAME));
        igsyajl_gen_string(*g, (const unsigned char *) call->reply->name, strlen (call->reply->name));
        
        arg = NULL;
        nbArgs = 0;
        DL_COUNT(call->reply->arguments, arg, nbArgs);
        if ((call->reply->arguments != NULL) && (nbArgs > 0)){
            igsyajl_gen_string(*g, (const unsigned char *) STR_ARGUMENTS, strlen(STR_ARGUMENTS));
            igsyajl_gen_array_open(*g);
            DL_FOREACH(call->reply->arguments, arg){
                igsyajl_gen_map_open(*g);
                igsyajl_gen_string(*g, (const unsigned char *) STR_NAME, strlen(STR_NAME));
                igsyajl_gen_string(*g, (const unsigned char *) arg->name, strlen(arg->name));
                igsyajl_gen_string(*g, (const unsigned char *) STR_TYPE, strlen(STR_TYPE));
                const char *type = value_type_to_string(arg->type);
                igsyajl_gen_string(*g, (const unsigned char *) type, strlen(type));
                igsyajl_gen_map_close(*g);
            }
            igsyajl_gen_array_close(*g);
        }
        
        igsyajl_gen_map_close(*g);
    }
    
    igsyajl_gen_map_close(*g);
}

// convert an igs_iop_t structure into json string
static void json_dump_iop (igsyajl_gen *g, igs_iop_t* aiop) {
    
    igsyajl_gen_map_open(*g);
    
    igsyajl_gen_string(*g, (const unsigned char *) STR_NAME, strlen(STR_NAME));
    igsyajl_gen_string(*g, (const unsigned char *) aiop->name, strlen (aiop->name));
    
    igsyajl_gen_string(*g, (const unsigned char *) STR_TYPE, strlen(STR_TYPE));
    igsyajl_gen_string(*g, (const unsigned char *) value_type_to_string(aiop->value_type), strlen(value_type_to_string(aiop->value_type)));
    
    igsyajl_gen_string(*g, (const unsigned char *) STR_VALUE, strlen(STR_VALUE));
    
    switch (aiop->value_type) {
        case IGS_INTEGER_T:
            igsyajl_gen_integer(*g, aiop->value.i);
            break;

        case IGS_DOUBLE_T:
            igsyajl_gen_double(*g, aiop->value.d);
            break;

        case IGS_BOOL_T:
            igsyajl_gen_string(*g, (const unsigned char *) boolean_to_string(aiop->value.b), strlen(boolean_to_string(aiop->value.b)));
            break;

        case IGS_STRING_T:
            {
                if (igsyajl_gen_string(*g, (const unsigned char *) aiop->value.s, strlen(aiop->value.s)) == igsyajl_gen_invalid_string)
                {
                    igs_warn("Mapping parser : json_dump_iop failed to dump a string value - it may not be a valid UTF8 string - %s\n", aiop->value.s);
                    igsyajl_gen_string(*g, (const unsigned char *) "", 0);
                }
            }
            break;

        case IGS_IMPULSION_T:
            igsyajl_gen_string(*g, (const unsigned char *) "", 0);
            break;

        case IGS_DATA_T:
            igsyajl_gen_string(*g, (const unsigned char *) "", 0);
            break;

        default:
            {
                igs_warn("unknown data type to convert in string (%d)", aiop->value_type);
                igsyajl_gen_string(*g, (const unsigned char *) "", 0);
            }
            break;
    }
    igsyajl_gen_map_close(*g);
}

// convert a definition structure into definition.json string
static void json_dump_definition (igsyajl_gen *g, igs_definition_t* def) {
    
    unsigned int hashCount = 0;
    igs_iop_t *d;
    
    igsyajl_gen_map_open(*g);
    
    igsyajl_gen_string(*g, (const unsigned char *) STR_NAME, strlen(STR_NAME));
    //Get the agent name from the network layer
    if (def->name == NULL){
        igsyajl_gen_string(*g, (const unsigned char *) DEF_NO_NAME, strlen (DEF_NO_NAME));
    }else{
        igsyajl_gen_string(*g, (const unsigned char *) def->name, strlen (def->name));
    }
    
    
    igsyajl_gen_string(*g, (const unsigned char *) STR_DESCRIPTION, strlen(STR_DESCRIPTION));
    if(def->description != NULL){
        igsyajl_gen_string(*g, (const unsigned char *) def->description, strlen (def->description));
    } else {
        igsyajl_gen_string(*g, (const unsigned char *) DEF_NO_DESCRIPTION, strlen(DEF_NO_DESCRIPTION));
    }
    
    igsyajl_gen_string(*g, (const unsigned char *) STR_VERSION, strlen(STR_VERSION));
    if(def->version != NULL){
        igsyajl_gen_string(*g, (const unsigned char *) def->version, strlen(def->version));
    } else {
        igsyajl_gen_string(*g, (const unsigned char *) DEF_NO_VERSION, strlen(DEF_NO_VERSION));
    }
    
    hashCount = HASH_COUNT(def->params_table);
    if (hashCount) {
        igsyajl_gen_string(*g, (const unsigned char *) STR_PARAMETERS, strlen(STR_PARAMETERS));
        igsyajl_gen_array_open(*g);
        for(d=def->params_table; d != NULL; d=d->hh.next) {
            json_dump_iop (g, d);
        }
        igsyajl_gen_array_close(*g);
    }
    
    hashCount = HASH_COUNT(def->inputs_table);
    if (hashCount) {
        igsyajl_gen_string(*g, (const unsigned char *) STR_INPUTS, strlen(STR_INPUTS));
        igsyajl_gen_array_open(*g);
        for(d=def->inputs_table; d != NULL; d=d->hh.next) {
            json_dump_iop (g, d);
        }
        igsyajl_gen_array_close(*g);
    }
    
    hashCount = HASH_COUNT(def->outputs_table);
    if (hashCount) {
        igsyajl_gen_string(*g, (const unsigned char *) STR_OUTPUTS, strlen(STR_OUTPUTS));
        igsyajl_gen_array_open(*g);
        for(d=def->outputs_table; d != NULL; d=d->hh.next) {
            json_dump_iop (g, d);
        }
        igsyajl_gen_array_close(*g);
    }
    
    hashCount = HASH_COUNT(def->calls_table);
    if (hashCount) {
        igsyajl_gen_string(*g, (const unsigned char *) STR_CALLS, strlen(STR_CALLS));
        igsyajl_gen_array_open(*g);
        igs_call_t *t = NULL, *tmp = NULL;
        HASH_ITER(hh, def->calls_table, t, tmp){
            json_dump_call (g, t);
        }
        igsyajl_gen_array_close(*g);
    }
    
//    struct category *cat;
//    hashCount = HASH_COUNT(def->categories);
//    if (hashCount) {
//        igsyajl_gen_string(*g, (const unsigned char *) STR_CATEGORIES, strlen(STR_CATEGORIES));
//        igsyajl_gen_array_open(*g);
//        for(cat=def->categories; cat != NULL; cat=cat->hh.next) {
//            json_dump_category(g, cat);
//        }
//        igsyajl_gen_array_close(*g);
//    }

    igsyajl_gen_map_close(*g);
}

//convert a mapping_out structure into json string
static void json_dump_mapping_out (igsyajl_gen *g, igs_mapping_element_t* mapp_out) {

    igsyajl_gen_map_open(*g);

    igsyajl_gen_string(*g, (const unsigned char *) "input_name", strlen("input_name"));
    igsyajl_gen_string(*g, (const unsigned char *) mapp_out->input_name, strlen (mapp_out->input_name));

    igsyajl_gen_string(*g, (const unsigned char *) "agent_name", strlen("agent_name"));
    igsyajl_gen_string(*g, (const unsigned char *) mapp_out->agent_name, strlen(mapp_out->agent_name));

    igsyajl_gen_string(*g, (const unsigned char *) "output_name", strlen("output_name"));
    igsyajl_gen_string(*g, (const unsigned char *) mapp_out->output_name, strlen(mapp_out->output_name));

    igsyajl_gen_map_close(*g);
}

/*
 * Function: json_dump_mapping_cat
 * -----------------------
 *   convert a mapping_cat structure into json string
 */

//static void json_dump_mapping_cat (igsyajl_gen *g, mapping_cat* mapp_cat) {
//
//    igsyajl_gen_map_open(*g);
//
//    igsyajl_gen_string(*g, (const unsigned char *) "agent_name", strlen("agent_name"));
//    igsyajl_gen_string(*g, (const unsigned char *) mapp_cat->agent_name, strlen (mapp_cat->agent_name));
//
//    igsyajl_gen_string(*g, (const unsigned char *) "category_name", strlen("category_name"));
//    igsyajl_gen_string(*g, (const unsigned char *) mapp_cat->category_name, strlen(mapp_cat->category_name));
//
//    igsyajl_gen_map_close(*g);
//}

//convert a mapping structure into mapping.json string
static void json_dump_mapping (igsyajl_gen *g, igs_mapping_t* mapp) {

    unsigned int hashCount = 0;
    igs_mapping_element_t *currentMapOut = NULL;

    if(mapp != NULL)
    {
        //    struct mapping_cat *currentMapCat = NULL;

        igsyajl_gen_map_open(*g);

        igsyajl_gen_string(*g, (const unsigned char *) STR_NAME, strlen(STR_NAME));
        if(mapp->name != NULL)
            igsyajl_gen_string(*g, (const unsigned char *) mapp->name, strlen (mapp->name));
        else
            igsyajl_gen_string(*g, (const unsigned char *) MAP_NO_NAME, strlen(MAP_NO_NAME));

        igsyajl_gen_string(*g, (const unsigned char *) STR_DESCRIPTION, strlen(STR_DESCRIPTION));
        if(mapp->description != NULL)
            igsyajl_gen_string(*g, (const unsigned char *) mapp->description, strlen (mapp->description));
        else
            igsyajl_gen_string(*g, (const unsigned char *) MAP_NO_DESCRIPTION, strlen(MAP_NO_DESCRIPTION));

        igsyajl_gen_string(*g, (const unsigned char *) STR_VERSION, strlen(STR_VERSION));
        if(mapp->version != NULL)
            igsyajl_gen_string(*g, (const unsigned char *) mapp->version, strlen(mapp->version));
        else
            igsyajl_gen_string(*g, (const unsigned char *) MAP_NO_VERSION, strlen(MAP_NO_VERSION));

        //Mapping_out
        hashCount = HASH_COUNT(mapp->map_elements);
        if (hashCount) {
            igsyajl_gen_string(*g, (const unsigned char *) "mapping_out", strlen("mapping_out"));
            igsyajl_gen_array_open(*g);
            for(currentMapOut = mapp->map_elements; currentMapOut != NULL; currentMapOut=currentMapOut->hh.next) {
                json_dump_mapping_out(g, currentMapOut);
            }
            igsyajl_gen_array_close(*g);
        }

    //    //Mapping_cat
    //    hashCount = HASH_COUNT(mapp->map_cat);
    //    if (hashCount) {
    //        igsyajl_gen_string(*g, (const unsigned char *) "mapping_cat", strlen("mapping_cat"));
    //        igsyajl_gen_array_open(*g);
    //        for(currentMapCat=mapp->map_cat; currentMapCat != NULL; currentMapCat=currentMapOut->hh.next) {
    //            json_dump_mapping_cat(g, currentMapCat);
    //        }
    //        igsyajl_gen_array_close(*g);
    //    }

        igsyajl_gen_map_close(*g);
    }
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
igs_definition_t* parser_loadDefinition (const char* json_str) {
    igs_definition_t *def = NULL;
    igsyajl_val node;
    if (json_callize(json_str, &node) != 0){
        def = json_parse_definition(node);
        igsyajl_tree_free(node);
    }
    return def;
}


igs_definition_t * parser_loadDefinitionFromPath (const char* path) {
    char *json_str = NULL;
    igs_definition_t *def = NULL;
    json_str = json_fetch(path);
    if (!json_str)
        return NULL;
    
    def = parser_loadDefinition(json_str);
    free (json_str);
    return def;
}


char* parser_export_definition(igs_definition_t* def){
    assert(def);
    char* result = NULL;
    if (def != NULL){
        const unsigned char * json_str = NULL;
        size_t len;
        igsyajl_gen g;
        
        g = igsyajl_gen_alloc(NULL);
        igsyajl_gen_config(g, igsyajl_gen_beautify, 1);
        igsyajl_gen_config(g, igsyajl_gen_validate_utf8, 1);
        
        igsyajl_gen_map_open(g);
        igsyajl_gen_string(g, (const unsigned char *) STR_DEFINITION, strlen(STR_DEFINITION));
        
        if(def != NULL){
            json_dump_definition(&g, def);
        }
        igsyajl_gen_map_close(g);
        
        // try to get our dumping result
        if (igsyajl_gen_get_buf(g, &json_str, &len) == igsyajl_gen_status_ok){
            result = strdup((const char*) json_str);
        }
        
        igsyajl_gen_free(g);
    }
    return result;
}


igs_mapping_t* parser_loadMapping(const char* json_str){
    if (!json_str || strlen(json_str) == 0)
        return NULL;
    igs_mapping_t *map = NULL;
    igsyajl_val node;
    if (json_callize(json_str, &node) != 0){
        map = json_parse_mapping(node);
        igsyajl_tree_free(node);
    }
    return map;
}


igs_mapping_t* parser_loadMappingFromPath (const char* path){
    char *json_str = NULL;
    igs_mapping_t *map = NULL;
    json_str = json_fetch(path);
    if (!json_str)
        return NULL;
    
    map = parser_loadMapping(json_str);
    free (json_str);
    return map;
}


char* parser_export_mapping(igs_mapping_t *mapping){
    char* result = NULL;
    if (mapping){
        const unsigned char *json_str = NULL;
        size_t len;
        igsyajl_gen g;
        
        g = igsyajl_gen_alloc(NULL);
        igsyajl_gen_config(g, igsyajl_gen_beautify, 1);
        igsyajl_gen_config(g, igsyajl_gen_validate_utf8, 1);
        
        igsyajl_gen_map_open(g);
        igsyajl_gen_string(g, (const unsigned char *) "mapping", strlen("mapping"));
        json_dump_mapping(&g, mapping);
        igsyajl_gen_map_close(g);
        
        if (igsyajl_gen_get_buf(g, &json_str, &len) == igsyajl_gen_status_ok)
            result = strdup((const char*) json_str);
        
        igsyajl_gen_free(g);
    }
    return result;
}


////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////
igs_result_t igsAgent_loadDefinition (igs_agent_t *agent, const char* json_str){
    assert(agent);
    assert(json_str);
    //Try to load definition
    igs_definition_t *tmp = parser_loadDefinition(json_str);
    if(tmp == NULL){
        igsAgent_debug(agent, "json string caused an error and was ignored");
        return IGS_FAILURE;
    }else{
        if (agent->definition != NULL){
            definition_freeDefinition(&agent->definition);
        }
        agent->definition = tmp;
        //Check the name of agent from network layer
        char *name = igsAgent_getAgentName(agent);
        if(streq(name, IGS_DEFAULT_AGENT_NAME)){
            //The name of the agent is default : we change it to definition name
            igsAgent_setAgentName(agent, agent->definition->name);
        }//else
            //The agent name was assigned by the developer : we keep it untouched
        free(name);
        agent->network_needToSendDefinitionUpdate = true;
    }
    return IGS_SUCCESS;
}


igs_result_t igsAgent_loadDefinitionFromPath (igs_agent_t *agent, const char* file_path){
    assert(agent);
    assert(file_path);
    //Try to load definition
    igs_definition_t *tmp = parser_loadDefinitionFromPath(file_path);
    if(tmp == NULL){
        igsAgent_debug(agent, "json file content at '%s' caused an error and was ignored", file_path);
        return IGS_FAILURE;
    }else{
        strncpy(definition_path, file_path, IGS_MAX_PATH_LENGTH - 1);
        if (agent->definition != NULL){
            definition_freeDefinition(&agent->definition);
        }
        agent->definition = tmp;
        //Check the name of agent from network layer
        char *name = igsAgent_getAgentName(agent);
        if(strcmp(name, IGS_DEFAULT_AGENT_NAME) == 0){
            //The name of the agent is default : we change it to definition name
            igsAgent_setAgentName(agent, agent->definition->name);
        }//else
            //The agent name was assigned by the developer : we keep it untouched
        free(name);
        agent->network_needToSendDefinitionUpdate = true;
    }
    return IGS_SUCCESS;
}

