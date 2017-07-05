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

mapping * mtic_my_agent_mapping = NULL;

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////

const char * map_state_to_string(map_state state){
    switch (state) {
        case ON:
            return "on";
            break;
        case OFF:
            return "off";
            break;
        case INCOMPATIBLE:
            return "incompatible";
            break;
        case GENERIC:
            return "generic";
            break;
        default:
            fprintf(stderr, "%s - ERROR -  unknown map state type to convert\n", __FUNCTION__);
            break;
    }

    return "";
}

void free_map_out (mapping_out** map_out){

    free((char*)(*map_out)->agent_name);
    (*map_out)->agent_name = NULL ;

    free((char*)(*map_out)->input_name);
    (*map_out)->input_name = NULL ;

    free((char*)(*map_out)->output_name);
    (*map_out)->output_name = NULL ;

    free ((*map_out));
}

void free_map_cat (mapping_cat** map_cat){

    free((char*)(*map_cat)->agent_name);
    (*map_cat)->agent_name = NULL ;

    free((char*)(*map_cat)->category_name);
    (*map_cat)->category_name = NULL ;

    free ((*map_cat));
}

void mapping_FreeMapping (mapping* mapp) {

    struct mapping_out *current_map_out, *tmp_map_out;
    struct mapping_cat *current_map_cat, *tmp_map_cat;

    free((char*)mapp->name);
    mapp->name = NULL;
    free((char*)mapp->version);
    mapp->version = NULL;
    free((char*)mapp->description);
    mapp->description = NULL;

    //Free mapping output
    HASH_ITER(hh, mapp->map_out, current_map_out, tmp_map_out) {
        HASH_DEL(mapp->map_out,current_map_out);
        free_map_out(&current_map_out);
    }

    //Free mapping category
    HASH_ITER(hh, mapp->map_cat, current_map_cat, tmp_map_cat) {
        HASH_DEL(mapp->map_cat,current_map_cat);
        free_map_cat(&current_map_cat);
    }
}

void copy_to_map_global(mapping *loaded){
    if(loaded == NULL)
        return;

    /*
     * General Information
     *
     */
    //Initialize the table mapping if it is not
    if(mtic_my_agent_mapping == NULL){
        mtic_my_agent_mapping = calloc (1, sizeof (struct mapping));
    }
    if(mtic_my_agent_mapping->name) {
        free((char*)mtic_my_agent_mapping->name);
        mtic_my_agent_mapping->name = NULL;
    }
    if(mtic_my_agent_mapping->description) {
        free((char*)mtic_my_agent_mapping->description);
        mtic_my_agent_mapping->description = NULL;
    }
    if(mtic_my_agent_mapping->version) {
        free((char*)mtic_my_agent_mapping->version);
        mtic_my_agent_mapping->version = NULL;
    }

    mtic_my_agent_mapping->name = strdup(loaded->name);
    mtic_my_agent_mapping->description = strdup(loaded->description);
    mtic_my_agent_mapping->version = strdup(loaded->version);

    /*
     * Mapping output
     *
     */
    mapping_out *temp;
    char map_description[100];
    char out_name[100];
    for(temp = loaded->map_out; temp != NULL; temp = temp->hh.next) {

        //Create map_description
        strcpy(map_description ,temp->agent_name);
        strcat(map_description, ".");//separator
        strcpy(out_name,temp->output_name);
        strcat(map_description, out_name);

        //mtic_map
        mtic_map(temp->input_name,map_description);
    }
}

