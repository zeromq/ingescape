//
//  mtic_mapping.c
//
//  Created by Patxi Berard
//  Modified by Mathieu Poirier
//  Modified by Vincent Deliencourt
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

/**
  * \file ../../src/include/mastic.h
  */

#include <stdio.h>
#include <stdlib.h>
#include "mastic_private.h"

mapping_t* mtic_internal_mapping = NULL;


////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////

//hash function to convert input + agent + output into a unique long number
//we use this function to give id value to map_elements in our mapping
//see http://www.cse.yorku.ca/~oz/hash.html
unsigned long djb2_hash (unsigned char *str)
{
    unsigned long hash = 5381;
    int c;
    
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    
    return hash;
}

void mapping_freeMappingElement (mapping_element_t* mapElmt){
    
    if (mapElmt == NULL){
        return;
    }
    if (mapElmt->input_name != NULL){
        free(mapElmt->input_name);
    }
    if (mapElmt->agent_name != NULL){
        free(mapElmt->agent_name);
    }
    if (mapElmt->output_name != NULL){
        free(mapElmt->output_name);
    }
    free(mapElmt);
}

//void free_map_cat (mapping_cat** map_cat){
//
//    free((char*)(*map_cat)->agent_name);
//    (*map_cat)->agent_name = NULL ;
//
//    free((char*)(*map_cat)->category_name);
//    (*map_cat)->category_name = NULL ;
//
//    free ((*map_cat));
//}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////

void mapping_freeMapping (mapping_t* map) {
    if (map == NULL){
        return;
    }
    
    if (map->name != NULL){
        free(map->name);
    }
    if (map->description != NULL){
        free(map->description);
    }
    if (map->version != NULL){
        free(map->version);
    }

    mapping_element_t *current_map_elmt, *tmp_map_elmt;
//    struct mapping_cat *current_map_cat, *tmp_map_cat;

    //Free mapping output
    HASH_ITER(hh, map->map_elements, current_map_elmt, tmp_map_elmt) {
        HASH_DEL(map->map_elements,current_map_elmt);
        mapping_freeMappingElement(current_map_elmt);
    }

//    //Free mapping category
//    HASH_ITER(hh, map->map_cat, current_map_cat, tmp_map_cat) {
//        HASH_DEL(map->map_cat,current_map_cat);
//        free_map_cat(&current_map_cat);
//    }
    free(map);
}

mapping_element_t * mapping_createMappingElement(const char * input_name,
                                                 const char *agent_name,
                                                 const char* output_name)
{
    
    if (input_name == NULL){
        mtic_debug("mapping_createMappingElement : input_name is NULL\n");
        return NULL;
    }
    if (agent_name == NULL){
        mtic_debug("mapping_createMappingElement : agent_name is NULL, no mapping element created\n");
        return NULL;
    }
    if (output_name == NULL){
        mtic_debug("mapping_createMappingElement : output_name is NULL, no mapping element created\n");
        return NULL;
    }
    
    mapping_element_t * new_map_elmt = calloc(1, sizeof(mapping_element_t));
    new_map_elmt->input_name = strdup(input_name);
    new_map_elmt->agent_name = strdup(agent_name);
    new_map_elmt->output_name = strdup(output_name);
    
    return new_map_elmt;
}

