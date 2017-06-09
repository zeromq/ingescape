//
//  mtic_definition.c
//
//  Created by Patxi Berard
//  Modified by Mathieu Poirier
//  Modified by Vincent Deliencourt
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "mastic.h"
#include "mastic_private.h"
#include "uthash/uthash.h"

definition * mtic_definition_loaded = NULL;
definition * mtic_definition_live = NULL;
definition * mtic_agents_defs_on_network = NULL;

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
 * Function: string_to_value_type
 * ----------------------------
 *   convert a string to a value_type enum
 *
 *   string      : string to convert
 *
 */
value_type string_to_value_type(const char* str) {

    if (!strcmp(str, "INTEGER"))
        return INTEGER;
    if (!strcmp(str, "DOUBLE"))
        return DOUBLE_TYPE;
    if (!strcmp(str, "STRING"))
        return STRING;
    if (!strcmp(str, "BOOL"))
        return BOOL_TYPE;
    if (!strcmp(str, "IMPULSION"))
        return IMPULSION;
    if (!strcmp(str, "STRUCTURE"))
        return STRUCTURE;

    fprintf(stderr, "%s - ERROR -  unknown string \"%s\" to convert\n", __FUNCTION__, str);
    return -1;
}

/*
 * Function: string_to_boolean
 * ----------------------------
 *   convert a string to a boolean enum
 *
 *   string      : string to convert
 *
 */
bool string_to_boolean(const char* str) {

    if (!strcmp(str, "true"))
        return true;

    if (!strcmp(str, "false"))
        return false;

    fprintf(stderr, "%s - ERROR -  unknown string \"%s\" to convert\n", __FUNCTION__, str);
    return -1;
}

/*
 * Function: value_type_to_string
 * ----------------------------
 *   convert a value_type to string
 *
 *   type      : value_type to convert
 *
 */
const char* value_type_to_string (value_type type) {
    switch (type) {
        case INTEGER:
            return "INTEGER";
            break;
        case DOUBLE_TYPE:
            return "DOUBLE";
            break;
        case STRING:
            return "STRING";
            break;
        case BOOL_TYPE:
            return "BOOL";
            break;
        case IMPULSION:
            return "IMPULSION";
            break;
        case STRUCTURE:
            return "STRUCTURE";
            break;
        default:
            fprintf(stderr, "%s - ERROR -  unknown value_type to convert\n", __FUNCTION__);
            break;
    }

    return "";
}

/*
 * Function: boolean_to_string
 * ----------------------------
 *   convert a boolean enum to string
 *
 *   bool      : boolean to convert
 *
 */
const char* boolean_to_string (bool boole) {
    if (boole)
        return "true";
   else
        return "false";
}

/*
 * Function: free_agent_iop
 * ----------------------------
 *   free a structure agent_iop and all its pointers
 *
 *   agent_iop  : a pointer to the agent_iop structure to free
 *
 */
void free_agent_iop (agent_iop* agent_iop){

    free((char*)agent_iop->name);
    agent_iop->name = NULL;

    switch (agent_iop->type) {
        case STRING:
            free((char*)agent_iop->old_value.s);
            agent_iop->old_value.s = NULL;

            free((char*)agent_iop->value.s);
            agent_iop->value.s = NULL;
            break;
        case IMPULSION:
            free((char*)agent_iop->old_value.impuls);
            agent_iop->old_value.impuls = NULL;

            free((char*)agent_iop->value.impuls);
            agent_iop->value.impuls = NULL;
            break;
        case STRUCTURE:
            free((char*)agent_iop->old_value.strct);
            agent_iop->old_value.strct = NULL;

            free((char*)agent_iop->value.strct);
            agent_iop->value.strct = NULL;
            break;
        default:
            break;
    }

    free(agent_iop);
}

// --------- public functions ----------------------

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
bool check_category(definition* def,
                       category *category,
                       category_check_type check_type)
{
    bool state = true;

    switch(check_type)
    {
    case INPUT_CAT:
        if(check_category_agent_iop(def->inputs_table,
                                    category->inputs_table) != true)
        {
            state = false;
        }
        break;

    case OUTPUT_CAT:
        if(check_category_agent_iop(def->outputs_table,
                                    category->outputs_table) != true)
        {
            state = false;
        }
        break;

    case GLOBAL_CAT:
        if(check_category_agent_iop(def->inputs_table,
                                    category->inputs_table) != true)
        {
            state = false;
        }


        if(check_category_agent_iop(def->outputs_table,
                                    category->outputs_table) != true)
        {
            state = false;
        }
        break;

    default:
        break;
    }

    return state;
}


bool check_category_agent_iop(agent_iop *ref_iop,
                                agent_iop *iop_to_check) {

    bool state = true;


    struct agent_iop *iop, *iop_found;

    for(iop = ref_iop; iop != NULL; iop = iop->hh.next) {
        //Init to null for the next
        iop_found = NULL;

        //Find the iop corresponding to name (key)
        HASH_FIND_STR(iop_to_check, iop->name, iop_found);

        //Check the type of the iop correspond
        if(iop_found == NULL || (iop_found->type != iop->type)) {
            state = false;
        }
    }

    return state;
}

