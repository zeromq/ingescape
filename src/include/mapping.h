//
//  mapping.h
//
//  Created by Patxi Berard
//  Modified by Mathieu Poirier
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

#ifndef MAPPING_H
#define MAPPING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "definition.h"
#include "model.h"
#include "network.h"

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
typedef struct mapping_out_t {
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
typedef struct mapping_cat_t {
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
 typedef struct mapping_t {
    char *name;//Need to be unique : the table hash key
    char *description;
    char *version;
    mapping_out *map_out;
    mapping_cat *map_cat;
    UT_hash_handle hh;
} mapping;

// the table which will contain the mapping
MASTICAPI_COMMON_DLLSPEC extern mapping * my_agent_mapping;

/*>>>>>>>>>>>>>>>>>> MAPPING <<<<<<<<<<<<<<<<<<<<<<<<<<< */

const char * map_state_to_string(map_state state);

/*
 * Function: map_all_mapping
 * ----------------------------
 *   Sub-function of 'load_map'.
 *   Read the table of all mappings loaded by the file json
 *   Call the 'map' function corresponding to the map
 *
 *   usage : map_all_mapping()
 *
 *   loaded : the structure 'mapping' corresponding to the loaded mapping from the JSON file.
 *
 */
void copy_to_map_global(mapping *loaded);

/*
 * Function: add_map_to_table
 * ----------------------------
 *   Check if the map is already present in the table
 *   Add the map in the table
 *
 *   usage : add_map_to_table('e1','A2', 's1')
 *
 *   input_name     : the input's name to be connected with : ex. 'e1'
 *
 *   agent_name     : the agent's name to be connected with : ex. 'A2' OR '*'
 *
 *   output_name    : the output's name to be connected with : ex. 'A2' OR '*'
 *.
 *   report         : the error code
 *                      0  : OK
 *                      1  : agent name empty
 *                      2  : output name empty
 *
 *   returns : map_out : mapping_out object newly created

 */
mapping_out * add_map_to_table(char * input_name,
                               char *agent_name,
                               char* output_name,
                               int* report);
/*
 * Function: split_map_description
 * ----------------------------
 *   Split the map description in the agent name & the output name
 *
 *   usage : split_map_description('A2.s1',agent_name,output_name)
 *
 *   map_description    : the description of the mapping 'A2.s1' OR '*.s1'
 *
 *   agent_name         : the pointer on the char * which will contains the name of the agent
 *
 *   output_name         : the pointer on the char * which will contains the output name
 *
 *   returns : the error code
 *   0  : OK
 *   1  : agent name empty
 *   2  : output name empty
 */
int split_map_description(char *map_description,
                           char *agent_name,
                           char *output_name);

/*
 * Function: map
 * ----------------------------
 *   Map an input with an external agent's ouput (one OR more).
 *   Check if the input is part of the agent.
 *   Check if the map is already present in the table
 *   Add the map in the table
 *
 *   usage : map('e1','A2.s1')
 *
 *   input_name         : the input's name to be connected with : ex. 'e1'
 *
 *   map_description    : the description of the mapping 'A2.s1' OR '*.s1'
 *
 *   returns : the error code
 *   0  : OK
 *   1  : agent name empty
 *   2  : output name empty
 */

MASTICAPI_COMMON_DLLSPEC int map(char* input_name, char* map_description);

/*
 * Function: find_map
 * ----------------------------
 *   Sub-function of 'check_map'.
 *   Find the map in the table
 *
 *   usage : find_map('e1','A2','s1')
 *
 *   input_name     : the input's name
 *
 *   agent_name     : the agent's name
 *
 *   output_name    : the output's name
 *
 *   returns : the mapping output pointer
 */
mapping_out *find_map(char * input_name,
                     char * agent_name,
                     char * output_name);

/*
 * Function: check_map
 * ----------------------------
 *   It's called in the function ENTER.
 *   Check if the agent is concerning by a mapping.
 *   Update the mapping state if some exist.
 *
 *   definition : the definition of the external entered agent
 *
 *   returns    : the table of outputs to subscribe OR null
 */

MASTICAPI_COMMON_DLLSPEC agent_iop* check_map (definition *definition);

/*
 * Function: unmap
 * ----------------------------
 *   It's called in the function EXIT.
 *   Check if the agent is concerning by a mapping.
 *   Update the mapping state to OFF.
 *
 *   definition : the definition of the external agent which will exit
 *
 *   returns    : the table of outputs passed to OFF and to unsubscribe OR null
 */

MASTICAPI_COMMON_DLLSPEC agent_iop* unmap (definition *definition);


/*
 * Function: update_mapping_out_state
 * ----------------------------
 *   Take a mapping_out object to update it's state according to the definition it's mapped with. .
 *   Check & find the output is part of this agent.
 *   Check if the type between the input and the output is compatible.
 *   If yes, switch the state to ON.
 *
 *   map_out             : the mapping_out object
 *
 *   external_definition : the external agent definition
 *
 *   return : the agent_iop object found in the external definition corresponding to map_out
 *
 */
agent_iop*  update_mapping_out_state(mapping_out* map_out, definition * external_definition);

/*
 * Function: map_received
 * ----------------------------
 *   It's called in the function SUB when receiving value from an another agent which we have subscribed.
 *   Call SET to copy the value and updated the live model of data.
 *
 *   agent_name : the name of the external agent
 *
 *   out_name : the name of the output concerning
 *
 *   value      : the new value
 *
 *   returns : the state of the 'set' function from model
 */
model_state map_received(const char * agent_name,
                         char * out_name,
                         void * value);

/* TODO : implement in the next version of the API
 * Function: map_category
 * ----------------------------
 *   Map the agent's inputs to the category which an external agent is in accordance with it,
 *   So all the outputs from the external agent in accordance with the agent'inputs will be mapped
 *
 *   usage : map_category('A2.c3')
 *
 *   map_description    : the description of the mapping 'A2.c3' OR '*.c3'
 *
 *   returns : the state of the mapping
 */

MASTICAPI_COMMON_DLLSPEC bool map_category (char* map_description);

/*
 * Function: free_mapping
 * ----------------------------
 *   free a mapping structure, all its pointers and hash tables
 *
 *   mapping : a pointer to the mapping structure to free
 *
 */
void free_mapping (mapping* mapping);




#endif // MAPPING_H