mapping_out * add_map_to_table(char * input_name,
                      char *agent_name,
                      char* output_name,
                      int* report)
{

    mapping_out * new_map_out = NULL;
    *report = 0;
    
    //Check in the mapping live table if the mapping already exist and if it's ON
    if(mtic_my_agent_mapping != NULL){
        mapping_out * map_out = NULL;

        for(map_out = mtic_my_agent_mapping->map_out; map_out != NULL && *report == 0; map_out = map_out->hh.next) {
            if(     (strcmp(map_out->input_name, input_name) == 0) &&
                    (strcmp(map_out->agent_name, agent_name) == 0) &&
                    (strcmp(map_out->output_name, output_name) == 0)
               ){
                char map_description[100];
                strcpy(map_description,"(");
                strcat(map_description, input_name);
                strcat(map_description, ",");
                strcat(map_description, agent_name);
                strcat(map_description, ".");
                strcat(map_description,output_name);
                strcat(map_description,")");

                fprintf (stderr, "%s : '%s' already exist in the table of mapping live. So it won't be added.\n",
                         __FUNCTION__,
                         map_description);

                new_map_out = map_out;
                *report = 5;
            }
        }
    }

    if(*report == 0)
    {
        new_map_out = calloc(1, sizeof(mapping_out));
        new_map_out->input_name = strdup(input_name);
        new_map_out->agent_name = strdup(agent_name);
        new_map_out->output_name = strdup(output_name);
        new_map_out->state = OFF;

        //Initialize the table mapping if it is not
        if(mtic_my_agent_mapping == NULL){
            mtic_my_agent_mapping = calloc (1, sizeof (struct mapping));
        }
        //Count actual mapping to define the map_int
        new_map_out->map_id = 0;
        new_map_out->map_id = HASH_COUNT(mtic_my_agent_mapping->map_out) + 1;

        //Add the input -> Agent name & output in the map table
        HASH_ADD_INT(mtic_my_agent_mapping->map_out, map_id, new_map_out);

        mtic_debug("Add agent mapping : %s -> %s.%s.\n",new_map_out->input_name,new_map_out->agent_name,new_map_out->output_name);
    }

    return new_map_out;
}

int split_map_description(char* map_description,
                          char * agent_name,
                          char * output_name){
    //Split the string of the map description to agent_name & output_name
    char *token;
    char *temp_agent_name = NULL;
    char *temp_output_name = NULL;

    //Copy the string because she has to be modifiable by the strtok
    char *copy = strdup(map_description);
    token = strtok(copy, ".");

    // Find agent name and output name : ex. 'A2.s1'
    while( token != NULL )
    {
        if(agent_name == NULL){        //Copy the fist part 'A2'
            temp_agent_name = strdup(token);
        }else if(output_name == NULL){ //Copy the second part 's1'
            temp_output_name = strdup(token);
        }

        token = strtok(NULL, ".");
    }

    if (temp_agent_name)
        agent_name = temp_agent_name;
    else {
        fprintf (stderr, "%s : agent to map has no name \n", __FUNCTION__);
        free((char*) copy);
        copy = NULL;
        if (output_name != NULL)
        {
            free(output_name);
            output_name = NULL;
        }
        return 3;
    }

    if(temp_output_name)
        output_name = temp_output_name;
    else {
        fprintf (stderr, "%s : agent's output to map has no name  \n", __FUNCTION__);
        free((char*) copy);
        copy = NULL;
        if (agent_name != NULL)
        {
            free(agent_name);
            agent_name = NULL;
        }
        return 4;
    }

    //Free copy after strdup
    free((char*) copy);
    copy = NULL;



    //Return OK
    return 0;
}

int mtic_map (char* input_name,char* map_description){

    int result = 0;
    agent_iop *input_to_map = NULL;     // the input to map
    char *agent_to_map_name = NULL;     // the external agent name to map
    char *output_to_map_name = NULL;   // the output name to map
    int error_code = -1;

    //Check if already initialized, and do it if not
    if(mtic_definition_live == NULL){
        mtic_definition_live = calloc(1, sizeof(struct definition));
    }

    //Find the input by the name in the table of the my agent's definition
    HASH_FIND_STR(mtic_definition_live->inputs_table, input_name, input_to_map);
    if(input_to_map == NULL){
        mtic_debug("%s : input name %s not found \n",
                   __FUNCTION__,
                   input_name);
        return 1;
    }

    if(map_description == NULL){
        mtic_debug ("%s : Map description is NULL \n", __FUNCTION__);
        return 2;
    }

    //Split the map description to : agent name & output name
    //Split the string of the map description to agent_name & output_name
    char *token;

    //Copy the string because she has to be modifiable by the strtok
    char *copy = strdup(map_description);
    token = strtok(copy, ".");

    // Find agent name and output name : ex. 'A2.s1'
    while( token != NULL )
    {
        if(agent_to_map_name == NULL){        //Copy the fist part 'A2'
            agent_to_map_name = strdup(token);
        }else if(output_to_map_name == NULL){ //Copy the second part 's1'
            output_to_map_name = strdup(token);
        }

        token = strtok(NULL, ".");
    }

    //Free copy after strdup
    free((char*) copy);
    copy = NULL;

    if(agent_to_map_name == NULL){
        mtic_debug ("%s : agent to map has no name \n", __FUNCTION__);

        // Clean-up
        if (output_to_map_name != NULL)
        {
            free(output_to_map_name);
            output_to_map_name = NULL;
        }

        return 3;
    }

    if(output_to_map_name == NULL){
        mtic_debug ("%s : agent's output to map has no name  \n", __FUNCTION__);

        // Clean-up
        if (agent_to_map_name != NULL)
        {
            free(agent_to_map_name);
            agent_to_map_name = NULL;
        }

        return 4;
    }

    // New map_out to be created
    mapping_out * new_map = NULL;

    //Find if the map is in table mapping & Add the map in the table mapping
    new_map = add_map_to_table(input_name,agent_to_map_name,output_to_map_name,&error_code);

    if(new_map != NULL)
    {
        if((error_code == 0 || error_code == 5) && new_map->state == OFF)
        {
            if(strcmp("*", agent_to_map_name) == 0)
            {
                definition *current, *tmp;
                HASH_ITER(hh, mtic_agents_defs_on_network, current, tmp) {
                    if(current != NULL)
                    {
                        // check and subscribe to the new added outputs if they exist and the corresponding agent is present.
                        network_checkAndSubscribeToPublisher((char*)current->name);
                    }
                }
            } else {
                // check and subscribe to the new added outputs if they exist and the corresponding agent is present.
                network_checkAndSubscribeToPublisher(agent_to_map_name);
            }
        } else {
            result = error_code;
        }
    } else {
        result = error_code;
    }

    free(agent_to_map_name);
    agent_to_map_name = NULL;
    free(output_to_map_name);
    output_to_map_name = NULL;


    return result;
}

