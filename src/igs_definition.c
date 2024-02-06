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


void s_definition_free_io (igs_io_t **io)
{
    assert (io);
    assert (*io);
    if ((*io)->name)
        free ((*io)->name);

    switch ((*io)->value_type) {
        case IGS_STRING_T:
            if ((*io)->value.s)
                free ((char *) (*io)->value.s);
            break;
        case IGS_DATA_T:
            if ((*io)->value.data)
                free ((*io)->value.data);
            break;
        default:
            break;
    }
    if ((*io)->callbacks) {
        igs_observe_wrapper_t *cb, *tmp;
        DL_FOREACH_SAFE ((*io)->callbacks, cb, tmp){
            DL_DELETE ((*io)->callbacks, cb);
            free (cb);
        }
    }
    if ((*io)->constraint)
        definition_free_constraint(&(*io)->constraint);
    if ((*io)->description)
        free((*io)->description);
    if ((*io)->detailed_type)
        free((*io)->detailed_type);
    if ((*io)->specification)
        free((*io)->specification);

    free (*io);
    *io = NULL;
}

igs_result_t definition_add_io_to_definition (igsagent_t *agent,
                                               igs_io_t *io,
                                               igs_io_type_t io_type,
                                               igs_definition_t *def)
{
    assert (agent);
    assert (io);
    assert (def);
    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return IGS_SUCCESS;
    }
    igs_io_t *previousIOP = NULL;
    switch (io_type) {
        case IGS_INPUT_T:
            HASH_FIND_STR (def->inputs_table, io->name, previousIOP);
            break;
        case IGS_OUTPUT_T:
            HASH_FIND_STR (def->outputs_table, io->name, previousIOP);
            break;
        case IGS_ATTRIBUTE_T:
            HASH_FIND_STR (def->params_table, io->name, previousIOP);
            break;
        default:
            break;
    }
    if (previousIOP) {
        igsagent_error (agent, "%s already exists and cannot be overwritten",
                         io->name);
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    switch (io_type) {
        case IGS_INPUT_T:
            HASH_ADD_STR (def->inputs_table, name, io);
            break;
        case IGS_OUTPUT_T:
            HASH_ADD_STR (def->outputs_table, name, io);
            break;
        case IGS_ATTRIBUTE_T:
            HASH_ADD_STR (def->params_table, name, io);
            break;
        default:
            break;
    }
    model_read_write_unlock (__FUNCTION__, __LINE__);
    return IGS_SUCCESS;
}