/*
 * Function: free_category
 * ----------------------------
 *   free a category structure, all its pointers and hash tables
 *
 *   category   : a pointer to the category structure to free
 *
 */
void free_category (category* cat){

    struct agent_iop *current, *tmp;

    free((char*)cat->name);
    cat->name = NULL ;
    free((char*)cat->version);
    cat->version = NULL;


    HASH_ITER(hh, cat->params_table, current, tmp) {
        HASH_DEL(cat->params_table,current);
        free_agent_iop(current);
        //current = NULL;
    }
    HASH_ITER(hh, cat->inputs_table, current, tmp) {
        HASH_DEL(cat->inputs_table,current);
        free_agent_iop(current);
        //current = NULL;
    }
    HASH_ITER(hh, cat->outputs_table, current, tmp) {
        HASH_DEL(cat->outputs_table,current);
        free_agent_iop(current);
        //current = NULL;
    }

    free (cat);
}

/*
 * Function: free_definition
 * ----------------------------
 *   free a definition structure, all its pointers and hash tables
 *
 *   definition : a pointer to the definition structure to free
 *
 */
void free_definition (definition* def) {

    struct agent_iop *current_iop, *tmp_iop;
    struct category *current_cat, *tmp_cat;

    free((char*)def->name);
    def->name = NULL;
    free((char*)def->version);
    def->version = NULL;
    free((char*)def->description);
    def->description = NULL;

    HASH_ITER(hh, def->params_table, current_iop, tmp_iop) {
        HASH_DEL(def->params_table,current_iop);
        free_agent_iop(current_iop);
        current_iop = NULL;
    }
    HASH_ITER(hh, def->inputs_table, current_iop, tmp_iop) {
        HASH_DEL(def->inputs_table,current_iop);
        free_agent_iop(current_iop);
        current_iop = NULL;
    }
    HASH_ITER(hh, def->outputs_table, current_iop, tmp_iop) {
        HASH_DEL(def->outputs_table,current_iop);
        free_agent_iop(current_iop);
        current_iop = NULL;
    }

    HASH_ITER(hh, def->categories, current_cat, tmp_cat) {
        HASH_DEL(def->categories,current_cat);
        free_category(current_cat);
        current_cat = NULL;
    }

    free(def);
}

/*
 * Function: mtic_iop_value_to_string
 * ----------------------------
 *   convert the iop value to string
 *
 *   agent_iop      : iop to convert
 *
 */
char* mtic_iop_value_to_string (agent_iop* iop)
{
    char str_value[BUFSIZ];
    if(iop != NULL)
    {
        switch (iop->type) {
            case INTEGER:
                sprintf(str_value,"%i",iop->value.i);
                break;
            case DOUBLE_TYPE:
                sprintf(str_value,"%lf",iop->value.d);
                break;
            case BOOL_TYPE:
                if(iop->value.b == true)
                {
                    sprintf(str_value,"%s","true");
                    
                } else {
                    sprintf(str_value,"%s","false");
                    
                }
                break;
            case STRING:
                sprintf(str_value,"%s",iop->value.s);
                break;
            case IMPULSION:
                sprintf(str_value,"%s",iop->value.impuls);
                break;
            case STRUCTURE:
                sprintf(str_value,"%s",iop->value.strct);
                break;
            default:
                break;
        }
    }
    
    return strdup(str_value);
}

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
const void* mtic_iop_value_string_to_real_type (agent_iop* iop, char* value)
{
    void * out_value = NULL;
    
    double* double_value = NULL;
    int* int_value = NULL;
    bool* bool_value = NULL;
    char* str_value = NULL;
    
    if(iop != NULL)
    {
        switch (iop->type) {
            case INTEGER:
                int_value=(int*)malloc(sizeof(int));
                sscanf(value, "%i", int_value);
                out_value = (void*) int_value;
                break;
            case DOUBLE_TYPE:
                double_value=(double*)malloc(sizeof(double));
                sscanf(value, "%lf", double_value);
                
                out_value = (void*) double_value;
                break;
            case BOOL_TYPE:
                bool_value=(bool*)malloc(sizeof(bool));
                if(strcmp(value,"true") == 0)
                {
                    *bool_value = true;
                } else {
                    *bool_value = false;
                }
                out_value = (void*) bool_value;
                break;
            case STRING:
            case IMPULSION:
            case STRUCTURE:
                str_value=(char*)malloc(BUFSIZ*sizeof(char));
                sscanf(value,"%s",str_value);
                out_value = (void*) str_value;
                break;
            default:
                break;
        }
    }
    
    return out_value;
}

/*
 * Function: iop_old_value_to_string
 * ----------------------------
 *   convert the iop old value to string
 *
 *   agent_iop      : iop to convert
 *
 */
