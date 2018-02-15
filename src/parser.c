//
//  mtic_parser.c
//
//  Created by Mathieu Poirier
//  Modified by Patxi Berard
//  Modified by Vincent Deliencourt
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include "unixfunctions.h"
#endif

#include "yajl_tree.h"
#include "yajl_gen.h"
#include "mastic_private.h"

#define STR_CATEGORY "category"
#define STR_DEFINITION "definition"
#define STR_NAME "name"
#define STR_DESCRIPTION "description"
#define STR_VERSION "version"
#define STR_PARAMETERS "parameters"
#define STR_OUTPUTS "outputs"
#define STR_INPUTS "inputs"
#define STR_CATEGORIES "categories"
#define STR_TYPE "type"
#define STR_VALUE "value"

#define DEF_NO_NAME "NO_NAME"
#define DEF_NO_DESCRIPTION "NO_DESCRIPTION"
#define DEF_NO_VERSION "NO_VERSION"

#define MAP_NO_NAME "NO_NAME"
#define MAP_NO_DESCRIPTION "NO_DESCRIPTION"
#define MAP_NO_VERSION "NO_VERSION"

char definitionPath[1024] = "";

iopType_t string_to_value_type(const char* str) {
    
    if (!strcmp(str, "INTEGER"))
        return INTEGER_T;
    if (!strcmp(str, "DOUBLE"))
        return DOUBLE_T;
    if (!strcmp(str, "STRING"))
        return STRING_T;
    if (!strcmp(str, "BOOL"))
        return BOOL_T;
    if (!strcmp(str, "IMPULSION"))
        return IMPULSION_T;
    if (!strcmp(str, "DATA"))
        return DATA_T;
    
    fprintf(stderr, "%s - ERROR -  unknown string \"%s\" to convert\n", __FUNCTION__, str);
    return -1;
}

bool string_to_boolean(const char* str) {
    
    if (!strcmp(str, "true"))
        return true;
    
    if (!strcmp(str, "false"))
        return false;
    
    fprintf(stderr, "%s - ERROR -  unknown string \"%s\" to convert\n", __FUNCTION__, str);
    return -1;
}

const char* value_type_to_string (iopType_t type) {
    switch (type) {
        case INTEGER_T:
            return "INTEGER";
            break;
        case DOUBLE_T:
            return "DOUBLE";
            break;
        case STRING_T:
            return "STRING";
            break;
        case BOOL_T:
            return "BOOL";
            break;
        case IMPULSION_T:
            return "IMPULSION";
            break;
        case DATA_T:
            return "DATA";
            break;
        default:
            fprintf(stderr, "%s - ERROR -  unknown iopType_t to convert\n", __FUNCTION__);
            break;
    }
    
    return "";
}

const char* boolean_to_string (bool boole) {
    if (boole)
        return "true";
    else
        return "false";
}



/*
 * Function: json_add_data_to_hash
 * ----------------------------
 *   parse a agent_iop data and add it to the corresponding hash table
 */

static void json_add_data_to_hash (struct agent_iop ** hasht,iop_t type,
                                   yajl_val obj){

    struct agent_iop *data = NULL;

    /* check if the key already exist */
    const char* name = YAJL_GET_STRING(obj->u.object.values[0]);
    HASH_FIND_STR(*hasht, name , data);
    if (data == NULL) {
        data = calloc (1, sizeof (struct agent_iop));
        char *n = strndup(name, MAX_IOP_NAME_LENGTH);
        bool spaceInName = false;
        size_t lengthOfN = strlen(n);
        for (size_t i = 0; i < lengthOfN; i++){
            if (n[i] == ' '){
                n[i] = '_';
                spaceInName = true;
            }
        }
        if (spaceInName){
            mtic_warn("Spaces are not allowed in IOP: %s has been renamed to %s\n", name, n);
        }
        data->name = n;

        data->value_type = string_to_value_type (YAJL_GET_STRING(obj->u.object.values[1]));
        switch (data->value_type) {
            case INTEGER_T:
                data->value.i =(int) YAJL_GET_INTEGER (obj->u.object.values[2]);
                break;
            case DOUBLE_T:
                data->value.d = YAJL_GET_DOUBLE (obj->u.object.values[2]);
                break;
            case BOOL_T:
                data->value.b = string_to_boolean (YAJL_GET_STRING(obj->u.object.values[2]));
                break;
            case STRING_T:
                data->value.s = strdup (YAJL_IS_STRING(obj->u.object.values[2]) ? obj->u.object.values[2]->u.string : "");
                break;
            case IMPULSION_T:
                //IMPULSION has no value
                break;
            case DATA_T:
                //FIXME : we store data as string but we should check it convert it to hexa
                //data->value.s = strdup (YAJL_IS_STRING(obj->u.object.values[2]) ? obj->u.object.values[2]->u.string : "");
                break;
            default:
                fprintf(stderr, "%s - ERROR -  unknown data type to load from json\n", __FUNCTION__);
                break;
        }
        data->is_muted = false;
        data->type = type;
        HASH_ADD_STR(*hasht , name, data );  /* id: name of key field */
    }
}

/*
 * Function: jason_add_data
 * ----------------------------
 *   parse a tab of agent_iop data and add them into the corresponding hash table
 */

