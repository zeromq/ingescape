/*  =========================================================================
 service - create/edit/remove services and services arguments
 
 Copyright (c) the Contributors as noted in the AUTHORS file.
 This file is part of Ingescape, see https://github.com/zeromq/ingescape.
 
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
 =========================================================================
 */

#include "ingescape.h"
#include "ingescape_private.h"
#include "uthash/uthash.h"
#include "uthash/utlist.h"
#include <stdio.h>
#include <zyre.h>

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
void s_service_free_service_arguments (igs_service_arg_t *args)
{
    igs_service_arg_t *arg, *tmp;
    LL_FOREACH_SAFE (args, arg, tmp)
    {
        LL_DELETE (args, arg);
        if (arg->name)
            free (arg->name);
        if (arg->type == IGS_DATA_T && arg->data)
            free (arg->data);
        else
            if (arg->type == IGS_STRING_T && arg->c)
                free (arg->c);
        free (arg);
    }
}

void service_free_service (igs_service_t *t)
{
    if (t) {
        if (t->name)
            free (t->name);
        s_service_free_service_arguments (t->arguments);
        if (t->replies) {
            igs_service_t *r, *r_tmp;
            HASH_ITER(hh, t->replies, r, r_tmp){
                free (r->name);
                s_service_free_service_arguments (r->arguments);
                HASH_DEL(t->replies, r);
                free(r);
            }
        }
        free (t);
    }
}

igs_result_t service_add_values_to_arguments_from_message (const char *name,
                                                           igs_service_arg_t *arg,
                                                           zmsg_t *msg){
    size_t nb_frames = zmsg_size (msg);
    size_t nb_args = 0;
    igs_service_arg_t *tmp = NULL;
    DL_COUNT (arg, tmp, nb_args);
    if (nb_frames != nb_args) {
        igs_error ("arguments count do not match in received message for service %s "
                   "(%zu vs. %zu expected)",
                   name, nb_frames, nb_args);
        return IGS_FAILURE;
    }
    igs_service_arg_t *current = NULL;
    DL_FOREACH (arg, current){
        zframe_t *f = zmsg_pop (msg);
        size_t size = zframe_size (f);
        switch (current->type) {
            case IGS_BOOL_T:
                memcpy (&(current->b), zframe_data (f), sizeof (bool));
                break;
            case IGS_INTEGER_T:
                memcpy (&(current->i), zframe_data (f), sizeof (int));
                break;
            case IGS_DOUBLE_T:
                memcpy (&(current->d), zframe_data (f), sizeof (double));
                break;
            case IGS_STRING_T:
                if (current->c)
                    free (current->c);
                current->c = (char *) zmalloc (size);
                memcpy (current->c, zframe_data (f), size);
                break;
            case IGS_DATA_T:
                if (current->data)
                    free (current->data);
                current->data = (char *) zmalloc (size);
                memcpy (current->data, zframe_data (f), size);
                break;
                
            default:
                break;
        }
        current->size = size;
        zframe_destroy (&f);
    }
    return IGS_SUCCESS;
}

igs_result_t service_copy_arguments (igs_service_arg_t *source,
                                     igs_service_arg_t *destination)
{
    assert (source);
    assert (destination);
    size_t nb_args_source = 0;
    size_t nb_args_destination = 0;
    igs_service_arg_t *tmp = NULL;
    DL_COUNT (source, tmp, nb_args_source);
    DL_COUNT (destination, tmp, nb_args_destination);
    
    if (nb_args_source != nb_args_destination) {
        igs_error (
                   "number of elements must be the same in source and destination");
        return IGS_FAILURE;
    }
    
    igs_service_arg_t *current_source = NULL;
    igs_service_arg_t *current_destination = NULL;
    DL_FOREACH (destination, current_destination)
    {
        // init source if needed
        if (current_source == NULL)
            current_source = source;
        
        size_t size = current_source->size;
        switch (current_destination->type) {
            case IGS_BOOL_T:
                memcpy (&(current_destination->b), &(current_source->b),
                        sizeof (bool));
                break;
            case IGS_INTEGER_T:
                memcpy (&(current_destination->i), &(current_source->i),
                        sizeof (int));
                break;
            case IGS_DOUBLE_T:
                memcpy (&(current_destination->d), &(current_source->d),
                        sizeof (double));
                break;
            case IGS_STRING_T:
                if (current_destination->c)
                    free (current_destination->c);
                current_destination->c = (char *) zmalloc (size + 1);
                memcpy (current_destination->c, current_source->c, size);
                break;
            case IGS_DATA_T:
                if (current_destination->data)
                    free (current_destination->data);
                current_destination->data = (void *) zmalloc (size);
                memcpy (current_destination->data, current_source->data, size);
                break;
                
            default:
                break;
        }
        current_destination->size = size;
        current_source = current_source->next;
    }
    return IGS_SUCCESS;
}

void service_free_values_in_arguments (igs_service_arg_t *arg)
{
    if (arg) {
        igs_service_arg_t *tmp = NULL;
        DL_FOREACH (arg, tmp)
        {
            if (tmp->type == IGS_DATA_T && tmp->data)
                free (tmp->data);
            else
                if (tmp->type == IGS_STRING_T && tmp->data)
                    free (tmp->c);
            tmp->data = NULL;
            tmp->size = 0;
        }
    }
}