igs_io_t *definition_create_io (igsagent_t *agent,
                                  const char *name,
                                  igs_io_type_t type,
                                  igs_io_value_type_t value_type,
                                  void *value,
                                  size_t size)
{
    assert (agent);
    assert (name);
    assert (agent->definition);
    if (value_type < IGS_UNKNOWN_T || value_type > IGS_DATA_T){
        igsagent_error(agent, "invalid value type %d", value_type);
        return NULL;
    }
    igs_io_t *io = (igs_io_t *) zmalloc (sizeof (igs_io_t));
    char *n = s_strndup (name, IGS_MAX_IO_NAME_LENGTH);
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
        igsagent_warn (agent, "spaces are not allowed in IOP name: '%s' has been renamed to '%s'", name, n);
    io->name = n;
    io->type = type;
    io->value_type = value_type;
    switch (type) {
        case IGS_INPUT_T:
            if (definition_add_io_to_definition (agent, io, IGS_INPUT_T,
                                                  agent->definition)
                != IGS_SUCCESS) {
                s_definition_free_io (&io);
                return NULL;
            }
            break;
        case IGS_OUTPUT_T:
            if (definition_add_io_to_definition (agent, io, IGS_OUTPUT_T,
                                                  agent->definition)
                != IGS_SUCCESS) {
                s_definition_free_io (&io);
                return NULL;
            }
            break;
        case IGS_ATTRIBUTE_T:
            if (definition_add_io_to_definition (agent, io, IGS_ATTRIBUTE_T,
                                                  agent->definition)
                != IGS_SUCCESS) {
                s_definition_free_io (&io);
                return NULL;
            }
            break;
        default:
            break;
    }
    model_write (agent, n, type, value_type, value, size);
    return io;
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
void definition_free_definition (igs_definition_t **def)
{
    assert (def);
    assert (*def);
    if ((*def)->my_class) {
        free ((char *) (*def)->my_class);
        (*def)->my_class = NULL;
    }
    if ((*def)->package) {
        free ((char *) (*def)->package);
        (*def)->package = NULL;
    }
    if ((*def)->name) {
        free ((char *) (*def)->name);
        (*def)->name = NULL;
    }
    if ((*def)->family) {
        free ((char *) (*def)->family);
        (*def)->family = NULL;
    }
    if ((*def)->description) {
        free ((char *) (*def)->description);
        (*def)->description = NULL;
    }
    if ((*def)->version) {
        free ((char *) (*def)->version);
        (*def)->version = NULL;
    }
    if ((*def)->json) {
        free ((char *) (*def)->json);
        (*def)->json = NULL;
    }
    if ((*def)->json_legacy_v3) {
        free ((char *) (*def)->json_legacy_v3);
        (*def)->json_legacy_v3 = NULL;
    }
    if ((*def)->json_legacy_v4) {
        free ((char *) (*def)->json_legacy_v4);
        (*def)->json_legacy_v4 = NULL;
    }
    igs_io_t *current_io, *tmp_io;
    HASH_ITER (hh, (*def)->params_table, current_io, tmp_io){
        HASH_DEL ((*def)->params_table, current_io);
        s_definition_free_io (&current_io);
    }
    HASH_ITER (hh, (*def)->inputs_table, current_io, tmp_io){
        HASH_DEL ((*def)->inputs_table, current_io);
        s_definition_free_io (&current_io);
    }
    HASH_ITER (hh, (*def)->outputs_table, current_io, tmp_io){
        HASH_DEL ((*def)->outputs_table, current_io);
        s_definition_free_io (&current_io);
    }
    igs_service_t *service, *tmp_service;
    HASH_ITER (hh, (*def)->services_table, service, tmp_service){
        HASH_DEL ((*def)->services_table, service);
        service_free_service (service);
    }
    free (*def);
    *def = NULL;
}

void definition_update_json (igs_definition_t *def)
{
    assert(def);
    if (def->json) {
        free ((char *) def->json);
        def->json = NULL;
    }
    if (def->json_legacy_v3) {
        free ((char *) def->json_legacy_v3);
        def->json_legacy_v3 = NULL;
    }
    if (def->json_legacy_v4) {
        free ((char *) def->json_legacy_v4);
        def->json_legacy_v4 = NULL;
    }
    def->json = parser_export_definition (def);
    def->json_legacy_v3 = parser_export_definition_legacy_v3 (def);
    def->json_legacy_v4 = parser_export_definition_legacy_v4 (def);
}

////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////
void igsagent_clear_definition (igsagent_t *agent)
{
    assert (agent);
    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
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
    } else {
        agent->definition->name = strdup (IGS_DEFAULT_AGENT_NAME);
        // igsagent_debug(agent, "Use default name '%s'", IGS_DEFAULT_AGENT_NAME);
    }
    definition_update_json (agent->definition);
    agent->network_need_to_send_definition_update = true;
    model_read_write_unlock (__FUNCTION__, __LINE__);
}

char *igsagent_definition_json (igsagent_t *agent)
{
    assert(agent);
    if (!agent->definition)
        return NULL;
    return (agent->definition->json)?strdup(agent->definition->json):NULL;
}

char *igsagent_definition_package (igsagent_t *agent)
{
    assert (agent);
    assert (agent->definition);
    return (agent->definition->package) ? strdup (agent->definition->package):NULL;
}

char *igsagent_definition_class (igsagent_t *agent)
{
    assert (agent);
    assert (agent->definition);
    return (agent->definition->my_class) ? strdup (agent->definition->my_class):NULL;
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
    if (agent->definition->family)
        free (agent->definition->family);
    agent->definition->family = s_strndup (family, IGS_MAX_FAMILY_LENGTH);
    definition_update_json (agent->definition);
    agent->network_need_to_send_definition_update = true;
}

void igsagent_definition_set_package (igsagent_t *agent,
                                      const char *package)
{
    assert (agent);
    assert (package);
    assert (agent->definition);
    if (agent->definition->package)
        free (agent->definition->package);
    agent->definition->package =
      s_strndup (package, IGS_MAX_DESCRIPTION_LENGTH);
    definition_update_json (agent->definition);
    agent->network_need_to_send_definition_update = true;
}

void igsagent_definition_set_class (igsagent_t *agent,
                                    const char *my_class)
{
    assert (agent);
    assert (my_class);
    assert (agent->definition);
    if (agent->definition->my_class)
        free (agent->definition->my_class);
    agent->definition->my_class =
      s_strndup (my_class, IGS_MAX_DESCRIPTION_LENGTH);
    definition_update_json (agent->definition);
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
    definition_update_json (agent->definition);
    agent->network_need_to_send_definition_update = true;
}

void igsagent_definition_set_version (igsagent_t *agent, const char *version)
{
    assert (agent);
    assert (version);
    assert (agent->definition);
    if (agent->definition->version)
        free (agent->definition->version);
    agent->definition->version = s_strndup (version, IGS_MAX_VERSION_LENGTH);
    definition_update_json (agent->definition);
    agent->network_need_to_send_definition_update = true;
}

igs_result_t igsagent_input_create (igsagent_t *agent,
                                     const char *name,
                                     igs_io_value_type_t value_type,
                                     void *value,
                                     size_t size)
{
    assert (agent);
    assert (name && strlen (name) > 0);
    assert (agent->definition);
    igs_io_t *io = definition_create_io (agent, name, IGS_INPUT_T, value_type, value, size);
    if (!io)
        return IGS_FAILURE;
    definition_update_json (agent->definition);
    agent->network_need_to_send_definition_update = true;
    return IGS_SUCCESS;
}