static void json_add_data (yajl_val node, const char** path,iop_t type,
                            struct agent_iop ** hasht) {
    yajl_val v;
    v = yajl_tree_get(node, path, yajl_t_array);

    if (v && YAJL_IS_ARRAY(v)){
        unsigned int  i;
        for (i = 0; i < v->u.array.len; i++ ){
            yajl_val obj = v->u.array.values[i];
            if( obj && YAJL_IS_OBJECT(obj))
                json_add_data_to_hash (hasht,type, obj);
        }
    }
}

/*
 * Function: json_fetch
 * ----------------------------
 *   fetch a Json file and convert it into string readable by the Json parser
 */

static char* json_fetch (const char* path) {

    FILE *file;
    char buff[BUFSIZ];
    char *js = NULL;
    int jslen = 0;
    unsigned int rd;

    file = fopen(path, "r");
    if (!file){
        fprintf(stderr, "json parser - ERROR - fopen(): file %s NOT found\n", path);
        return 0;
    }

    /* read the whole config file */
    do {

        rd = (unsigned int) fread(buff, 1, sizeof(buff) , file);
        /* file read error handling */
        if (rd == 0 && !feof(stdin)) {
            fprintf(stderr, "json parser - ERROR -  fread(): error encountered on file read\n");
            return 0;
        }

        /* rebuild the json string */
        js = realloc(js, jslen + rd + 1);
        if (!js) {
            fprintf(stderr, "json parser - ERROR -  realloc(): error encountered realloc\n");
            return 0;
        }
        strncpy(js + jslen, buff, rd);
        jslen = jslen + rd;

    }while (rd >= sizeof(buff));

    if (file)
        fclose(file);

    return js;
}

/*
 * Function: json_tokenized
 * ----------------------------
 *   convert un string (json) into a DOM
 */

static int json_tokenized (const char* json_str,
                           yajl_val *node) {

    char errbuf[BUFSIZ];

    /* we have the whole config file in memory.  let's parse it ... */
    *node = yajl_tree_parse(json_str, errbuf, sizeof(errbuf));

    /* parse error handling */
    if (!node || strlen(errbuf) > 0) {
        fprintf(stderr, "json parser - ERROR : ");
        if (strlen(errbuf))
            fprintf(stderr, " %s", errbuf);
        else
            fprintf(stderr, "unknown error");
        fprintf(stderr, "\n");
        return 0;
    }

    return 1;
}

/*
 * Function: json_parse_category
 * ----------------------------
 *   convert a category.json file into a category structure
 */

static category* json_parse_category (yajl_val node) {

    yajl_val v;
    struct category* cat = NULL;
    cat = (category*) calloc(1, sizeof(category));
    const char * path[] = { STR_CATEGORY, "",  (const char *) 0 };

    path[1] = STR_NAME;
    v = yajl_tree_get(node, path, yajl_t_any);
    if (v)
        cat->name = strdup (YAJL_IS_STRING(v) ? (v)->u.string : "");

    path[1] = STR_VERSION;
    v = yajl_tree_get(node, path, yajl_t_any);
    if (v)
        cat->version = strdup (YAJL_IS_STRING(v) ? (v)->u.string : "");

    path[1] = STR_PARAMETERS;
    json_add_data (node, path,PARAMETER_T, &cat->params_table);

    path[1] = STR_INPUTS;
    json_add_data (node, path,INPUT_T, &cat->inputs_table);

    path[1] = STR_OUTPUTS;
    json_add_data (node, path,OUTPUT_T, &cat->outputs_table);

    return cat;
}

/*
 * Function: json_add_category_to_hash
 * ----------------------------
 *   parse a tab of categories and add them into the corresponding hash table
 */

//static void json_add_category_to_hash (struct category** hasht,
//                                       yajl_val current_cat){
//
//    struct category *cat = NULL;
//    yajl_val v;
//    const char * path_in_current[] = { "", (const char *) 0 };
//
//    path_in_current[0] = STR_NAME;
//    v = yajl_tree_get(current_cat, path_in_current, yajl_t_any);
//    if (v) {
//        /* check if the key already exist */
//        const char* name = YAJL_GET_STRING(v);
//        HASH_FIND_STR(*hasht, name , cat);
//        if (cat == NULL){
//            cat = calloc (1, sizeof (struct category));
//            cat->name = strdup (name);
//
//            path_in_current[0] = STR_VERSION;
//            v = yajl_tree_get(current_cat, path_in_current, yajl_t_any);
//            if (v)
//                cat->version = strdup (YAJL_IS_STRING(v) ? (v)->u.string : "");
//
//            path_in_current[0] = STR_PARAMETERS;
//            json_add_data (current_cat, path_in_current,PARAMETER_T, &cat->params_table);
//
//            path_in_current[0] = STR_INPUTS;
//            json_add_data (current_cat, path_in_current,INPUT_T, &cat->inputs_table);
//
//            path_in_current[0] = STR_OUTPUTS;
//            json_add_data (current_cat, path_in_current,OUTPUT_T, &cat->outputs_table);
//
//            HASH_ADD_STR(*hasht , name, cat );  /* id: name of key field */
//        }
//    }
//}

/*
 * Function: json_parse_definition
 * ----------------------------
 *   convert a definition.json file into a definition structure
 */