void service_log_received_service (igsagent_t *agent,
                                   const char *caller_agent_name,
                                   const char *caller_agentuuid,
                                   const char *service_name,
                                   igs_service_arg_t *list)
{
    char service_log[IGS_MAX_LOG_LENGTH] = "";
    char *service_log_cursor = service_log;
    service_log_cursor += snprintf (service_log, IGS_MAX_LOG_LENGTH, "received service %s from %s(%s) ",
                                    service_name, caller_agent_name, caller_agentuuid);
    igs_service_arg_t *current_arg = NULL;
    LL_FOREACH (list, current_arg){
        if (service_log_cursor - service_log >= IGS_MAX_LOG_LENGTH)
            break;
        switch (current_arg->type) {
            case IGS_BOOL_T:
                service_log_cursor += snprintf (service_log_cursor,
                                                IGS_MAX_LOG_LENGTH - (service_log_cursor - service_log),
                                                " %d", current_arg->b);
                break;
            case IGS_INTEGER_T:
                service_log_cursor += snprintf (service_log_cursor,
                                                IGS_MAX_LOG_LENGTH - (service_log_cursor - service_log),
                                                " %d", current_arg->i);
                break;
            case IGS_DOUBLE_T:
                service_log_cursor += snprintf (service_log_cursor,
                                                IGS_MAX_LOG_LENGTH - (service_log_cursor - service_log),
                                                " %f", current_arg->d);
                break;
            case IGS_STRING_T:
                service_log_cursor += snprintf (service_log_cursor,
                                                IGS_MAX_LOG_LENGTH - (service_log_cursor - service_log),
                                                " %s", current_arg->c);
                break;
            case IGS_DATA_T: {
                zchunk_t *chunk = zchunk_new (current_arg->data, current_arg->size);
                char *hex_str = zchunk_strhex (chunk);
                if (hex_str) {
                    service_log_cursor += snprintf (service_log_cursor,
                                                    IGS_MAX_LOG_LENGTH - (service_log_cursor - service_log),
                                                    " %s", hex_str);
                    free (hex_str);
                }
                else
                    service_log_cursor += snprintf (service_log_cursor,
                                                    IGS_MAX_LOG_LENGTH - (service_log_cursor - service_log),
                                                    " 00");
                zchunk_destroy (&chunk);
            } break;
                
            default:
                break;
        }
    }
    igsagent_debug (agent, "%s", service_log);
}

////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////
void igs_service_args_destroy (igs_service_arg_t **list)
{
    if (list && *list) {
        s_service_free_service_arguments (*list);
        *list = NULL;
    }
}

igs_service_arg_t *igs_service_args_clone (igs_service_arg_t *list)
{
    assert (list);
    igs_service_arg_t *res = NULL;
    igs_service_arg_t *arg = NULL;
    LL_FOREACH (list, arg){
        if (arg) {
            igs_service_arg_t *new = (igs_service_arg_t *) zmalloc (sizeof (igs_service_arg_t));
            new->type = arg->type;
            if (arg->name)
                new->name = strdup (arg->name);
            new->size = arg->size;
            switch (arg->type) {
                case IGS_BOOL_T:
                    new->b = arg->b;
                    break;
                case IGS_INTEGER_T:
                    new->i = arg->i;
                    break;
                case IGS_DOUBLE_T:
                    new->d = arg->d;
                    break;
                case IGS_STRING_T:
                    if (arg->c)
                        new->c = strdup (arg->c);
                    break;
                case IGS_DATA_T:
                    new->data = (void *) zmalloc (arg->size);
                    memcpy (new->data, arg->data, arg->size);
                    break;
                    
                default:
                    break;
            }
            LL_APPEND (res, new);
        }
    }
    return res;
}

void igs_service_args_add_int (igs_service_arg_t **list, int value)
{
    igs_service_arg_t *new = (igs_service_arg_t *) zmalloc (sizeof (igs_service_arg_t));
    new->type = IGS_INTEGER_T;
    new->i = value;
    new->size = sizeof (int);
    LL_APPEND (*list, new);
}

void igs_service_args_add_bool (igs_service_arg_t **list, bool value)
{
    igs_service_arg_t *new = (igs_service_arg_t *) zmalloc (sizeof (igs_service_arg_t));
    new->type = IGS_BOOL_T;
    new->b = value;
    new->size = sizeof (bool);
    LL_APPEND (*list, new);
}

void igs_service_args_add_double (igs_service_arg_t **list, double value)
{
    igs_service_arg_t *new = (igs_service_arg_t *) zmalloc (sizeof (igs_service_arg_t));
    new->type = IGS_DOUBLE_T;
    new->d = value;
    new->size = sizeof (double);
    LL_APPEND (*list, new);
}

void igs_service_args_add_string (igs_service_arg_t **list, const char *value)
{
    igs_service_arg_t *new = (igs_service_arg_t *) zmalloc (sizeof (igs_service_arg_t));
    new->type = IGS_STRING_T;
    if (value) {
        new->c = strdup (value);
        new->size = strlen (value) + 1;
    }
    else {
        new->c = NULL;
        new->size = 0;
    }
    LL_APPEND (*list, new);
}

