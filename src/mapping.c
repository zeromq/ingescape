//
//  mtic_mapping.c
//
//  Created by Patxi Berard
//  Modified by Mathieu Poirier
//  Modified by Vincent Deliencourt
//  Modified by Stephane Vales
//  Copyright © 2017 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include "unixfunctions.h"
#endif
#include "mastic_private.h"

mapping_t* mtic_internal_mapping = NULL;
char mappingPath[MAX_PATH] = "";

////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////

//hash function to convert input + agent + output into a unique long number
//we use this function to give id value to map_elements in our mapping
//see http://www.cse.yorku.ca/~oz/hash.html
unsigned long djb2_hash (unsigned char *str){
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
    HASH_ITER(hh, map->map_elements, current_map_elmt, tmp_map_elmt) {
        HASH_DEL(map->map_elements,current_map_elmt);
        mapping_freeMappingElement(current_map_elmt);
    }
    free(map);
}

mapping_element_t * mapping_createMappingElement(const char * input_name,
                                                 const char *agent_name,
                                                 const char* output_name){
    if (input_name == NULL){
        mtic_error("Input name is NULL");
        return NULL;
    }
    if (agent_name == NULL){
        mtic_error("Agent name is NULL");
        return NULL;
    }
    if (output_name == NULL){
        mtic_error("Output name is NULL");
        return NULL;
    }
    
    mapping_element_t * new_map_elmt = calloc(1, sizeof(mapping_element_t));
    new_map_elmt->input_name = strdup(input_name);
    new_map_elmt->agent_name = strdup(agent_name);
    new_map_elmt->output_name = strdup(output_name);
    
    return new_map_elmt;
}

