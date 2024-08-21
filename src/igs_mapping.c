/*  =========================================================================
 mapping - create/edit/delete mappings
 
 Copyright (c) the Contributors as noted in the AUTHORS file.
 This file is part of Ingescape, see https://github.com/zeromq/ingescape.
 
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
 =========================================================================
 */

#include "ingescape_private.h"
#include <stdio.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////

// hash function to convert input + agent + output into a unique long number
// we use this function to give id value to map_elements in our mapping
// see http://www.cse.yorku.ca/~oz/hash.html
uint64_t mapping_djb2_hash (unsigned char *str)
{
    uint64_t hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

void s_mapping_free_mapping_element (igs_map_t **map_elmt)
{
    assert (map_elmt);
    assert (*map_elmt);
    if ((*map_elmt)->from_input)
        free ((*map_elmt)->from_input);
    if ((*map_elmt)->to_agent)
        free ((*map_elmt)->to_agent);
    if ((*map_elmt)->to_output)
        free ((*map_elmt)->to_output);
    free (*map_elmt);
    *map_elmt = NULL;
}

void mapping_free_mapping (igs_mapping_t **mapping)
{
    assert (mapping);
    assert (*mapping);
    if ((*mapping)->json) {
        free ((char *) (*mapping)->json);
        (*mapping)->json = NULL;
    }
    if ((*mapping)->json_legacy) {
        free ((char *) (*mapping)->json_legacy);
        (*mapping)->json_legacy = NULL;
    }
    igs_map_t *current_map_elmt = zlist_first((*mapping)->map_elements);
    while (current_map_elmt) {
        //zlist_remove((*mapping)->map_elements, current_map_elmt);
        s_mapping_free_mapping_element (&current_map_elmt);
        current_map_elmt = zlist_next((*mapping)->map_elements);
    }
    zlist_destroy(&(*mapping)->map_elements);
    
    igs_split_t *current_split_elmt = zlist_first((*mapping)->split_elements);
    while (current_split_elmt) {
        split_free_split_element (&current_split_elmt);
        current_split_elmt = zlist_next((*mapping)->split_elements);
    }
    zlist_destroy(&(*mapping)->split_elements);
    free (*mapping);
    *mapping = NULL;
}

bool mapping_is_equal (const char *first_str, const char *second_str)
{
    if (!first_str && !second_str)
        return true;
    //NB: is one is NULL and not the other, we consider
    //the comparison is false even is the not NULL one
    //is not valid.
    if ((first_str && !second_str)
        || (!first_str && second_str))
        return false;
    
    igs_mapping_t *first = parser_load_mapping (first_str);
    igs_mapping_t *second = parser_load_mapping (second_str);
    //if both strings are invalid, we consider the comparison is true
    if (!first && !second)
        return true;
    if ((first && !second) || (second && !first)) {
        if (first)
            mapping_free_mapping (&first);
        if (second)
            mapping_free_mapping (&second);
        return false;
    }
    
    //beyond this point, both strings are valid JSON
    bool res = true;
    
    if (!first->map_elements) {
        if (second->map_elements) {
            res = false;
            goto END;
        }
    }
    if (!second->map_elements) {
        if (first->map_elements) {
            res = false;
            goto END;
        }
    }
    //comparing number of entries in mapping elements
    size_t firstS = zlist_size(first->map_elements);
    size_t secondS = zlist_size(second->map_elements);
    if (firstS != secondS) {
        res = false;
        goto END;
    }
    //comparing ids
    //NB: comparing ids (which are hashes) is sufficient to compare the whole entries.
    igs_map_t *first_elmt = zlist_first(first->map_elements);
    while (first_elmt) {
        igs_map_t *second_elmt = zlist_first(second->map_elements);
        bool found_elmt = false;
        while (second_elmt) {
            if (first_elmt->id == second_elmt->id){
                found_elmt = true;
                break;
            }
            second_elmt = zlist_next(second->map_elements);
        }
        if (!found_elmt) {
            res = false;
            goto END;
        }
        first_elmt = zlist_next(first->map_elements);
    }
    
    //same for splits
    size_t first_split = zlist_size(first->split_elements);
    size_t second_split = zlist_size(second->split_elements);
    if (first_split != second_split) {
        res = false;
        goto END;
    }
    igs_split_t *first_elmt_split = zlist_first(first->split_elements);
    while (first_elmt_split) {
        igs_split_t *second_elmt_split = zlist_first(second->split_elements);
        bool found_elmt = false;
        while (second_elmt_split) {
            if (first_elmt_split->id == second_elmt_split->id){
                found_elmt = true;
                break;
            }
            second_elmt_split = zlist_next(second->split_elements);
        }
        if (!found_elmt){
            res = false;
            goto END;
        }
        first_elmt_split = zlist_next(first->split_elements);
    }
    
END:
    mapping_free_mapping (&first);
    mapping_free_mapping (&second);
    return res;
}

igs_map_t *mapping_create_mapping_element (const char *from_input,
                                           const char *to_agent,
                                           const char *to_output)
{
    assert(from_input);
    assert(to_agent);
    assert(to_output);
    igs_map_t *new_map_elmt = (igs_map_t *) zmalloc (sizeof (igs_map_t));
    new_map_elmt->from_input = strdup (from_input);
    new_map_elmt->to_agent = strdup (to_agent);
    new_map_elmt->to_output = strdup (to_output);
    return new_map_elmt;
}

igs_split_t *mapping_create_split_element (const char *from_input,
                                           const char *to_agent,
                                           const char *to_output)
{
    assert(from_input);
    assert(to_agent);
    assert(to_output);
    igs_split_t *new_split_elmt = (igs_split_t *) zmalloc (sizeof (igs_map_t));
    new_split_elmt->from_input = strdup (from_input);
    new_split_elmt->to_agent = strdup (to_agent);
    new_split_elmt->to_output = strdup (to_output);
    return new_split_elmt;
}

bool mapping_check_input_output_compatibility (igsagent_t *agent,
                                               igs_io_t *input,
                                               igs_io_t *output)
{
    // For compatibility reasons, only DATA outputs imply limitations.
    // The rest is covnerted automatically in model_write.
    bool is_compatible = true;
    igs_io_value_type_t type = input->value_type;
    if (output->value_type == IGS_DATA_T) {
        if (type != IGS_DATA_T && type != IGS_IMPULSION_T) {
            is_compatible = false;
            igsagent_warn (agent, "DATA outputs can only be mapped by DATA or IMPULSION inputs");
        }
    }
    return is_compatible;
}

void mapping_update_json (igs_mapping_t *mapping)
{
    assert(mapping);
    if (mapping->json) {
        free ((char *) mapping->json);
        mapping->json = NULL;
    }
    if (mapping->json_legacy) {
        free ((char *) mapping->json_legacy);
        mapping->json_legacy = NULL;
    }
    mapping->json = parser_export_mapping (mapping);
    mapping->json_legacy = parser_export_mapping_legacy (mapping);
}

////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////

igs_result_t igsagent_mapping_load_str (igsagent_t *agent,
                                        const char *json_str)
{
    assert (agent);
    assert (json_str);
    char *current_mapping = igsagent_mapping_json (agent);
    model_read_write_lock(__FUNCTION__, __LINE__);
    if (mapping_is_equal (json_str, current_mapping)) {
        igs_info ("new mapping is the same as the current one : nothing to do");
        if (current_mapping)
            free (current_mapping);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_SUCCESS;
    }
    if (current_mapping)
        free (current_mapping);
    igs_mapping_t *tmp = parser_load_mapping (json_str);
    if (tmp == NULL) {
        igsagent_error (agent, "mapping could not be loaded from json string '%s'", json_str);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }else {
        if (agent->mapping)
            mapping_free_mapping (&agent->mapping);
        agent->mapping = tmp;
        mapping_update_json(agent->mapping);
        agent->network_need_to_send_mapping_update = true;
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return IGS_SUCCESS;
}

igs_result_t igsagent_mapping_load_file (igsagent_t *agent,
                                         const char *file_path)
{
    assert (agent);
    assert (file_path);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_mapping_t *tmp = parser_load_mapping_from_path (file_path);
    if (tmp == NULL) {
        igsagent_error (agent, "mapping could not be loaded from path '%s'", file_path);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    if (agent->mapping)
        mapping_free_mapping (&agent->mapping);
    agent->mapping_path = s_strndup (file_path, IGS_MAX_PATH_LENGTH - 1);
    agent->mapping = tmp;
    mapping_update_json(agent->mapping);
    agent->network_need_to_send_mapping_update = true;
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return IGS_SUCCESS;
}

void igsagent_clear_mappings (igsagent_t *agent)
{
    model_read_write_lock(__FUNCTION__, __LINE__);
    if (agent->mapping)
        mapping_free_mapping (&agent->mapping);
    agent->mapping = (struct igs_mapping *) zmalloc (sizeof (struct igs_mapping));
    agent->mapping->map_elements = zlist_new();
    agent->mapping->split_elements = zlist_new();
    mapping_update_json(agent->mapping);
    agent->network_need_to_send_mapping_update = true;
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

void igsagent_clear_mappings_with_agent (igsagent_t *agent,
                                         const char *agent_name)
{
    model_read_write_lock(__FUNCTION__, __LINE__);
    if (agent->mapping) {
        igs_map_t *elmt = zlist_first(agent->mapping->map_elements);
        while (elmt) {
            if (streq (elmt->to_agent, agent_name)) {
                zlist_remove(agent->mapping->map_elements, elmt);
                s_mapping_free_mapping_element (&elmt);
                agent->network_need_to_send_mapping_update = true;
            }
            elmt = zlist_next(agent->mapping->map_elements);
        }
        if (agent->network_need_to_send_mapping_update)
            mapping_update_json(agent->mapping);
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

void igsagent_clear_mappings_for_input (igsagent_t *agent,
                                        const char *input_name)
{
    model_read_write_lock(__FUNCTION__, __LINE__);
    assert(agent);
    assert(agent->mapping);
    igs_map_t *elmt = zlist_first(agent->mapping->map_elements);
    while (elmt) {
        if (streq (elmt->from_input, input_name)) {
            zlist_remove(agent->mapping->map_elements, elmt);
            s_mapping_free_mapping_element (&elmt);
            mapping_update_json(agent->mapping);
            agent->network_need_to_send_mapping_update = true;
        }
        elmt = zlist_next(agent->mapping->map_elements);
    }
    if (agent->network_need_to_send_mapping_update)
        mapping_update_json(agent->mapping);
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

char *igsagent_mapping_json (igsagent_t *agent)
{
    assert(agent);
    assert(agent->mapping);
    model_read_write_lock(__FUNCTION__, __LINE__);
    char *res = (agent->mapping->json)?strdup(agent->mapping->json):NULL;
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

size_t igsagent_mapping_count (igsagent_t *agent)
{
    assert (agent);
    assert (agent->mapping);
    model_read_write_lock(__FUNCTION__, __LINE__);
    size_t res = zlist_size(agent->mapping->map_elements);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

uint64_t igsagent_mapping_add (igsagent_t *agent,
                               const char *from_our_input,
                               const char *to_agent,
                               const char *with_output)
{
    assert (agent);
    assert (from_our_input && strlen (from_our_input) > 0);
    assert (to_agent && strlen (to_agent) > 0);
    assert (with_output && strlen (with_output) > 0);
    
    // from_our_input
    char *reviewed_from_our_input = s_strndup (from_our_input, IGS_MAX_IO_NAME_LENGTH);
    bool space_in_name = false;
    size_t i = 0;
    size_t length_of_reviewed_from_our_input = strlen (reviewed_from_our_input);
    for (i = 0; i < length_of_reviewed_from_our_input; i++) {
        if (reviewed_from_our_input[i] == ' ') {
            space_in_name = true;
            break;
        }
    }
    if (space_in_name) {
        igsagent_error (agent, "spaces are not allowed in IOP name '%s'",from_our_input);
        free (reviewed_from_our_input);
        return 0;
    }
    
    // to_agent
    char *reviewed_to_agent = s_strndup (to_agent, IGS_MAX_IO_NAME_LENGTH);
    size_t length_of_reviewed_to_agent = strlen (reviewed_to_agent);
    space_in_name = false;
    for (i = 0; i < length_of_reviewed_to_agent; i++) {
        if (reviewed_to_agent[i] == ' ') {
            space_in_name = true;
            break;
        }
    }
    if (space_in_name) {
        igsagent_error (agent, "spaces are not allowed in agent name '%s'", to_agent);
        free (reviewed_from_our_input);
        free (reviewed_to_agent);
        return 0;
    }
    
    char *a_name = igsagent_name (agent);
    if (streq (reviewed_to_agent, a_name))
        igsagent_warn (agent, "mapping inputs to outputs of the same agent will not work EXCEPT from one clone or variant to others");
    free (a_name);
    
    // with_output
    char *reviewed_with_output = s_strndup (with_output, IGS_MAX_IO_NAME_LENGTH);
    size_t length_of_reviewed_with_output = strlen (reviewed_with_output);
    space_in_name = false;
    for (i = 0; i < length_of_reviewed_with_output; i++) {
        if (reviewed_with_output[i] == ' ') {
            space_in_name = true;
            break;
        }
    }
    if (space_in_name) {
        igsagent_error (agent, "spaces are not allowed in IOP '%s'", with_output);
        free (reviewed_from_our_input);
        free (reviewed_to_agent);
        free (reviewed_with_output);
        return 0;
    }
    
    model_read_write_lock(__FUNCTION__, __LINE__);
    assert (agent->mapping);
    // Add the new mapping element if not already there
    size_t len = strlen (from_our_input) + strlen (to_agent) + strlen (with_output) + 2 + 1;
    char *mashup = (char *) zmalloc (len * sizeof (char));
    strcpy (mashup, reviewed_from_our_input);
    strcat (mashup, "."); // separator
    strcat (mashup, reviewed_to_agent);
    strcat (mashup, "."); // separator
    strcat (mashup, reviewed_with_output);
    mashup[len - 1] = '\0';
    uint64_t hash = mapping_djb2_hash ((unsigned char *) mashup);
    free (mashup);
    
    bool mapping_already_exists = false;
    igs_map_t *map_elmt = zlist_first(agent->mapping->map_elements);
    while (map_elmt) {
        if (map_elmt->id == hash){
            mapping_already_exists = true;
            break;
        }
        map_elmt = zlist_next(agent->mapping->map_elements);
    }
    if (!mapping_already_exists) {
        // NB: we allow mappings involving inputs that do not exist (ever, yet or anymore)
        igs_map_t *new = mapping_create_mapping_element (reviewed_from_our_input, reviewed_to_agent, reviewed_with_output);
        new->id = hash;
        zlist_append(agent->mapping->map_elements, new);
        mapping_update_json(agent->mapping);
        agent->network_need_to_send_mapping_update = true;
    } else
        igsagent_debug (agent,"mapping combination %s->%s.%s already exists : will not be duplicated",
                        reviewed_from_our_input, reviewed_to_agent, reviewed_with_output);
    free (reviewed_from_our_input);
    free (reviewed_to_agent);
    free (reviewed_with_output);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return hash;
}

igs_result_t igsagent_mapping_remove_with_id (igsagent_t *agent,
                                              uint64_t the_id)
{
    assert (agent);
    assert (agent->mapping);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_map_t *lookup = NULL;
    igs_map_t *el = zlist_first(agent->mapping->map_elements);
    while (el) {
        if (el->id == the_id){
            lookup = el;
            break;
        }
        el = zlist_next(agent->mapping->map_elements);
    }
    if (!lookup) {
        igsagent_error (agent, "id %llu is not part of the current mappings", the_id);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    zlist_remove(agent->mapping->map_elements, lookup);
    s_mapping_free_mapping_element (&lookup);
    mapping_update_json(agent->mapping);
    agent->network_need_to_send_mapping_update = true;
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return IGS_SUCCESS;
}

igs_result_t igsagent_mapping_remove_with_name (igsagent_t *agent,
                                                const char *from_our_input,
                                                const char *to_agent,
                                                const char *with_output)
{
    assert (agent);
    assert (from_our_input);
    assert (to_agent);
    assert (with_output);
    assert (agent->mapping);
    if (agent->mapping->map_elements == NULL) {
        igsagent_error (agent, "no mapping elements defined yet");
        return IGS_FAILURE;
    }
    
    model_read_write_lock(__FUNCTION__, __LINE__);
    size_t len = strlen (from_our_input) + strlen (to_agent) + strlen (with_output) + 2 + 1; //two separators and ending 0
    char *mashup = (char *) zmalloc (len * sizeof (char));
    strcpy (mashup, from_our_input);
    strcat (mashup, "."); // separator
    strcat (mashup, to_agent);
    strcat (mashup, "."); // separator
    strcat (mashup, with_output);
    mashup[len - 1] = '\0';
    uint64_t h = mapping_djb2_hash ((unsigned char *) mashup);
    free (mashup);
    
    igs_map_t *lookup = NULL;
    igs_map_t *el = zlist_first(agent->mapping->map_elements);
    while (el) {
        if (el->id == h){
            lookup = el;
            break;
        }
        el = zlist_next(agent->mapping->map_elements);
    }
    if (!lookup) {
        igsagent_debug (agent, "mapping combination %s->%s.%s does NOT exist", from_our_input, to_agent, with_output);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    zlist_remove(agent->mapping->map_elements, lookup);;
    s_mapping_free_mapping_element (&lookup);
    mapping_update_json(agent->mapping);
    agent->network_need_to_send_mapping_update = true;
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return IGS_SUCCESS;
}

void igsagent_mapping_set_path (igsagent_t *agent, const char *path)
{
    assert (agent);
    assert (path);
    model_read_write_lock(__FUNCTION__, __LINE__);
    if (agent->mapping_path)
        free (agent->mapping_path);
    agent->mapping_path = s_strndup (path, IGS_MAX_PATH_LENGTH);
    if (core_context->network_actor && core_context->node) {
        s_lock_zyre_peer (__FUNCTION__, __LINE__);
        zmsg_t *msg = zmsg_new ();
        zmsg_addstr (msg, MAPPING_FILE_PATH_MSG);
        zmsg_addstr (msg, agent->mapping_path);
        zmsg_addstr (msg, agent->uuid);
        zyre_shout (core_context->node, IGS_PRIVATE_CHANNEL, &msg);
        s_unlock_zyre_peer (__FUNCTION__, __LINE__);
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

void igsagent_mapping_save (igsagent_t *agent)
{
    assert (agent);
    assert (agent->mapping);
    if (!agent->mapping_path) {
        igsagent_error (agent, "no path configured to save mapping");
        return;
    }
    model_read_write_lock(__FUNCTION__, __LINE__);
    FILE *fp = NULL;
    fp = fopen (agent->mapping_path, "w+");
    igsagent_info (agent, "save to path %s", agent->mapping_path);
    if (!fp)
        igsagent_error (agent, "Could not open %s for writing", agent->mapping_path);
    else if (agent->mapping->json){
        fprintf (fp, "%s", agent->mapping->json);
        fflush (fp);
        fclose (fp);
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
}