bool mapping_checkCompatibilityInputOutput(agent_iop *foundInput, agent_iop *foundOutput)
{
    // Remarks:
    //TODO case output is a string not handled for numerical input. Case of Bool ("true" and "false")
    //TODO Impulsion case. New value output = impulsion triggered in input.
    
    bool isCompatible = false;
    
    switch (foundInput->value_type) {
        case INTEGER_T:
            if( foundOutput->value_type == DATA_T || foundOutput->value_type == STRING_T) {
                isCompatible = false;
                mtic_debug("%s: input '%s' and output '%s' have incompatible types", __FUNCTION__, foundInput->name, foundOutput->name);
            }
            else {
                isCompatible = true;
            }
            break;
            
        case DOUBLE_T:
            if(foundOutput->value_type == DATA_T  || foundOutput->value_type == STRING_T) {
                isCompatible = false;
                mtic_debug("%s: input '%s' and output '%s' have incompatible types", __FUNCTION__, foundInput->name, foundOutput->name);
            }
            else {
                isCompatible = true;
            }
            break;
            
        case BOOL_T:
            if(foundOutput->value_type == DATA_T  || foundOutput->value_type == STRING_T) {
                isCompatible = false;
                mtic_debug("%s: input '%s' and output '%s' have incompatible types", __FUNCTION__, foundInput->name, foundOutput->name);
            }
            else {
                isCompatible = true;
            }
            break;

        case STRING_T:
            if(foundOutput->value_type == DATA_T) {
                isCompatible = false;
                mtic_debug("%s: input '%s' and output '%s' have incompatible types", __FUNCTION__, foundInput->name, foundOutput->name);
            }
            else {
                isCompatible = true;
            }
            break;
            
        case IMPULSION_T:
            if(foundOutput->value_type == DATA_T) {
                isCompatible = false;
                mtic_debug("%s: input '%s' and output '%s' have incompatible types", __FUNCTION__, foundInput->name, foundOutput->name);
            }
            else {
                isCompatible = true;
            }
            break;
            
        case DATA_T:
            //At the developer's discretion
            if(foundOutput->value_type == DATA_T){
                isCompatible = true;
            }
            else
            {
                 mtic_debug("%s: input '%s' and output '%s' have incompatible types", __FUNCTION__, foundInput->name, foundOutput->name);
                isCompatible = false;
            }
            
            break;
    }
    
    return isCompatible;
}

////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////

/**
 *  \defgroup loadClearGetMapFct Agent's mapping: Load / Clear / Get functions
 *
 */

/**
 * \fn int mtic_loadMapping (const char* json_str)
 * \ingroup loadClearGetMapFct
 * \brief load mapping in variable 'mtic_internal_mapping' from a json string
 *
 * \param json_str String in json format. Can't be NULL.
 * \return The error. 1 is OK, 0 json string is NULL or empty, -1 Mapping has not been loaded
 */
int mtic_loadMapping (const char* json_str){

    //Check if the json string is null or empty
    if((json_str == NULL) || (strlen(json_str) == 0))
    {
        mtic_debug("mtic_loadMapping : json string is null or empty\n");
        return 0;
    }

    //Load definition and init variable : mtic_definition_loaded
    mapping_t *tmp = parser_LoadMap(json_str);

    if(tmp == NULL)
    {
        mtic_debug("mtic_loadMapping : mapping could not be loaded from json string : %s\n", json_str );
        return -1;
    }else{
        mtic_internal_mapping = tmp;
        network_needToUpdateMapping = true;
    }
    
    return 1;
}

/**
 * \fn int mtic_loadMappingFromPath (const char* file_path)
 * \ingroup loadClearGetMapFct
 * \brief load mapping in variable 'mtic_internal_mapping' from a file path
 *
 * \param file_path The string which contains the json file path. Can't be NULL.
 * \return The error. 1 is OK, 0 file path is NULL or empty, -1 Definition file has not been loaded
 */
int mtic_loadMappingFromPath (const char* file_path){
    
    //Check if the json string is null or empty
    if((file_path == NULL) || (strlen(file_path) == 0))
    {
        mtic_debug("mtic_loadMappingFromPath : json file path is null or empty\n");
        return 0;
    }
    
    //Load definition and init variable : mtic_definition_loaded
    mapping_t *tmp = parser_LoadMapFromPath(file_path);
    
    if(tmp == NULL)
    {
        mtic_debug("mtic_loadMappingFromPath : mapping could not be loaded from path %s\n", file_path );
        return -1;
    }else{
        mtic_internal_mapping = tmp;
        network_needToUpdateMapping = true;
    }
    
    return 1;
}

/**
 * \fn int mtic_clearMapping()
 * \ingroup loadClearGetMapFct
 * \brief Clear the variable 'mtic_internal_mapping' and free all structures inside and itself
 *
 * \return The error. 1 is OK,
 * 0 file path is NULL or empty
 */
int mtic_clearMapping(){
    mtic_debug("Clear current mapping and initiate an empty one\n");
    if(mtic_internal_mapping != NULL){
        mapping_freeMapping(mtic_internal_mapping);
    }
    mtic_internal_mapping = calloc(1, sizeof(struct mapping));
    mtic_internal_mapping->name = NULL;
    mtic_internal_mapping->description = NULL;
    mtic_internal_mapping->version = NULL;
    mtic_internal_mapping->map_elements = NULL;
    network_needToUpdateMapping = true;
    return 1;
}