bool mapping_checkCompatibilityInputOutput(agent_iop *input, agent_iop *output){
    //for compatibility, only DATA outputs imply limitations
    //the rest is handled correctly in model_writeIOP
    bool isCompatible = true;
    iopType_t type = input->value_type;
    if (output->value_type == DATA_T){
        if (type != DATA_T && type != IMPULSION_T){
            isCompatible = false;
            mtic_warn("DATA inputs can only be mapped to DATA or IMPULSION outputs");
        }
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
    if(json_str == NULL || strlen(json_str) == 0){
        mtic_error("Json string is null or empty");
        return 0;
    }
    mapping_t *tmp = parser_LoadMap(json_str);
    if(tmp == NULL){
        mtic_error("Mapping could not be loaded from json string : %s", json_str);
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
    if(file_path == NULL || strlen(file_path) == 0){
        mtic_error("Json file path is null or empty");
        return 0;
    }
    mapping_t *tmp = parser_LoadMapFromPath(file_path);
    if(tmp == NULL){
        mtic_error("Mapping could not be loaded from path %s", file_path);
        return -1;
    }else{
        strncpy(mappingPath, file_path, MAX_PATH);
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
    mtic_info("Clear current mapping if needed and initiate an empty one");
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
        mtic_warn("No mapping defined yet");
        return NULL;
    }
    mappingJson = parser_export_mapping(mtic_internal_mapping);
    return mappingJson;
}

char* mtic_getMappingName(void){
    if (mtic_internal_mapping != NULL && mtic_internal_mapping->name != NULL){
        return strdup(mtic_internal_mapping->name);
    }else{
        return NULL;
    }
}

char* mtic_getMappingDescription(void){
    if (mtic_internal_mapping != NULL && mtic_internal_mapping->description != NULL){
        return strdup(mtic_internal_mapping->description);
    }else{
        return NULL;
    }
}

char* mtic_getMappingVersion(void){
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
int mtic_setMappingName(const char *name){
    if(name == NULL){
        mtic_error("Mapping name cannot be NULL");
        return 0;
    }
    if (strlen(name) == 0){
        mtic_error("Mapping name cannot be empty");
        return -1;
    }
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
int mtic_setMappingDescription(const char *description){
    if(description == NULL){
        mtic_error("Mapping description cannot be NULL");
        return 0;
    }
    if (strlen(description) == 0){
        mtic_error("Mapping description cannot be empty");
        return -1;
    }
    if(mtic_internal_mapping == NULL){
        mtic_clearMapping();
    }
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
int mtic_setMappingVersion(const char *version){
    if(version == NULL){
        mtic_error("Mapping version cannot be NULL");
        return 0;
    }
    if (strlen(version) == 0){
        mtic_error("Mapping version cannot be empty");
        return -1;
    }
    if(mtic_internal_mapping == NULL){
        mtic_clearMapping();
    }
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
int mtic_getMappingEntriesNumber(){
    if(mtic_internal_mapping == NULL){
        mtic_warn("No mapping defined yet");
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
unsigned long mtic_addMappingEntry(const char *fromOurInput,
                                   const char *toAgent,
                                   const char *withOutput){
    //fromOurInput
    if(fromOurInput == NULL || strlen(fromOurInput) == 0){
        mtic_error("Input name to be mapped cannot be NULL or empty");
        return 0;
    }
    char *reviewedFromOurInput = strndup(fromOurInput, MAX_IOP_NAME_LENGTH);
    bool spaceInName = false;
    size_t lengthOfReviewedFromOurInput = strlen(reviewedFromOurInput);
    for (size_t i = 0; i < lengthOfReviewedFromOurInput; i++){
        if (reviewedFromOurInput[i] == ' '){
            reviewedFromOurInput[i] = '_';
            spaceInName = true;
        }
    }
    if (spaceInName){
        mtic_warn("Spaces are not allowed in IOP name : %s has been renamed to %s\n", fromOurInput, reviewedFromOurInput);
    }

    //toAgent
    if(toAgent == NULL || strlen(toAgent) == 0){
        mtic_error("Agent name to be mapped cannot be NULL or empty");
        return 0;
    }
    char *reviewedToAgent = strndup(toAgent, MAX_IOP_NAME_LENGTH);
    size_t lengthOfReviewedToAgent = strlen(reviewedToAgent);
    spaceInName = false;
    for (size_t i = 0; i < lengthOfReviewedToAgent; i++){
        if (reviewedToAgent[i] == ' '){
            reviewedToAgent[i] = '_';
            spaceInName = true;
        }
    }
    if (spaceInName){
        mtic_warn("Spaces are not allowed in agent name: %s has been renamed to %s", toAgent, reviewedToAgent);
    }

    //withOutput
    if((withOutput == NULL) || (strlen(withOutput) == 0)){
        mtic_error("Agent output name to be mapped cannot be NULL or empty");
        return 0;
    }
    char *reviewedWithOutput = strndup(withOutput, MAX_IOP_NAME_LENGTH);
    size_t lengthOfReviewedWithOutput = strlen(reviewedWithOutput);
    spaceInName = false;
    for (size_t i = 0; i < lengthOfReviewedWithOutput; i++){
        if (reviewedWithOutput[i] == ' '){
            reviewedWithOutput[i] = '_';
            spaceInName = true;
        }
    }
    if (spaceInName){
        mtic_warn("Spaces are not allowed in IOP: %s has been renamed to %s", withOutput, reviewedWithOutput);
    }

    //Check if already initialized, and do it if not
    if(mtic_internal_mapping == NULL){
        mtic_clearMapping();
    }

    //Add the new mapping element if not already there
    unsigned long len = strlen(fromOurInput)+strlen(toAgent)+strlen(withOutput)+3+1;
    char *mashup = calloc(1, len*sizeof(char));
    strcpy(mashup, reviewedFromOurInput);
    strcat(mashup, ".");//separator
    strcat(mashup, reviewedToAgent);
    strcat(mashup, ".");//separator
    strcat(mashup, reviewedWithOutput);
    mashup[len -1] = '\0';
    unsigned long h = djb2_hash((unsigned char *)mashup);
    free (mashup);
    
    mapping_element_t *tmp = NULL;
    if (mtic_internal_mapping->map_elements != NULL){
        HASH_FIND(hh, mtic_internal_mapping->map_elements, &h, sizeof(unsigned long), tmp);
    }
    if (tmp == NULL){
        //element does not exist yet : create and register it
        //check input against definition and reject if input does not exist in definition
        if (mtic_checkInputExistence(reviewedFromOurInput)){
            mapping_element_t *new = mapping_createMappingElement(reviewedFromOurInput, reviewedToAgent, reviewedWithOutput);
            new->id = h;
            HASH_ADD(hh, mtic_internal_mapping->map_elements, id, sizeof(unsigned long), new);
            network_needToUpdateMapping = true;
        }else{
            mtic_error("Input %s does not exist in our definition : cannot create mapping entry for it", reviewedFromOurInput);
            free(reviewedFromOurInput);
            free(reviewedToAgent);
            free(reviewedWithOutput);
            return 0;
        }
        
    }else{
        mtic_warn("Mapping combination %s.%s->%s already exists", reviewedFromOurInput, reviewedToAgent, reviewedWithOutput);
    }
    free(reviewedFromOurInput);
    free(reviewedToAgent);
    free(reviewedWithOutput);
    return h;
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
        mtic_error("No mapping defined yet");
        return -1;
    }
    if(mtic_internal_mapping->map_elements == NULL){
        mtic_error("No mapping elements defined yet");
        return -2;
    }
    HASH_FIND(hh, mtic_internal_mapping->map_elements, &theId, sizeof(unsigned long), el);
    if(el == NULL){
        mtic_warn("id %ld is not part of the current mapping", theId);
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
    if(fromOurInput == NULL || strlen(fromOurInput) == 0){
        mtic_error("Input name to be mapped cannot be NULL or empty");
        return 0;
    }
    if(toAgent == NULL || strlen(toAgent) == 0){
        mtic_error("Agent name to be mapped cannot be NULL or empty");
        return -1;
    }
    if(withOutput == NULL || strlen(withOutput) == 0){
        mtic_error("Agent output name to be mapped cannot be NULL or empty");
        return -2;
    }
    if(mtic_internal_mapping == NULL){
        mtic_clearMapping();
        mtic_error("No mapping defined yet");
        return -3;
    }
    if(mtic_internal_mapping->map_elements == NULL){
        mtic_error("No mapping elements defined yet");
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
        mtic_warn("Mapping combination %s.%s->%s does NOT exist", fromOurInput, toAgent, withOutput);
        return -5;
    }else{
        HASH_DEL(mtic_internal_mapping->map_elements, tmp);
        mapping_freeMappingElement(tmp);
        network_needToUpdateMapping = true;
        return 1;
    }
}

void mtic_setMappingPath(const char *path){
    strncpy(mappingPath, path, MAX_PATH - 1);
}

void mtic_writeMappingToPath(void){
    FILE *fp = NULL;
    fp = fopen (mappingPath,"w+");
    if (fp == NULL){
        mtic_error("Could not open %s for writing", mappingPath);
    }else{
        char *map = parser_export_mapping(mtic_internal_mapping);
        fprintf(fp, "%s", map);
        fflush(fp);
        fclose(fp);
        free(map);
    }
}