void igs_service_args_add_data (igs_service_arg_t **list,
                                void *value,
                                size_t size)
{
    igs_service_arg_t *new = (igs_service_arg_t *) zmalloc (sizeof (igs_service_arg_t));
    new->type = IGS_DATA_T;
    new->data = (void *) zmalloc (size);
    memcpy (new->data, value, size);
    new->size = size;
    LL_APPEND (*list, new);
}

igs_result_t igsagent_service_init (igsagent_t *agent,
                                    const char *name,
                                    igsagent_service_fn cb,
                                    void *my_data)
{
    assert (agent);
    assert (name && strlen (name) > 0);
    assert (cb);
    igs_service_t *t = NULL;
    if (agent->definition == NULL)
        agent->definition =
        (igs_definition_t *) zmalloc (sizeof (igs_definition_t));
    
    HASH_FIND_STR (agent->definition->services_table, name, t);
    if (t && t->cb) {
        igsagent_error (
                        agent, "service with name %s already exists and has a callback",
                        name);
        return IGS_FAILURE;
    }
    if (cb == NULL) {
        igsagent_error (agent,
                        "non-NULL callback is mandatory at service creation");
        return IGS_FAILURE;
    }
    if (t == NULL) {
        // service is completely new: allocate it
        t = (igs_service_t *) zmalloc (sizeof (igs_service_t));
        if (strnlen (name, IGS_MAX_STRING_MSG_LENGTH) == IGS_MAX_STRING_MSG_LENGTH) {
            t->name = s_strndup (name, IGS_MAX_STRING_MSG_LENGTH);
            igsagent_warn (agent, "service name has been shortened to %s",
                           t->name);
        } else {
            t->name = s_strndup (name, IGS_MAX_STRING_MSG_LENGTH);
        }
        HASH_ADD_STR (agent->definition->services_table, name, t);
        agent->network_need_to_send_definition_update = true;
    }
    t->cb = cb;
    t->cb_data = my_data;
    return IGS_SUCCESS;
}

igs_result_t igsagent_service_remove (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    igs_service_t *t = NULL;
    if (agent->definition == NULL) {
        igsagent_error (agent, "No definition available yet");
        return IGS_FAILURE;
    }
    HASH_FIND_STR (agent->definition->services_table, name, t);
    if (t == NULL) {
        igsagent_error (agent, "service with name '%s' does not exist", name);
        return IGS_FAILURE;
    }
    HASH_DEL (agent->definition->services_table, t);
    service_free_service (t);
    agent->network_need_to_send_definition_update = true;
    return IGS_SUCCESS;
}

igs_result_t igsagent_service_arg_add (igsagent_t *agent,
                                       const char *service_name,
                                       const char *arg_name,
                                       igs_iop_value_type_t type)
{
    assert (agent);
    assert (service_name);
    assert (arg_name && strlen (arg_name) > 0);
    igs_service_t *t = NULL;
    if (agent->definition == NULL) {
        igsagent_error (agent, "No definition available yet");
        return IGS_FAILURE;
    }
    HASH_FIND_STR (agent->definition->services_table, service_name, t);
    if (type == IGS_IMPULSION_T) {
        igsagent_error (agent,
                        "impulsion type is not allowed as a service argument");
        return IGS_FAILURE;
    }
    if (type == IGS_UNKNOWN_T) {
        igsagent_error (agent,
                        "unknown type is not allowed as a service argument");
        return IGS_FAILURE;
    }
    if (!t) {
        igsagent_error (agent, "service with name %s does not exist",
                        service_name);
        return IGS_FAILURE;
    }
    igs_service_arg_t *a = (igs_service_arg_t *) zmalloc (sizeof (igs_service_arg_t));
    if (strnlen (arg_name, IGS_MAX_STRING_MSG_LENGTH) == IGS_MAX_STRING_MSG_LENGTH) {
        a->name = s_strndup (arg_name, IGS_MAX_STRING_MSG_LENGTH);
        igsagent_warn (agent, "service argument name has been shortened to %s",
                       a->name);
    }
    else
        a->name = s_strndup (arg_name, IGS_MAX_STRING_MSG_LENGTH);
    switch (type) {
        case IGS_BOOL_T:
            a->size = sizeof (bool);
            break;
        case IGS_INTEGER_T:
            a->size = sizeof (int);
            break;
        case IGS_DOUBLE_T:
            a->size = sizeof (double);
            break;
        case IGS_STRING_T:
            a->size = 0;
            break;
        case IGS_DATA_T:
            a->size = 0;
            break;
        default:
            break;
    }
    a->type = type;
    LL_APPEND (t->arguments, a);
    agent->network_need_to_send_definition_update = true;
    return IGS_SUCCESS;
}