int mtic_map_received(const char *agent_name, char *out_name, char *value, long size){
    mapping_out *temp;
    int state = 1;

    // Check the existence of our map
    if(mtic_my_agent_mapping != NULL)
    {
        for(temp = mtic_my_agent_mapping->map_out; temp != NULL; temp = temp->hh.next) {
            if( (strcmp(agent_name, temp->agent_name) == 0)
               && (strcmp(out_name, temp->output_name) == 0) &&
               (temp->state == ON)){
                state = mtic_writeInput(temp->input_name, value, size);
            }
        }
    }
    return state;
}

bool mtic_map_category (char* map_description){
    return false;
}

bool check_iop_type(char * input_name,
                    agent_iop* output){

    agent_iop * input = NULL;
    HASH_FIND_STR(mtic_definition_live->inputs_table,input_name,input);

    if(input == NULL)
        return false;

    //Check if the type iop (input -> output) is correct
    if(input->value_type != output->value_type){
        fprintf (stderr, "%s : Incompatibility of the type between input named :'%s' & output named : '%s' \n",
                 __FUNCTION__,
                 input->name,
                 output->name);

        return false;
    }

    return true;
}

mapping_out * mtic_find_map(char * input_name,
                       char * agent_name,
                       char * output_name){
    mapping_out *temp = NULL;
    for(temp = mtic_my_agent_mapping->map_out; temp != NULL; temp = temp->hh.next) {
        if((strcmp(input_name, temp->input_name) == 0) &&
           (strcmp(agent_name, temp->agent_name) == 0) &&
           (strcmp(output_name, temp->output_name) == 0)){
            return temp;
        }
    }
    return NULL;
}