static definition* json_parse_definition (yajl_val node) {
    definition* def;
    yajl_val v;
    def = (definition*) calloc(1, sizeof(definition));
    const char * path[] = { STR_DEFINITION, "", (const char *) 0 };

    path[1] = STR_NAME;
    v = yajl_tree_get(node, path, yajl_t_any);

    if (v){
        def->name = strdup (YAJL_IS_STRING(v) ? (v)->u.string : "");
    }

    path[1] = STR_DESCRIPTION;
    v = yajl_tree_get(node, path, yajl_t_any);
    if (v)
        def->description = strdup (YAJL_IS_STRING(v) ? (v)->u.string : "");

    path[1] = STR_VERSION;
    v = yajl_tree_get(node, path, yajl_t_any);
    if (v)
        def->version = strdup (YAJL_IS_STRING(v) ? (v)->u.string : "");

    path[1] = STR_INPUTS;
    json_add_data (node, path,INPUT_T, &def->inputs_table);

    path[1] = STR_OUTPUTS;
    json_add_data (node, path,OUTPUT_T, &def->outputs_table);

    path[1] = STR_PARAMETERS;
    json_add_data (node, path,PARAMETER_T, &def->params_table);

    path[1] = STR_CATEGORIES;
    v = yajl_tree_get(node, path, yajl_t_array);
    if (v && YAJL_IS_ARRAY(v)){
        unsigned int  i;
        for (i = 0; i < v->u.array.len; i++ ){
//            yajl_val obj = v->u.array.values[i];
//            if( obj && YAJL_IS_OBJECT(obj))
//                json_add_category_to_hash (&def->categories, obj);
        }
    }

    return def;
}

/*
 * Function: json_add_map_out_to_hash
 * ----------------------------
 *   parse a tab of mapping output type and add them into the corresponding hash table
 */

static void json_add_map_out_to_hash (mapping_element_t** hasht,
                                       yajl_val current_map_out){

    const char* input_name;
    const char* agent_name;
    const char* output_name;
    yajl_val v;
    const char * path_in_current[] = { "", (const char *) 0 };

    //input_name
    path_in_current[0] = "input_name";
    v = yajl_tree_get(current_map_out, path_in_current, yajl_t_any);
    if (v){
        input_name = YAJL_GET_STRING(v);
    }
    char *reviewedFromOurInput = strndup(input_name, MAX_IOP_NAME_LENGTH);
    bool spaceInName = false;
    size_t lengthOfReviewedFromOurInput = strlen(reviewedFromOurInput);
    for (size_t i = 0; i < lengthOfReviewedFromOurInput; i++){
        if (reviewedFromOurInput[i] == ' '){
            reviewedFromOurInput[i] = '_';
            spaceInName = true;
        }
    }
    if (spaceInName){
        mtic_warn("Mapping parser : spaces are not allowed in IOP: %s has been renamed to %s\n", input_name, reviewedFromOurInput);
    }
    
    //agent_name
    path_in_current[0] = "agent_name";
    v = yajl_tree_get(current_map_out, path_in_current, yajl_t_any);
    if (v){
        agent_name = YAJL_GET_STRING(v);
    }
    char *reviewedToAgent = strndup(agent_name, MAX_IOP_NAME_LENGTH);
    size_t lengthOfReviewedToAgent = strlen(reviewedToAgent);
    spaceInName = false;
    for (size_t i = 0; i < lengthOfReviewedToAgent; i++){
        if (reviewedToAgent[i] == ' '){
            reviewedToAgent[i] = '_';
            spaceInName = true;
        }
    }
    if (spaceInName){
        mtic_warn("Mapping parser : spaces are not allowed in agent name: %s has been renamed to %s\n", agent_name, reviewedToAgent);
    }
    
    //output_name
    path_in_current[0] = "output_name";
    v = yajl_tree_get(current_map_out, path_in_current, yajl_t_any);
    if (v){
        output_name = YAJL_GET_STRING(v);
    }
    char *reviewedWithOutput = strndup(output_name, MAX_IOP_NAME_LENGTH);
    size_t lengthOfReviewedWithOutput = strlen(reviewedWithOutput);
    spaceInName = false;
    for (size_t i = 0; i < lengthOfReviewedWithOutput; i++){
        if (reviewedWithOutput[i] == ' '){
            reviewedWithOutput[i] = '_';
            spaceInName = true;
        }
    }
    if (spaceInName){
        mtic_warn("Mapping parser : spaces are not allowed in IOP: %s has been renamed to %s\n", output_name, reviewedWithOutput);
    }
    
    unsigned long len = strlen(reviewedFromOurInput)+strlen(reviewedToAgent)+strlen(reviewedWithOutput)+3+1;
    char *mashup = calloc(1, len*sizeof(char));
    strcpy(mashup, reviewedFromOurInput);
    strcat(mashup, ".");//separator
    strcat(mashup, reviewedToAgent);
    strcat(mashup, ".");//separator
    strcat(mashup, reviewedWithOutput);
    mashup[len -1] = '\0';
    unsigned long h = djb2_hash((unsigned char *)mashup);
    free (mashup);
    
    mapping_element_t *tmp = NULL;
    if (*hasht != NULL){
        HASH_FIND(hh, *hasht, &h, sizeof(unsigned long), tmp);
    }
    if (tmp == NULL){
        //element does not exist yet : create and register it
        mapping_element_t *new = mapping_createMappingElement(reviewedFromOurInput, reviewedToAgent, reviewedWithOutput);
        new->id = h;
        HASH_ADD(hh, *hasht, id, sizeof(unsigned long), new);
    }
    free(reviewedFromOurInput);
    free(reviewedToAgent);
    free(reviewedWithOutput);
}