igs_result_t igsagent_service_arg_remove (igsagent_t *agent,
                                          const char *service_name,
                                          const char *arg_name)
{
    assert (agent);
    assert (service_name);
    assert (arg_name);
    igs_service_t *t = NULL;
    if (agent->definition == NULL) {
        igsagent_error (agent, "No definition available yet");
        return IGS_FAILURE;
    }
    HASH_FIND_STR (agent->definition->services_table, service_name, t);
    if (t == NULL) {
        igsagent_error (agent, "service with name %s does not exist",
                        service_name);
        return IGS_FAILURE;
    }
    igs_service_arg_t *arg = NULL, *tmp = NULL;
    bool found = false;
    LL_FOREACH_SAFE (t->arguments, arg, tmp) {
        if (streq (arg_name, arg->name)) {
            LL_DELETE (t->arguments, arg);
            free (arg->name);
            if (arg->type == IGS_DATA_T && arg->data)
                free (arg->data);
            else
                if (arg->type == IGS_STRING_T && arg->data)
                    free (arg->c);
            free (arg);
            found = true;
            agent->network_need_to_send_definition_update = true;
            break;
        }
    }
    if (!found)
        igsagent_debug (agent, "no argument named %s for service %s", arg_name,
                        service_name);
    return IGS_SUCCESS;
}

igs_result_t igsagent_service_reply_add(igsagent_t *agent, const char *service_name, const char *reply_name){
    assert (agent);
    assert (service_name);
    assert (reply_name);
    igs_service_t *s = NULL;
    igs_service_t *r = NULL;
    if (agent->definition == NULL) {
        igsagent_error (agent, "No definition available yet");
        return IGS_FAILURE;
    }
    HASH_FIND_STR (agent->definition->services_table, service_name, s);
    if (!s) {
        igsagent_error (agent, "service with name %s does not exist",
                        service_name);
        return IGS_FAILURE;
    }
    HASH_FIND_STR (s->replies, reply_name, r);
    if (r) {
        igsagent_error (agent, "service reply with name %s already exists",
                        reply_name);
        return IGS_FAILURE;
    }
    r = (igs_service_t *) zmalloc (sizeof (igs_service_t));
    if (strnlen (reply_name, IGS_MAX_STRING_MSG_LENGTH) == IGS_MAX_STRING_MSG_LENGTH) {
        r->name = s_strndup (reply_name, IGS_MAX_STRING_MSG_LENGTH);
        igsagent_warn (agent, "service name has been shortened to %s", r->name);
    } else
        r->name = s_strndup (reply_name, IGS_MAX_STRING_MSG_LENGTH);
    HASH_ADD_STR(s->replies, name, r);
    agent->network_need_to_send_definition_update = true;
    return IGS_SUCCESS;
}

igs_result_t igsagent_service_reply_remove(igsagent_t *agent, const char *service_name, const char *reply_name){
    assert(agent);
    assert(service_name);
    assert(reply_name);
    igs_service_t *s = NULL;
    if (agent->definition == NULL) {
        igsagent_error (agent, "No definition available yet");
        return IGS_FAILURE;
    }
    HASH_FIND_STR (agent->definition->services_table, service_name, s);
    if (!s) {
        igsagent_error (agent, "service with name %s does not exist", service_name);
        return IGS_FAILURE;
    }
    igs_service_t *r = NULL;
    HASH_FIND_STR(s->replies, reply_name, r);
    if (r){
        HASH_DEL(s->replies, r);
        service_free_service (r);
        agent->network_need_to_send_definition_update = true;
        return IGS_SUCCESS;
    }else{
        igsagent_error (agent, "service with name %s  has no reply named %s", service_name, reply_name);
        return IGS_FAILURE;
    }
}

igs_result_t igsagent_service_reply_arg_add(igsagent_t *agent, const char *service_name, const char *reply_name,
                                            const char *arg_name, igs_iop_value_type_t type){
    assert (agent);
    assert (service_name);
    assert(reply_name);
    assert(arg_name);
    igs_service_t *s = NULL;
    if (agent->definition == NULL) {
        igsagent_error (agent, "No definition available yet");
        return IGS_FAILURE;
    }
    HASH_FIND_STR (agent->definition->services_table, service_name, s);
    if (!s) {
        igsagent_error (agent, "service with name %s does not exist", service_name);
        return IGS_FAILURE;
    }
    igs_service_t *r = NULL;
    HASH_FIND_STR(s->replies, reply_name, r);
    if (!r){
        igsagent_error (agent, "service with name %s  has no reply named %s", service_name, reply_name);
        return IGS_FAILURE;
    }
    if (type == IGS_IMPULSION_T) {
        igsagent_error (agent, "impulsion type is not allowed as a service argument");
        return IGS_FAILURE;
    }
    if (type == IGS_UNKNOWN_T) {
        igsagent_error (agent, "unknown type is not allowed as a service argument");
        return IGS_FAILURE;
    }
    igs_service_arg_t *a = (igs_service_arg_t *) zmalloc (sizeof (igs_service_arg_t));
    if (strnlen (arg_name, IGS_MAX_STRING_MSG_LENGTH) == IGS_MAX_STRING_MSG_LENGTH) {
        a->name = s_strndup (arg_name, IGS_MAX_STRING_MSG_LENGTH);
        igsagent_warn (agent, "service argument name has been shortened to %s", a->name);
    } else
        a->name = s_strndup (arg_name, IGS_MAX_STRING_MSG_LENGTH);
    switch (type) {
        case IGS_BOOL_T:
            a->size = sizeof (bool);
            break;
        case IGS_INTEGER_T:
            a->size = sizeof (int);
            break;
        case IGS_DOUBLE_T:
            a->size = sizeof (double);
            break;
        case IGS_STRING_T:
            a->size = 0;
            break;
        case IGS_DATA_T:
            a->size = 0;
            break;
        default:
            break;
    }
    a->type = type;
    LL_APPEND (r->arguments, a);
    agent->network_need_to_send_definition_update = true;
    return IGS_SUCCESS;
}

