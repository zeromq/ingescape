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

void mapping_freeMapping (igs_mapping_t **map) {
    assert (map);
    assert(*map);
    if ((*map) == NULL){
        return;
    }
    if ((*map)->name != NULL){
        free((*map)->name);
    }
    if ((*map)->description != NULL){
        free((*map)->description);
    }
    if ((*map)->version != NULL){
        free((*map)->version);
    }
    igs_mapping_element_t *current_map_elmt, *tmp_map_elmt;
    HASH_ITER(hh, (*map)->map_elements, current_map_elmt, tmp_map_elmt) {
        HASH_DEL((*map)->map_elements,current_map_elmt);
        mapping_freeMappingElement(current_map_elmt);
    }
    free(*map);
    *map = NULL;
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

igs_result_t igsAgent_loadMapping (igs_agent_t *agent, const char* json_str){
    assert(agent);
    assert(json_str);
    igs_mapping_t *tmp = parser_loadMapping(json_str);
    if(tmp == NULL){
        igsAgent_error(agent, "mapping could not be loaded from json string");
        return IGS_FAILURE;
    }else{
        agent->mapping = tmp;
        agent->network_needToUpdateMapping = true;
    }
    return IGS_SUCCESS;
}

igs_result_t igsAgent_loadMappingFromPath (igs_agent_t *agent, const char* file_path){
    assert(agent);
    assert(file_path);
    igs_mapping_t *tmp = parser_loadMappingFromPath(file_path);
    if(tmp == NULL){
        igsAgent_error(agent, "mapping could not be loaded from path '%s'", file_path);
        return IGS_FAILURE;
    }else{
        agent->mappingPath = strndup(file_path, IGS_MAX_PATH_LENGTH - 1);
        agent->mapping = tmp;
        agent->network_needToUpdateMapping = true;
    }
    return IGS_FAILURE;
}

void igsAgent_clearMapping(igs_agent_t *agent){
    igsAgent_debug(agent, "clear current mapping if needed and initiate an empty one");
    if(agent->mapping != NULL){
        mapping_freeMapping(&agent->mapping);
    }
    agent->mapping = calloc(1, sizeof(struct igs_mapping));
    agent->network_needToUpdateMapping = true;
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

void igsAgent_setMappingName(igs_agent_t *agent, const char *name){
    assert(agent);
    assert(name);
    //init mapping if neeed
    if(agent->mapping == NULL){
        agent->mapping = calloc(1, sizeof(struct igs_mapping));
        agent->network_needToUpdateMapping = true;
    }
    if(agent->mapping->name != NULL){
        free(agent->mapping->name);
    }
    agent->mapping->name = strndup(name, IGS_MAX_MAPPING_NAME_LENGTH);
}

void igsAgent_setMappingDescription(igs_agent_t *agent, const char *description){
    assert(agent);
    assert(description);
    //init mapping if neeed
    if(agent->mapping == NULL){
        agent->mapping = calloc(1, sizeof(struct igs_mapping));
        agent->network_needToUpdateMapping = true;
    }
    if(agent->mapping->description != NULL){
        free(agent->mapping->description);
    }
    agent->mapping->description = strndup(description, IGS_MAX_DESCRIPTION_LENGTH);
}

void igsAgent_setMappingVersion(igs_agent_t *agent, const char *version){
    assert(agent);
    assert(version);
    //init mapping if neeed
    if(agent->mapping == NULL){
        agent->mapping = calloc(1, sizeof(struct igs_mapping));
        agent->network_needToUpdateMapping = true;
    }
    if(agent->mapping->version != NULL){
        free(agent->mapping->version);
    }
    agent->mapping->version = strndup(version, 64);
}

size_t igsAgent_getMappingEntriesNumber(igs_agent_t *agent){
    assert(agent);
    if(agent->mapping == NULL){
        igsAgent_warn(agent, "No mapping defined yet");
        return 0;
    }
    return HASH_COUNT(agent->mapping->map_elements);;
}

unsigned long igsAgent_addMappingEntry(igs_agent_t *agent,
                                       const char *fromOurInput,
                                       const char *toAgent,
                                       const char *withOutput){
    assert(agent);
    assert(fromOurInput && strlen(fromOurInput) > 0);
    assert(toAgent && strlen(toAgent) > 0);
    assert(withOutput && strlen(withOutput) > 0);
    //fromOurInput
    char *reviewedFromOurInput = strndup(fromOurInput, IGS_MAX_IOP_NAME_LENGTH);
    bool spaceInName = false;
    size_t i = 0;
    size_t lengthOfReviewedFromOurInput = strlen(reviewedFromOurInput);
    for (i = 0; i < lengthOfReviewedFromOurInput; i++){
        if (reviewedFromOurInput[i] == ' '){
            spaceInName = true;
            break;
        }
    }
    if (spaceInName){
        igsAgent_error(agent, "spaces are not allowed in IOP name '%s'", fromOurInput);
        return 0;
    }

    //toAgent
    char *reviewedToAgent = strndup(toAgent, IGS_MAX_IOP_NAME_LENGTH);
    size_t lengthOfReviewedToAgent = strlen(reviewedToAgent);
    spaceInName = false;
    for (i = 0; i < lengthOfReviewedToAgent; i++){
        if (reviewedToAgent[i] == ' '){
            spaceInName = true;
            break;
        }
    }
    if (spaceInName){
        igsAgent_error(agent, "spaces are not allowed in agent name '%s'", toAgent);
        return 0;
    }
    char *aName = igsAgent_getAgentName(agent);
    if (strcmp(reviewedToAgent, aName) == 0){
        igsAgent_warn(agent, "mapping inputs to outputs of the same agent will not work EXCEPT from one clone or variant to others");
    }
    free(aName);

    //withOutput
    char *reviewedWithOutput = strndup(withOutput, IGS_MAX_IOP_NAME_LENGTH);
    size_t lengthOfReviewedWithOutput = strlen(reviewedWithOutput);
    spaceInName = false;
    for (i = 0; i < lengthOfReviewedWithOutput; i++){
        if (reviewedWithOutput[i] == ' '){
            spaceInName = true;
            break;
        }
    }
    if (spaceInName){
        igsAgent_error(agent, "spaces are not allowed in IOP '%s'", withOutput);
        return 0;
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
    unsigned long hash = djb2_hash((unsigned char *)mashup);
    free (mashup);
    
    igs_mapping_element_t *tmp = NULL;
    if (agent->mapping->map_elements != NULL){
        HASH_FIND(hh, agent->mapping->map_elements, &hash, sizeof(unsigned long), tmp);
    }
    if (tmp == NULL){
        //element does not exist yet : create and register it
        //check input against definition and reject if input does not exist in definition
        if (!igsAgent_checkInputExistence(agent, reviewedFromOurInput)){
            igsAgent_warn(agent, "input %s does not exist in our definition (will be stored anyway)", reviewedFromOurInput);
        }
        igs_mapping_element_t *new = mapping_createMappingElement(reviewedFromOurInput, reviewedToAgent, reviewedWithOutput);
        new->id = hash;
        HASH_ADD(hh, agent->mapping->map_elements, id, sizeof(unsigned long), new);
        agent->network_needToUpdateMapping = true;
    }else{
        igsAgent_warn(agent, "mapping combination %s->%s.%s already exists (will not be duplicated)", reviewedFromOurInput, reviewedToAgent, reviewedWithOutput);
    }
    free(reviewedFromOurInput);
    free(reviewedToAgent);
    free(reviewedWithOutput);
    return hash;
}

igs_result_t igsAgent_removeMappingEntryWithId(igs_agent_t *agent, unsigned long theId){
    assert(agent);
    assert(theId > 0);
    igs_mapping_element_t *el = NULL;
    if(agent->mapping == NULL){
        igsAgent_error(agent, "no mapping defined yet");
        return IGS_FAILURE;
    }
    if(agent->mapping->map_elements == NULL){
        igsAgent_error(agent, "no mapping elements defined yet");
        return IGS_FAILURE;
    }
    HASH_FIND(hh, agent->mapping->map_elements, &theId, sizeof(unsigned long), el);
    if(el == NULL){
        igsAgent_error(agent, "id %ld is not part of the current mapping", theId);
        return IGS_FAILURE;
    }else{
        HASH_DEL(agent->mapping->map_elements, el);
        mapping_freeMappingElement(el);
        agent->network_needToUpdateMapping = true;
    }
    return IGS_SUCCESS;
}

igs_result_t igsAgent_removeMappingEntryWithName(igs_agent_t *agent, const char *fromOurInput,
                                                 const char *toAgent, const char *withOutput){
    assert(agent);
    assert(fromOurInput);
    assert(toAgent);
    assert(withOutput);
    if(agent->mapping == NULL){
        igsAgent_clearMapping(agent);
        igsAgent_error(agent, "no mapping defined yet");
        return IGS_FAILURE;
    }
    if(agent->mapping->map_elements == NULL){
        igsAgent_error(agent, "no mapping elements defined yet");
        return IGS_FAILURE;
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
        igsAgent_error(agent, "mapping combination %s->%s.%s does NOT exist", fromOurInput, toAgent, withOutput);
        return IGS_FAILURE;
    }else{
        HASH_DEL(agent->mapping->map_elements, tmp);
        mapping_freeMappingElement(tmp);
        agent->network_needToUpdateMapping = true;
        return IGS_SUCCESS;
    }
}

void igsAgent_setMappingPath(igs_agent_t *agent, const char *path){
    assert(agent);
    assert(path);
    if (agent->mappingPath != NULL)
        free(agent->mappingPath);
    agent->mappingPath = strndup(path, IGS_MAX_PATH_LENGTH);
    if (coreContext->networkActor != NULL && coreContext->node != NULL){
        bus_zyreLock();
        zmsg_t *msg = zmsg_new();
        zmsg_addstr(msg, "MAPPING_FILE_PATH");
        zmsg_addstr(msg, agent->mappingPath);
        zmsg_addstr(msg, agent->uuid);
        zyre_shout(coreContext->node, IGS_PRIVATE_CHANNEL, &msg);
        bus_zyreUnlock();
    }
}

void igsAgent_writeMappingToPath(igs_agent_t *agent){
    assert(agent);
    if (agent->mapping == NULL){
        igsAgent_error(agent, "mapping is NULL and cannot be written to path");
        return;
    }
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