/*
 * Function: json_add_map_cat_to_hash
 * ----------------------------
 *   parse a tab of mapping category type and add them into the corresponding hash table
 */

static void json_add_map_cat_to_hash (mapping_element_t** hasht,
                                       yajl_val current_map_out){
    MASTIC_UNUSED(hasht)
    MASTIC_UNUSED(current_map_out)

    return;
//    const char* agent_name;
//    const char* category_name;
//    struct mapping_cat *map_cat = NULL;
//    yajl_val v;
//    const char * path_in_current[] = { "", (const char *) 0 };
//
//    path_in_current[0] = "map_cat_id";
//    v = yajl_tree_get(current_map_out, path_in_current, yajl_t_any);
//    if (v) {
//        /* check if the key already exist */
//        int map_cat_id = (int) YAJL_GET_INTEGER(v);
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
//            v = yajl_tree_get(current_map_out, path_in_current, yajl_t_any);
//            if (v){
//                agent_name = YAJL_GET_STRING(v);
//                map_cat->agent_name = strdup (agent_name);
//            }
//
//            //category_name
//            path_in_current[0] = "category_name";
//            v = yajl_tree_get(current_map_out, path_in_current, yajl_t_any);
//            if (v){
//                category_name = YAJL_GET_STRING(v);
//                map_cat->category_name = strdup (category_name);
//            }
//
//            HASH_ADD_INT(*hasht , map_cat_id, map_cat);  /* id: name of key field */
//        }
//    }
}

/*
 * Function: json_parse_mapping
 * ----------------------------
 *   convert a map.json file into a mapping (output & category) structure
 */

static mapping_t* json_parse_mapping (yajl_val node) {

    mapping_t* mapp;
    yajl_val v;
    mapp = (mapping_t*) calloc(1, sizeof(mapping_t));
    const char * path[] = { "mapping", "", (const char *) 0 };

    path[1] = STR_NAME;
    v = yajl_tree_get(node, path, yajl_t_any);
    if (v){
        const char* name = YAJL_GET_STRING(v);
        mapp->name = strdup (name);
    }


    path[1] = STR_DESCRIPTION;
    v = yajl_tree_get(node, path, yajl_t_any);
    if (v){
        const char* description = YAJL_GET_STRING(v);
        mapp->description = strdup (description);
    }

    path[1] = STR_VERSION;
    v = yajl_tree_get(node, path, yajl_t_any);
    if (v){
        const char* version = YAJL_GET_STRING(v);
        mapp->version = strdup (version);
    }

    path[1] = "mapping_out";
    v = yajl_tree_get(node, path, yajl_t_array);
    if (v && YAJL_IS_ARRAY(v)){
        unsigned int  i;
        for (i = 0; i < v->u.array.len; i++ ){
            yajl_val obj = v->u.array.values[i];
            if( obj && YAJL_IS_OBJECT(obj))
                json_add_map_out_to_hash (&mapp->map_elements, obj);
        }
    }

    path[1] = "mapping_cat";
    v = yajl_tree_get(node, path, yajl_t_array);
    if (v && YAJL_IS_ARRAY(v)){
        unsigned int  i;
        for (i = 0; i < v->u.array.len; i++ ){
            yajl_val obj = v->u.array.values[i];
            if( obj && YAJL_IS_OBJECT(obj))
                json_add_map_cat_to_hash (&mapp->map_elements, obj);
        }
    }

    return mapp;
}

/*
 * Function: json_dump_iop
 * -----------------------
 *   convert an agent_iop structure into json string
 */

static void json_dump_iop (yajl_gen *g, agent_iop* aiop) {
    
    yajl_gen_map_open(*g);
    
    yajl_gen_string(*g, (const unsigned char *) STR_NAME, strlen(STR_NAME));
    yajl_gen_string(*g, (const unsigned char *) aiop->name, strlen (aiop->name));
    
    yajl_gen_string(*g, (const unsigned char *) STR_TYPE, strlen(STR_TYPE));
    yajl_gen_string(*g, (const unsigned char *) value_type_to_string(aiop->value_type), strlen(value_type_to_string(aiop->value_type)));
    
    yajl_gen_string(*g, (const unsigned char *) STR_VALUE, strlen(STR_VALUE));
    
    switch (aiop->value_type) {
        case INTEGER_T:
            yajl_gen_integer(*g, aiop->value.i);
            break;
        case DOUBLE_T:
            yajl_gen_double(*g, aiop->value.d);
            break;
        case BOOL_T:
            yajl_gen_string(*g, (const unsigned char *) boolean_to_string(aiop->value.b), strlen(boolean_to_string(aiop->value.b)));
            break;
        case STRING_T:
            yajl_gen_string(*g, (const unsigned char *) aiop->value.s, strlen(aiop->value.s));
            break;
        case IMPULSION_T:
            yajl_gen_string(*g, (const unsigned char *) "", 0);
            break;
        case DATA_T:
            yajl_gen_string(*g, (const unsigned char *) "", 0);
            break;
        default:
            fprintf(stderr, "%s - ERROR -  unknown data type to convert in string\n", __FUNCTION__);
            break;
    }
    yajl_gen_map_close(*g);
}