igs_result_t igsagent_output_create (igsagent_t *agent,
                                      const char *name,
                                      igs_io_value_type_t value_type,
                                      void *value,
                                      size_t size)
{
    assert (agent);
    assert (name && strlen (name) > 0);
    assert (agent->definition);
    igs_io_t *io = definition_create_io (agent, name, IGS_OUTPUT_T,
                                            value_type, value, size);
    if (!io)
        return IGS_FAILURE;
    definition_update_json (agent->definition);
    agent->network_need_to_send_definition_update = true;
    return IGS_SUCCESS;
}

igs_result_t igsagent_attribute_create (igsagent_t *agent,
                                         const char *name,
                                         igs_io_value_type_t value_type,
                                         void *value,
                                         size_t size)
{
    assert (agent);
    assert (name && strlen (name) > 0);
    assert (agent->definition);
    igs_io_t *io = definition_create_io (agent, name, IGS_ATTRIBUTE_T,
                                            value_type, value, size);
    if (!io)
        return IGS_FAILURE;
    definition_update_json (agent->definition);
    agent->network_need_to_send_definition_update = true;
    return IGS_SUCCESS;
}

igs_result_t igsagent_input_remove (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    assert (agent->definition);
    igs_io_t *io = model_find_io_by_name (agent, name, IGS_INPUT_T);
    if (io == NULL) {
        igsagent_error (agent, "The input %s could not be found", name);
        return IGS_FAILURE;
    }
    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return IGS_SUCCESS;
    }
    HASH_DEL (agent->definition->inputs_table, io);
    s_definition_free_io (&io);
    definition_update_json (agent->definition);
    agent->network_need_to_send_definition_update = true;
    model_read_write_unlock (__FUNCTION__, __LINE__);
    return IGS_SUCCESS;
}

igs_result_t igsagent_output_remove (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    assert (agent->definition);
    igs_io_t *io = model_find_io_by_name (agent, name, IGS_OUTPUT_T);
    if (io == NULL) {
        igsagent_error (agent, "The output %s could not be found", name);
        return IGS_FAILURE;
    }
    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return IGS_SUCCESS;
    }
    HASH_DEL (agent->definition->outputs_table, io);
    s_definition_free_io (&io);
    definition_update_json (agent->definition);
    agent->network_need_to_send_definition_update = true;
    model_read_write_unlock (__FUNCTION__, __LINE__);
    return IGS_SUCCESS;
}

igs_result_t igsagent_attribute_remove (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    assert (agent->definition);
    igs_io_t *io = model_find_io_by_name (agent, name, IGS_ATTRIBUTE_T);
    if (io == NULL) {
        igsagent_error (agent, "The attribute %s could not be found", name);
        return IGS_FAILURE;
    }
    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return IGS_SUCCESS;
    }
    HASH_DEL (agent->definition->params_table, io);
    s_definition_free_io (&io);
    definition_update_json (agent->definition);
    agent->network_need_to_send_definition_update = true;
    model_read_write_unlock (__FUNCTION__, __LINE__);
    return IGS_SUCCESS;
}

void igsagent_definition_set_path (igsagent_t *agent, const char *path)
{
    assert (agent);
    assert (path);
    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return;
    }
    if (agent->definition_path)
        free (agent->definition_path);
    agent->definition_path = s_strndup (path, IGS_MAX_PATH_LENGTH);
    if (core_context->network_actor && core_context->node) {
        s_lock_zyre_peer (__FUNCTION__, __LINE__);
        zmsg_t *msg = zmsg_new ();
        zmsg_addstr (msg, DEFINITION_FILE_PATH_MSG);
        zmsg_addstr (msg, agent->definition_path);
        zmsg_addstr (msg, agent->uuid);
        zyre_shout (core_context->node, IGS_PRIVATE_CHANNEL, &msg);
        s_unlock_zyre_peer (__FUNCTION__, __LINE__);
    }
    model_read_write_unlock (__FUNCTION__, __LINE__);
}

void igsagent_definition_save (igsagent_t *agent)
{
    assert (agent);
    assert (agent->definition);
    if (!agent->definition_path) {
        igsagent_error (agent, "no path configured to save definition");
        return;
    }
    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return;
    }
    FILE *fp = NULL;
    fp = fopen (agent->definition_path, "w+");
    igsagent_info (agent, "save to path %s", agent->definition_path);
    if (!fp)
        igsagent_error (agent, "Could not open '%s' for writing", agent->definition_path);
    else if (agent->definition->json) {
        fprintf (fp, "%s", agent->definition->json);
        fflush (fp);
        fclose (fp);
    }
    model_read_write_unlock (__FUNCTION__, __LINE__);
}