agent_iop* mtic_check_map(definition *definition){
    agent_iop *outputs = NULL;

    // If my agent_mapping has been created
    if(mtic_my_agent_mapping != NULL)
    {
        //Check if the agent definition is involved in our mapping
        char * agent_to_map_name = NULL;
        agent_to_map_name = strdup(definition->name);

        if(agent_to_map_name == NULL)
            return NULL;

        //Find the mapping
        mapping_out *temp;
        mapping_out *current_map;

        for(temp = mtic_my_agent_mapping->map_out; temp != NULL; temp = temp->hh.next) {
            if((strcmp(agent_to_map_name, temp->agent_name) == 0) ||
               (strcmp("*", temp->agent_name) == 0)){
                current_map = NULL;

                //NB : maybe add the current agent has new mapping if '*'
                //if it's not already in the table
                //As that we have a table updated with the name of the agent
                //And the state ON is always associated with an agent name and not a generic one
                //See how to associated the ON with the named agent and not the generic name '*'
                if(strcmp("*", temp->agent_name) == 0){
                    temp->state = GENERIC;

                    mapping_out * new_map = NULL;
                    int report = 0;
                    new_map = add_map_to_table(temp->input_name,
                                               agent_to_map_name,
                                               temp->output_name,
                                               &report);

                    // Get the new mapping_out if created
                    if(new_map != NULL)
                    {
                        current_map = new_map;
                    }

                }

                //If we have not add the map
                if(current_map == NULL){
                    current_map = temp;
                }

                // Update the current map state
                agent_iop * found = NULL;
                found = mtic_update_mapping_out_state(current_map, definition);

                if(current_map->state == ON && found != NULL)
                {
                    //Add the output concerning to the returning variable if not already
                    agent_iop * already = NULL;
                    HASH_FIND_STR( outputs, found->name, already );
                    if(already == NULL)
                    {
                        // Copie the element to return it
                        agent_iop* new_iop  = calloc(1, sizeof(*found));
                        memcpy(new_iop, found, sizeof(*found));

                        HASH_ADD_KEYPTR(hh,outputs, new_iop->name, strlen(new_iop->name), new_iop);
                    }
                } else {
                    printf("Error : Unable to map %s.%s to %s.%s\n",mtic_definition_live->name, current_map->input_name, definition->name, current_map->output_name);
                    if(current_map->state == ON)
                        current_map->state = OFF;
                }
            }
        }

        free(agent_to_map_name);
        agent_to_map_name = NULL;
    }


    return outputs;
}

agent_iop* mtic_unmap(definition *definition){
    agent_iop *return_out_table = NULL;
    agent_iop *out_found = NULL;
    mapping_out *current_map_out = NULL;

    //Read the table of mapping
    if(mtic_my_agent_mapping != NULL)
    {
        for(current_map_out = mtic_my_agent_mapping->map_out; current_map_out != NULL; current_map_out = current_map_out->hh.next){

            //The agent name is found
            if(strcmp(current_map_out->agent_name, definition->name) == 0){

                //Pass the state to OFF
                current_map_out->state = OFF;

                //Find the output concerning by the mapping
                HASH_FIND_STR( definition->outputs_table, current_map_out->output_name, out_found);

                //If the output exist in the definition
                if(out_found != NULL){
                    agent_iop * already = NULL;

                    //Check if the output mapped name is not already part of the returning output table
                    HASH_FIND_STR( return_out_table, current_map_out->output_name, already );
                    if(already == NULL)
                    {
                        // Copie the element to return it
                        agent_iop* new_iop  = calloc(1, sizeof(*out_found));
                        memcpy(new_iop, out_found, sizeof(*out_found));

                        //Add the output concerning to the returning variable if not already
                        HASH_ADD_KEYPTR(hh,return_out_table, new_iop->name, strlen(new_iop->name), new_iop);
                    }
                }else{
                    fprintf (stderr, "%s : the output named : '%s' is not part of the agent named : '%s' \n",
                             __FUNCTION__, current_map_out->output_name, definition->name);
                }
            }
        }
    }

    return return_out_table;
}