/*
 * Function: json_dump_category
 * ----------------------------
 *   convert a category structure into category.json string
 */

static void json_dump_category (yajl_gen *g, category* cat) {
    
    unsigned int hashCount = 0;
    struct agent_iop *d;
    
    yajl_gen_map_open(*g);
    
    yajl_gen_string(*g, (const unsigned char *) STR_NAME, strlen(STR_NAME));
    yajl_gen_string(*g, (const unsigned char *) cat->name, strlen (cat->name));
    
    yajl_gen_string(*g, (const unsigned char *) STR_VERSION, strlen(STR_VERSION));
    yajl_gen_string(*g, (const unsigned char *) cat->version, strlen(cat->version));
    
    hashCount = HASH_COUNT(cat->params_table);
    if (hashCount) {
        yajl_gen_string(*g, (const unsigned char *) STR_PARAMETERS, strlen(STR_PARAMETERS));
        yajl_gen_array_open(*g);
        for(d=cat->params_table; d != NULL; d=d->hh.next) {
            json_dump_iop (g, d);
        }
        yajl_gen_array_close(*g);
    }
    
    hashCount = HASH_COUNT(cat->inputs_table);
    if (hashCount) {
        yajl_gen_string(*g, (const unsigned char *) STR_INPUTS, strlen(STR_INPUTS));
        yajl_gen_array_open(*g);
        for(d=cat->inputs_table; d != NULL; d=d->hh.next) {
            json_dump_iop (g, d);
        }
        yajl_gen_array_close(*g);
    }
    
    hashCount = HASH_COUNT(cat->outputs_table);
    if (hashCount) {
        yajl_gen_string(*g, (const unsigned char *) STR_OUTPUTS, strlen(STR_OUTPUTS));
        yajl_gen_array_open(*g);
        for(d=cat->outputs_table; d != NULL; d=d->hh.next) {
            json_dump_iop (g, d);
        }
        yajl_gen_array_close(*g);
    }

    yajl_gen_map_close(*g);
}

/*
 * Function: json_dump_definition
 * ------------------------------
 *   convert a definition structure into definition.json string
 */

static void json_dump_definition (yajl_gen *g, definition* def) {
    
    unsigned int hashCount = 0;
    struct agent_iop *d;
    
    yajl_gen_map_open(*g);
    
    yajl_gen_string(*g, (const unsigned char *) STR_NAME, strlen(STR_NAME));
    //Get the agent name from the network layer
    if (def->name == NULL){
        yajl_gen_string(*g, (const unsigned char *) DEF_NO_NAME, strlen (DEF_NO_NAME));
    }else{
        yajl_gen_string(*g, (const unsigned char *) def->name, strlen (def->name));
    }
    
    
    yajl_gen_string(*g, (const unsigned char *) STR_DESCRIPTION, strlen(STR_DESCRIPTION));
    if(def->description != NULL){
        yajl_gen_string(*g, (const unsigned char *) def->description, strlen (def->description));
    } else {
        yajl_gen_string(*g, (const unsigned char *) DEF_NO_DESCRIPTION, strlen(DEF_NO_DESCRIPTION));
    }
    
    yajl_gen_string(*g, (const unsigned char *) STR_VERSION, strlen(STR_VERSION));
    if(def->version != NULL){
        yajl_gen_string(*g, (const unsigned char *) def->version, strlen(def->version));
    } else {
        yajl_gen_string(*g, (const unsigned char *) DEF_NO_VERSION, strlen(DEF_NO_VERSION));
    }
    
    hashCount = HASH_COUNT(def->params_table);
    if (hashCount) {
        yajl_gen_string(*g, (const unsigned char *) STR_PARAMETERS, strlen(STR_PARAMETERS));
        yajl_gen_array_open(*g);
        for(d=def->params_table; d != NULL; d=d->hh.next) {
            json_dump_iop (g, d);
        }
        yajl_gen_array_close(*g);
    }
    
    hashCount = HASH_COUNT(def->inputs_table);
    if (hashCount) {
        yajl_gen_string(*g, (const unsigned char *) STR_INPUTS, strlen(STR_INPUTS));
        yajl_gen_array_open(*g);
        for(d=def->inputs_table; d != NULL; d=d->hh.next) {
            json_dump_iop (g, d);
        }
        yajl_gen_array_close(*g);
    }
    
    hashCount = HASH_COUNT(def->outputs_table);
    if (hashCount) {
        yajl_gen_string(*g, (const unsigned char *) STR_OUTPUTS, strlen(STR_OUTPUTS));
        yajl_gen_array_open(*g);
        for(d=def->outputs_table; d != NULL; d=d->hh.next) {
            json_dump_iop (g, d);
        }
        yajl_gen_array_close(*g);
    }
    
//    struct category *cat;
//    hashCount = HASH_COUNT(def->categories);
//    if (hashCount) {
//        yajl_gen_string(*g, (const unsigned char *) STR_CATEGORIES, strlen(STR_CATEGORIES));
//        yajl_gen_array_open(*g);
//        for(cat=def->categories; cat != NULL; cat=cat->hh.next) {
//            json_dump_category(g, cat);
//        }
//        yajl_gen_array_close(*g);
//    }

    yajl_gen_map_close(*g);
}

/*
 * Function: json_dump_mapping_out
 * -----------------------
 *   convert a mapping_out structure into json string
 */

