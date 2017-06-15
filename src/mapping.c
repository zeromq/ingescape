//
//  mtic_mapping.c
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



// Global parameter declaration
mapping * mtic_my_agent_mapping = NULL;


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

void free_map_out (mapping_out* map_out){

    free((char*)map_out->agent_name);
    map_out->agent_name = NULL ;

    free((char*)map_out->input_name);
    map_out->input_name = NULL ;

    free((char*)map_out->output_name);
    map_out->output_name = NULL ;

    free (map_out);
}

void free_map_cat (mapping_cat* map_cat){

    free((char*)map_cat->agent_name);
    map_cat->agent_name = NULL ;

    free((char*)map_cat->category_name);
    map_cat->category_name = NULL ;

    free (map_cat);
}

/*
 * Function: free_mapping
 * ----------------------------
 *   free a mapping structure, all its pointers and hash tables
 *
 *   mapping : a pointer to the mapping structure to free
 *
 */
void mtic_free_mapping (mapping* mapp) {

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
        free_map_out(current_map_out);
        // current_map_out = NULL;
    }

    //Free mapping category
    HASH_ITER(hh, mapp->map_cat, current_map_cat, tmp_map_cat) {
        HASH_DEL(mapp->map_cat,current_map_cat);
        free_map_cat(current_map_cat);
        //current_map_cat = NULL;
    }

    free(mapp);
}

/*
 * Function: map_all_mapping
 * ----------------------------
 *   Sub-function of 'load_map'.
 *   Read the table of all mappings loaded by the file json
 *   Call the 'mtic_map' function corresponding to the mtic_map
 *
 *   usage : map_all_mapping()
 *
 *   loaded : the structure 'mapping' corresponding to the loaded mapping from the JSON file.
 *
 */
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

/*
 * Function: add_map_to_table
 * ----------------------------
 *   Check if the map is already present in the table
 *   Add the map in the table
 *
 *   usage : add_map_to_table('e1','A2', 's1')
 *
 *   input_name     : the input's name to be connected with : ex. 'e1'
 *
 *   agent_name     : the agent's name to be connected with : ex. 'A2' OR '*'
 *
 *   output_name    : the output's name to be connected with : ex. 'A2' OR '*'
 *.
 *   report         : the error code
 *                      0  : OK
 *                      1  : agent name empty
 *                      2  : output name empty
 *
 *   returns : map_out : mapping_out object newly created
 
 */
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

/*
 * Function: split_map_description
 * ----------------------------
 *   Split the map description in the agent name & the output name
 *
 *   usage : split_map_description('A2.s1',agent_name,output_name)
 *
 *   map_description    : the description of the mapping 'A2.s1' OR '*.s1'
 *
 *   agent_name         : the pointer on the char * which will contains the name of the agent
 *
 *   output_name         : the pointer on the char * which will contains the output name
 *
 *   returns : the error code
 *   0  : OK
 *   1  : agent name empty
 *   2  : output name empty
 */

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

/*
 * Function: map
 * ----------------------------
 *   Map an input with an external agent's ouput (one OR more).
 *   Check if the input is part of the agent.
 *   Check if the map is already present in the table
 *   Add the map in the table
 *
 *   usage : mtic_map('e1','A2.s1')
 *
 *   input_name         : the input's name to be connected with : ex. 'e1'
 *
 *   map_description    : the description of the mapping 'A2.s1' OR '*.s1'
 *
 *   returns : the error code
 *   0  : OK
 *   1  : agent name empty
 *   2  : output name empty
 */
