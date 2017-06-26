//
//  mastic_private.h
//  mastic
//
//  Created by Stephane Vales on 27/04/2017.
//  Modified by Mathieu Poirier
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
#define MASTICAPI_COMMON_DLLSPEC
#endif

#include <stdbool.h>
#include <string.h>

#include "uthash/uthash.h"

#ifdef _WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <iphlpapi.h>
#else
# define MAX_PATH 260
#endif

#include "mastic.h"

//////////////////  structures and enums   //////////////////

//definition //////////////////
/*
 * The variable 'value_type' contains the data type of the value, use to parse
 * the JSON and affect the value tu right union corresponding
 */
//TODO : replace by iopType_t in mastic.h and remove
//typedef enum {
//    INTEGER,
//    DOUBLE_TYPE,
//    STRING,
//    BOOL,
//    IMPULSION,
//    STRUCTURE
//} value_type;

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
    iopType_t type;
    union {
        int i;                  //in accordance to type INTEGER_T ex. '10'
        double d;               //in accordance to type DOUBLE ex. '10.01'
        char* s;                //in accordance to type STRING ex. 'display the image'
        bool b;                 //in accordance to type BOOL ex. 'TRUE'
        char* impuls;           //in accordance to type IMPULSION ex. 'released()'
        char* data;            //in accordance to type DATA ex. '{int:x, int:y, string:gesture_name} <=> {int:10, int:45, string:swap}
    } old_value;
    union {
        int i;                  //in accordance to type INTEGER_T ex. '10'
        double d;               //in accordance to type DOUBLE ex. '10.01'
        char* s;                //in accordance to type STRING ex. 'display the image'
        bool b;                 //in accordance to type BOOL ex. 'TRUE'
        char* impuls;           //in accordance to type IMPULSION ex. 'released()'
        char* data;            //in accordance to type DATA ex. '{int:x, int:y, string:gesture_name} <=> {int:10, int:45, string:swap}
    } value;
    bool is_muted;              // flag indicated if the iop is muted (specially used for outputs)
    UT_hash_handle hh;         /* makes this structure hashable */
};

typedef struct agent_iop agent_iop;
typedef struct category category;
typedef struct definition definition;

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

/*
 * Define the state during the checking of a category
 */
typedef enum {
    GLOBAL_CAT,
    OUTPUT_CAT,
    INPUT_CAT
} category_check_type;


//model //////////////////
//Define a state to return in functions
typedef enum{
    OK,
    NOK,
    NAME_EMPTY,
    NAME_NOT_EXIST,
    TYPE_INPUT,
    TYPE_OUTPUT,
    TYPE_PARAM,
    NO_CALLBACK
} model_state;


//mapping //////////////////
/*
 * Define the state of a mapping ON or OFF
 */
typedef enum {OFF, ON, INCOMPATIBLE, GENERIC} map_state;
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

typedef struct mapping_out mapping_out;
typedef struct mapping_cat mapping_cat;
typedef struct mapping mapping;


//////////////////  functions and global variables   //////////////////

//////////////////  definition   //////////////////
extern definition * mtic_definition_loaded;
extern definition * mtic_definition_live;
extern definition * mtic_agents_defs_on_network;

iopType_t string_to_value_type(const char* string);
bool string_to_boolean(const char* string);
const char* value_type_to_string (iopType_t type);
const char* boolean_to_string (bool boole);

int get_iop_value_as_int(agent_iop *iop);
double get_iop_value_as_double(agent_iop *iop);

bool check_category (definition* def, category* category, category_check_type check_type);
bool check_category_agent_iop(agent_iop* def_iop, agent_iop* iop_cat_to_check);
void free_agent_iop (agent_iop **agent_iop);
void free_category (category* category);
void free_definition (definition* definition);


//////////////////  mapping   //////////////////

// the table which will contain the mapping
extern mapping * mtic_my_agent_mapping;

MASTICAPI_COMMON_DLLSPEC char* mtic_iop_value_to_string (agent_iop* iop);
MASTICAPI_COMMON_DLLSPEC const void* mtic_iop_value_string_to_real_type (agent_iop* iop, char* value);

const char * map_state_to_string(map_state state);
int mtic_map(char* input_name, char* map_description);
agent_iop* mtic_check_map (definition *definition);
agent_iop* mtic_unmap (definition *definition);
agent_iop*  mtic_update_mapping_out_state(mapping_out* map_out, definition * external_definition);
model_state mtic_map_received(const char * agent_name,
                              char * out_name,
                              void * value);
bool mtic_map_category (char* map_description);
void mapping_FreeMapping (mapping* mapping);


//////////////////  model   //////////////////
agent_iop * model_findIopByName(const char* name, model_state *code);
agent_iop * mtic_find_iop_by_name_on_definition(const char *name, definition* definition, model_state *code);
model_state mtic_set(const char* iop_name, void* new_value);
void * mtic_get(const char* name_iop, model_state* state);
// Conversions
char* model_IntToString(const int value);
char* model_DoubleToString(const double value);


//////////////////  network   //////////////////
#define AGENT_NAME_DEFAULT "mtic_undefined"
int network_publishOutput (const char* output_name);
int network_checkAndSubscribeToPublisher(const char* agentName);
void mtic_debug(const char *fmt, ...);


//////////////////  parser   //////////////////
extern bool agentNameChangedByDefinition;

category* load_category (const char* json_str);
category* load_category_from_path (const char* file_path);
const char* export_category (category* cat);
definition* parser_loadDefinition (const char* json_str);
definition* parser_loadDefinitionFromPath (const char* file_path);
char* export_definition (definition* def);
char* export_mapping(mapping * mapp);
mapping* parser_LoadMap (const char* json_str);
mapping* parser_LoadMapFromPath (const char* load_file);
int mtic_init_mapping (const char* mapping_file_path);
int mtic_init_internal_data (const char* definition_file_path);

#endif /* mastic_private_h */
