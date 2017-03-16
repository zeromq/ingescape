//
//  parseur.c
//
//  Created by Patxi Berard
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

#include "yajl/yajl_tree.h"
#include <yajl/yajl_gen.h>
#include "parser.h"

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



// --------- static functions used for json parsing --------------------//

/*
 * Function: json_add_data_to_hash
 * ----------------------------
 *   parse a agent_iop data and add it to the corresponding hash table
 */

static void json_add_data_to_hash (struct agent_iop_t** hasht,
                                   yajl_val obj){

    struct agent_iop_t *data = NULL;

    /* check if the key already exist */
    const char* name = YAJL_GET_STRING(obj->u.object.values[0]);
    HASH_FIND_STR(*hasht, name , data);
    if (data == NULL) {
        data = calloc (1, sizeof (struct agent_iop_t));
        data->name = strdup (name);

        data->type = string_to_value_type (YAJL_GET_STRING(obj->u.object.values[1]));
        switch (data->type) {
            case INTEGER:
                data->value.i =(int) YAJL_GET_INTEGER (obj->u.object.values[2]);
                break;
            case DOUBLE_TYPE:
                data->value.d = YAJL_GET_DOUBLE (obj->u.object.values[2]);
                break;
            case BOOL_TYPE:
                data->value.b = string_to_boolean (YAJL_GET_STRING(obj->u.object.values[2]));
                break;
            case STRING:
                data->value.s = strdup (YAJL_GET_STRING(obj->u.object.values[2]));
                break;
            case IMPULSION:
                data->value.impuls = strdup (YAJL_GET_STRING(obj->u.object.values[2]));
                break;
            case STRUCTURE:
                data->value.strct = strdup (YAJL_GET_STRING(obj->u.object.values[2]));
                break;
            default:
                break;
        }
        data->is_muted = false;
        HASH_ADD_STR(*hasht , name, data );  /* id: name of key field */
    }
}

/*
 * Function: jason_add_data
 * ----------------------------
 *   parse a tab of agent_iop data and add them into the corresponding hash table
 */