igs_result_t igsagent_service_reply_arg_remove(igsagent_t *agent, const char *service_name, const char *reply_name,
                                               const char *arg_name){
    assert (agent);
    assert (service_name);
    assert(reply_name);
    assert(arg_name);
    igs_service_t *s = NULL;
    if (agent->definition == NULL) {
        igsagent_error (agent, "No definition available yet");
        return IGS_FAILURE;
    }
    HASH_FIND_STR (agent->definition->services_table, service_name, s);
    if (!s) {
        igsagent_error (agent, "service with name %s does not exist", service_name);
        return IGS_FAILURE;
    }
    igs_service_t *r = NULL;
    HASH_FIND_STR(s->replies, reply_name, r);
    if (!r){
        igsagent_error (agent, "service with name %s  has no reply named %s", service_name, reply_name);
        return IGS_FAILURE;
    }
    igs_service_arg_t *arg = NULL, *tmp = NULL;
    bool found = false;
    LL_FOREACH_SAFE (r->arguments, arg, tmp) {
        if (streq (arg_name, arg->name)) {
            LL_DELETE (r->arguments, arg);
            free (arg->name);
            if (arg->type == IGS_DATA_T && arg->data)
                free (arg->data);
            else
                if (arg->type == IGS_STRING_T && arg->data)
                    free (arg->c);
            free (arg);
            found = true;
            agent->network_need_to_send_definition_update = true;
            break;
        }
    }
    if (!found) {
        igsagent_debug (agent, "no argument named %s for reply %s in service %s", arg_name, reply_name, service_name);
        return IGS_FAILURE;
    }
    return IGS_SUCCESS;
}

void s_service_log_sent_service (igsagent_t *agent,
                                 const char *target_agent_name,
                                 const char *target_agentuuid,
                                 const char *service_name,
                                 igs_service_arg_t *list)
{
    char service_log[IGS_MAX_LOG_LENGTH] = "";
    char *service_log_cursor = service_log;
    service_log_cursor +=
    snprintf (service_log, IGS_MAX_LOG_LENGTH, "send service %s(%s).%s",
              target_agent_name, target_agentuuid, service_name);
    igs_service_arg_t *current_arg = NULL;
    LL_FOREACH (list, current_arg)
    {
        if (service_log_cursor - service_log >= IGS_MAX_LOG_LENGTH)
            break;
        switch (current_arg->type) {
            case IGS_BOOL_T:
                service_log_cursor += snprintf (
                                                service_log_cursor,
                                                IGS_MAX_LOG_LENGTH - (service_log_cursor - service_log),
                                                " %d", current_arg->b);
                break;
            case IGS_INTEGER_T:
                service_log_cursor += snprintf (
                                                service_log_cursor,
                                                IGS_MAX_LOG_LENGTH - (service_log_cursor - service_log),
                                                " %d", current_arg->i);
                break;
            case IGS_DOUBLE_T:
                service_log_cursor += snprintf (
                                                service_log_cursor,
                                                IGS_MAX_LOG_LENGTH - (service_log_cursor - service_log),
                                                " %f", current_arg->d);
                break;
            case IGS_STRING_T:
                service_log_cursor += snprintf (
                                                service_log_cursor,
                                                IGS_MAX_LOG_LENGTH - (service_log_cursor - service_log),
                                                " %s", current_arg->c);
                break;
            case IGS_DATA_T: {
                zchunk_t *chunk =
                zchunk_new (current_arg->data, current_arg->size);
                char *hex_str = zchunk_strhex (chunk);
                if (hex_str) {
                    service_log_cursor += snprintf (
                                                    service_log_cursor,
                                                    IGS_MAX_LOG_LENGTH - (service_log_cursor - service_log),
                                                    " %s", hex_str);
                    free (hex_str);
                }
                else
                    service_log_cursor += snprintf (
                                                    service_log_cursor,
                                                    IGS_MAX_LOG_LENGTH - (service_log_cursor - service_log),
                                                    " 00");
                zchunk_destroy (&chunk);
            } break;
            default:
                break;
        }
    }
    igsagent_debug (agent, "%s", service_log);
}

