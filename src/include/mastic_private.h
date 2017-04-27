//
//  mastic_private.h
//  mastic
//
//  Created by Stephane Vales on 27/04/2017.
//  Copyright © 2017 Ingenuity i/o. All rights reserved.
//

#ifndef mastic_private_h
#define mastic_private_h

#if defined WINDOWS
#if defined MASTIC
#define MASTICAPI_COMMON_DLLSPEC __declspec(dllexport)
#else
#define MASTICAPI_COMMON_DLLSPEC __declspec(dllimport)
#endif
#else
#define MASTICAPI_COMMON_DLLSPEC extern
#endif

#include "uthash/uthash.h"
#include "mapping.h"

//////////////////  definition   //////////////////
/*
 * Define the structure agent_iop (input, output, parameter) :
 * 'name'       : the input/output/parameter's name
 * 'type'       : the input/output/parameter'value type (int, double, ...)
 * 'value'      : the input/output/parameter'value
 * 'is_muted'   : the input/output/parameter muted
 * NB : it's true that 3 'const char*' are defined and it's useless, only to differentiate in accordance to the type for using ex. 'e1.impulse = 'released()'
 */
struct agent_iop {
    const char * name;          //Need to be unique : the table hash key
    value_type type;
    union {
        int i;                  //in accordance to type INTEGER ex. '10'
        double d;               //in accordance to type DOUBLE ex. '10.01'
        char* s;                //in accordance to type STRING ex. 'display the image'
        bool b;                 //in accordance to type BOOL ex. 'TRUE'
        char* impuls;           //in accordance to type IMPULSION ex. 'released()'
        char* strct;            //in accordance to type STRUCTURE ex. '{int:x, int:y, string:gesture_name} <=> {int:10, int:45, string:swap}
    } old_value;
    union {
        int i;                  //in accordance to type INTEGER ex. '10'
        double d;               //in accordance to type DOUBLE ex. '10.01'
        char* s;                //in accordance to type STRING ex. 'display the image'
        bool b;                 //in accordance to type BOOL ex. 'TRUE'
        char* impuls;           //in accordance to type IMPULSION ex. 'released()'
        char* strct;            //in accordance to type STRUCTURE ex. '{int:x, int:y, string:gesture_name} <=> {int:10, int:45, string:swap}
    } value;
    bool is_muted;              // flag indicated if the iop is muted (specially used for outputs)
    UT_hash_handle hh;         /* makes this structure hashable */
};

/*
 * Define the structure DEFINITION :
 * 'name'                   : agent name
 * 'description'            : human readable description of the agent
 * 'version'                : the version of the agent
 * 'parameters'             : list of parameters contains by the agent
 * 'inputs'                 : list of inputs contains by the agent
 * 'outputs'                : list of outputs contains by the agent
 */
typedef struct definition {
    const char * name; //Need to be unique the key
    const char * description;
    const char * version;
    category * categories;
    agent_iop* params_table;
    agent_iop* inputs_table;
    agent_iop* outputs_table;
    UT_hash_handle hh;
} definition;

/*
 * Define the structure CATEGORY :
 * 'name'                   : name of the category
 * 'version'                : version of the category
 * 'parameters'             : list of parameters which describe the category
 * 'inputs'                 : list of inputs which describe the category
 * 'outputs'                : list of outputs which describe the category
 */
typedef struct category {
    const char* name;
    const char * version;
    agent_iop* params_table;
    agent_iop* inputs_table;
    agent_iop* outputs_table;
    UT_hash_handle hh;
} category;

MASTICAPI_COMMON_DLLSPEC definition * mtic_definition_loaded;
MASTICAPI_COMMON_DLLSPEC definition * mtic_definition_live;
MASTICAPI_COMMON_DLLSPEC definition * mtic_agents_defs_on_network;


//////////////////  mapping   //////////////////
/*
 * Define the structure 'mapping_out' which contains mapping between an input and an (one or all) external agent's output :
 * 'map_id'                 : the key of the table
 * 'input name'             : agent's input name to connect
 * 'agent name to connect'  : external agent's name to connect with (one or all)
 * 'output name to connect' : external agent(s) output name to connect with
 */
typedef struct mapping_out {
    int map_id; //Need to be unique : the table hash key
    char *input_name;
    char *agent_name;
    char *output_name;
    map_state state;
    UT_hash_handle hh;
} mapping_out;

/*
 * Define the structure 'mapping_cat' which contains mapping between an input and an (one or all) external agent's category :
 * 'map_id'                         : the key of the table
 * 'agent name to connect'          : external agent's name to connect with (one or all)
 * 'category unique name to connect': external agent(s) category to connect with
 */
typedef struct mapping_cat {
    int map_cat_id;//Need to be unique : the table hash key
    char* agent_name;
    char *category_name;
    map_state state;
    UT_hash_handle hh;
} mapping_cat;

/*
 * Define the structure 'mapping' which contains the json description of all mapping (output & category):
 * 'name'           : The name of the mapping need to be unique
 * 'description     :
 * 'version'        :
 * 'mapping out'    : the table of the mapping output
 * 'mapping cat'    : the table of the mapping category
 */
typedef struct mapping {
    char *name;//Need to be unique : the table hash key
    char *description;
    char *version;
    mapping_out *map_out;
    mapping_cat *map_cat;
    UT_hash_handle hh;
} mapping;

// the table which will contain the mapping
MASTICAPI_COMMON_DLLSPEC mapping * mtic_my_agent_mapping;



#endif /* mastic_private_h */
