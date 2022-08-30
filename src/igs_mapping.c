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
uint64_t s_djb2_hash (unsigned char *str)
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
    if ((*mapping) == NULL)
        return;

    igs_map_t *current_map_elmt, *tmp_map_elmt;
    HASH_ITER (hh, (*mapping)->map_elements, current_map_elmt, tmp_map_elmt){
        HASH_DEL ((*mapping)->map_elements, current_map_elmt);
        s_mapping_free_mapping_element (&current_map_elmt);
    }

    igs_split_t *current_split_elmt, *tmp_split_elmt;
    HASH_ITER (hh, (*mapping)->split_elements, current_split_elmt, tmp_split_elmt){
        HASH_DEL ((*mapping)->split_elements, current_split_elmt);
        split_free_split_element (&current_split_elmt);
    }

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
    size_t firstS = HASH_COUNT (first->map_elements);
    size_t secondS = HASH_COUNT (second->map_elements);
    if (firstS != secondS) {
        res = false;
        goto END;
    }
    //comparing ids
    //NB: comparing ids (which are hashes) is sufficient to compare
    //the whole entries.
    igs_map_t *elmt, *tmp, *second_elmt;
    HASH_ITER (hh, first->map_elements, elmt, tmp){
        second_elmt = NULL;
        HASH_FIND (hh, second->map_elements, &elmt->id, sizeof (uint64_t), second_elmt);
        if (!second_elmt){
            res = false;
            goto END;
        }
    }

    //same for splits
    size_t first_split = HASH_COUNT (first->split_elements);
    size_t second_split = HASH_COUNT (second->split_elements);
    if (first_split != second_split) {
        res = false;
        goto END;
    }
    igs_split_t *elmt_split, *tmp_split, *second_elmt_split;
    HASH_ITER (hh, first->split_elements, elmt_split, tmp_split){
        second_elmt_split = NULL;
        HASH_FIND (hh, second->split_elements, &elmt_split->id, sizeof (uint64_t), second_elmt_split);
        if (!second_elmt_split) {
            res = false;
            goto END;
        }
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

bool mapping_check_input_output_compatibility (igsagent_t *agent,
                                               igs_iop_t *input,
                                               igs_iop_t *output)
{
    // for compatibility, only DATA outputs imply limitations
    // the rest is handled correctly in model_write_iop
    bool is_compatible = true;
    igs_iop_value_type_t type = input->value_type;
    if (output->value_type == IGS_DATA_T) {
        if (type != IGS_DATA_T && type != IGS_IMPULSION_T) {
            is_compatible = false;
            igsagent_warn (
              agent,
              "DATA outputs can only be mapped by DATA or IMPULSION inputs");
        }
    }
    return is_compatible;
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
    if (mapping_is_equal (json_str, current_mapping)) {
        igs_info ("new mapping is the same as the current one : nothing to do");
        if (current_mapping)
            free (current_mapping);
        return IGS_SUCCESS;
    }
    if (current_mapping)
        free (current_mapping);
    igs_mapping_t *tmp = parser_load_mapping (json_str);
    if (tmp == NULL) {
        igsagent_error (
          agent, "mapping could not be loaded from json string '%s'", json_str);
        return IGS_FAILURE;
    }
    else {
        model_read_write_lock (__FUNCTION__, __LINE__);
        // check that this agent has not been destroyed when we were locked
        if (!agent || !(agent->uuid)) {
            model_read_write_unlock (__FUNCTION__, __LINE__);
            return IGS_FAILURE;
        }
        if (agent->mapping)
            mapping_free_mapping (&agent->mapping);
        agent->mapping = tmp;
        agent->network_need_to_send_mapping_update = true;
        model_read_write_unlock (__FUNCTION__, __LINE__);
    }
    return IGS_SUCCESS;
}

igs_result_t igsagent_mapping_load_file (igsagent_t *agent,
                                           const char *file_path)
{
    assert (agent);
    assert (file_path);
    igs_mapping_t *tmp = parser_load_mapping_from_path (file_path);
    if (tmp == NULL) {
        igsagent_error (agent, "mapping could not be loaded from path '%s'",
                         file_path);
        return IGS_FAILURE;
    }
    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    if (agent->mapping)
        mapping_free_mapping (&agent->mapping);
    agent->mapping_path = s_strndup (file_path, IGS_MAX_PATH_LENGTH - 1);
    agent->mapping = tmp;
    agent->network_need_to_send_mapping_update = true;
    model_read_write_unlock (__FUNCTION__, __LINE__);
    return IGS_SUCCESS;
}

void igsagent_clear_mappings (igsagent_t *agent)
{
    //    igsagent_debug(agent, "clear current mapping for %s and initiate an
    //    empty one",
    //                   agent->definition->name);
    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return;
    }
    if (agent->mapping)
        mapping_free_mapping (&agent->mapping);
    agent->mapping =
      (struct igs_mapping *) zmalloc (sizeof (struct igs_mapping));
    agent->network_need_to_send_mapping_update = true;
    model_read_write_unlock (__FUNCTION__, __LINE__);
}

