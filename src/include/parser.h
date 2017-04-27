//
//  mtic_parseur.h
//
//  Created by Patxi Berard
//  Modified by Vincent Deliencourt
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

#ifndef MTIC_PARSER_H
#define MTIC_PARSER_H

#include "mapping.h"

/*
* Function: load_category
* ----------------------------
*   Load a category in the standartised format JSON to initialize a category structure from string.
*   The category structure is dynamically allocated. You will have to use free_category function to deallocated it correctly.
*
*   json_str      : a string (json format)
*
*   returns: a pointer on a category structure or NULL if it has failed
*/
category* load_category (const char* json_str);

/*
 * Function: load_category_from_path
 * ---------------------------------
 *   Load a category in the standartised format JSON to initialize a category structure from a local file path.
 *   The category structure is dynamically allocated. You will have to use free_category function to deallocated it correctly.
 *
 *   file_path      : the file path
 *
 *   returns: a pointer on a category structure or NULL if it has failed
 */
category* load_category_from_path (const char* file_path);

/*
 * Function: export_category
 * ----------------------------
 *   Returns a categorie structure into a standartised format json string UTF8 to send it throught the BUS or save it in a file
 *
 *   cat    : the category dump in string
 *
 *   returns: a category json format string UTF8
 */
const char* export_category (category* cat);

/*
 * Function: load_definition
 * ----------------------------
 *   Load a agent definition in the standartised format JSON to initialize a definition structure from a string.
 *   The definition structure is dynamically allocated. You will have to use free_definition function to deallocated it correctly.
 *
 *   json_str      : a string (json format)
 *
 *   returns: a pointer on a category structure or NULL if it has failed
 */
definition* load_definition (const char* json_str);

/*
 * Function: load_definition_from_path
 * -----------------------------------
 *   Load a agent definition in the standartised format JSON to initialize a definition structure from a local file path.
 *   The definition structure is dynamically allocated. You will have to use free_definition function to deallocated it correctly.
 *
 *   file_path      : the file path
 *
 *   returns: a pointer on a category structure or NULL if it has failed
 */
definition* load_definition_from_path (const char* file_path);

/*
 * Function: export_definition
 * ----------------------------
 *   Returns a agent's definition structure into a standartised format json string UTF8 to send it throught the BUS or save it in a file
 *
 *   def    : the agent's definition dump in string
 *
 *   returns: a definition json format string UTF8
 */
char* export_definition (definition* def);

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
mapping* load_map (const char* json_str);


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
mapping* load_map_from_path (const char* load_file);

/*
 * Function: init_mapping
 * ----------------------------
 *   read mapping from file path and init inernal mapping data
 *
 *   mapping_file_path : path to the agent mapping file
 *
 */
MASTICAPI_COMMON_DLLSPEC int mtic_init_mapping (const char* mapping_file_path);

/*
 * Function: mtic_init_internal_data
 * ----------------------------
 *   read definition from file path and init inernal agent data
 *   initialize definition_load and mtic_definition_live data structures
 *
 *   definition_file_path : path to the agent definiton file
 *
 */
MASTICAPI_COMMON_DLLSPEC int mtic_init_internal_data (const char* definition_file_path);

#endif // MTIC_PARSER_H
