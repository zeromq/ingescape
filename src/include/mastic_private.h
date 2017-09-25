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
#include <czmq.h>

#include "uthash/uthash.h"

#ifdef _WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <iphlpapi.h>
#else
# define MAX_PATH 260
#endif

#include "mastic.h"

//////////////////  STRUCTURES AND ENUMS   //////////////////

/*
 * Define the structure agent_iop (input, output, parameter) :
 * 'name'       : the input/output/parameter's name. Need to be unique in each type of iop (input/output/parameter)
 * 'value_type' : the type of the value (int, double, string, impulsion ...)
 * 'type'       : the type of the iop : input / output / parameter
 * 'value'      : the input/output/parameter'value
 * 'valueSize'  : the size of the value
 * 'is_muted'   : flag indicated if the iop is muted (specially used for outputs)
 */
struct agent_iop {
    const char* name;
    iopType_t value_type;
    iop_t type;          //Size of pointer on data
    struct {
        int i;                  //in accordance to type INTEGER_T ex. '10'
        double d;               //in accordance to type DOUBLE_T ex. '10.01'
        char* s;                //in accordance to type STRING_T ex. 'display the image'
        bool b;                 //in accordante to type BOOL_T ex. 'true' or 'false'
        void* data;             //in accordance to type DATA_T ex. '{int:x, int:y, string:gesture_name} <=> {int:10, int:45, string:swap}
    } value;
    long valueSize;
    bool is_muted;
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
    const char* name; //hash key
    const char* description;
    const char* version;
    category* categories;
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
    const char* version;
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

/*
 * Define the state of a mapping ON or OFF
 */
typedef enum {OFF, ON, INCOMPATIBLE, GENERIC} map_state;

/*
 * Define the structure 'mapping_out' which contains mapping between an input and an (one or all) external agent's output :
 * 'map_id'                 : the key of the table. Need to be unique : the table hash key
 * 'input name'             : agent's input name to connect
 * 'agent name to connect'  : external agent's name to connect with (one or all)
 * 'output name to connect' : external agent(s) output name to connect with
 */

typedef struct mapping_out {
    int map_id;
    char* input_name;
    char* agent_name;
    char* output_name;
    map_state state;
    UT_hash_handle hh;
} mapping_out;

/*
 * Define the structure 'mapping_cat' which contains mapping between an input and an (one or all) external agent's category :
 * 'map_id'                         : the key of the table. Need to be unique : the table hash key
 * 'agent name to connect'          : external agent's name to connect with (one or all)
 * 'category unique name to connect': external agent(s) category to connect with
 */
typedef struct mapping_cat {
    int map_cat_id;
    char* agent_name;
    char*category_name;
    map_state state;
    UT_hash_handle hh;
} mapping_cat;

/*
 * Define the structure 'mapping' which contains the json description of all mapping (output & category):
 * 'name'           : The name of the mapping. Need to be unique
 * 'description     :
 * 'version'        :
 * 'mapping out'    : the table of the mapping output
 * 'mapping cat'    : the table of the mapping category
 */
typedef struct mapping {
    char* name;
    char* description;
    char* version;
    mapping_out* map_out;
    mapping_cat* map_cat;
    UT_hash_handle hh;
} mapping;

typedef struct mapping_out mapping_out;
typedef struct mapping_cat mapping_cat;
typedef struct mapping mapping;

typedef struct subscriber{
    const char *agentName;
    const char *agentPeerId;
    zsock_t *subscriber;
    zmq_pollitem_t *pollItem;
    definition *definition;
    UT_hash_handle hh;
} subscriber_t;

//////////////////  FUNCTIONS  //////////////////

//  definition

extern definition* mtic_internal_definition;

int definition_get_iop_value_as_int(agent_iop*iop, iop_t type);
double definition_get_iop_value_as_double(agent_iop*iop, iop_t type);
char* definition_get_iop_value_as_string (agent_iop* iop);

void definition_free_definition (definition* definition);


//  mapping

extern mapping* mtic_my_agent_mapping;

agent_iop* mapping_check_map (definition* definition);
agent_iop* mapping_unmap (definition* definition);
int mapping_map_received(const char* agent_name, char* out_name, char* value, long size);


// model

extern bool isWholeAgentMuted;

agent_iop* model_find_iop_by_name_in_definition(const char*name, definition* definition);
agent_iop* model_findIopByName(const char* name, iop_t type);
agent_iop* model_findInputByName(const char* name);
agent_iop* model_findOutputByName(const char* name);
agent_iop* model_findParameterByName(const char* name);
void* model_get(const char*name_iop, iop_t type);


// network
#define AGENT_NAME_DEFAULT "mtic_undefined"
int network_publishOutput (const char* output_name);
int network_checkAndSubscribeToPublisher(const char* agentName);
void mtic_debug(const char*fmt, ...);


// parser

definition* parser_loadDefinition (const char* json_str);
definition* parser_loadDefinitionFromPath (const char* file_path);
char* parser_export_definition (definition* def);
char* parser_export_mapping(mapping* mapp);
mapping* parser_LoadMap (const char* json_str);
mapping* parser_LoadMapFromPath (const char* load_file);

#endif /* mastic_private_h */