igs_result_t igsagent_service_call (igsagent_t *agent,
                                    const char *agent_name_or_uuid,
                                    const char *service_name,
                                    igs_service_arg_t **list,
                                    const char *token)
{
    assert (agent);
    assert (agent_name_or_uuid);
    assert (service_name);
    assert ((list == NULL) || (*list));
    
    bool found = false;
    
    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return IGS_SUCCESS;
    }
    
    // 1- iteration on remote agents
    if (core_context->node) {
        igs_remote_agent_t *remote_agent = NULL, *tmp = NULL;
        HASH_ITER (hh, agent->context->remote_agents, remote_agent, tmp)
        {
            if ((remote_agent->definition
                 && streq (remote_agent->definition->name, agent_name_or_uuid))
                || streq (remote_agent->uuid, agent_name_or_uuid)) {
                // we found a matching agent
                igs_service_arg_t *arg = NULL;
                found = true;
                
                /*
                 We remove verifications on the service on sender side to enable
                 proper proxy implementation (local proxy does not implement
                 services but relays them to remote clients and virtual agents).
                 
                 if (remote_agent->definition == NULL){
                 igsagent_warn(agent, "definition is unknown for %s(%s) : cannot
                 verify service before sending it", remote_agent->name,
                 agent_name_or_uuid);
                 //continue; //commented to allow sending the message anyway
                 }else{
                 igs_service_t *service = NULL;
                 HASH_FIND_STR(remote_agent->definition->services_table,
                 service_name, service); if (service){ size_t nb_arguments = 0;
                 if (list && *list)
                 LL_COUNT(*list, arg, nb_arguments);
                 size_t defined_nb_arguments = 0;
                 LL_COUNT(service->arguments, arg, defined_nb_arguments);
                 if (nb_arguments != defined_nb_arguments){
                 igsagent_error(agent, "passed number of arguments is not
                 correct (received: %zu / expected: %zu) : service will not be sent",
                 nb_arguments, defined_nb_arguments);
                 continue;
                 }
                 }else{
                 igsagent_warn(agent, "could not find service named %s for %s
                 (%s) : cannot verify service before sending it", service_name,
                 remote_agent->name, remote_agent->uuid);
                 //continue; //commented to allow sending the message anyway
                 }
                 }
                 */
                zmsg_t *msg = zmsg_new ();
                if (remote_agent->peer->protocol
                    && (streq (remote_agent->peer->protocol, "v2")
                        || streq (remote_agent->peer->protocol, "v3"))) {
                    igs_warn ("Remote agent %s(%s) uses an older version of Ingescape with deprecated protocol. Please upgrade this agent.", remote_agent->definition->name, remote_agent->uuid);
                    zmsg_addstr (msg, CALL_SERVICE_MSG_DEPRECATED);
                }
                else
                    zmsg_addstr (msg, CALL_SERVICE_MSG);
                
                zmsg_addstr (msg, agent->uuid);
                zmsg_addstr (msg, remote_agent->uuid);
                zmsg_addstr (msg, service_name);
                if (token)
                    zmsg_addstr (msg, token);
                else
                    zmsg_addstr (msg, "");
                if (list) {
                    LL_FOREACH (*list, arg)
                    {
                        zframe_t *frame = NULL;
                        switch (arg->type) {
                            case IGS_BOOL_T:
                                frame = zframe_new (&arg->b, sizeof (int));
                                break;
                            case IGS_INTEGER_T:
                                frame = zframe_new (&arg->i, sizeof (int));
                                break;
                            case IGS_DOUBLE_T:
                                frame = zframe_new (&arg->d, sizeof (double));
                                break;
                            case IGS_STRING_T: {
                                if (arg->c)
                                    frame =
                                    zframe_new (arg->c, strlen (arg->c) + 1);
                                else
                                    frame = zframe_new (NULL, 0);
                                break;
                            }
                            case IGS_DATA_T:
                                frame = zframe_new (arg->data, arg->size);
                                break;
                            default:
                                break;
                        }
                        assert (frame);
                        zmsg_add (msg, frame);
                    }
                }
                s_lock_zyre_peer (__FUNCTION__, __LINE__);
                zyre_shouts (agent->context->node, agent->igs_channel,
                             "SERVICE %s(%s) called %s.%s(%s)",
                             agent->definition->name, agent->uuid,
                             remote_agent->definition->name, service_name,
                             remote_agent->uuid);
                zyre_whisper (agent->context->node, remote_agent->peer->peer_id, &msg);
                s_unlock_zyre_peer (__FUNCTION__, __LINE__);
                if (core_context->enable_service_logging)
                    s_service_log_sent_service (agent, remote_agent->definition->name, remote_agent->uuid,
                                                service_name, *list);
                else
                    igsagent_debug (agent, "calling %s(%s).%s",
                                    remote_agent->definition->name,
                                    remote_agent->uuid, service_name);
            }
        }
    }
    
    // 2- iteration on local agents
    if (!agent->is_virtual) {
        igsagent_t *local_agent, *atmp;
        HASH_ITER (hh, agent->context->agents, local_agent, atmp)
        {
            if (streq (local_agent->definition->name, agent_name_or_uuid)
                || streq (local_agent->uuid, agent_name_or_uuid)) {
                // we found a matching agent
                igs_service_arg_t *arg = NULL;
                found = true;
                if (local_agent->definition == NULL) {
                    igsagent_error (agent, "definition is unknown for %s(%s) : service will not be sent",
                                    local_agent->definition->name,agent_name_or_uuid);
                    continue;
                }
                else {
                    igs_service_t *service = NULL;
                    HASH_FIND_STR (local_agent->definition->services_table,
                                   service_name, service);
                    if (service) {
                        size_t nb_arguments = 0;
                        if (list && *list)
                            LL_COUNT (*list, arg, nb_arguments);
                        size_t defined_nb_arguments = 0;
                        LL_COUNT (service->arguments, arg,
                                  defined_nb_arguments);
                        if (nb_arguments != defined_nb_arguments) {
                            igsagent_error (agent, "passed number of arguments is not correct (received: %zu / "
                                            "expected: %zu) : service will not be sent", nb_arguments, defined_nb_arguments);
                            continue;
                        }
                        else {
                            // update service arguments values with new ones
                            if (service->arguments && list)
                                service_copy_arguments (*list, service->arguments);
                            if (service->cb) {
                                model_read_write_unlock (__FUNCTION__, __LINE__);
                                (service->cb) (local_agent, agent->definition->name,
                                               agent->uuid, service_name, service->arguments,
                                               nb_arguments, token, service->cb_data);
                                model_read_write_lock (__FUNCTION__, __LINE__);
                                service_free_values_in_arguments (service->arguments);
                                if (core_context->enable_service_logging)
                                    service_log_received_service (local_agent, agent->definition->name,
                                                                  agent->uuid, service_name, *list);
                            }
                            else
                                igsagent_error (agent, "no defined callback to handle received service %s", service_name);
                        }
                    }
                    else {
                        igsagent_error (agent, "could not find service named %s for %s (%s) : service will not be sent",
                                        service_name, local_agent->definition->name,local_agent->uuid);
                        continue;
                    }
                }
                
                s_lock_zyre_peer (__FUNCTION__, __LINE__);
                if (core_context->node) {
                    zyre_shouts (agent->context->node, agent->igs_channel,
                                 "SERVICE %s(%s) called %s.%s(%s)",
                                 agent->definition->name, agent->uuid,
                                 local_agent->definition->name, service_name,
                                 local_agent->uuid);
                }
                s_unlock_zyre_peer (__FUNCTION__, __LINE__);
                
                if (core_context->enable_service_logging)
                    s_service_log_sent_service (agent, local_agent->definition->name, local_agent->uuid,
                                                service_name, *list);
                else
                    igsagent_debug (agent, "calling %s.%s(%s)",
                                    local_agent->definition->name,
                                    service_name, local_agent->uuid);
            }
        }
    }
    
    if ((list) && (*list)) {
        s_service_free_service_arguments (*list);
        *list = NULL;
    }
    model_read_write_unlock (__FUNCTION__, __LINE__);
    if (!found) {
        igsagent_error (agent, "could not find an agent with name or UUID : %s",
                        agent_name_or_uuid);
        return IGS_FAILURE;
    }
    return IGS_SUCCESS;
}