/**
 * \fn char* mtic_getMapping()
 * \ingroup loadClearGetMapFct
 * \brief the agent mapping getter
 *
 * \return The loaded mapping string in json format. NULL if mtic_internal_mapping was not initialized.
 * \warning Allocate memory that should be freed by the user.
 */
char* mtic_getMapping(){
    char * mappingJson = NULL;

    if(mtic_internal_mapping == NULL){
        mtic_debug("mtic_getMapping : no mapping defined yet\n");
        return NULL;
    }
    mappingJson = parser_export_mapping(mtic_internal_mapping);

    return mappingJson;
}

char *mtic_getMappingName(void){
    if (mtic_internal_mapping != NULL && mtic_internal_mapping->name != NULL){
        return strdup(mtic_internal_mapping->name);
    }else{
        return NULL;
    }
}

char *mtic_getMappingDescription(void){
    if (mtic_internal_mapping != NULL && mtic_internal_mapping->description != NULL){
        return strdup(mtic_internal_mapping->description);
    }else{
        return NULL;
    }
}

char *mtic_getMappingVersion(void){
    if (mtic_internal_mapping != NULL && mtic_internal_mapping->version != NULL){
        return strdup(mtic_internal_mapping->version);
    }else{
        return NULL;
    }
}

/**
 *  \defgroup EditMapFct Agent's mapping: Edit functions
 *
 */

/**
 * \fn int mtic_setMappingName(char *name)
 * \ingroup EditMapFct
 * \brief the agent mapping name setter
 *
 * \param name The string which contains the name of the agent's mapping. Can't be NULL.
 * \return The error. 1 is OK, 0 Mapping name is NULL, -1 Mapping name is empty
 */
int mtic_setMappingName(char *name){
    if(name == NULL){
        mtic_debug("mtic_setMappingName : mapping name cannot be NULL \n");
        return 0;
    }

    if (strlen(name) == 0){
        mtic_debug("mtic_setMappingName : mapping name cannot be empty\n");
        return -1;
    }

    //Check if already initialized, and do it if not
    if(mtic_internal_mapping == NULL){
        mtic_clearMapping();
    }

    if(mtic_internal_mapping->name != NULL){
        free(mtic_internal_mapping->name);
    }
    mtic_internal_mapping->name = strdup(name);

    return 1;
}

/**
 * \fn int mtic_setMappingDescription(char *description)
 * \ingroup EditMapFct
 * \brief the agent mapping description setter
 *
 * \param description The string which contains the description of the agent's mapping. Can't be NULL.
 * \return The error. 1 is OK, 0 Mapping description is NULL, -1 Mapping description is empty
 */
int mtic_setMappingDescription(char *description){
    if(description == NULL){
        mtic_debug("Mapping description cannot be NULL \n");
        return 0;
    }

    if (strlen(description) == 0){
        mtic_debug("Mapping description cannot be empty\n");
        return -1;
    }

    //Check if already initialized, and do it if not
    if(mtic_internal_mapping == NULL){
        mtic_clearMapping();
    }

    //Copy the description in the structure
    if(mtic_internal_mapping->description != NULL){
        free(mtic_internal_mapping->description);
    }
    mtic_internal_mapping->description = strdup(description);

    return 1;
}

/**
 * \fn int mtic_setMappingVersion(char *version)
 * \ingroup EditMapFct
 * \brief the agent mapping version setter
 *
 * \param version The string which contains the version of the agent's mapping. Can't be NULL.
 * \return The error. 1 is OK, 0 Mapping version is NULL, -1 Mapping version is empty
 */
int mtic_setMappingVersion(char *version){
    if(version == NULL){
        mtic_debug("Mapping version cannot be NULL \n");
        return 0;
    }

    if (strlen(version) == 0){
        mtic_debug("Mapping version cannot be empty\n");
        return -1;
    }

    //Check if already initialized, and do it if not
    if(mtic_internal_mapping == NULL){
        mtic_clearMapping();
    }

    //Copy the description in the structure
    if(mtic_internal_mapping->version != NULL){
        free(mtic_internal_mapping->version);
    }
    mtic_internal_mapping->version = strdup(version);

    return 1;
}