void igsagent_clear_mappings_with_agent (igsagent_t *agent,
                                          const char *agent_name)
{
    if (agent->mapping) {
        model_read_write_lock (__FUNCTION__, __LINE__);
        // check that this agent has not been destroyed when we were locked
        if (!agent || !(agent->uuid)) {
            model_read_write_unlock (__FUNCTION__, __LINE__);
            return;
        }
        igs_map_t *elmt, *tmp;
        HASH_ITER (hh, agent->mapping->map_elements, elmt, tmp)
        {
            if (streq (elmt->to_agent, agent_name)) {
                HASH_DEL (agent->mapping->map_elements, elmt);
                s_mapping_free_mapping_element (&elmt);
                agent->network_need_to_send_mapping_update = true;
            }
        }
        model_read_write_unlock (__FUNCTION__, __LINE__);
    }
}

char *igsagent_mapping_json (igsagent_t *agent)
{
    char *mapping_json = NULL;
    if (agent->mapping == NULL) {
        igsagent_warn (agent, "No mapping defined yet");
        return NULL;
    }
    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return NULL;
    }
    mapping_json = parser_export_mapping (agent->mapping);
    model_read_write_unlock (__FUNCTION__, __LINE__);
    return mapping_json;
}

size_t igsagent_mapping_count (igsagent_t *agent)
{
    assert (agent);
    assert (agent->mapping);
    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return 0;
    }
    size_t res = HASH_COUNT (agent->mapping->map_elements);
    model_read_write_unlock (__FUNCTION__, __LINE__);
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
    char *reviewed_from_our_input =
      s_strndup (from_our_input, IGS_MAX_IOP_NAME_LENGTH);
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
        igsagent_error (agent, "spaces are not allowed in IOP name '%s'",
                         from_our_input);
        return 0;
    }

    // to_agent
    char *reviewed_to_agent = s_strndup (to_agent, IGS_MAX_IOP_NAME_LENGTH);
    size_t length_of_reviewed_to_agent = strlen (reviewed_to_agent);
    space_in_name = false;
    for (i = 0; i < length_of_reviewed_to_agent; i++) {
        if (reviewed_to_agent[i] == ' ') {
            space_in_name = true;
            break;
        }
    }
    if (space_in_name) {
        igsagent_error (agent, "spaces are not allowed in agent name '%s'",
                         to_agent);
        return 0;
    }
    char *a_name = igsagent_name (agent);
    if (streq (reviewed_to_agent, a_name))
        igsagent_warn (
          agent, "mapping inputs to outputs of the same agent will not work "
                 "EXCEPT from one clone or variant to others");
    free (a_name);

    // with_output
    char *reviewed_with_output =
      s_strndup (with_output, IGS_MAX_IOP_NAME_LENGTH);
    size_t length_of_reviewed_with_output = strlen (reviewed_with_output);
    space_in_name = false;
    for (i = 0; i < length_of_reviewed_with_output; i++) {
        if (reviewed_with_output[i] == ' ') {
            space_in_name = true;
            break;
        }
    }
    if (space_in_name) {
        igsagent_error (agent, "spaces are not allowed in IOP '%s'",
                         with_output);
        return 0;
    }
    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return 0;
    }
    assert (agent->mapping);

    // Add the new mapping element if not already there
    size_t len = strlen (from_our_input) + strlen (to_agent)
                 + strlen (with_output) + 3 + 1;
    char *mashup = (char *) zmalloc (len * sizeof (char));
    strcpy (mashup, reviewed_from_our_input);
    strcat (mashup, "."); // separator
    strcat (mashup, reviewed_to_agent);
    strcat (mashup, "."); // separator
    strcat (mashup, reviewed_with_output);
    mashup[len - 1] = '\0';
    uint64_t hash = s_djb2_hash ((unsigned char *) mashup);
    free (mashup);

    igs_map_t *tmp = NULL;
    if (agent->mapping->map_elements)
        HASH_FIND (hh, agent->mapping->map_elements, &hash,sizeof (uint64_t), tmp);
    if (!tmp) {
        // element does not exist yet : create and register it
        // check input against definition and reject if input does not exist in
        // definition
        if (!igsagent_input_exists (agent, reviewed_from_our_input))
            igsagent_warn (agent,
                           "input %s does not exist in our definition (will be stored anyway)",
                           reviewed_from_our_input);

        igs_map_t *new = mapping_create_mapping_element (reviewed_from_our_input, reviewed_to_agent, reviewed_with_output);
        new->id = hash;
        HASH_ADD (hh, agent->mapping->map_elements, id, sizeof (uint64_t), new);
        agent->network_need_to_send_mapping_update = true;
    } else
        igsagent_warn (agent,
                       "mapping combination %s->%s.%s already exists : will not be duplicated",
                       reviewed_from_our_input, reviewed_to_agent,
                       reviewed_with_output);
    free (reviewed_from_our_input);
    free (reviewed_to_agent);
    free (reviewed_with_output);
    model_read_write_unlock (__FUNCTION__, __LINE__);
    return hash;
}