size_t igsagent_service_count (igsagent_t *agent)
{
    if (agent->definition == NULL) {
        igsagent_warn (agent, "definition is NULL");
        return 0;
    }
    return HASH_COUNT (agent->definition->services_table);
}

bool igsagent_service_exists (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (agent->definition == NULL)
        return false;
    igs_service_t *t = NULL;
    HASH_FIND_STR (agent->definition->services_table, name, t);
    return (t);
}

char **igsagent_service_list (igsagent_t *agent, size_t *nb_of_elements)
{
    assert (agent);
    if (agent->definition == NULL) {
        *nb_of_elements = 0;
        return NULL;
    }
    size_t nb = HASH_COUNT (agent->definition->services_table);
    if (nb == 0) {
        *nb_of_elements = 0;
        return NULL;
    }
    *nb_of_elements = nb;
    char **res = (char **) zmalloc (nb * sizeof (char *));
    igs_service_t *el, *tmp;
    size_t i = 0;
    HASH_ITER (hh, agent->definition->services_table, el, tmp)
    {
        res[i++] = strdup (el->name);
    }
    return res;
}

void igs_free_services_list (char **list, size_t nb_of_services)
{
    assert(list);
    size_t i = 0;
    for (i = 0; i < nb_of_services; i++) {
        if (list[i])
            free (list[i]);
    }
    free (list);
}

igs_service_arg_t *igsagent_service_args_first (igsagent_t *agent,
                                                const char *service_name)
{
    assert(agent);
    assert(service_name);
    if (!agent->definition) {
        igsagent_error (agent, "agent definition is NULL");
        return NULL;
    }
    igs_service_t *t = NULL;
    HASH_FIND_STR (agent->definition->services_table, service_name, t);
    if (!t) {
        igsagent_debug (agent, "could not find service with name %s",
                        service_name);
        return NULL;
    }
    return t->arguments;
}

size_t igsagent_service_args_count (igsagent_t *agent,
                                    const char *service_name)
{
    assert(agent);
    assert(service_name);
    if (!agent->definition) {
        igsagent_error (agent, "agent definition is NULL");
        return 0;
    }
    igs_service_t *t = NULL;
    HASH_FIND_STR (agent->definition->services_table, service_name, t);
    if (t == NULL) {
        igsagent_debug (agent, "could not find service with name %s",
                        service_name);
        return 0;
    }
    size_t nb = 0;
    igs_service_arg_t *a = NULL;
    LL_COUNT (t->arguments, a, nb);
    return nb;
}

