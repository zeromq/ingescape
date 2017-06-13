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
//#include "mastic.h"
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
iopType_t string_to_value_type(const char* str) {

    if (!strcmp(str, "INTEGER"))
        return INTEGER_T;
    if (!strcmp(str, "DOUBLE"))
        return DOUBLE_T;
    if (!strcmp(str, "STRING"))
        return STRING_T;
    if (!strcmp(str, "BOOL"))
        return BOOL_T;
    if (!strcmp(str, "IMPULSION"))
        return IMPULSION_T;
    if (!strcmp(str, "DATA"))
        return DATA_T;

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
 * Function: iopType_t_to_string
 * ----------------------------
 *   convert a iopType_t to string
 *
 *   type      : iopType_t to convert
 *
 */
const char* value_type_to_string (iopType_t type) {
    switch (type) {
        case INTEGER_T:
            return "INTEGER";
            break;
        case DOUBLE_T:
            return "DOUBLE";
            break;
        case STRING_T:
            return "STRING";
            break;
        case BOOL_T:
            return "BOOL";
            break;
        case IMPULSION_T:
            return "IMPULSION";
            break;
        case DATA_T:
            return "STRUCTURE";
            break;
        default:
            fprintf(stderr, "%s - ERROR -  unknown iopType_t to convert\n", __FUNCTION__);
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
        case STRING_T:
            free((char*)agent_iop->old_value.s);
            agent_iop->old_value.s = NULL;

            free((char*)agent_iop->value.s);
            agent_iop->value.s = NULL;
            break;
        case IMPULSION_T:
            free((char*)agent_iop->old_value.impuls);
            agent_iop->old_value.impuls = NULL;

            free((char*)agent_iop->value.impuls);
            agent_iop->value.impuls = NULL;
            break;
        case DATA_T:
            free((char*)agent_iop->old_value.data);
            agent_iop->old_value.data = NULL;

            free((char*)agent_iop->value.data);
            agent_iop->value.data = NULL;
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
            case INTEGER_T:
                sprintf(str_value,"%i",iop->value.i);
                break;
            case DOUBLE_T:
                sprintf(str_value,"%lf",iop->value.d);
                break;
            case BOOL_T:
                if(iop->value.b == true)
                {
                    sprintf(str_value,"%s","true");
                    
                } else {
                    sprintf(str_value,"%s","false");
                    
                }
                break;
            case STRING_T:
                sprintf(str_value,"%s",iop->value.s);
                break;
            case IMPULSION_T:
                sprintf(str_value,"%s",iop->value.impuls);
                break;
            case DATA_T:
                sprintf(str_value,"%s",iop->value.data);
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
            case INTEGER_T:
                int_value=(int*)malloc(sizeof(int));
                sscanf(value, "%i", int_value);
                out_value = (void*) int_value;
                break;
            case DOUBLE_T:
                double_value=(double*)malloc(sizeof(double));
                sscanf(value, "%lf", double_value);
                
                out_value = (void*) double_value;
                break;
            case BOOL_T:
                bool_value=(bool*)malloc(sizeof(bool));
                if(strcmp(value,"true") == 0)
                {
                    *bool_value = true;
                } else {
                    *bool_value = false;
                }
                out_value = (void*) bool_value;
                break;
            case STRING_T:
            case IMPULSION_T:
            case DATA_T:
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
            case INTEGER_T:
                sprintf(str_value,"%i",iop->old_value.i);
                break;
            case DOUBLE_T:
                sprintf(str_value,"%lf",iop->old_value.d);
                break;
            case BOOL_T:
                if(iop->old_value.b == true)
                {
                    sprintf(str_value,"%s","true");
                    
                } else {
                    sprintf(str_value,"%s","false");
                    
                }
                break;
            case STRING_T:
                sprintf(str_value,"%s",iop->old_value.s);
                break;
            case IMPULSION_T:
                sprintf(str_value,"%s",iop->old_value.impuls);
                break;
            case DATA_T:
                sprintf(str_value,"%s",iop->old_value.data);
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

//1 is OK, 0 iop is NULL
int definition_setIopValue(agent_iop *iop, void * value)
{
    if(iop == NULL)
        return 0;

    switch (iop->type) {
        case INTEGER_T:
            iop->value.i = *(int*)(value);
            break;
        case DOUBLE_T:
            iop->value.d = *(double*)(value);
            break;
        case BOOL_T:
            iop->value.b = *(bool*)(value);
            break;
        case STRING_T:
            free(iop->value.s);
            iop->value.s = strdup(value);
            break;
        case IMPULSION_T:
            free(iop->value.impuls);
            iop->value.impuls = strdup(value);
        break;
        case DATA_T:
            free(iop->value.data);
            iop->value.data = strdup(value);
        break;
        default:
            break;
    }

    return 1;
}

agent_iop* definition_createIop(const char *name, iopType_t type, void *value)
{
    //Create the iop
    agent_iop *iop = NULL;
    iop = calloc (1, sizeof (struct agent_iop));
    iop->name = strdup(name);
    iop->type = type;

    //Set value
    definition_setIopValue(iop,value);

    return iop;
}

// iop is copied so shall be free
// 1 OK, 0 iop already exists by name
int definition_addIop(agent_iop *iop, iop_t iop_type, definition **def)
{
    agent_iop *iop_to_add = NULL;

    //Check if already initialized, and do it if not
    if((*def) == NULL){
        (*def) = calloc(1, sizeof(struct definition));
    }

    //Check if the key already exist
    switch (iop_type) {
    case INPUT_T:
        HASH_FIND_STR((*def)->inputs_table, iop->name , iop_to_add);
        break;
    case OUTPUT_T:
        HASH_FIND_STR((*def)->outputs_table, iop->name , iop_to_add);
        break;
    case PARAMETER_T:
        HASH_FIND_STR((*def)->params_table, iop->name , iop_to_add);
        break;
    default:
        break;
    }

    if(iop_to_add != NULL)
        return 0;

    //Copy the iop
    iop_to_add = calloc(1, sizeof(struct agent_iop));
    memcpy(iop_to_add, iop, sizeof(*iop));

    //Add the iop
    switch (iop_type) {
    case INPUT_T:
        HASH_ADD_STR((*def)->inputs_table, name, iop_to_add);
        break;
    case OUTPUT_T:
        HASH_ADD_STR((*def)->outputs_table, name, iop_to_add);
        break;
    case PARAMETER_T:
        HASH_ADD_STR((*def)->params_table, name, iop_to_add);
        break;
    default:
        break;
    }

    return 1;
}

////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////

//int mtic_loadDefinition (const char* json_str){
//    return 1;
//}
//int mtic_loadDefinitionFromPath (const char* file_path){
//    return 1;
//}
/**
 * \fn int mtic_clearDefinition()
 * \brief TODO : need to be defined
 *
 * \param
 * \return The error.
 */
int mtic_clearDefinition(){ //clears definition data for the agent
    //TODO : implement
    mtic_debug("mtic_clearDefinition : function need to be defined and implement it !");

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

/**
 * \fn mtic_createInput(const char *name, iopType_t type, void *value)
 * \brief Create and add an input for the agent
 *
 * \param name The name of the Iop
 * \param type The Iop type : input, output or parameter
 * \param value The pointer on the value (the value will be copied)
 * \return The error. 1 is OK, 0 not able to add in definition loaded, -1 not able to add in definition live
 */
int mtic_createInput(const char *name, iopType_t type, void *value){ //value must be copied in function : copied in definition_setIopValue

    //Create the iop
    agent_iop* iop = definition_createIop(name,type,value);

    //Add iop in structure def loaded, need to be copied
    if (definition_addIop(iop,INPUT_T, &mtic_definition_loaded) < 1){
        return 0;
    }

    //Add iop in structure def live, need to be copied
    if (definition_addIop(iop,INPUT_T, &mtic_definition_live) < 1){
        return -1;
    }

    //free iop
    free(iop);

    return 1;
}

/**
 * \fn mtic_createOutput(const char *name, iopType_t type, void *value)
 * \brief Create and add a output for the agent
 *
 * \param name The name of the Iop
 * \param type The Iop type : input, output or parameter
 * \param value The pointer on the value (the value will be copied)
 * \return The error. 1 is OK, 0 not able to add in definition loaded, -1 not able to add in definition live
 */
int mtic_createOutput(const char *name, iopType_t type, void *value){ //value must be copied in function
    //Create the iop
    agent_iop* iop = definition_createIop(name,type,value);

    //Add iop in structure def loaded, need to be copied
    if (definition_addIop(iop,OUTPUT_T, &mtic_definition_loaded) < 1){
        return 0;
    }

    //Add iop in structure def live, need to be copied
    if (definition_addIop(iop,OUTPUT_T, &mtic_definition_live) < 1){
        return -1;
    }

    //free iop
    free(iop);

    return 1;
}

/**
 * \fn mtic_createParameter(const char *name, iopType_t type, void *value)
 * \brief Create and add a parameter for the agent
 *
 * \param name The name of the Iop
 * \param type The Iop type : input, output or parameter
 * \param value The pointer on the value (the value will be copied)
 * \return The error. 1 is OK, 0 not able to add in definition loaded, -1 not able to add in definition live
 */
int mtic_createParameter(const char *name, iopType_t type, void *value){ //value must be copied in function
    //Create the iop
    agent_iop* iop = definition_createIop(name,type,value);

    //Add iop in structure def loaded, need to be copied
    if (definition_addIop(iop,PARAMETER_T, &mtic_definition_loaded) < 1){
        return 0;
    }

    //Add iop in structure def live, need to be copied
    if (definition_addIop(iop,PARAMETER_T, &mtic_definition_live) < 1){
        return -1;
    }

    //free iop
    free(iop);

    return 1;
}
/**
 * \fn mtic_removeInput(const char *name)
 * \brief Remove and free an input for the agent
 *
 * \param name The name of the Iop
 * \return The error. 1 is OK, 0 Definition loaded is NULL, -1 Definition live is NULL, -2 An error occurs while finding the iop by name
 */
int mtic_removeInput(const char *name){
    //check if def loaded exist
    if(mtic_definition_loaded == NULL){
        mtic_debug("Definition loaded is NULL.");
        return 0;
    }

    //check if def live iexist
    if(mtic_definition_live == NULL){
        mtic_debug("Definition live is NULL.");
        return -1;
    }

    //Find Iop
    model_state state;
    agent_iop * iop = model_findIopByName(name,&state);

    //Check if iop exist
    if(iop == NULL){
        mtic_debug("An error occurs while finding the iop by name : the iop is NULL.");
        return -2;
    }
    //remove in definition loaded
    HASH_DEL(mtic_definition_loaded->inputs_table, iop);

    //remove in definition live
    HASH_DEL(mtic_definition_live->inputs_table, iop);

    //free Iop
    free_agent_iop(iop);

    return 1;
}
int mtic_removeOutput(const char *name){
    return 1;
}
int mtic_removeParameter(const char *name){
    return 1;
}