/**
 * \fn int mtic_getMappingEntriesNumber()
 * \ingroup EditMapFct
 * \brief the agent mapping entries number getter
 *
 * \return The number of mapping type output entries. If -1 The structure mtic_internal_mapping is NULL.
 */
int mtic_getMappingEntriesNumber(){ //number of entries in the mapping

    if(mtic_internal_mapping == NULL || mtic_internal_mapping->map_elements == NULL){
        mtic_debug("mtic_getMappingEntriesNumber : no mapping defined yet \n");
        return 0;
    }
    return HASH_COUNT(mtic_internal_mapping->map_elements);;
}

/**
 * \fn int mtic_addMappingEntry(char *fromOurInput, char *toAgent, char *withOutput)
 * \ingroup EditMapFct
 * \brief this function allows the user to add a new mapping entry dynamically
 *
 * \param fromOurInput The string which contains the name of the input to be mapped. Can't be NULL.
 * \param toAgent The string which contains the name of the extern agent. Can't be NULL.
 * \param withOutput The string which contains the name of the output of the extern agent to be mapped. Can't be NULL.
 * \return The error. 1 is OK.
 *  0 Our input name to be mapped cannot be NULL or empty.
 * -1 Agent name to be mapped cannot be NULL or empty.
 * -2 Extern agent output name to be mapped cannot be NULL or empty.
 */
unsigned long mtic_addMappingEntry(const char *fromOurInput, const char *toAgent, const char *withOutput){ //returns mapping id or 0 if creation failed

    /***    Check the string    ***/
    //fromOurInput
    if((fromOurInput == NULL) || (strlen(fromOurInput) == 0)){
        mtic_debug("mtic_addMappingEntry : input name to be mapped cannot be NULL or empty\n");
        return 0;
    }

    //toAgent
    if((toAgent == NULL) || (strlen(toAgent) == 0)){
        mtic_debug("mtic_addMappingEntry : agent name to be mapped cannot be NULL or empty\n");
        return 0;
    }

    //withOutput
    if((withOutput == NULL) || (strlen(withOutput) == 0)){
        mtic_debug("mtic_addMappingEntry : agent output name to be mapped cannot be NULL or empty\n");
        return 0;
    }

    //Check if already initialized, and do it if not
    if(mtic_internal_mapping == NULL){
        mtic_clearMapping();
    }

    //Add the new mapping element if not already there
    unsigned long len = strlen(fromOurInput)+strlen(toAgent)+strlen(withOutput)+3+1;
    char *mashup = calloc(1, len*sizeof(char));
    strcpy(mashup, fromOurInput);
    strcat(mashup, ".");//separator
    strcat(mashup, toAgent);
    strcat(mashup, ".");//separator
    strcat(mashup, withOutput);
    mashup[len -1] = '\0';
    unsigned long h = djb2_hash((unsigned char *)mashup);
    free (mashup);
    
    mapping_element_t *tmp = NULL;
    if (mtic_internal_mapping->map_elements != NULL){
        HASH_FIND(hh, mtic_internal_mapping->map_elements, &h, sizeof(unsigned long), tmp);
    }
    if (tmp == NULL){
        //element does not exist yet : create and register it
        //TODO: check input against definition and reject if input does not exist in definition
        mapping_element_t *new = mapping_createMappingElement(fromOurInput, toAgent, withOutput);
        new->id = h;
        HASH_ADD(hh, mtic_internal_mapping->map_elements, id, sizeof(unsigned long), new);
        network_needToUpdateMapping = true;
        return h;
    }else{
        mtic_debug("mtic_addMappingEntry : mapping combination (%s,%s%s) already exists\n", fromOurInput, toAgent, withOutput);
        return h;
    }
}

/**
 * \fn int mtic_removeMappingEntryWithId(int theId)
 * \ingroup EditMapFct
 * \brief this function allows the user to remove a mapping in table by its id
 *
 * \param theId The id of the mapping. Cannot be negative.
 * \return The error. 1 is OK.
 * 0 The id of the mapping cannot be negative.
 * -1 The structure mtic_internal_mapping is NULL.
 * -2 The structure mapping out is NULL.
 */