agent_iop*  mtic_update_mapping_out_state(mapping_out* map_out, definition * external_definition)
{
    agent_iop* outputFound = NULL;

    // If the dinition  and the map_out object are defined
    if(external_definition != NULL && map_out != NULL)
    {
        HASH_FIND_STR(external_definition->outputs_table,map_out->output_name,outputFound);
        
        //Check if the output exist
        if(outputFound != NULL){
            
            if((check_iop_type(map_out->input_name, outputFound) == true)){
                map_out->state = ON;
            }else{
                map_out->state = INCOMPATIBLE;
            }
        }else{
            printf("Could not find output %s in %s definition\n", map_out->output_name, map_out->agent_name);
        }
    }else{
        printf("%s : one of the arguments is NULL\n", __FUNCTION__);
    }
    
    return outputFound;
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
 * \brief load mapping in variable 'mtic_my_agent_mapping' from a json string
 *
 * \param json_str String in json format. Can't be NULL.
 * \return The error. 1 is OK, 0 json string is NULL or empty, -1 Mapping has not been loaded
 */
int mtic_loadMapping (const char* json_str){
    mtic_my_agent_mapping = NULL;

    //Check if the json string is null or empty
    if((json_str == NULL) || (strlen(json_str) == 1))
    {
        mtic_debug("json string is null or empty \n");
        return 0;
    }

    //Load definition and init variable : mtic_definition_loaded
    mtic_my_agent_mapping = parser_LoadMap(json_str);

    if(mtic_my_agent_mapping == NULL)
    {
        mtic_debug("Mapping has not been loaded from json string : %s\n", json_str );
        return -1;
    }

    return 1;
}

/**
 * \fn int mtic_loadMappingFromPath (const char* file_path)
 * \ingroup loadClearGetMapFct
 * \brief load mapping in variable 'mtic_my_agent_mapping' from a file path
 *
 * \param file_path The string which contains the json file path. Can't be NULL.
 * \return The error. 1 is OK, 0 file path is NULL or empty, -1 Definition file has not been loaded
 */
int mtic_loadMappingFromPath (const char* file_path){

    mtic_my_agent_mapping = NULL;

    //Check if the json string is null or empty
    if((file_path == NULL) || (strlen(file_path) == 0))
    {
        mtic_debug("File path is null or empty \n");
        return 0;
    }

    //Load definition and init variable : mtic_definition_loaded
    mtic_my_agent_mapping = parser_LoadMapFromPath(file_path);

    if(mtic_definition_loaded == NULL)
    {
        mtic_debug("Mapping file has not been loaded from file path : %s\n", file_path);
        return -1;
    }

    return 1;
}

/**
 * \fn int mtic_clearMapping()
 * \ingroup loadClearGetMapFct
 * \brief Clear the variable 'mtic_my_agent_mapping' and free all structures inside and itself
 *
 * \return The error. 1 is OK,
 * 0 file path is NULL or empty
 */
int mtic_clearMapping(){

    if(mtic_my_agent_mapping == NULL){
        mtic_debug("The structure mtic_my_agent_mapping is NULL. \n");
        return 0;
    }

    mapping_FreeMapping(mtic_my_agent_mapping);

    //Put ptr at NULL
    mtic_my_agent_mapping = NULL;

    return 1;
}

/**
 * \fn char* mtic_getMapping()
 * \ingroup loadClearGetMapFct
 * \brief the agent mapping getter
 *
 * \return The loaded mapping string in json format. NULL if mtic_my_agent_mapping was not initialized.
 * \warning Allocate memory that should be freed by the user.
 */
char* mtic_getMapping(){
    char * mappingJson = NULL;

    if(mtic_my_agent_mapping == NULL){
        mtic_debug("The structure mtic_my_agent_mapping is NULL. \n");
        return NULL;
    }

    mappingJson = export_mapping(mtic_my_agent_mapping);

    return mappingJson;
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
        mtic_debug("Mapping name cannot be NULL \n");
        return 0;
    }

    if (strlen(name) == 0){
        mtic_debug("Mapping name cannot be empty\n");
        return -1;
    }

    //Check if already initialized, and do it if not
    if(mtic_my_agent_mapping == NULL){
        mtic_my_agent_mapping = calloc(1, sizeof(struct mapping));
    }

    //Copy the name in the structure
    if(mtic_my_agent_mapping->name != NULL)//Free the field if needed
        free(mtic_my_agent_mapping->name);
    mtic_my_agent_mapping->name = strdup(name);

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
    if(mtic_my_agent_mapping == NULL){
        mtic_my_agent_mapping = calloc(1, sizeof(struct mapping));
    }

    //Copy the description in the structure
    if(mtic_my_agent_mapping->description != NULL)//Free the field if needed
        free(mtic_my_agent_mapping->description);
    mtic_my_agent_mapping->description = strdup(description);

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
    if(mtic_my_agent_mapping == NULL){
        mtic_my_agent_mapping = calloc(1, sizeof(struct mapping));
    }

    //Copy the description in the structure
    if(mtic_my_agent_mapping->version != NULL)//Free the field if needed
        free(mtic_my_agent_mapping->version);
    mtic_my_agent_mapping->version = strdup(version);

    return 1;
}

/**
 * \fn int mtic_getMappingEntriesNumber()
 * \ingroup EditMapFct
 * \brief the agent mapping entries number getter
 *
 * \return The number of mapping type output entries. If -1 The structure mtic_my_agent_mapping is NULL.
 */
