/*  =========================================================================
    definition - create/edit/delete definitions

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "ingescape_private.h"
#include "uthash/uthash.h"
#include "uthash/utlist.h"
#include <stdio.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////
void definition_free_constraint (igs_constraint_t **c){
    assert(c);
    assert(*c);
    if ((*c)->type == IGS_CONSTRAINT_REGEXP){
        if ((*c)->regexp.rex)
            zrex_destroy(&(*c)->regexp.rex);
        if ((*c)->regexp.string)
            free((*c)->regexp.string);
    }
    free(*c);
    *c = NULL;
}


void s_definition_free_iop (igs_iop_t **iop)
{
    assert (iop);
    assert (*iop);
    if ((*iop)->name)
        free ((*iop)->name);

    switch ((*iop)->value_type) {
        case IGS_STRING_T:
            if ((*iop)->value.s)
                free ((char *) (*iop)->value.s);
            break;
        case IGS_DATA_T:
            if ((*iop)->value.data)
                free ((*iop)->value.data);
            break;
        default:
            break;
    }
    if ((*iop)->callbacks) {
        igs_observe_wrapper_t *cb, *tmp;
        DL_FOREACH_SAFE ((*iop)->callbacks, cb, tmp){
            DL_DELETE ((*iop)->callbacks, cb);
            free (cb);
        }
    }
    if ((*iop)->constraint)
        definition_free_constraint(&(*iop)->constraint);
    if ((*iop)->description)
        free((*iop)->description);

    free (*iop);
    *iop = NULL;
}

igs_result_t definition_add_iop_to_definition (igsagent_t *agent,
                                               igs_iop_t *iop,
                                               igs_iop_type_t iop_type,
                                               igs_definition_t *def)
{
    assert (agent);
    assert (iop);
    assert (def);
    model_read_write_lock ();
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock ();
        return IGS_SUCCESS;
    }
    igs_iop_t *previousIOP = NULL;
    switch (iop_type) {
        case IGS_INPUT_T:
            HASH_FIND_STR (def->inputs_table, iop->name, previousIOP);
            break;
        case IGS_OUTPUT_T:
            HASH_FIND_STR (def->outputs_table, iop->name, previousIOP);
            break;
        case IGS_PARAMETER_T:
            HASH_FIND_STR (def->params_table, iop->name, previousIOP);
            break;
        default:
            break;
    }
    if (previousIOP != NULL) {
        igsagent_error (agent, "%s already exists and cannot be overwritten",
                         iop->name);
        model_read_write_unlock ();
        return IGS_FAILURE;
    }
    switch (iop_type) {
        case IGS_INPUT_T:
            HASH_ADD_STR (def->inputs_table, name, iop);
            break;
        case IGS_OUTPUT_T:
            HASH_ADD_STR (def->outputs_table, name, iop);
            break;
        case IGS_PARAMETER_T:
            HASH_ADD_STR (def->params_table, name, iop);
            break;
        default:
            break;
    }
    model_read_write_unlock ();
    return IGS_SUCCESS;
}

igs_iop_t *definition_create_iop (igsagent_t *agent,
                                  const char *name,
                                  igs_iop_type_t type,
                                  igs_iop_value_type_t value_type,
                                  void *value,
                                  size_t size)
{
    assert (agent);
    assert (name);
    assert (agent->definition);
    igs_iop_t *iop = (igs_iop_t *) zmalloc (sizeof (igs_iop_t));
    char *n = s_strndup (name, IGS_MAX_IOP_NAME_LENGTH);
    bool space_in_name = false;
    size_t length_of_n = strlen (n);
    size_t i = 0;
    for (i = 0; i < length_of_n; i++) {
        if (n[i] == ' ') {
            n[i] = '_';
            space_in_name = true;
        }
    }
    if (space_in_name)
        igsagent_warn (
          agent,
          "spaces are not allowed in IOP name: '%s' has been renamed to '%s'",
          name, n);
    iop->name = n;
    iop->type = type;
    iop->value_type = value_type;
    switch (type) {
        case IGS_INPUT_T:
            if (definition_add_iop_to_definition (agent, iop, IGS_INPUT_T,
                                                  agent->definition)
                != IGS_SUCCESS) {
                s_definition_free_iop (&iop);
                return NULL;
            }
            break;
        case IGS_OUTPUT_T:
            if (definition_add_iop_to_definition (agent, iop, IGS_OUTPUT_T,
                                                  agent->definition)
                != IGS_SUCCESS) {
                s_definition_free_iop (&iop);
                return NULL;
            }
            break;
        case IGS_PARAMETER_T:
            if (definition_add_iop_to_definition (agent, iop, IGS_PARAMETER_T,
                                                  agent->definition)
                != IGS_SUCCESS) {
                s_definition_free_iop (&iop);
                return NULL;
            }
            break;
        default:
            break;
    }
    model_write_iop (agent, n, type, value_type, value, size);
    return iop;
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
void definition_free_definition (igs_definition_t **def)
{
    assert (def);
    assert (*def);
    if ((*def)->name != NULL) {
        free ((char *) (*def)->name);
        (*def)->name = NULL;
    }
    if ((*def)->family != NULL) {
        free ((char *) (*def)->family);
        (*def)->family = NULL;
    }
    if ((*def)->description != NULL) {
        free ((char *) (*def)->description);
        (*def)->description = NULL;
    }
    if ((*def)->version != NULL) {
        free ((char *) (*def)->version);
        (*def)->version = NULL;
    }
    igs_iop_t *current_iop, *tmp_iop;
    HASH_ITER (hh, (*def)->params_table, current_iop, tmp_iop)
    {
        HASH_DEL ((*def)->params_table, current_iop);
        s_definition_free_iop (&current_iop);
    }
    HASH_ITER (hh, (*def)->inputs_table, current_iop, tmp_iop)
    {
        HASH_DEL ((*def)->inputs_table, current_iop);
        s_definition_free_iop (&current_iop);
    }
    HASH_ITER (hh, (*def)->outputs_table, current_iop, tmp_iop)
    {
        HASH_DEL ((*def)->outputs_table, current_iop);
        s_definition_free_iop (&current_iop);
    }
    igs_service_t *service, *tmp_service;
    HASH_ITER (hh, (*def)->services_table, service, tmp_service)
    {
        HASH_DEL ((*def)->services_table, service);
        service_free_service (service);
    }
    free (*def);
    *def = NULL;
}

////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////
void igsagent_clear_definition (igsagent_t *agent)
{
    assert (agent);
    model_read_write_lock ();
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock ();
        return;
    }
    char *previous_name = NULL;
    if (agent->definition) {
        if (agent->definition->name)
            previous_name = strdup (agent->definition->name);
        definition_free_definition (&agent->definition);
    }
    agent->definition = (igs_definition_t *) zmalloc (sizeof (igs_definition_t));
    if (previous_name) {
        agent->definition->name = previous_name;
        igsagent_debug (agent, "Reuse previous name '%s'", previous_name);
    }
    else {
        agent->definition->name = strdup (IGS_DEFAULT_AGENT_NAME);
        // igsagent_debug(agent, "Use default name '%s'", IGS_DEFAULT_AGENT_NAME);
    }
    agent->network_need_to_send_definition_update = true;
    model_read_write_unlock ();
}

char *igsagent_definition_json (igsagent_t *agent)
{
    assert (agent);
    char *def = NULL;
    if (!agent->definition)
        return NULL;
    def = parser_export_definition (agent->definition);
    return def;
}

char *igsagent_family (igsagent_t *agent)
{
    assert (agent);
    assert (agent->definition);
    return (agent->definition->family) ? strdup (agent->definition->family)
                                       : NULL;
}

char *igsagent_definition_description (igsagent_t *agent)
{
    assert (agent);
    assert (agent->definition);
    return (agent->definition->description)
             ? strdup (agent->definition->description)
             : NULL;
}

char *igsagent_definition_version (igsagent_t *agent)
{
    assert (agent);
    assert (agent->definition);
    return (agent->definition->version) ? strdup (agent->definition->version)
                                        : NULL;
}

void igsagent_set_family (igsagent_t *agent, const char *family)
{
    assert (agent);
    assert (agent->definition);
    assert (family);
    if (agent->definition->family != NULL)
        free (agent->definition->family);
    agent->definition->family = s_strndup (family, IGS_MAX_FAMILY_LENGTH);
    agent->network_need_to_send_definition_update = true;
}

void igsagent_definition_set_description (igsagent_t *agent,
                                           const char *description)
{
    assert (agent);
    assert (description);
    assert (agent->definition);
    if (agent->definition->description)
        free (agent->definition->description);
    agent->definition->description =
      s_strndup (description, IGS_MAX_DESCRIPTION_LENGTH);
    agent->network_need_to_send_definition_update = true;
}

void igsagent_definition_set_version (igsagent_t *agent, const char *version)
{
    assert (agent);
    assert (version);
    assert (agent->definition);
    if (agent->definition->version != NULL)
        free (agent->definition->version);
    agent->definition->version = s_strndup (version, IGS_MAX_VERSION_LENGTH);
    agent->network_need_to_send_definition_update = true;
}

igs_result_t igsagent_input_create (igsagent_t *agent,
                                     const char *name,
                                     igs_iop_value_type_t value_type,
                                     void *value,
                                     size_t size)
{
    assert (agent);
    assert (name && strlen (name) > 0);
    assert (agent->definition);
    igs_iop_t *iop =
      definition_create_iop (agent, name, IGS_INPUT_T, value_type, value, size);
    if (!iop)
        return IGS_FAILURE;
    agent->network_need_to_send_definition_update = true;
    return IGS_SUCCESS;
}

igs_result_t igsagent_output_create (igsagent_t *agent,
                                      const char *name,
                                      igs_iop_value_type_t value_type,
                                      void *value,
                                      size_t size)
{
    assert (agent);
    assert (name && strlen (name) > 0);
    assert (agent->definition);
    igs_iop_t *iop = definition_create_iop (agent, name, IGS_OUTPUT_T,
                                            value_type, value, size);
    if (!iop)
        return IGS_FAILURE;
    agent->network_need_to_send_definition_update = true;
    return IGS_SUCCESS;
}

igs_result_t igsagent_parameter_create (igsagent_t *agent,
                                         const char *name,
                                         igs_iop_value_type_t value_type,
                                         void *value,
                                         size_t size)
{
    assert (agent);
    assert (name && strlen (name) > 0);
    assert (agent->definition);
    igs_iop_t *iop = definition_create_iop (agent, name, IGS_PARAMETER_T,
                                            value_type, value, size);
    if (!iop)
        return IGS_FAILURE;
    agent->network_need_to_send_definition_update = true;
    return IGS_SUCCESS;
}

igs_result_t igsagent_input_remove (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    assert (agent->definition);
    igs_iop_t *iop = model_find_iop_by_name (agent, name, IGS_INPUT_T);
    if (iop == NULL) {
        igsagent_error (agent, "The input %s could not be found", name);
        return IGS_FAILURE;
    }
    model_read_write_lock ();
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock ();
        return IGS_SUCCESS;
    }
    HASH_DEL (agent->definition->inputs_table, iop);
    s_definition_free_iop (&iop);
    model_read_write_unlock ();
    agent->network_need_to_send_definition_update = true;
    return IGS_SUCCESS;
}

igs_result_t igsagent_output_remove (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    assert (agent->definition);
    igs_iop_t *iop = model_find_iop_by_name (agent, name, IGS_OUTPUT_T);
    if (iop == NULL) {
        igsagent_error (agent, "The output %s could not be found", name);
        return IGS_FAILURE;
    }
    model_read_write_lock ();
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock ();
        return IGS_SUCCESS;
    }
    HASH_DEL (agent->definition->outputs_table, iop);
    s_definition_free_iop (&iop);
    model_read_write_unlock ();
    agent->network_need_to_send_definition_update = true;
    return IGS_SUCCESS;
}

igs_result_t igsagent_parameter_remove (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    assert (agent->definition);
    igs_iop_t *iop = model_find_iop_by_name (agent, name, IGS_PARAMETER_T);
    if (iop == NULL) {
        igsagent_error (agent, "The parameter %s could not be found", name);
        return IGS_FAILURE;
    }
    model_read_write_lock ();
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock ();
        return IGS_SUCCESS;
    }
    HASH_DEL (agent->definition->params_table, iop);
    s_definition_free_iop (&iop);
    model_read_write_unlock ();
    agent->network_need_to_send_definition_update = true;
    return IGS_SUCCESS;
}

void igsagent_definition_set_path (igsagent_t *agent, const char *path)
{
    assert (agent);
    assert (path);
    model_read_write_lock ();
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock ();
        return;
    }
    if (agent->definition_path)
        free (agent->definition_path);
    agent->definition_path = s_strndup (path, IGS_MAX_PATH_LENGTH);
    if (core_context->network_actor && core_context->node) {
        s_lock_zyre_peer ();
        zmsg_t *msg = zmsg_new ();
        zmsg_addstr (msg, DEFINITION_FILE_PATH_MSG);
        zmsg_addstr (msg, agent->definition_path);
        zmsg_addstr (msg, agent->uuid);
        zyre_shout (core_context->node, IGS_PRIVATE_CHANNEL, &msg);
        s_unlock_zyre_peer ();
    }
    model_read_write_unlock ();
}

void igsagent_definition_save (igsagent_t *agent)
{
    assert (agent);
    assert (agent->definition);
    if (!agent->definition_path) {
        igsagent_error (agent, "no path configured to save definition");
        return;
    }
    model_read_write_lock ();
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock ();
        return;
    }
    FILE *fp = NULL;
    fp = fopen (agent->definition_path, "w+");
    igsagent_info (agent, "save to path %s", agent->definition_path);
    if (fp == NULL)
        igsagent_error (agent, "Could not open '%s' for writing",
                         agent->definition_path);
    else {
        char *def = parser_export_definition (agent->definition);
        assert (def);
        fprintf (fp, "%s", def);
        fflush (fp);
        fclose (fp);
        free (def);
    }
    model_read_write_unlock ();
}