int mtic_removeMappingEntryWithId(unsigned long theId){

    mapping_element_t *el = NULL;

    if(mtic_internal_mapping == NULL){
        mtic_debug("mtic_removeMappingEntryWithId : no mapping defined yet\n");
        return -1;
    }

    if(mtic_internal_mapping->map_elements == NULL){
        mtic_debug("mtic_removeMappingEntryWithId : no mapping defined yet\n");
        return -2;
    }

    HASH_FIND(hh, mtic_internal_mapping->map_elements, &theId, sizeof(unsigned long), el);

    if(el == NULL){
        mtic_debug("mtic_removeMappingEntryWithId : this id is not part of the current mapping\n");
        return 0;
    }else{
        HASH_DEL(mtic_internal_mapping->map_elements, el);
        mapping_freeMappingElement(el);
        network_needToUpdateMapping = true;
    }

    return 1;
}

/**
 * \fn int mtic_removeMappingEntryWithName(char *fromOurInput, char *toAgent, char *withOutput)
 * \ingroup EditMapFct
 * \brief this function allows the user to remove a mapping in table by the input name, the extern agent's name, the extern agent's output
 *
 * \param fromOurInput The string which contains the name of the input mapped. Can't be NULL.
 * \param toAgent The string which contains the name of the extern agent. Can't be NULL.
 * \param withOutput The string which contains the name of the output mapped of the extern agent. Can't be NULL.
 * \return The error. 1 is OK.
 *  0 Our input name to be mapped cannot be NULL or empty.
 * -1 Agent name to be mapped cannot be NULL or empty.
 * -2 Extern agent output name to be mapped cannot be NULL or empty.
 * -3 The structure mtic_internal_mapping is NULL.
 * -4 The structure mapping out is NULL.
 */
int mtic_removeMappingEntryWithName(const char *fromOurInput, const char *toAgent, const char *withOutput){
    /***    Check the string    ***/
    //fromOurInput
    if((fromOurInput == NULL) || (strlen(fromOurInput) == 0)){
        mtic_debug("mtic_removeMappingEntryWithName : input name to be mapped cannot be NULL or empty\n");
        return 0;
    }
    
    //toAgent
    if((toAgent == NULL) || (strlen(toAgent) == 0)){
        mtic_debug("mtic_removeMappingEntryWithName : agent name to be mapped cannot be NULL or empty\n");
        return -1;
    }
    
    //withOutput
    if((withOutput == NULL) || (strlen(withOutput) == 0)){
        mtic_debug("mtic_removeMappingEntryWithName : agent output name to be mapped cannot be NULL or empty\n");
        return -2;
    }

    //Check if already initialized, and do it if not
    if(mtic_internal_mapping == NULL){
        mtic_clearMapping();
        mtic_debug("mtic_removeMappingEntryWithName : no mapping defined yet\n");
        return -3;
    }

    if(mtic_internal_mapping->map_elements == NULL){
        mtic_debug("mtic_removeMappingEntryWithName : no mapping defined yet\n");
        return -4;
    }

    unsigned long len = strlen(fromOurInput)+strlen(toAgent)+strlen(withOutput)+3+1;
    char *mashup = calloc(1, len*sizeof(char));
    strcpy(mashup, fromOurInput);
    strcat(mashup, ".");//separator
    strcat(mashup, toAgent);
    strcat(mashup, ".");//separator
    strcat(mashup, withOutput);
    mashup[len -1] = '\0';
    unsigned long h = djb2_hash((unsigned char *)mashup);
    free (mashup);
    
    mapping_element_t *tmp = NULL;
    if (mtic_internal_mapping->map_elements != NULL){
        HASH_FIND(hh, mtic_internal_mapping->map_elements, &h, sizeof(unsigned long), tmp);
    }
    if (tmp == NULL){
        //element does not exist
        mtic_debug("mtic_removeMappingEntryWithName : this combination is not part of the current mapping\n");
        return -5;
    }else{
        HASH_DEL(mtic_internal_mapping->map_elements, tmp);
        mapping_freeMappingElement(tmp);
        network_needToUpdateMapping = true;
        return 1;
    }
}