const char* iop_old_value_to_string (agent_iop* iop)
{
    char str_value[BUFSIZ];
    if(iop != NULL)
    {
        switch (iop->type) {
            case INTEGER:
                sprintf(str_value,"%i",iop->old_value.i);
                break;
            case DOUBLE_TYPE:
                sprintf(str_value,"%lf",iop->old_value.d);
                break;
            case BOOL_TYPE:
                if(iop->old_value.b == true)
                {
                    sprintf(str_value,"%s","true");
                    
                } else {
                    sprintf(str_value,"%s","false");
                    
                }
                break;
            case STRING:
                sprintf(str_value,"%s",iop->old_value.s);
                break;
            case IMPULSION:
                sprintf(str_value,"%s",iop->old_value.impuls);
                break;
            case STRUCTURE:
                sprintf(str_value,"%s",iop->old_value.strct);
                break;
            default:
                break;
        }
    }
    
    return strdup(str_value);
}


int get_iop_value_as_int(agent_iop *iop){
    model_state m;
    int val = *(int *)(mtic_get(iop->name, &m));
    return val;
}

double get_iop_value_as_double(agent_iop *iop){
    model_state m;
    double val = *(double *)(mtic_get(iop->name, &m));
    return val;
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
//à remplir ou déplacer ici



////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////

//int mtic_loadDefinition (const char* json_str){
//    return 1;
//}
//int mtic_loadDefinitionFromPath (const char* file_path){
//    return 1;
//}
int mtic_clearDefinition(){ //clears definition data for the agent
    return 1;
}

/**
 * \fn char* mtic_getDefinition()
 * \brief the agent definition getter
 *
 * \return The loaded definition string in json format (allocated). NULL if mtic_definition_loaded was not initialized.
 */
char* mtic_getDefinition(){
    char * def = NULL;

    if(mtic_definition_loaded == NULL)
        return NULL;

    def = export_definition(mtic_definition_loaded);

    return def;
}

/**
 * \fn int mtic_setDefinitionDescription(char *description)
 * \brief the agent definition description setter
 *
 * \param description The string which contains the description of the agent. Can't be NULL.
 * \return The error. 1 is OK, 0 Agent description is NULL, -1 Agent description is empty
 */
int mtic_setDefinitionDescription(char *description){

    if(description == NULL){
        mtic_debug("mtic_setDefinitionDescription : Agent description cannot be NULL \n");
        return 0;
    }

    if (strlen(description) == 0){
        mtic_debug("mtic_setDefinitionDescription : Agent description cannot be empty\n");
        return -1;
    }

    //Check if already initialized, and do it if not
    if(mtic_definition_loaded == NULL){
        mtic_definition_loaded = calloc(1, sizeof(struct definition));
    }

    //Copy the description in the structure in loaded definition
    if(mtic_definition_loaded->description != NULL)//Free the field if needed
        free(mtic_definition_loaded->description);
    mtic_definition_loaded->description = strdup(description);

    //Check if already initialized, and do it if not
    if(mtic_definition_live == NULL){
        mtic_definition_live = calloc(1, sizeof(struct definition));
    }

    // Live data corresponds to a copy of the initial definition
    if(mtic_definition_live->description != NULL)//Free the field if needed
        free(mtic_definition_live->description);
     mtic_definition_live->description = strdup(mtic_definition_loaded->description);

    return 1;
}

/**
 * \fn mtic_setDefinitionVersion(char *description)
 * \brief the agent definition version setter
 *
 * \param version The string which contains the version of the agent. Can't be NULL.
 * \return The error. 1 is OK, 0 Agent version is NULL, -1 Agent version is empty
 */
int mtic_setDefinitionVersion(char *version){

    if(version == NULL){
        mtic_debug("mtic_setDefinitionVersion : Agent version cannot be NULL \n");
        return 0;
    }

    if (strlen(version) == 0){
        mtic_debug("mtic_setDefinitionVersion : Agent version cannot be empty\n");
        return -1;
    }

    //Check if already initialized, and do it if not
    if(mtic_definition_loaded == NULL){
        mtic_definition_loaded = calloc(1, sizeof(struct definition));
    }

    //Copy the description in the structure in loaded definition
    if(mtic_definition_loaded->version != NULL)//Free the field if needed
        free(mtic_definition_loaded->version);
    mtic_definition_loaded->version = strdup(version);

    //Check if already initialized, and do it if not
    if(mtic_definition_live == NULL){
        mtic_definition_live = calloc(1, sizeof(struct definition));
    }

    // Live data corresponds to a copy of the initial definition
    if(mtic_definition_live->version != NULL)//Free the field if needed
        free(mtic_definition_live->version);
    mtic_definition_live->version = strdup(mtic_definition_loaded->version);

    return 1;
}


//edit the definition using the API
int mtic_createInput(const char *name, iopType_t type, void *value){ //value must be copied in function
    return 1;
}
int mtic_createOutput(const char *name, iopType_t type, void *value){ //value must be copied in function
    return 1;
}
int mtic_createParameter(const char *name, iopType_t type, void *value){ //value must be copied in function
    return 1;
}

int mtic_removeInput(const char *name){
    return 1;
}
int mtic_removeOutput(const char *name){
    return 1;
}
int mtic_removeParameter(const char *name){
    return 1;
}
