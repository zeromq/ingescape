//
//  mtic_definition.h
//
//  Created by Patxi Berard
//  Modified by Mathieu Poirier
//  Modified by Vincent Deliencourt
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

#ifndef MTIC_DEFINITION_H
#define MTIC_DEFINITION_H

#if defined Q_OS_WIN
#if defined MASTIC
#define MASTICAPI_COMMON_DLLSPEC __declspec(dllexport)
#else
#define MASTICAPI_COMMON_DLLSPEC __declspec(dllimport)
#endif
#else
#define MASTICAPI_COMMON_DLLSPEC extern
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "uthash/uthash.h"

/*>>>>>>>>>>>>>>>>>> COMMON STRUCTURES <<<<<<<<<<<<<<<<<<<<<<<<<<< */

/*
 * Define the structure agent_port (name, port_number) :
 * 'name' : the agent name
 * 'port' : the port number used for connection
 */
typedef struct agent_port_t {
    const char * name;          //Need to be unique : the table hash key
    int port;
    UT_hash_handle hh;         /* makes this structure hashable */
} agent_port;

/*
 * The variable 'value_type' contains the data type of the value, use to parse
 * the JSON and affect the value tu right union corresponding
 */
typedef enum {
    INTEGER,
    DOUBLE_TYPE,
    STRING,
    BOOL_TYPE,
    IMPULSION,
    STRUCTURE
} value_type;

/*
 * Define the structure agent_iop (input, output, parameter) :
 * 'name'       : the input/output/parameter's name
 * 'type'       : the input/output/parameter'value type (int, double, ...)
 * 'value'      : the input/output/parameter'value
 * 'is_muted'   : the input/output/parameter muted
 * NB : it's true that 3 'const char*' are defined and it's useless, only to differentiate in accordance to the type for using ex. 'e1.impulse = 'released()'
 */
typedef struct agent_iop_t {
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
} agent_iop;


/*
 * Define the state during the checking of a category
 * TODO: complete or remove useless stuff
 */
typedef enum {GLOBAL, OUTPUT, INPUT_TYPE} category_check_type;

/*
 * Define the structure CATEGORY :
 * 'name'                   : name of the category
 * 'version'                : version of the category
 * 'parameters'             : list of parameters which describe the category
 * 'inputs'                 : list of inputs which describe the category
 * 'outputs'                : list of outputs which describe the category
 */
typedef struct category_t {
    const char* name;
    const char * version;
    agent_iop* params_table;
    agent_iop* inputs_table;
    agent_iop* outputs_table;
    UT_hash_handle hh;
} category;


/*
 * Define the structure DEFINITION :
 * 'name'                   : agent name
 * 'description'            : human readable description of the agent
 * 'version'                : the version of the agent
 * 'parameters'             : list of parameters contains by the agent
 * 'inputs'                 : list of inputs contains by the agent
 * 'outputs'                : list of outputs contains by the agent
 */
typedef struct definition_t {
    const char * name; //Need to be unique the key
    const char * description;
    const char * version;
    category * categories;
    agent_iop* params_table;
    agent_iop* inputs_table;
    agent_iop* outputs_table;
    UT_hash_handle hh;
} definition;

MASTICAPI_COMMON_DLLSPEC definition * mtic_definition_loaded;
MASTICAPI_COMMON_DLLSPEC definition * mtic_definition_live;
MASTICAPI_COMMON_DLLSPEC definition * mtic_agents_defs_on_network;

/*
 * Function: string_to_value_type
 * ----------------------------
 *   convert a string to a value_type enum
 *
 *   string      : string to convert
 *
 */
value_type string_to_value_type(const char* string);

/*
 * Function: string_to_boolean
 * ----------------------------
 *   convert a string to a boolean enum
 *
 *   string      : string to convert
 *
 */
bool string_to_boolean(const char* string);

/*
 * Function: value_type_to_string
 * ----------------------------
 *   convert a value_type to string
 *
 *   type      : value_type to convert
 *
 */
const char* value_type_to_string (value_type type);

/*
 * Function: boolean_to_string
 * ----------------------------
 *   convert a boolean enum to string
 *
 *   bool      : boolean to convert
 *
 */
const char* boolean_to_string (bool boole);

/*
 * Function: mtic_iop_value_to_string
 * ----------------------------
 *   convert the iop value to string
 *
 *   agent_iop      : iop to convert
 *
 */
MASTICAPI_COMMON_DLLSPEC char* mtic_iop_value_to_string (agent_iop* iop);

/*
 * Function: iop_old_value_to_string
 * ----------------------------
 *   convert the iop old value to string
 *
 *   agent_iop      : iop to convert
 *
 */
const char* iop_old_value_to_string (agent_iop* iop);

/*
 * Function: mtic_iop_value_string_to_real_type
 * ----------------------------
 *   convert the iop value to string
 *
 *   agent_iop      : boolean to convert
 *
 *   value          : string value
 *
 */
MASTICAPI_COMMON_DLLSPEC const void* mtic_iop_value_string_to_real_type (agent_iop* iop, char* value);


/*>>>>>>>>>>>>>>>>>> DEFINION & CATEGORY <<<<<<<<<<<<<<<<<<<<<<<<<<< */

/*
 * Function: check_category
 * ----------------------------
 *   Check if the agent is in accordance with the category
 *   GLOBAL : during the load of the agent's definition to check if ok with the parameters,inputs,outputs
 *   OUTPUT : when another agent display on the BUS his category
 *   INPUT  : when a map_category is called to check if the inputs is in accordance with the external agent's outputs
 *
 *   definition         : the definition to compare with
 *
 *   category           : the category to check
 *
 *   check_type         : the type of checking : GLOBAL, OUTPUT, INPUT
 *
 *   returns            : the state of the checking OK or NOK
 */


bool check_category (definition* def, category* category, category_check_type check_type);

/*
 * Function: check_agent_iop
 * ----------------------------
 *   Check if an agent iop is compatible with an another reference one.
 *   the iops have to have the same name and the same type of data.
 *
 *   ref_iop        : the reference iop
 *
 *   iop_to_check   : the iop to check
 *
 *   returns        : the state of the checking OK (compatible) or NOK (uncompatible)
 */
bool check_agent_iop(agent_iop* ref_iop, agent_iop* iop_to_check);


bool check_category_agent_iop(agent_iop* def_iop, agent_iop* iop_cat_to_check);

/*
 * Function: free_agent_iop
 * ----------------------------
 *   free a structure agent_iop and all its pointers
 *
 *   agent_iop  : a pointer to the agent_iop structure to free
 *
 */
void free_agent_iop (agent_iop* agent_iop);

/*
 * Function: free_category
 * ----------------------------
 *   free a category structure, all its pointers and hash tables
 *
 *   category   : a pointer to the category structure to free
 *
 */
void free_category (category* category);

/*
 * Function: free_definition
 * ----------------------------
 *   free a definition structure, all its pointers and hash tables
 *
 *   definition : a pointer to the definition structure to free
 *
 */
void free_definition (definition* definition);

#endif // MTIC_DEFINITION_H