static void json_dump_mapping_out (yajl_gen *g, mapping_element_t* mapp_out) {

    yajl_gen_map_open(*g);

    yajl_gen_string(*g, (const unsigned char *) "input_name", strlen("input_name"));
    yajl_gen_string(*g, (const unsigned char *) mapp_out->input_name, strlen (mapp_out->input_name));

    yajl_gen_string(*g, (const unsigned char *) "agent_name", strlen("agent_name"));
    yajl_gen_string(*g, (const unsigned char *) mapp_out->agent_name, strlen(mapp_out->agent_name));

    yajl_gen_string(*g, (const unsigned char *) "output_name", strlen("output_name"));
    yajl_gen_string(*g, (const unsigned char *) mapp_out->output_name, strlen(mapp_out->output_name));

    yajl_gen_map_close(*g);
}

/*
 * Function: json_dump_mapping_cat
 * -----------------------
 *   convert a mapping_cat structure into json string
 */

//static void json_dump_mapping_cat (yajl_gen *g, mapping_cat* mapp_cat) {
//
//    yajl_gen_map_open(*g);
//
//    yajl_gen_string(*g, (const unsigned char *) "agent_name", strlen("agent_name"));
//    yajl_gen_string(*g, (const unsigned char *) mapp_cat->agent_name, strlen (mapp_cat->agent_name));
//
//    yajl_gen_string(*g, (const unsigned char *) "category_name", strlen("category_name"));
//    yajl_gen_string(*g, (const unsigned char *) mapp_cat->category_name, strlen(mapp_cat->category_name));
//
//    yajl_gen_map_close(*g);
//}

/*
 * Function: json_dump_mapping
 * ------------------------------
 *   convert a mapping structure into mapping.json string
 */

static void json_dump_mapping (yajl_gen *g, mapping_t* mapp) {

    unsigned int hashCount = 0;
    mapping_element_t *currentMapOut = NULL;

    if(mapp != NULL)
    {
        //    struct mapping_cat *currentMapCat = NULL;

        yajl_gen_map_open(*g);

        yajl_gen_string(*g, (const unsigned char *) STR_NAME, strlen(STR_NAME));
        if(mapp->name != NULL)
            yajl_gen_string(*g, (const unsigned char *) mapp->name, strlen (mapp->name));
        else
            yajl_gen_string(*g, (const unsigned char *) MAP_NO_NAME, strlen(MAP_NO_NAME));

        yajl_gen_string(*g, (const unsigned char *) STR_DESCRIPTION, strlen(STR_DESCRIPTION));
        if(mapp->description != NULL)
            yajl_gen_string(*g, (const unsigned char *) mapp->description, strlen (mapp->description));
        else
            yajl_gen_string(*g, (const unsigned char *) MAP_NO_DESCRIPTION, strlen(MAP_NO_DESCRIPTION));

        yajl_gen_string(*g, (const unsigned char *) STR_VERSION, strlen(STR_VERSION));
        if(mapp->version != NULL)
            yajl_gen_string(*g, (const unsigned char *) mapp->version, strlen(mapp->version));
        else
            yajl_gen_string(*g, (const unsigned char *) MAP_NO_VERSION, strlen(MAP_NO_VERSION));

        //Mapping_out
        hashCount = HASH_COUNT(mapp->map_elements);
        if (hashCount) {
            yajl_gen_string(*g, (const unsigned char *) "mapping_out", strlen("mapping_out"));
            yajl_gen_array_open(*g);
            for(currentMapOut = mapp->map_elements; currentMapOut != NULL; currentMapOut=currentMapOut->hh.next) {
                json_dump_mapping_out(g, currentMapOut);
            }
            yajl_gen_array_close(*g);
        }

    //    //Mapping_cat
    //    hashCount = HASH_COUNT(mapp->map_cat);
    //    if (hashCount) {
    //        yajl_gen_string(*g, (const unsigned char *) "mapping_cat", strlen("mapping_cat"));
    //        yajl_gen_array_open(*g);
    //        for(currentMapCat=mapp->map_cat; currentMapCat != NULL; currentMapCat=currentMapOut->hh.next) {
    //            json_dump_mapping_cat(g, currentMapCat);
    //        }
    //        yajl_gen_array_close(*g);
    //    }

        yajl_gen_map_close(*g);
    }
}

/*
 * Function: load_category
 * ----------------------------
 *   Load a category in the standartised format JSON to initialize a category structure from string.
 *   The category structure is dynamically allocated. You will have to use free_category function to deallocate it correctly.
 *
 *   json_str      : a string (json format)
 *
 *   returns: a pointer on a category structure or NULL if it has failed
 */

category* load_category (const char* json_str) {
    
    category *cat = NULL;
    yajl_val node;
    
    json_tokenized(json_str, &node);
    cat = json_parse_category (node);
    
    yajl_tree_free(node);
    node = NULL;
    
    return cat;
}

category* load_category_from_path (const char* path) {

    char *json_str = NULL;
    category *cat = NULL;
    
    json_str = json_fetch(path);
    if (!json_str)
        return NULL;

    cat = load_category (json_str);
    
    free (json_str);
    json_str = NULL;

    return cat;
}

/*
 * Function: export_category
 * ----------------------------
 *   Returns a categorie structure into a standartised format json string UTF8 to send it throught the BUS or save it in a file
 *
 *   cat    : the category dump in string
 *
 *   returns: a category json format string UTF8
 */

