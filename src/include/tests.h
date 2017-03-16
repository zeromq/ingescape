//
//  tests.h
//
//  Created by Patxi Berard
//  Modified by Mathieu Poirier
//  Modified by Vincent Deliencourt
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

#ifndef TESTS_H
#define TESTS_H


#include "mapping.h"
#include "parser.h"
#include "definition.h"

//void test_copy_definition();

void test_map();

/*
 * Function: test_model_workflow
 * ----------------------------
 * Test all the model workflow.
 * Initialize a definition from test_definition_my_agent.json file.
 * Observe input 'e1'.Set new value to this input. And get the value.
 * Observe input 'e2'.Set new value to this input. And get the value.
 * This test allows to test all functions from model and validate these.
 */
void test_model_workflow();

/*
 * Function: test_mapping_workflow
 * ----------------------------
 * Test all the 'mapping' workflow.
 * Initialize a definition from test_definition_my_agent.json file.
 * Load a map to a variable temp from test_map.json file.
 * Copy to the global variable by 'map_all_mapping' function.
 * Simulate the 'ENTER' :
 *  -   Initialize a external definition from external_definition.json file.
 *  -   Check if it's concerning by a map
 *  -   simulate the reception from the output mapped (subscribed) to the input (map_received)
 * Print the table of all mappings and theirs states.
 * This test allows to test all functions from mapping and validate these.
 */
void test_mapping_workflow();

/*
 * Function: test_check_category
 * ----------------------------
 * Test the function. Initialize a definition from definition.json file.
 * Initialize a category compatible from cat_compatible.json file.
 * Initialize a category not compatible from cat_incompatible.json file.
 * To be compatible the iop has to have the same 'name' & the same value 'type'.
 */

void test_check_category();

/***** PRINT *****/
void print_iop(agent_iop *iop);
void print_category(category *cat);
void print_definition(definition *def);
void print_mapping(mapping *mapp);

/***** COMMAND PARSING *****/
void parse(char buffer[],int* num_of_args, char **arguments);


#endif // TESTS_H