static void json_add_data (yajl_val node, const char** path,
                            struct agent_iop_t** hasht) {
    yajl_val v;
    v = yajl_tree_get(node, path, yajl_t_array);
    if (v && YAJL_IS_ARRAY(v))
        for (int  i = 0; i < v->u.array.len; i++ ){
            yajl_val obj = v->u.array.values[i];
            if( obj && YAJL_IS_OBJECT(obj))
                json_add_data_to_hash (hasht, obj);
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
    int rd;

    file = fopen(path, "r");
    if (!file){
        fprintf(stderr, "json parser - ERROR - fopen(): file %s NOT found\n", path);
        return 0;
    }

    /* read the whole config file */
    do {

        rd = (int) fread(buff, 1, sizeof(buff) , file);
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
    struct category_t* cat = NULL;
    cat = (category*) calloc(1, sizeof(category));
    const char * path[] = { STR_CATEGORY, "",  (const char *) 0 };

    path[1] = STR_NAME;
    v = yajl_tree_get(node, path, yajl_t_any);
    if (v)
        cat->name = strdup (YAJL_GET_STRING(v));

    path[1] = STR_VERSION;
    v = yajl_tree_get(node, path, yajl_t_any);
    if (v)
        cat->version = strdup (YAJL_GET_STRING(v));

    path[1] = STR_PARAMETERS;
    json_add_data (node, path, &cat->params_table);

    path[1] = STR_INPUTS;
    json_add_data (node, path, &cat->inputs_table);

    path[1] = STR_OUTPUTS;
    json_add_data (node, path, &cat->outputs_table);

    return cat;
}

/*
 * Function: json_add_category_to_hash
 * ----------------------------
 *   parse a tab of categories and add them into the corresponding hash table
 */

static void json_add_category_to_hash (struct category_t** hasht,
                                       yajl_val current_cat){

    struct category_t *cat = NULL;
    yajl_val v;
    const char * path_in_current[] = { "", (const char *) 0 };

    path_in_current[0] = STR_NAME;
    v = yajl_tree_get(current_cat, path_in_current, yajl_t_any);
    if (v) {
        /* check if the key already exist */
        const char* name = YAJL_GET_STRING(v);
        HASH_FIND_STR(*hasht, name , cat);
        if (cat == NULL){
            cat = calloc (1, sizeof (struct category_t));
            cat->name = strdup (name);

            path_in_current[0] = STR_VERSION;
            v = yajl_tree_get(current_cat, path_in_current, yajl_t_any);
            if (v)
                cat->version = strdup (YAJL_GET_STRING(v));

            path_in_current[0] = STR_PARAMETERS;
            json_add_data (current_cat, path_in_current, &cat->params_table);

            path_in_current[0] = STR_INPUTS;
            json_add_data (current_cat, path_in_current, &cat->inputs_table);

            path_in_current[0] = STR_OUTPUTS;
            json_add_data (current_cat, path_in_current, &cat->outputs_table);

            HASH_ADD_STR(*hasht , name, cat );  /* id: name of key field */
        }
    }
}

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
    if (v)
        def->name = strdup (YAJL_GET_STRING(v));

    path[1] = STR_DESCRIPTION;
    v = yajl_tree_get(node, path, yajl_t_any);
    if (v)
        def->description = strdup (YAJL_GET_STRING(v));

    path[1] = STR_VERSION;
    v = yajl_tree_get(node, path, yajl_t_any);
    if (v)
        def->version = strdup (YAJL_GET_STRING(v));

    path[1] = STR_INPUTS;
    json_add_data (node, path, &def->inputs_table);

    path[1] = STR_OUTPUTS;
    json_add_data (node, path, &def->outputs_table);

    path[1] = STR_PARAMETERS;
    json_add_data (node, path, &def->params_table);

    path[1] = STR_CATEGORIES;
    v = yajl_tree_get(node, path, yajl_t_array);
    if (v && YAJL_IS_ARRAY(v)){
        for (int  i = 0; i < v->u.array.len; i++ ){
            yajl_val obj = v->u.array.values[i];
            if( obj && YAJL_IS_OBJECT(obj))
                json_add_category_to_hash (&def->categories, obj);
        }
    }

    return def;
}

/*
 * Function: json_add_map_out_to_hash
 * ----------------------------
 *   parse a tab of mapping output type and add them into the corresponding hash table
 */

static void json_add_map_out_to_hash (struct mapping_out_t** hasht,
                                       yajl_val current_map_out){

    const char* input_name;
    const char* agent_name;
    const char* output_name;
    struct mapping_out_t *map_out = NULL;
    yajl_val v;
    const char * path_in_current[] = { "", (const char *) 0 };

    //generate the map_id (hash key) in dynamic
    int map_id = 0;
    map_id = HASH_COUNT(*hasht) + 1;

    HASH_FIND_INT(*hasht, &map_id , map_out);

    if (map_out == NULL){
        map_out = calloc (1, sizeof (struct mapping_out_t));
        map_out->map_id = map_id;
        map_out->state = OFF;

        //input_name
        path_in_current[0] = "input_name";
        v = yajl_tree_get(current_map_out, path_in_current, yajl_t_any);
        if (v){
            input_name = YAJL_GET_STRING(v);
            map_out->input_name = strdup (input_name);
        }

        //agent_name
        path_in_current[0] = "agent_name";
        v = yajl_tree_get(current_map_out, path_in_current, yajl_t_any);
        if (v){
            agent_name = YAJL_GET_STRING(v);
            map_out->agent_name = strdup (agent_name);
        }


        //output_name
        path_in_current[0] = "output_name";
        v = yajl_tree_get(current_map_out, path_in_current, yajl_t_any);
        if (v){
            output_name = YAJL_GET_STRING(v);
            map_out->output_name = strdup (output_name);
        }

        HASH_ADD_INT(*hasht , map_id, map_out );  /* id: name of key field */
    }
}

/*
 * Function: json_add_map_cat_to_hash
 * ----------------------------
 *   parse a tab of mapping category type and add them into the corresponding hash table
 */

static void json_add_map_cat_to_hash (struct mapping_cat_t** hasht,
                                       yajl_val current_map_out){

    const char* agent_name;
    const char* category_name;
    struct mapping_cat_t *map_cat = NULL;
    yajl_val v;
    const char * path_in_current[] = { "", (const char *) 0 };

    path_in_current[0] = "map_cat_id";
    v = yajl_tree_get(current_map_out, path_in_current, yajl_t_any);
    if (v) {
        /* check if the key already exist */
        int map_cat_id = (int) YAJL_GET_INTEGER(v);

        HASH_FIND_INT(*hasht, &map_cat_id , map_cat);

        if (map_cat == NULL){
            map_cat = calloc (1, sizeof (struct mapping_cat_t));
            map_cat->map_cat_id = map_cat_id;
            map_cat->state = OFF;

            //agent_name
            path_in_current[0] = "agent_name";
            v = yajl_tree_get(current_map_out, path_in_current, yajl_t_any);
            if (v){
                agent_name = YAJL_GET_STRING(v);
                map_cat->agent_name = strdup (agent_name);
            }

            //category_name
            path_in_current[0] = "category_name";
            v = yajl_tree_get(current_map_out, path_in_current, yajl_t_any);
            if (v){
                category_name = YAJL_GET_STRING(v);
                map_cat->category_name = strdup (category_name);
            }

            HASH_ADD_INT(*hasht , map_cat_id, map_cat);  /* id: name of key field */
        }
    }
}

/*
 * Function: json_parse_mapping
 * ----------------------------
 *   convert a map.json file into a mapping (output & category) structure
 */

static mapping* json_parse_mapping (yajl_val node) {

    mapping* mapp;
    yajl_val v;
    mapp = (mapping*) calloc(1, sizeof(mapping));
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
        for (int  i = 0; i < v->u.array.len; i++ ){
            yajl_val obj = v->u.array.values[i];
            if( obj && YAJL_IS_OBJECT(obj))
                json_add_map_out_to_hash (&mapp->map_out, obj);
        }
    }

    path[1] = "mapping_cat";
    v = yajl_tree_get(node, path, yajl_t_array);
    if (v && YAJL_IS_ARRAY(v)){
        for (int  i = 0; i < v->u.array.len; i++ ){
            yajl_val obj = v->u.array.values[i];
            if( obj && YAJL_IS_OBJECT(obj))
                json_add_map_cat_to_hash (&mapp->map_cat, obj);
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
    yajl_gen_string(*g, (const unsigned char *) value_type_to_string(aiop->type), strlen(value_type_to_string(aiop->type)));
    
    yajl_gen_string(*g, (const unsigned char *) STR_VALUE, strlen(STR_VALUE));
    
    switch (aiop->type) {
        case INTEGER:
            yajl_gen_integer(*g, aiop->value.i);
            break;
        case DOUBLE_TYPE:
            yajl_gen_double(*g, aiop->value.d);
            break;
        case BOOL_TYPE:
            yajl_gen_string(*g, (const unsigned char *) boolean_to_string(aiop->value.b), strlen(boolean_to_string(aiop->value.b)));
            break;
        case STRING:
            yajl_gen_string(*g, (const unsigned char *) aiop->value.s, strlen(aiop->value.s));
            break;
        case IMPULSION:
            yajl_gen_string(*g, (const unsigned char *) aiop->value.impuls, strlen(aiop->value.impuls));
            break;
        case STRUCTURE:
            yajl_gen_string(*g, (const unsigned char *) aiop->value.strct, strlen(aiop->value.strct));
            break;
        default:
            fprintf(stderr, "%s - ERROR -  unknown value type to convert in string\n", __FUNCTION__);
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
    struct agent_iop_t *d;
    
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
    struct agent_iop_t *d;
    
    yajl_gen_map_open(*g);
    
    yajl_gen_string(*g, (const unsigned char *) STR_NAME, strlen(STR_NAME));
    yajl_gen_string(*g, (const unsigned char *) def->name, strlen (def->name));
    
    yajl_gen_string(*g, (const unsigned char *) STR_DESCRIPTION, strlen(STR_DESCRIPTION));
    yajl_gen_string(*g, (const unsigned char *) def->description, strlen (def->description));
    
    yajl_gen_string(*g, (const unsigned char *) STR_VERSION, strlen(STR_VERSION));
    yajl_gen_string(*g, (const unsigned char *) def->version, strlen(def->version));
    
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
    
    struct category_t *cat;
    hashCount = HASH_COUNT(def->categories);
    if (hashCount) {
        yajl_gen_string(*g, (const unsigned char *) STR_CATEGORIES, strlen(STR_CATEGORIES));
        yajl_gen_array_open(*g);
        for(cat=def->categories; cat != NULL; cat=cat->hh.next) {
            json_dump_category(g, cat);
        }
        yajl_gen_array_close(*g);
    }

    yajl_gen_map_close(*g);
}


// --------- Public API for json parsing / dumping --------------------//

mapping* load_map(const char* json_str){
    
    mapping *mapp = NULL;
    yajl_val node;
    
    json_tokenized(json_str, &node);
    mapp = json_parse_mapping (node);

    //Copy the mapp structure to the global variable map
    //copy_to_map_global(mapp);
    
    yajl_tree_free(node);
    node = NULL;
    
    return mapp;
}

mapping* load_map_from_path (const char* path){

    char *json_str = NULL;
    mapping *mapp = NULL;

    json_str = json_fetch(path);
    if (!json_str)
        return NULL;

    mapp = load_map(json_str);

    free (json_str);
    json_str = NULL;

    return mapp;
}

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

char* export_definition (definition* def) {
    
    char* result = NULL;
    const unsigned char * json_str = NULL;
    size_t len;
    yajl_gen g;
    
    g = yajl_gen_alloc(NULL);
    yajl_gen_config(g, yajl_gen_beautify, 1);
    yajl_gen_config(g, yajl_gen_validate_utf8, 1);
    
    yajl_gen_map_open(g);
    yajl_gen_string(g, (const unsigned char *) STR_DEFINITION, strlen(STR_DEFINITION));
    json_dump_definition(&g, def);
    yajl_gen_map_close(g);
    
    // try to get our dumping result
    if (yajl_gen_get_buf(g, &json_str, &len) == yajl_gen_status_ok)
    {
        result = strdup((const char*) json_str);
    }
    
    yajl_gen_free(g);
    
    return result;
}

definition * load_definition (const char* json_str) {
    
    definition *def = NULL;
    yajl_val node;
    
    json_tokenized(json_str, &node);
    def = json_parse_definition (node);
    
    yajl_tree_free(node);
    node = NULL;
    
    return def;
}

definition * load_definition_from_path (const char* path) {

    char *json_str = NULL;
    definition *def = NULL;

    json_str = json_fetch(path);
    if (!json_str)
        return 0;

    def = load_definition(json_str);

    free (json_str);
    json_str = NULL;

    return def;
}


int init_mapping (const char* mapping_file_path)
{
    int errorCode = -1;

    if (mapping_file_path != NULL){
        // Init definition
        my_agent_mapping = load_map_from_path(mapping_file_path);
        if(my_agent_mapping == NULL)
        {
            fprintf(stderr, "Error : Mapping file has not been loaded : %s\n", mapping_file_path );
            exit(EXIT_FAILURE);
        } else {
            errorCode = 0;
        }
    }

    return errorCode;
}


int init_internal_data (const char* definition_file_path)
{
    int errorCode = -1;
    if (definition_file_path != NULL){
        // Init definition
        definition_loaded = load_definition_from_path(definition_file_path);

        if(definition_loaded != NULL)
        {
            // Live data corresponds to a copy of the initial definition
            definition_live = calloc(1, sizeof(struct definition_t));
            memcpy(definition_live, definition_loaded, sizeof(*definition_loaded));
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