int mtic_map (char* input_name,char* map_description){

    int result = 0;
    agent_iop *input_to_map = NULL;     // the input to map
    char *agent_to_map_name = NULL;     // the external agent name to map
    char * output_to_map_name = NULL;   // the output name to map
    int error_code = -1;

    //Find the input by the name in the table of the my agent's definition
    HASH_FIND_STR(mtic_definition_loaded->inputs_table, input_name, input_to_map);

    if(input_to_map == NULL){
        fprintf (stderr, "%s : input name %s not found \n",
                 __FUNCTION__,
                 input_name);
        return 1;

    }

    if(map_description == NULL){
        fprintf (stderr, "%s : Map description is NULL \n", __FUNCTION__);

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
        fprintf (stderr, "%s : agent to map has no name \n", __FUNCTION__);

        // Clean-up
        if (output_to_map_name != NULL)
        {
            free(output_to_map_name);
            output_to_map_name = NULL;
        }

        return 3;
    }

    if(output_to_map_name == NULL){
        fprintf (stderr, "%s : agent's output to map has no name  \n", __FUNCTION__);

        // Clean-up
        if (agent_to_map_name != NULL)
        {
            free(agent_to_map_name);
            agent_to_map_name = NULL;
        }

        return 4;
    }

    // New map_out to created
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

/*
 * Function: map_received
 * ----------------------------
 *   It's called in the function SUB when receiving value from an another agent which we have subscribed.
 *   Call SET to copy the value and updated the live model of data.
 *
 *   agent_name : the name of the external agent
 *
 *   out_name : the name of the output concerning
 *
 *   value      : the new value
 *
 *   returns : the state of the 'mtic_set' function from model
 */

model_state mtic_map_received(const char *agent_name, char *out_name, void *value){
    mapping_out *temp;
    model_state state = NOK;

    // Check the existence of our map
    if(mtic_my_agent_mapping != NULL)
    {
        for(temp = mtic_my_agent_mapping->map_out; temp != NULL; temp = temp->hh.next) {
            if( (strcmp(agent_name, temp->agent_name) == 0)
               && (strcmp(out_name, temp->output_name) == 0) &&
               (temp->state == ON)){

                // Set the new input and update the live model
                state = mtic_set(temp->input_name, value);
            }
        }
    }
    return state;
}

/* TODO : implement in the next version of the API
 * Function: map_category
 * ----------------------------
 *   Map the agent's inputs to the category which an external agent is in accordance with it,
 *   So all the outputs from the external agent in accordance with the agent'inputs will be mapped
 *
 *   usage : map_category('A2.c3')
 *
 *   map_description    : the description of the mapping 'A2.c3' OR '*.c3'
 *
 *   returns : the state of the mapping
 */
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
    if(input->type != output->type){
        fprintf (stderr, "%s : Incompatibility of the type between input named :'%s' & output named : '%s' \n",
                 __FUNCTION__,
                 input->name,
                 output->name);

        return false;
    }

    return true;
}

/*
 * Function: find_map
 * ----------------------------
 *   Sub-function of 'check_map'.
 *   Find the map in the table
 *
 *   usage : find_map('e1','A2','s1')
 *
 *   input_name     : the input's name
 *
 *   agent_name     : the agent's name
 *
 *   output_name    : the output's name
 *
 *   returns : the mapping output pointer
 */
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

/*
 * Function: check_map
 * ----------------------------
 *   It's called in the function ENTER.
 *   Check if the agent is concerning by a mapping.
 *   Update the mapping state if some exist.
 *
 *   definition : the definition of the external entered agent
 *
 *   returns    : the table of outputs to subscribe OR null
 */
agent_iop* mtic_check_map(definition *definition){
    agent_iop *outputs = NULL;

    // If my agent_mapping has been created
    if(mtic_my_agent_mapping != NULL)
    {
        //Check if the agent is concerning by a mapping
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
                    printf("Error : Unable to map %s with %s\n.",mtic_definition_live->name, definition->name);
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

/*
 * Function: unmap
 * ----------------------------
 *   It's called in the function EXIT.
 *   Check if the agent is concerning by a mapping.
 *   Update the mapping state to OFF.
 *
 *   definition : the definition of the external agent which will exit
 *
 *   returns    : the table of outputs passed to OFF and to unsubscribe OR null
 */
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

/*
 * Function: update_mapping_out_state
 * ----------------------------
 *   Take a mapping_out object to update it's state according to the definition it's mapped with. .
 *   Check & find the output is part of this agent.
 *   Check if the type between the input and the output is compatible.
 *   If yes, switch the state to ON.
 *
 *   map_out             : the mapping_out object
 *
 *   external_definition : the external agent definition
 *
 *   return : the agent_iop object found in the external definition corresponding to map_out
 *
 */
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
        }
    }
    
    return outputFound;
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
//à remplir ou déplacer ici



////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////
/**
 * \fn int mtic_loadMapping (const char* json_str)
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
int mtic_clearMapping(){ //clears mapping data for the agent
    return 1;
}

/**
 * \fn char* mtic_getMapping()
 * \brief the agent mapping getter
 *
 * \return The loaded mapping string in json format (allocated). NULL if mtic_my_agent_mapping was not initialized.
 */
char* mtic_getMapping(){ //returns json string
    char * mappingJson = NULL;

    if(mtic_my_agent_mapping == NULL){
        mtic_debug("The structure mtic_my_agent_mapping is NULL");
        return NULL;
    }

    mappingJson = export_mapping(mtic_my_agent_mapping);

    return mappingJson;
}

//edit mapping using the API
int mtic_setMappingName(char *name){
    return 1;
}
int mtic_setMappingDescription(char *description){
    return 1;
}
int mtic_setMappingVersion(char *description){
    return 1;
}
int mtic_getMappingEntriesNumber(){ //number of entries in the mapping
    return 1;
}
int mtic_addMappingEntry(char *fromOurInput, char *toAgent, char *withOutput){ //returns mapping id or 0 if creation failed
    return 1;
}
int mtic_removeMappingEntryWithId(int theId){
    return 1;
}
int mtic_removeMappingEntryWithName(char *fromOurInput, char *toAgent, char *withOutput){
    return 1;
}


