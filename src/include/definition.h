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

#if defined WINDOWS
#if defined MASTIC
#define MASTICAPI_COMMON_DLLSPEC __declspec(dllexport)
#else
#define MASTICAPI_COMMON_DLLSPEC __declspec(dllimport)
#endif
#else
#define MASTICAPI_COMMON_DLLSPEC extern
#endif

#include <stdbool.h>
#include <string.h>

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
 * Define the state during the checking of a category
 * TODO: complete or remove useless stuff
 */
typedef enum {
    GLOBAL,
    OUTPUT,
    INPUT_TYPE
} category_check_type;

//forward declaration of internal structs
typedef struct agent_iop agent_iop;
typedef struct category category;
typedef struct definition definition;



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