bool igsagent_service_arg_exists (igsagent_t *agent,
                                  const char *service_name,
                                  const char *arg_name)
{
    assert(agent);
    assert(service_name);
    assert(arg_name);
    if (!agent->definition) {
        igsagent_error (agent, "agent definition is NULL");
        return false;
    }
    igs_service_t *t = NULL;
    HASH_FIND_STR (agent->definition->services_table, service_name, t);
    if (t == NULL) {
        igsagent_debug (agent, "could not find service with name %s",
                        service_name);
        return false;
    }
    igs_service_arg_t *a = NULL;
    LL_FOREACH (t->arguments, a)
    {
        if (streq (a->name, arg_name))
            return true;
    }
    return false;
}

bool igsagent_service_has_replies(igsagent_t *agent, const char *service_name){
    assert(agent);
    assert(service_name);
    igs_service_t *s = NULL;
    HASH_FIND_STR (agent->definition->services_table, service_name, s);
    if (!s) {
        igsagent_debug (agent, "could not find service with name %s", service_name);
        return false;
    }
    if (s->replies)
        return true;
    else
        return false;
}

bool igsagent_service_has_reply(igsagent_t *agent, const char *service_name, const char *reply_name){
    assert(agent);
    assert(service_name);
    assert(reply_name);
    igs_service_t *s = NULL;
    HASH_FIND_STR (agent->definition->services_table, service_name, s);
    if (!s) {
        igsagent_debug (agent, "could not find service with name %s", service_name);
        return false;
    }
    igs_service_t *r = NULL;
    HASH_FIND_STR(s->replies, reply_name, r);
    if (r)
        return true;
    else
        return false;
}

char ** igsagent_service_reply_names(igsagent_t *agent, const char *service_name, size_t *service_replies_nbr){
    assert(agent);
    assert(service_name);
    assert(service_replies_nbr);
    igs_service_t *s = NULL;
    HASH_FIND_STR (agent->definition->services_table, service_name, s);
    if (!s) {
        igsagent_debug (agent, "could not find service with name %s", service_name);
        return NULL;
    }
    igs_service_t *r, *tmp_r;
    *service_replies_nbr = HASH_COUNT(s->replies);
    if (!(*service_replies_nbr))
        return NULL;
    char ** names = (char**)calloc(*service_replies_nbr, sizeof(char*));
    size_t index = 0;
    HASH_ITER(hh, s->replies, r, tmp_r){
        names[index] = strdup(r->name);
        index++;
    }
    return names;
}

igs_service_arg_t * igsagent_service_reply_args_first(igsagent_t *agent, const char *service_name, const char *reply_name){
    assert(agent);
    assert(service_name);
    assert(reply_name);
    if (!agent->definition) {
        igsagent_error (agent, "agent definition is NULL");
        return NULL;
    }
    igs_service_t *s = NULL;
    HASH_FIND_STR (agent->definition->services_table, service_name, s);
    if (!s) {
        igsagent_debug (agent, "could not find service with name %s", service_name);
        return NULL;
    }
    igs_service_t *r = NULL;
    HASH_FIND_STR(s->replies, reply_name, r);
    if (r)
        return r->arguments;
    else {
        igsagent_debug (agent, "could not find service with name %s and reply %s", service_name, reply_name);
        return NULL;
    }
}

size_t igsagent_service_reply_args_count(igsagent_t *agent, const char *service_name, const char *reply_name){
    assert(agent);
    assert(service_name);
    assert(reply_name);
    if (!agent->definition) {
        igsagent_error (agent, "agent definition is NULL");
        return 0;
    }
    igs_service_t *s = NULL;
    HASH_FIND_STR (agent->definition->services_table, service_name, s);
    if (!s) {
        igsagent_debug (agent, "could not find service with name %s", service_name);
        return 0;
    }
    igs_service_t *r = NULL;
    HASH_FIND_STR(s->replies, reply_name, r);
    if (r){
        size_t res = 0;
        igs_service_arg_t *arg;
        LL_COUNT(r->arguments, arg, res);
        return res;
    }else {
        igsagent_debug (agent, "could not find service with name %s and reply %s", service_name, reply_name);
        return 0;
    }
}

bool igsagent_service_reply_arg_exists(igsagent_t *agent, const char *service_name, const char *reply_name, const char *arg_name){
    assert(agent);
    assert(service_name);
    assert(reply_name);
    assert(arg_name);
    if (!agent->definition) {
        igsagent_error (agent, "agent definition is NULL");
        return false;
    }
    igs_service_t *s = NULL;
    HASH_FIND_STR (agent->definition->services_table, service_name, s);
    if (!s) {
        igsagent_debug (agent, "could not find service with name %s", service_name);
        return false;
    }
    igs_service_t *r = NULL;
    HASH_FIND_STR(s->replies, reply_name, r);
    if (r){
        igs_service_arg_t *a = NULL;
        LL_FOREACH (r->arguments, a) {
            if (streq (a->name, arg_name))
                return true;
        }
        return false;
    }else {
        igsagent_debug (agent, "could not find service with name %s and reply named %s", service_name, reply_name);
        return false;
    }
}