igs_result_t igsagent_mapping_remove_with_id (igsagent_t *agent,
                                               uint64_t the_id)
{
    assert (agent);
    assert (the_id > 0);
    assert (agent->mapping);
    igs_map_t *el = NULL;
    if (agent->mapping->map_elements == NULL) {
        igsagent_error (agent, "no mapping elements defined yet");
        return IGS_FAILURE;
    }
    HASH_FIND (hh, agent->mapping->map_elements, &the_id,
               sizeof (uint64_t), el);
    if (el == NULL) {
        igsagent_error (agent, "id %llu is not part of the current mapping", the_id);
        return IGS_FAILURE;
    }
    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return IGS_SUCCESS;
    }
    HASH_DEL (agent->mapping->map_elements, el);
    s_mapping_free_mapping_element (&el);
    agent->network_need_to_send_mapping_update = true;
    model_read_write_unlock (__FUNCTION__, __LINE__);
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

    size_t len = strlen (from_our_input) + strlen (to_agent)
                 + strlen (with_output) + 3 + 1;
    char *mashup = (char *) zmalloc (len * sizeof (char));
    strcpy (mashup, from_our_input);
    strcat (mashup, "."); // separator
    strcat (mashup, to_agent);
    strcat (mashup, "."); // separator
    strcat (mashup, with_output);
    mashup[len - 1] = '\0';
    uint64_t h = s_djb2_hash ((unsigned char *) mashup);
    free (mashup);

    igs_map_t *tmp = NULL;
    if (agent->mapping->map_elements)
        HASH_FIND (hh, agent->mapping->map_elements, &h, sizeof (uint64_t),
                   tmp);
    if (tmp == NULL) {
        igsagent_error (agent, "mapping combination %s->%s.%s does NOT exist",
                         from_our_input, to_agent, with_output);
        return IGS_FAILURE;
    }
    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return IGS_SUCCESS;
    }
    HASH_DEL (agent->mapping->map_elements, tmp);
    s_mapping_free_mapping_element (&tmp);
    agent->network_need_to_send_mapping_update = true;
    model_read_write_unlock (__FUNCTION__, __LINE__);
    return IGS_SUCCESS;
}

void igsagent_mapping_set_path (igsagent_t *agent, const char *path)
{
    assert (agent);
    assert (path);
    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return;
    }
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
    model_read_write_unlock (__FUNCTION__, __LINE__);
}

void igsagent_mapping_save (igsagent_t *agent)
{
    assert (agent);
    assert (agent->mapping);
    if (!agent->mapping_path) {
        igsagent_error (agent, "no path configured to save mapping");
        return;
    }
    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return;
    }
    FILE *fp = NULL;
    fp = fopen (agent->mapping_path, "w+");
    igsagent_info (agent, "save to path %s", agent->mapping_path);
    if (fp == NULL)
        igsagent_error (agent, "Could not open %s for writing",
                         agent->mapping_path);
    else {
        char *map = parser_export_mapping (agent->mapping);
        assert (map);
        fprintf (fp, "%s", map);
        fflush (fp);
        fclose (fp);
        free (map);
    }
    model_read_write_unlock (__FUNCTION__, __LINE__);
}