int mtic_getMappingEntriesNumber(){ //number of entries in the mapping
    int number = -1;

    if(mtic_my_agent_mapping == NULL){
        mtic_debug("The structure mtic_my_agent_mapping is NULL \n");
        return -1;
    }

    if(mtic_my_agent_mapping->map_out == NULL){
        mtic_debug("The structure mapping out is NULL \n");
        return 0;
    }

    number = HASH_COUNT(mtic_my_agent_mapping->map_out);

    return number;
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
int mtic_addMappingEntry(char *fromOurInput, char *toAgent, char *withOutput){ //returns mapping id or 0 if creation failed

    /***    Check the string    ***/
    //fromOurInput
    if((fromOurInput == NULL) || (strlen(fromOurInput) == 0)){
        mtic_debug("Our input name to be mapped cannot be NULL or empty\n");
        return 0;
    }

    //toAgent
    if((toAgent == NULL) || (strlen(toAgent) == 0)){
        mtic_debug("Agent name to be mapped cannot be NULL or empty\n");
        return -1;
    }

    //withOutput
    if((withOutput == NULL) || (strlen(withOutput) == 0)){
        mtic_debug("Extern agent output name to be mapped cannot be NULL or empty\n");
        return -2;
    }

    //Check if already initialized, and do it if not
    if(mtic_my_agent_mapping == NULL){
        mtic_my_agent_mapping = calloc(1, sizeof(struct mapping));
    }

    /***** Add the new mapping out *****/

    char map_description[MAX_PATH];
    char out_name[MAX_PATH];

    //Create map_description
    strcpy(map_description , toAgent);
    strcat(map_description, ".");//separator
    strcpy(out_name, withOutput);
    strcat(map_description, out_name);

    //Check if the mapping already exist & Add map if not
    mtic_map(fromOurInput, map_description);

    return 1;
}

/**
 * \fn int mtic_removeMappingEntryWithId(int theId)
 * \ingroup EditMapFct
 * \brief this function allows the user to remove a mapping in table by its id
 *
 * \param theId The id of the mapping. Cannot be negative.
 * \return The error. 1 is OK.
 * 0 The id of the mapping cannot be negative.
 * -1 The structure mtic_my_agent_mapping is NULL.
 * -2 The structure mapping out is NULL.
 */
int mtic_removeMappingEntryWithId(int theId){

    if(theId < 0){
        mtic_debug("The id of the mapping cannot be negative. \n");
        return 0;
    }

    //Get the mapping output by id
    mapping_out * mapp_out = NULL;

    if(mtic_my_agent_mapping == NULL){
        mtic_debug("The structure mtic_my_agent_mapping is NULL \n");
        return -1;
    }

    if(mtic_my_agent_mapping->map_out == NULL){
        mtic_debug("The structure mapping out is NULL \n");
        return -2;
    }

    HASH_FIND_INT(mtic_my_agent_mapping->map_out, &theId, mapp_out);

    if(mapp_out == NULL){
        mtic_debug("The id of the mapping is not part of the table. \n");
        return 0;
    }

    //Remove this one
    HASH_DEL(mtic_my_agent_mapping->map_out, mapp_out);
    free(mapp_out);

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
 * -3 The structure mtic_my_agent_mapping is NULL.
 * -4 The structure mapping out is NULL.
 */
int mtic_removeMappingEntryWithName(char *fromOurInput, char *toAgent, char *withOutput){
    /***    Check the string    ***/
    //fromOurInput
    if((fromOurInput == NULL) || (strlen(fromOurInput) == 0)){
        mtic_debug("Our input name to be mapped cannot be NULL or empty\n");
        return 0;
    }

    //toAgent
    if((toAgent == NULL) || (strlen(toAgent) == 0)){
        mtic_debug("Agent name to be mapped cannot be NULL or empty\n");
        return -1;
    }

    //withOutput
    if((withOutput == NULL) || (strlen(withOutput) == 0)){
        mtic_debug("Extern agent output name to be mapped cannot be NULL or empty\n");
        return -2;
    }

    if(mtic_my_agent_mapping == NULL){
        mtic_debug("The structure mtic_my_agent_mapping is NULL \n");
        return -3;
    }

    if(mtic_my_agent_mapping->map_out == NULL){
        mtic_debug("The structure mapping out is NULL \n");
        return -4;
    }

    //Check in the mapping live table if the mapping already exist and if it's ON
    if(mtic_my_agent_mapping != NULL)
    {
        mapping_out * map_out = NULL;

        for(map_out = mtic_my_agent_mapping->map_out; map_out != NULL; map_out = map_out->hh.next)
        {
            if(     (strcmp(map_out->input_name, fromOurInput) == 0) &&
                    (strcmp(map_out->agent_name, toAgent) == 0) &&
                    (strcmp(map_out->output_name, withOutput) == 0)
               ){
                    //Delete this mapping
                    HASH_DEL(mtic_my_agent_mapping->map_out, map_out);

                }
        }

        free(map_out);
    }

    return 1;
}