const char* export_category (category* cat) {
   
    const char* result = NULL;
    const unsigned char* json_str = NULL;
    size_t len;
    yajl_gen g;
    
    g = yajl_gen_alloc(NULL);
    yajl_gen_config(g, yajl_gen_beautify, 1);
    yajl_gen_config(g, yajl_gen_validate_utf8, 1);
    
    yajl_gen_map_open(g);
    yajl_gen_string(g, (const unsigned char *) STR_CATEGORY, strlen(STR_CATEGORY));
    json_dump_category(&g, cat);
    yajl_gen_map_close(g);
    
    // try to get our dumping result
    if (yajl_gen_get_buf(g, &json_str, &len) == yajl_gen_status_ok)
    {
        result = strdup((const char*) json_str);
    }
    
    yajl_gen_free(g);
    
    return result;
}



/*
 * Function: init_mapping
 * ----------------------------
 *   read mapping from file path and init inernal mapping data
 *
 *   mapping_file_path : path to the agent mapping file
 *
 */

int mtic_init_mapping (const char* mapping_file_path)
{
    int errorCode = -1;

    if (mapping_file_path != NULL){
        // Init definition
        mtic_internal_mapping = parser_LoadMapFromPath(mapping_file_path);
        if(mtic_internal_mapping == NULL)
        {
            fprintf(stderr, "Error : Mapping file has not been loaded : %s\n", mapping_file_path );
            exit(EXIT_FAILURE);
        } else {
            errorCode = 0;
        }
    }

    return errorCode;
}

/*
 * Function: mtic_init_internal_data
 * ----------------------------
 *   read definition from file path and init inernal agent data
 *   initialize definition_load and mtic_internal_definition data structures
 *
 *   definition_file_path : path to the agent definiton file
 *
 */

