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
 * Define the state during the checking of a category
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


bool check_category (definition* def, category* category, category_check_type check_type);
bool check_category_agent_iop(agent_iop* def_iop, agent_iop* iop_cat_to_check);
void free_agent_iop (agent_iop* agent_iop);
void free_category (category* category);
void free_definition (definition* definition);

#endif // MTIC_DEFINITION_H
