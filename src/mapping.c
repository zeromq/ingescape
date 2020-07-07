//
//  igs_mapping.c
//
//  Created by Patxi Berard & Stephane Vales
//  Copyright © 2017 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#if (defined WIN32 || defined _WIN32)
#include "unixfunctions.h"
#endif
#include "ingescape_private.h"

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

void mapping_freeMappingElement (igs_mapping_element_t* mapElmt){
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

void mapping_freeMapping (igs_mapping_t* map) {
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
    igs_mapping_element_t *current_map_elmt, *tmp_map_elmt;
    HASH_ITER(hh, map->map_elements, current_map_elmt, tmp_map_elmt) {
        HASH_DEL(map->map_elements,current_map_elmt);
        mapping_freeMappingElement(current_map_elmt);
    }
    free(map);
}

igs_mapping_element_t * mapping_createMappingElement(const char * input_name,
                                                 const char *agent_name,
                                                 const char* output_name){
    if (input_name == NULL){
        igs_error("Input name is NULL");
        return NULL;
    }
    if (agent_name == NULL){
        igs_error("Agent name is NULL");
        return NULL;
    }
    if (output_name == NULL){
        igs_error("Output name is NULL");
        return NULL;
    }
    
    igs_mapping_element_t * new_map_elmt = calloc(1, sizeof(igs_mapping_element_t));
    new_map_elmt->input_name = strdup(input_name);
    new_map_elmt->agent_name = strdup(agent_name);
    new_map_elmt->output_name = strdup(output_name);
    
    return new_map_elmt;
}

bool mapping_checkInputOutputCompatibility(igs_agent_t *agent, igs_iop_t *input, igs_iop_t *output){
    //for compatibility, only DATA outputs imply limitations
    //the rest is handled correctly in model_writeIOP
    bool isCompatible = true;
    iopType_t type = input->value_type;
    if (output->value_type == IGS_DATA_T){
        if (type != IGS_DATA_T && type != IGS_IMPULSION_T){
            isCompatible = false;
            igsAgent_warn(agent, "DATA outputs can only be mapped by DATA or IMPULSION inputs");
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
 * \fn int igs_loadMapping (const char* json_str)
 * \ingroup loadClearGetMapFct
 * \brief load mapping in variable 'agent->mapping' from a json string
 *
 * \param json_str String in json format. Can't be NULL.
 * \return The error. 1 is OK, 0 json string is NULL or empty, -1 Mapping has not been loaded
 */
int igsAgent_loadMapping (igs_agent_t *agent, const char* json_str){
    if(json_str == NULL || strlen(json_str) == 0){
        igsAgent_error(agent, "Json string is null or empty");
        return 0;
    }
    igs_mapping_t *tmp = parser_LoadMap(json_str);
    if(tmp == NULL){
        igsAgent_error(agent, "Mapping could not be loaded from json string : %s", json_str);
        return -1;
    }else{
        agent->mapping = tmp;
        agent->network_needToUpdateMapping = true;
    }
    return 1;
}

/**
 * \fn int igs_loadMappingFromPath (const char* file_path)
 * \ingroup loadClearGetMapFct
 * \brief load mapping in variable 'agent->mapping' from a file path
 *
 * \param file_path The string which contains the json file path. Can't be NULL.
 * \return The error. 1 is OK, 0 file path is NULL or empty, -1 Definition file has not been loaded
 */
int igsAgent_loadMappingFromPath (igs_agent_t *agent, const char* file_path){
    if(file_path == NULL || strlen(file_path) == 0){
        igsAgent_error(agent, "Json file path is null");
        return 0;
    }
    if (strlen(file_path) == 0){
        igsAgent_debug(agent, "Json file path is empty");
        return 1;
    }
    igs_mapping_t *tmp = parser_LoadMapFromPath(file_path);
    if(tmp == NULL){
        igsAgent_error(agent, "Mapping could not be loaded from path %s", file_path);
        return -1;
    }else{
        strncpy(agent->mappingPath, file_path, IGS_MAX_PATH_LENGTH - 1);
        agent->mapping = tmp;
        agent->network_needToUpdateMapping = true;
    }
    return 1;
}

/**
 * \fn int igs_clearMapping()
 * \ingroup loadClearGetMapFct
 * \brief Clear the variable 'agent->mapping' and free all structures inside and itself
 *
 * \return The error. 1 is OK,
 * 0 file path is NULL or empty
 */
int igsAgent_clearMapping(igs_agent_t *agent){
    igsAgent_debug(agent, "Clear current mapping if needed and initiate an empty one");
    if(agent->mapping != NULL){
        mapping_freeMapping(agent->mapping);
    }
    agent->mapping = calloc(1, sizeof(struct igs_mapping));
    agent->mapping->name = NULL;
    agent->mapping->description = NULL;
    agent->mapping->version = NULL;
    agent->mapping->map_elements = NULL;
    agent->network_needToUpdateMapping = true;
    return 1;
}

/**
 * \fn char* igs_getMapping()
 * \ingroup loadClearGetMapFct
 * \brief the agent mapping getter
 *
 * \return The loaded mapping string in json format. NULL if agent->mapping was not initialized.
 * \warning Allocate memory that should be freed by the user.
 */
char* igsAgent_getMapping(igs_agent_t *agent){
    char * mappingJson = NULL;
    if(agent->mapping == NULL){
        igsAgent_warn(agent, "No mapping defined yet");
        return NULL;
    }
    mappingJson = parser_export_mapping(agent->mapping);
    return mappingJson;
}

char* igsAgent_getMappingName(igs_agent_t *agent){
    if (agent->mapping != NULL && agent->mapping->name != NULL){
        return strdup(agent->mapping->name);
    }else{
        return NULL;
    }
}

char* igsAgent_getMappingDescription(igs_agent_t *agent){
    if (agent->mapping != NULL && agent->mapping->description != NULL){
        return strdup(agent->mapping->description);
    }else{
        return NULL;
    }
}

char* igsAgent_getMappingVersion(igs_agent_t *agent){
    if (agent->mapping != NULL && agent->mapping->version != NULL){
        return strdup(agent->mapping->version);
    }else{
        return NULL;
    }
}

/**
 *  \defgroup EditMapFct Agent's mapping: Edit functions
 *
 */

/**
 * \fn int igs_setMappingName(char *name)
 * \ingroup EditMapFct
 * \brief the agent mapping name setter
 *
 * \param name The string which contains the name of the agent's mapping. Can't be NULL.
 * \return The error. 1 is OK, 0 Mapping name is NULL, -1 Mapping name is empty
 */
int igsAgent_setMappingName(igs_agent_t *agent, const char *name){
    if(name == NULL){
        igsAgent_error(agent, "Mapping name cannot be NULL");
        return 0;
    }
    if (strlen(name) == 0){
        igsAgent_error(agent, "Mapping name cannot be empty");
        return -1;
    }
    if(agent->mapping == NULL){
        igsAgent_clearMapping(agent);
    }
    if(agent->mapping->name != NULL){
        free(agent->mapping->name);
    }
    agent->mapping->name = strndup(name, IGS_MAX_MAPPING_NAME_LENGTH);
    return 1;
}

/**
 * \fn int igs_setMappingDescription(char *description)
 * \ingroup EditMapFct
 * \brief the agent mapping description setter
 *
 * \param description The string which contains the description of the agent's mapping. Can't be NULL.
 * \return The error. 1 is OK, 0 Mapping description is NULL, -1 Mapping description is empty
 */
int igsAgent_setMappingDescription(igs_agent_t *agent, const char *description){
    if(description == NULL){
        igsAgent_error(agent, "Mapping description cannot be NULL");
        return 0;
    }
    if (strlen(description) == 0){
        igsAgent_error(agent, "Mapping description cannot be empty");
        return -1;
    }
    if(agent->mapping == NULL){
        igsAgent_clearMapping(agent);
    }
    if(agent->mapping->description != NULL){
        free(agent->mapping->description);
    }
    agent->mapping->description = strndup(description, IGS_MAX_DESCRIPTION_LENGTH);
    return 1;
}

/**
 * \fn int igs_setMappingVersion(char *version)
 * \ingroup EditMapFct
 * \brief the agent mapping version setter
 *
 * \param version The string which contains the version of the agent's mapping. Can't be NULL.
 * \return The error. 1 is OK, 0 Mapping version is NULL, -1 Mapping version is empty
 */
int igsAgent_setMappingVersion(igs_agent_t *agent, const char *version){
    if(version == NULL){
        igsAgent_error(agent, "Mapping version cannot be NULL");
        return 0;
    }
    if (strlen(version) == 0){
        igsAgent_error(agent, "Mapping version cannot be empty");
        return -1;
    }
    if(agent->mapping == NULL){
        igsAgent_clearMapping(agent);
    }
    if(agent->mapping->version != NULL){
        free(agent->mapping->version);
    }
    agent->mapping->version = strndup(version, 64);
    return 1;
}

/**
 * \fn int igs_getMappingEntriesNumber()
 * \ingroup EditMapFct
 * \brief the agent mapping entries number getter
 *
 * \return The number of mapping type output entries. If -1 The structure agent->mapping is NULL.
 */
int igsAgent_getMappingEntriesNumber(igs_agent_t *agent){
    if(agent->mapping == NULL){
        igsAgent_warn(agent, "No mapping defined yet");
        return 0;
    }
    return HASH_COUNT(agent->mapping->map_elements);;
}

/**
 * \fn int igs_addMappingEntry(char *fromOurInput, char *toAgent, char *withOutput)
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
unsigned long igsAgent_addMappingEntry(igs_agent_t *agent,
                                       const char *fromOurInput,
                                       const char *toAgent,
                                       const char *withOutput){
    //fromOurInput
    if(fromOurInput == NULL || strlen(fromOurInput) == 0){
        igsAgent_error(agent, "Input name to be mapped cannot be NULL or empty");
        return 0;
    }
    char *reviewedFromOurInput = strndup(fromOurInput, IGS_MAX_IOP_NAME_LENGTH);
    bool spaceInName = false;
    size_t i = 0;
    size_t lengthOfReviewedFromOurInput = strlen(reviewedFromOurInput);
    for (i = 0; i < lengthOfReviewedFromOurInput; i++){
        if (reviewedFromOurInput[i] == ' '){
            reviewedFromOurInput[i] = '_';
            spaceInName = true;
        }
    }
    if (spaceInName){
        igsAgent_warn(agent, "Spaces are not allowed in IOP name : %s has been renamed to %s\n", fromOurInput, reviewedFromOurInput);
    }

    //toAgent
    if(toAgent == NULL || strlen(toAgent) == 0){
        igsAgent_error(agent, "Agent name to be mapped cannot be NULL or empty");
        free(reviewedFromOurInput);
        return 0;
    }
    char *reviewedToAgent = strndup(toAgent, IGS_MAX_IOP_NAME_LENGTH);
    size_t lengthOfReviewedToAgent = strlen(reviewedToAgent);
    spaceInName = false;
    for (i = 0; i < lengthOfReviewedToAgent; i++){
        if (reviewedToAgent[i] == ' '){
            reviewedToAgent[i] = '_';
            spaceInName = true;
        }
    }
    if (spaceInName){
        igsAgent_warn(agent, "Spaces are not allowed in agent name: %s has been renamed to %s", toAgent, reviewedToAgent);
    }
    char *aName = igsAgent_getAgentName(agent);
    if (strcmp(reviewedToAgent, aName) == 0){
        igsAgent_warn(agent, "mapping inputs to outputs of the same agent will not work (except from one clone or variant to others)");
    }
    free(aName);

    //withOutput
    if((withOutput == NULL) || (strlen(withOutput) == 0)){
        igsAgent_error(agent, "Agent output name to be mapped cannot be NULL or empty");
        free(reviewedToAgent);
        return 0;
    }
    char *reviewedWithOutput = strndup(withOutput, IGS_MAX_IOP_NAME_LENGTH);
    size_t lengthOfReviewedWithOutput = strlen(reviewedWithOutput);
    spaceInName = false;
    for (i = 0; i < lengthOfReviewedWithOutput; i++){
        if (reviewedWithOutput[i] == ' '){
            reviewedWithOutput[i] = '_';
            spaceInName = true;
        }
    }
    if (spaceInName){
        igsAgent_warn(agent, "Spaces are not allowed in IOP: %s has been renamed to %s", withOutput, reviewedWithOutput);
    }

    //Check if already initialized, and do it if not
    if(agent->mapping == NULL){
        igsAgent_clearMapping(agent);
    }

    //Add the new mapping element if not already there
    size_t len = strlen(fromOurInput)+strlen(toAgent)+strlen(withOutput)+3+1;
    char *mashup = calloc(1, len*sizeof(char));
    strcpy(mashup, reviewedFromOurInput);
    strcat(mashup, ".");//separator
    strcat(mashup, reviewedToAgent);
    strcat(mashup, ".");//separator
    strcat(mashup, reviewedWithOutput);
    mashup[len -1] = '\0';
    unsigned long h = djb2_hash((unsigned char *)mashup);
    free (mashup);
    
    igs_mapping_element_t *tmp = NULL;
    if (agent->mapping->map_elements != NULL){
        HASH_FIND(hh, agent->mapping->map_elements, &h, sizeof(unsigned long), tmp);
    }
    if (tmp == NULL){
        //element does not exist yet : create and register it
        //check input against definition and reject if input does not exist in definition
        if (igsAgent_checkInputExistence(agent, reviewedFromOurInput)){
            igs_mapping_element_t *new = mapping_createMappingElement(reviewedFromOurInput, reviewedToAgent, reviewedWithOutput);
            new->id = h;
            HASH_ADD(hh, agent->mapping->map_elements, id, sizeof(unsigned long), new);
            agent->network_needToUpdateMapping = true;
        }else{
            igsAgent_error(agent, "Input %s does not exist in our definition : cannot create mapping entry for it", reviewedFromOurInput);
            free(reviewedFromOurInput);
            free(reviewedToAgent);
            free(reviewedWithOutput);
            return 0;
        }
        
    }else{
        igsAgent_warn(agent, "Mapping combination %s.%s->%s already exists", reviewedFromOurInput, reviewedToAgent, reviewedWithOutput);
    }
    free(reviewedFromOurInput);
    free(reviewedToAgent);
    free(reviewedWithOutput);
    return h;
}

/**
 * \fn int igs_removeMappingEntryWithId(int theId)
 * \ingroup EditMapFct
 * \brief this function allows the user to remove a mapping in table by its id
 *
 * \param theId The id of the mapping. Cannot be negative.
 * \return The error. 1 is OK.
 * 0 The id of the mapping cannot be negative.
 * -1 The structure agent->mapping is NULL.
 * -2 The structure mapping out is NULL.
 */
int igsAgent_removeMappingEntryWithId(igs_agent_t *agent, unsigned long theId){
    igs_mapping_element_t *el = NULL;
    if(agent->mapping == NULL){
        igsAgent_error(agent, "No mapping defined yet");
        return -1;
    }
    if(agent->mapping->map_elements == NULL){
        igsAgent_error(agent, "No mapping elements defined yet");
        return -2;
    }
    HASH_FIND(hh, agent->mapping->map_elements, &theId, sizeof(unsigned long), el);
    if(el == NULL){
        igsAgent_warn(agent, "id %ld is not part of the current mapping", theId);
        return 0;
    }else{
        HASH_DEL(agent->mapping->map_elements, el);
        mapping_freeMappingElement(el);
        agent->network_needToUpdateMapping = true;
    }
    return 1;
}

/**
 * \fn int igs_removeMappingEntryWithName(char *fromOurInput, char *toAgent, char *withOutput)
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
 * -3 The structure agent->mapping is NULL.
 * -4 The structure mapping out is NULL.
 */
int igsAgent_removeMappingEntryWithName(igs_agent_t *agent, const char *fromOurInput,
                                        const char *toAgent, const char *withOutput){
    if(fromOurInput == NULL || strlen(fromOurInput) == 0){
        igsAgent_error(agent, "Input name to be mapped cannot be NULL or empty");
        return 0;
    }
    if(toAgent == NULL || strlen(toAgent) == 0){
        igsAgent_error(agent, "Agent name to be mapped cannot be NULL or empty");
        return -1;
    }
    if(withOutput == NULL || strlen(withOutput) == 0){
        igsAgent_error(agent, "Agent output name to be mapped cannot be NULL or empty");
        return -2;
    }
    if(agent->mapping == NULL){
        igsAgent_clearMapping(agent);
        igsAgent_error(agent, "No mapping defined yet");
        return -3;
    }
    if(agent->mapping->map_elements == NULL){
        igsAgent_error(agent, "No mapping elements defined yet");
        return -4;
    }

    size_t len = strlen(fromOurInput)+strlen(toAgent)+strlen(withOutput)+3+1;
    char *mashup = calloc(1, len*sizeof(char));
    strcpy(mashup, fromOurInput);
    strcat(mashup, ".");//separator
    strcat(mashup, toAgent);
    strcat(mashup, ".");//separator
    strcat(mashup, withOutput);
    mashup[len -1] = '\0';
    unsigned long h = djb2_hash((unsigned char *)mashup);
    free (mashup);
    
    igs_mapping_element_t *tmp = NULL;
    if (agent->mapping->map_elements != NULL){
        HASH_FIND(hh, agent->mapping->map_elements, &h, sizeof(unsigned long), tmp);
    }
    if (tmp == NULL){
        igsAgent_warn(agent, "Mapping combination %s.%s->%s does NOT exist", fromOurInput, toAgent, withOutput);
        return -5;
    }else{
        HASH_DEL(agent->mapping->map_elements, tmp);
        mapping_freeMappingElement(tmp);
        agent->network_needToUpdateMapping = true;
        return 1;
    }
}

void igsAgent_setMappingPath(igs_agent_t *agent, const char *path){
    strncpy(agent->mappingPath, path, IGS_MAX_PATH_LENGTH - 1);
    if (coreContext != NULL && coreContext->node != NULL){
        bus_zyreLock();
        zyre_shouts(coreContext->node, IGS_PRIVATE_CHANNEL, "MAPPING_FILE_PATH=%s", agent->mappingPath);
        bus_zyreUnlock();
    }
}

void igsAgent_writeMappingToPath(igs_agent_t *agent){
    FILE *fp = NULL;
    fp = fopen (agent->mappingPath,"w+");
    if (fp == NULL){
        igsAgent_error(agent, "Could not open %s for writing", agent->mappingPath);
    }else{
        char *map = parser_export_mapping(agent->mapping);
        fprintf(fp, "%s", map);
        fflush(fp);
        fclose(fp);
        free(map);
    }
}