int mtic_init_internal_data (const char* definition_file_path)
{
    int errorCode = -1;
    if (definition_file_path != NULL){
        // Init definition
        mtic_internal_definition = parser_loadDefinitionFromPath(definition_file_path);

        if(mtic_internal_definition != NULL)
        {
            // Live data corresponds to a copy of the initial definition
            mtic_internal_definition = calloc(1, sizeof(struct definition));
            errorCode = 0;
        } else {
            fprintf(stderr, "Error : Definition file has not been loaded : %s\n", definition_file_path );
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Error : Definition file has not been specified\n");
        exit(EXIT_FAILURE);
    }

    return errorCode;
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
/*
 * Function: load_definition
 * ----------------------------
 *   Load a agent definition in the standartised format JSON to initialize a definition structure from a string.
 *   The definition structure is dynamically allocated. You will have to use definition_freeDefinition function to deallocated it correctly.
 *
 *   json_str      : a string (json format)
 *
 *   returns: a pointer on a category structure or NULL if it has failed
 */

definition* parser_loadDefinition (const char* json_str) {
    
    definition *def = NULL;
    yajl_val node;
    
    json_tokenized(json_str, &node);
    def = json_parse_definition (node);
    
    yajl_tree_free(node);
    node = NULL;
    
    return def;
}

/*
 * Function: load_definition_from_path
 * -----------------------------------
 *   Load a agent definition in the standartised format JSON to initialize a definition structure from a local file path.
 *   The definition structure is dynamically allocated. You will have to use definition_freeDefinition function to deallocated it correctly.
 *
 *   file_path      : the file path
 *
 *   returns: a pointer on a category structure or NULL if it has failed
 */

definition * parser_loadDefinitionFromPath (const char* path) {
    
    char *json_str = NULL;
    definition *def = NULL;
    
    json_str = json_fetch(path);
    if (!json_str)
        return 0;
    
    def = parser_loadDefinition(json_str);
    
    free (json_str);
    json_str = NULL;
    
    return def;
}

/*
 * Function: parser_export_definition
 * ----------------------------
 *   Returns a agent's definition structure into a standartised format json string UTF8 to send it throught the BUS or save it in a file
 *
 *   def    : the agent's definition dump in string
 *
 *   returns: a definition json format string UTF8
 */

char* parser_export_definition (definition* def) {
    
    char* result = NULL;
    if (def != NULL){
        const unsigned char * json_str = NULL;
        size_t len;
        yajl_gen g;
        
        g = yajl_gen_alloc(NULL);
        yajl_gen_config(g, yajl_gen_beautify, 1);
        yajl_gen_config(g, yajl_gen_validate_utf8, 1);
        
        yajl_gen_map_open(g);
        yajl_gen_string(g, (const unsigned char *) STR_DEFINITION, strlen(STR_DEFINITION));
        
        if(def != NULL)
        {
            json_dump_definition(&g, def);
        }
        yajl_gen_map_close(g);
        
        // try to get our dumping result
        if (yajl_gen_get_buf(g, &json_str, &len) == yajl_gen_status_ok)
        {
            result = strdup((const char*) json_str);
        }
        
        yajl_gen_free(g);
    }
    return result;
}

/*
 * Function: parser_export_mapping
 * ----------------------------
 *   Returns a agent's mapping structure into a standartised format json string UTF8 to send it throught the BUS or save it in a file
 *
 *   mapp    : the agent's mapping dump in string
 *
 *   returns: a mapping json format string UTF8
 */

char* parser_export_mapping(mapping_t *mapp){
    char* result = NULL;
    if (mapp != NULL){
        const unsigned char * json_str = NULL;
        size_t len;
        yajl_gen g;
        
        g = yajl_gen_alloc(NULL);
        yajl_gen_config(g, yajl_gen_beautify, 1);
        yajl_gen_config(g, yajl_gen_validate_utf8, 1);
        
        yajl_gen_map_open(g);
        yajl_gen_string(g, (const unsigned char *) "mapping", strlen("mapping"));
        json_dump_mapping(&g, mapp);
        yajl_gen_map_close(g);
        
        // try to get our dumping result
        if (yajl_gen_get_buf(g, &json_str, &len) == yajl_gen_status_ok)
        {
            result = strdup((const char*) json_str);
        }
        
        yajl_gen_free(g);
    }
    return result;
}

/*
 * Function: load_map
 * ------------------
 *   Load a mapping in the standartised format JSON to initialize a mapping structure from a string.
 *   The mapping structure is dynamically allocated. You will have to use free_mapping function to deallocated it correctly.
 *
 *   json_str      : a string (json format)
 *
 *   returns : a pointer on a mapping structure or NULL if it has failed
 */

mapping_t* parser_LoadMap(const char* json_str){
    
    mapping_t *mapp = NULL;
    yajl_val node;
    
    json_tokenized(json_str, &node);
    mapp = json_parse_mapping (node);
    
    //Copy the mapp structure to the global variable map
    //copy_to_map_global(mapp);
    
    yajl_tree_free(node);
    node = NULL;
    
    return mapp;
}

/*
 * Function: load_map_from_path
 * ----------------------------
 *   Load a mapping in the standartised format JSON to initialize a mapping structure from a local file path.
 *   The mapping structure is dynamically allocated. You will have to use free_mapping function to deallocated it correctly.
 *
 *   file_path      : the file path
 *
 *   returns : a pointer on a mapping structure or NULL if it has failed
 */

mapping_t* parser_LoadMapFromPath (const char* path){
    
    char *json_str = NULL;
    mapping_t *mapp = NULL;
    
    json_str = json_fetch(path);
    if (!json_str)
        return NULL;
    
    mapp = parser_LoadMap(json_str);
    
    free (json_str);
    json_str = NULL;
    
    return mapp;
}


////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////

/**
 * \fn int mtic_loadDefinition (const char* json_str)
 * \ingroup loadSetGetDefFct
 * \brief load definition in variable 'mtic_definition_loaded' & copy in 'mtic_internal_definition"
 *      from a json string
 *
 * \param json_str String in json format. Can't be NULL.
 * \return The error. 1 is OK, 0 json string is NULL, -1 Definition file has not been loaded
 */
int mtic_loadDefinition (const char* json_str){
    
    //Check if the json string is null
    if(json_str == NULL)
    {
        mtic_debug("mtic_loadDefinition : json string is null \n");
        return 0;
    }

    //Try to load definition
    definition *tmp = parser_loadDefinition(json_str);

    if(tmp == NULL)
    {
        mtic_debug("mtic_loadDefinition : json string caused an error and was ignored\n%s\n", json_str );
        return -1;
    }else{
        if (mtic_internal_definition != NULL){
            definition_freeDefinition(mtic_internal_definition);
            mtic_internal_definition = NULL;
        }
        mtic_internal_definition = tmp;
        //Check the name of agent from network layer
        char *name = mtic_getAgentName();
        if(strcmp(name, AGENT_NAME_DEFAULT) == 0){
            //The name of the agent is default : we change it to definition name
            mtic_setAgentName(mtic_internal_definition->name);
        }//else
            //The agent name was assigned by the developer : we keep it untouched
        free(name);
        network_needToSendDefinitionUpdate = true;
    }

    return 1;
}

/**
 * \fn int mtic_loadDefinitionFromPath (const char* file_path)
 * \ingroup loadSetGetDefFct
 * \brief load definition in variable 'mtic_definition_loaded' & copy in 'mtic_internal_definition"
 *      from a file path
 *
 * \param file_path The string which contains the json file path. Can't be NULL.
 * \return The error. 1 is OK, 0 json string is NULL, -1 Definition file has not been loaded
 */
int mtic_loadDefinitionFromPath (const char* file_path){
    
    //Check if the json string is null
    if(file_path == NULL)
    {
        mtic_debug("Error : file path is null \n");
        return 0;
    }

    //Try to load definition
    definition *tmp = parser_loadDefinitionFromPath(file_path);
    

    if(tmp == NULL)
    {
        mtic_debug("mtic_loadDefinitionFromPath : %s caused an error and was ignored\n", file_path);
        return -1;
    }else{
        strncpy(definitionPath, file_path, 1023);
        if (mtic_internal_definition != NULL){
            definition_freeDefinition(mtic_internal_definition);
            mtic_internal_definition = NULL;
        }
        mtic_internal_definition = tmp;
        //Check the name of agent from network layer
        char *name = mtic_getAgentName();
        if(strcmp(name, AGENT_NAME_DEFAULT) == 0){
            //The name of the agent is default : we change it to definition name
            mtic_setAgentName(mtic_internal_definition->name);
        }//else
            //The agent name was assigned by the developer : we keep it untouched
        free(name);
        network_needToSendDefinitionUpdate = true;
    }

    return 1;
}

