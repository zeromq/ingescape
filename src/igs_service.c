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
#include <stdio.h>
#include <zyre.h>

////////////////////////////////////////////////////////////////////////
#pragma mark INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////

void s_service_free_service_arguments (igs_service_arg_t **args)
{
    assert(args);
    igs_service_arg_t *arg = *args;
    igs_service_arg_t *previous = NULL;
    while (arg) {
        if (arg->name){
            free (arg->name);
            arg->name = NULL;
        }
        if (arg->type == IGS_DATA_T && arg->data)
            free (arg->data);
        else if (arg->type == IGS_STRING_T && arg->c)
            free (arg->c);
        previous = arg;
        arg = arg->next;
        free (previous);
    }
    *args = NULL;
}

igs_result_t s_service_copy_arguments (igs_service_arg_t **source,
                                       zlist_t *destination)
{
    assert (destination);
    if(!source)
        return IGS_SUCCESS;
    assert(*source);
    //NB: nb of arguments SHALL be the same between source and destination
    igs_service_arg_t *current_source = *source;
    igs_service_arg_t *current_destination = zlist_first(destination);
    igs_service_arg_t *previous_destination = NULL;
    while (current_source && current_destination) {
        assert(current_source);
        size_t size = current_source->size;
        switch (current_destination->type) {
            case IGS_BOOL_T:
                memcpy (&(current_destination->b), &(current_source->b), sizeof (bool));
                break;
            case IGS_INTEGER_T:
                memcpy (&(current_destination->i), &(current_source->i), sizeof (int));
                break;
            case IGS_DOUBLE_T:
                memcpy (&(current_destination->d), &(current_source->d), sizeof (double));
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
        previous_destination = current_destination;
        current_destination = zlist_next(destination);
        previous_destination->next = current_destination;
    }
    return IGS_SUCCESS;
}

void s_service_log_sent_service (igsagent_t *agent,
                                 const char *target_agent_name,
                                 const char *target_agentuuid,
                                 const char *service_name,
                                 igs_service_arg_t *list,
                                 int64_t timestamp)
{
    assert(agent);
    assert(target_agent_name);
    assert(target_agentuuid);
    assert(service_name);
    char service_log[IGS_MAX_LOG_LENGTH] = "";
    char *service_log_cursor = service_log;
    service_log_cursor += snprintf (service_log, IGS_MAX_LOG_LENGTH, "send service %s(%s).%s", target_agent_name, target_agentuuid, service_name);
    igs_service_arg_t *current_arg = list;
    while (current_arg) {
        if (service_log_cursor - service_log >= IGS_MAX_LOG_LENGTH)
            break;
        switch (current_arg->type) {
            case IGS_BOOL_T:
                service_log_cursor += snprintf (service_log_cursor, IGS_MAX_LOG_LENGTH - (service_log_cursor - service_log), " %d", current_arg->b);
                break;
            case IGS_INTEGER_T:
                service_log_cursor += snprintf (service_log_cursor, IGS_MAX_LOG_LENGTH - (service_log_cursor - service_log), " %d", current_arg->i);
                break;
            case IGS_DOUBLE_T:
                service_log_cursor += snprintf (service_log_cursor, IGS_MAX_LOG_LENGTH - (service_log_cursor - service_log), " %f", current_arg->d);
                break;
            case IGS_STRING_T:
                service_log_cursor += snprintf (service_log_cursor, IGS_MAX_LOG_LENGTH - (service_log_cursor - service_log), " %s", current_arg->c);
                break;
            case IGS_DATA_T: {
                zchunk_t *chunk = zchunk_new (current_arg->data, current_arg->size);
                char *hex_str = zchunk_strhex (chunk);
                if (hex_str) {
                    service_log_cursor += snprintf (service_log_cursor, IGS_MAX_LOG_LENGTH - (service_log_cursor - service_log), " %s", hex_str);
                    free (hex_str);
                }else
                    service_log_cursor += snprintf (service_log_cursor, IGS_MAX_LOG_LENGTH - (service_log_cursor - service_log), " 00");
                zchunk_destroy (&chunk);
            } break;
            default:
                break;
        }
        current_arg = current_arg->next;
    }
    
    if (service_log_cursor - service_log < IGS_MAX_LOG_LENGTH && timestamp != INT64_MIN)
        snprintf (service_log_cursor, IGS_MAX_LOG_LENGTH - (service_log_cursor - service_log), " with timestamp %lld", timestamp);
    igsagent_debug (agent, "%s", service_log);
}

////////////////////////////////////////////////////////////////////////
#pragma mark PRIVATE API
////////////////////////////////////////////////////////////////////////
void service_free_service (igs_service_t **s)
{
    assert(s);
    assert(*s);
    if ((*s)->name){
        free ((*s)->name);
        (*s)->name = NULL;
    }
    if ((*s)->description)
        free ((*s)->description);
    s_service_free_service_arguments (&(*s)->arguments);
    zlist_destroy(&(*s)->replies_names_ordered);
    igs_service_t *r = zhashx_first((*s)->replies);
    while (r) {
        service_free_service(&r);
        r = zhashx_next((*s)->replies);
    }
    zhashx_destroy(&(*s)->replies);
    free (*s);
    *s = NULL;
}

igs_result_t service_make_values_to_arguments_from_message (igs_service_arg_t **args,
                                                            igs_service_t *service,
                                                            zmsg_t *msg){
    assert(args && !*args);
    assert(service);
    assert(msg);
    
    igs_service_arg_t *previous = NULL;
    igs_service_arg_t *current = NULL;
    igs_service_arg_t *current_from_service = service->arguments;
    while (current_from_service){
        current = (igs_service_arg_t *) zmalloc (sizeof(igs_service_arg_t));
        if (!*args)
            *args = current;
        if (previous)
            previous->next = current;
        current->name = strdup(current_from_service->name);
        current->type = current_from_service->type;
        zframe_t *f = zmsg_pop (msg);
        if (f){
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
        }else{
            igs_error("passed message misses elements to match with the expected args for service %s (stopped at %s)",
                      service->name, current->name);
            return IGS_FAILURE;
        }
        previous = current;
        current_from_service = current_from_service->next;
    }
    return IGS_SUCCESS;
}

void service_free_values_in_arguments (zlist_t *args)
{
    assert(args);
    igs_service_arg_t *arg = zlist_first(args);
    while (arg) {
        if (arg->type == IGS_DATA_T && arg->data)
            free (arg->data);
        else
            if (arg->type == IGS_STRING_T && arg->data)
                free (arg->c);
        arg->data = NULL;
        arg->size = 0;
        arg = zlist_next(args);
    }
}

void service_log_received_service (igsagent_t *agent,
                                   const char *caller_agent_name,
                                   const char *caller_agentuuid,
                                   const char *service_name,
                                   igs_service_arg_t *args,
                                   int64_t timestamp)
{
    assert(agent);
    assert(caller_agent_name);
    assert(caller_agentuuid);
    assert(service_name);
    char service_log[IGS_MAX_LOG_LENGTH] = "";
    char *service_log_cursor = service_log;
    service_log_cursor += snprintf (service_log, IGS_MAX_LOG_LENGTH, "received service %s from %s(%s) ",
                                    service_name, caller_agent_name, caller_agentuuid);
    igs_service_arg_t *current_arg = args;
    while (current_arg) {
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
        current_arg = current_arg->next;
    }
    if (service_log_cursor - service_log < IGS_MAX_LOG_LENGTH && timestamp != INT64_MIN)
        snprintf (service_log_cursor, IGS_MAX_LOG_LENGTH - (service_log_cursor - service_log)," with timestamp %lld", timestamp);
    igsagent_debug (agent, "%s", service_log);
}

////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////
void igs_service_args_destroy (igs_service_arg_t **list)
{
    assert(list);
    if(!*list)
        return;
    s_service_free_service_arguments(list);
}

igs_service_arg_t *igs_service_args_clone (igs_service_arg_t *list)
{
    assert (list);
    igs_service_arg_t *first = NULL, *current = NULL;
    igs_service_arg_t *arg = list;
    while (arg) {
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
        if (!current)
            current = first = new;
        else{
            current->next = new;
            current = new;
        }
        arg = arg->next;
    }
    return first;
}

void igs_service_args_add_int (igs_service_arg_t **list, int value)
{
    assert(list);
    igs_service_arg_t *new = (igs_service_arg_t *) zmalloc (sizeof (igs_service_arg_t));
    new->type = IGS_INTEGER_T;
    new->i = value;
    new->size = sizeof (int);
    if (!*list)
        *list = new;
    else{
        igs_service_arg_t *current = *list;
        while (current->next)
            current = current->next;
        current->next = new;
    }
}

void igs_service_args_add_bool (igs_service_arg_t **list, bool value)
{
    assert(list);
    igs_service_arg_t *new = (igs_service_arg_t *) zmalloc (sizeof (igs_service_arg_t));
    new->type = IGS_BOOL_T;
    new->b = value;
    new->size = sizeof (bool);
    if (!*list)
        *list = new;
    else{
        igs_service_arg_t *current = *list;
        while (current->next)
            current = current->next;
        current->next = new;
    }
}

void igs_service_args_add_double (igs_service_arg_t **list, double value)
{
    assert(list);
    igs_service_arg_t *new = (igs_service_arg_t *) zmalloc (sizeof (igs_service_arg_t));
    new->type = IGS_DOUBLE_T;
    new->d = value;
    new->size = sizeof (double);
    if (!*list)
        *list = new;
    else{
        igs_service_arg_t *current = *list;
        while (current->next)
            current = current->next;
        current->next = new;
    }
}

void igs_service_args_add_string (igs_service_arg_t **list, const char *value)
{
    assert(list);
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
    if (!*list)
        *list = new;
    else{
        igs_service_arg_t *current = *list;
        while (current->next)
            current = current->next;
        current->next = new;
    }
}

void igs_service_args_add_data (igs_service_arg_t **list,
                                void *value,
                                size_t size)
{
    assert(list);
    igs_service_arg_t *new = (igs_service_arg_t *) zmalloc (sizeof (igs_service_arg_t));
    new->type = IGS_DATA_T;
    new->data = (void *) zmalloc (size);
    memcpy (new->data, value, size);
    new->size = size;
    if (!*list)
        *list = new;
    else{
        igs_service_arg_t *current = *list;
        while (current->next)
            current = current->next;
        current->next = new;
    }
}

igs_result_t igsagent_service_init (igsagent_t *agent,
                                    const char *name,
                                    igsagent_service_fn cb,
                                    void *my_data)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name && strlen (name) > 0);
    assert (cb);
    assert(agent->definition);
    
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_service_t *s = zhashx_lookup(agent->definition->services_table, name);
    if (s && s->service_cb) {
        igsagent_error (agent, "service with name %s exists and already has a callback", name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }else if (!s){
        // service is completely new: allocate it
        s = (igs_service_t *) zmalloc (sizeof (igs_service_t));
        if (strnlen (name, IGS_MAX_STRING_MSG_LENGTH) == IGS_MAX_STRING_MSG_LENGTH) {
            s->name = s_strndup (name, IGS_MAX_STRING_MSG_LENGTH);
            igsagent_warn (agent, "service name has been shortened to %s", s->name);
        } else
            s->name = s_strndup (name, IGS_MAX_STRING_MSG_LENGTH);
        s->replies_names_ordered = zlist_new();
        zlist_comparefn(s->replies_names_ordered, (zlist_compare_fn*) strcmp);
        zlist_autofree(s->replies_names_ordered);
        s->replies = zhashx_new();
        zlist_append(agent->definition->services_names_ordered, strdup(s->name));
        zhashx_insert(agent->definition->services_table, s->name, s);
        definition_update_json (agent->definition);
        agent->network_need_to_send_definition_update = true;
    }
    s->service_cb = cb;
    s->cb_data = my_data;
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return IGS_SUCCESS;
}

igs_result_t igsagent_service_remove (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    assert (agent->definition);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_service_t *s = zhashx_lookup(agent->definition->services_table, name);
    if (!s) {
        igsagent_error (agent, "service with name '%s' does not exist", name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    zlist_remove(agent->definition->services_names_ordered, (char*)name);
    zhashx_delete(agent->definition->services_table, name);
    service_free_service (&s);
    definition_update_json (agent->definition);
    agent->network_need_to_send_definition_update = true;
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return IGS_SUCCESS;
}

igs_result_t igsagent_service_arg_add (igsagent_t *agent,
                                       const char *service_name,
                                       const char *arg_name,
                                       igs_io_value_type_t type)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (service_name);
    assert (arg_name && strlen (arg_name) > 0);
    assert (agent->definition);
    if (type == IGS_IMPULSION_T) {
        igsagent_error (agent, "impulsion type is not allowed as a service argument");
        return IGS_FAILURE;
    }
    if (type == IGS_UNKNOWN_T) {
        igsagent_error (agent, "unknown type is not allowed as a service argument");
        return IGS_FAILURE;
    }
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_service_t *s = zhashx_lookup(agent->definition->services_table, service_name);
    if (!s) {
        igsagent_error (agent, "service with name %s does not exist", service_name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    igs_service_arg_t *a = (igs_service_arg_t *) zmalloc (sizeof (igs_service_arg_t));
    if (strnlen (arg_name, IGS_MAX_STRING_MSG_LENGTH) == IGS_MAX_STRING_MSG_LENGTH) {
        a->name = s_strndup (arg_name, IGS_MAX_STRING_MSG_LENGTH);
        igsagent_warn (agent, "service argument name has been shortened to %s", a->name);
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
    igs_service_arg_t *previous_arg = s->arguments;
    while (previous_arg && previous_arg->next) {
        previous_arg = previous_arg->next;
    }
    if (previous_arg)
        previous_arg->next = a;
    else
        s->arguments = a;
    definition_update_json (agent->definition);
    agent->network_need_to_send_definition_update = true;
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return IGS_SUCCESS;
}

igs_result_t igsagent_service_arg_remove (igsagent_t *agent,
                                          const char *service_name,
                                          const char *arg_name)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (service_name);
    assert (arg_name);
    assert (agent->definition);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_service_t *s = zhashx_lookup(agent->definition->services_table, service_name);
    if (!s) {
        igsagent_error (agent, "service with name %s does not exist", service_name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    bool found = false;
    igs_service_arg_t *arg = s->arguments;
    igs_service_arg_t *previous_arg = NULL;
    while (arg) {
        if (streq (arg_name, arg->name)) {
            if (previous_arg)
                previous_arg->next = arg->next;
            else
                s->arguments = arg->next;
            free (arg->name);
            if (arg->type == IGS_DATA_T && arg->data)
                free (arg->data);
            else if (arg->type == IGS_STRING_T && arg->data)
                free (arg->c);
            free (arg);
            found = true;
            definition_update_json (agent->definition);
            agent->network_need_to_send_definition_update = true;
            break;
        }
        previous_arg = arg;
        arg = arg->next;
    }
    if (!found)
        igsagent_debug (agent, "no argument named %s for service %s", arg_name, service_name);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return IGS_SUCCESS;
}

igs_result_t igsagent_service_reply_add(igsagent_t *agent, const char *service_name, const char *reply_name){
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (service_name);
    assert (reply_name);
    assert (agent->definition);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_service_t *s = zhashx_lookup(agent->definition->services_table, service_name);
    if (!s) {
        igsagent_error (agent, "service with name %s does not exist", service_name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    igs_service_t *r = zhashx_lookup(s->replies, reply_name);
    if (r) {
        igsagent_error (agent, "service reply with name %s already exists", reply_name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    r = (igs_service_t *) zmalloc (sizeof (igs_service_t));
    if (strnlen (reply_name, IGS_MAX_STRING_MSG_LENGTH) == IGS_MAX_STRING_MSG_LENGTH) {
        r->name = s_strndup (reply_name, IGS_MAX_STRING_MSG_LENGTH);
        igsagent_warn (agent, "service name has been shortened to %s", r->name);
    } else
        r->name = s_strndup (reply_name, IGS_MAX_STRING_MSG_LENGTH);
    r->replies_names_ordered = zlist_new();
    zlist_comparefn(r->replies_names_ordered, (zlist_compare_fn*) strcmp);
    zlist_autofree(r->replies_names_ordered);
    r->replies = zhashx_new();
    zlist_append(s->replies_names_ordered, strdup(r->name));
    zhashx_insert(s->replies, r->name, r);
    definition_update_json (agent->definition);
    agent->network_need_to_send_definition_update = true;
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return IGS_SUCCESS;
}

igs_result_t igsagent_service_reply_remove(igsagent_t *agent, const char *service_name, const char *reply_name){
    assert(agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert(service_name);
    assert(reply_name);
    assert(agent->definition);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_service_t *s = zhashx_lookup(agent->definition->services_table, service_name);
    if (!s) {
        igsagent_error (agent, "service with name %s does not exist", service_name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    igs_service_t *r = zhashx_lookup(s->replies, reply_name);
    igs_result_t res = IGS_SUCCESS;
    if (r){
        zlist_remove(s->replies_names_ordered, (char*)reply_name);
        zhashx_delete(s->replies, reply_name);
        service_free_service (&r);
        definition_update_json (agent->definition);
        agent->network_need_to_send_definition_update = true;
    }else{
        igsagent_error (agent, "service with name %s has no reply named %s", service_name, reply_name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

igs_result_t igsagent_service_reply_arg_add(igsagent_t *agent, const char *service_name, const char *reply_name,
                                            const char *arg_name, igs_io_value_type_t type){
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (service_name);
    assert (reply_name);
    assert (arg_name);
    assert (agent->definition);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_service_t *s = zhashx_lookup(agent->definition->services_table, service_name);
    if (!s) {
        igsagent_error (agent, "service with name %s does not exist", service_name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    igs_service_t *r = zhashx_lookup(s->replies, reply_name);
    if (!r){
        igsagent_error (agent, "service with name %s  has no reply named %s", service_name, reply_name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    if (type == IGS_IMPULSION_T) {
        igsagent_error (agent, "impulsion type is not allowed as a service argument");
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    if (type == IGS_UNKNOWN_T) {
        igsagent_error (agent, "unknown type is not allowed as a service argument");
        model_read_write_unlock(__FUNCTION__, __LINE__);
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
    igs_service_arg_t *previous_arg = r->arguments;
    while (previous_arg && previous_arg->next) {
        previous_arg = previous_arg->next;
    }
    if (previous_arg)
        previous_arg->next = a;
    else
        r->arguments = a;
    definition_update_json (agent->definition);
    agent->network_need_to_send_definition_update = true;
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return IGS_SUCCESS;
}

igs_result_t igsagent_service_reply_arg_remove(igsagent_t *agent, const char *service_name, 
                                               const char *reply_name, const char *arg_name){
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (service_name);
    assert (reply_name);
    assert (arg_name);
    assert (agent->definition);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_service_t *s = zhashx_lookup(agent->definition->services_table, service_name);
    if (!s) {
        igsagent_error (agent, "service with name %s does not exist", service_name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    igs_service_t *r = zhashx_lookup(s->replies, reply_name);
    if (!r){
        igsagent_error (agent, "service with name %s  has no reply named %s", service_name, reply_name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    bool found = false;
    igs_service_arg_t *arg = r->arguments;
    igs_service_arg_t *previous_arg = NULL;
    while (arg) {
        if (streq (arg_name, arg->name)) {
            if (previous_arg)
                previous_arg->next = arg->next;
            else
                r->arguments = arg->next;
            free (arg->name);
            if (arg->type == IGS_DATA_T && arg->data)
                free (arg->data);
            else
                if (arg->type == IGS_STRING_T && arg->data)
                    free (arg->c);
            free (arg);
            found = true;
            definition_update_json (agent->definition);
            agent->network_need_to_send_definition_update = true;
            break;
        }
        previous_arg = arg;
        arg = arg->next;
    }
    if (!found) {
        igsagent_debug (agent, "no argument named %s for reply %s in service %s", arg_name, reply_name, service_name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return IGS_SUCCESS;
}

igs_result_t igsagent_service_call (igsagent_t *agent,
                                    const char *agent_name_or_uuid,
                                    const char *service_name,
                                    igs_service_arg_t **list,
                                    const char *token)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (agent_name_or_uuid);
    assert (service_name);
    assert ((list == NULL) || (*list));
    
    model_read_write_lock(__FUNCTION__, __LINE__);
    bool found = false;
    
    int64_t current_microseconds = INT64_MIN;
    if (agent->rt_timestamps_enabled){
        if (agent->context && agent->context->rt_current_microseconds != INT64_MIN)
            current_microseconds = agent->context->rt_current_microseconds;
        else
            current_microseconds = zclock_usecs();
    }
    
    // 1- iteration on remote agents
    if (agent->context && agent->context->node) {
        igs_remote_agent_t *remote_agent = zhashx_first(agent->context->remote_agents);
        while (remote_agent) {
            if ((remote_agent->definition && streq (remote_agent->definition->name, agent_name_or_uuid))
                || streq (remote_agent->uuid, agent_name_or_uuid)) {
                // we found a matching agent
                found = true;
                /*NB: We removed verifications on the service on sender side to enable
                 proper proxy implementation (local proxy does not implement
                 services but relays them to remote clients and virtual agents)*/
                zmsg_t *msg = zmsg_new ();
                if (remote_agent->peer->protocol
                    && (streq (remote_agent->peer->protocol, "v2")
                        || streq (remote_agent->peer->protocol, "v3"))) {
                    igs_warn ("Remote agent %s(%s) uses an older version of Ingescape with deprecated protocol. Please upgrade this agent.", remote_agent->definition->name, remote_agent->uuid);
                    zmsg_addstr (msg, CALL_SERVICE_MSG_DEPRECATED);
                } else
                    zmsg_addstr (msg, CALL_SERVICE_MSG);
                
                zmsg_addstr (msg, agent->uuid);
                zmsg_addstr (msg, remote_agent->uuid);
                zmsg_addstr (msg, service_name);
                if (token)
                    zmsg_addstr (msg, token);
                else
                    zmsg_addstr (msg, "");
                if (list && *list) {
                    igs_service_arg_t *arg = *list;
                    while (arg) {
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
                        arg = arg->next;
                    }
                }
                if (agent->rt_timestamps_enabled)
                    zmsg_addmem(msg, &current_microseconds, sizeof(int64_t));
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
                                                service_name, list ? *list : NULL, current_microseconds);
                else
                    igsagent_debug (agent, "calling %s(%s).%s on the network",
                                    remote_agent->definition->name,
                                    remote_agent->uuid, service_name);
            }
            remote_agent = zhashx_next(agent->context->remote_agents);
        }
    } else if (agent->context && !agent->context->node)
        igsagent_debug (agent, "peer is not started, service was not called on the network");
    
    // 2- iteration on local agents
    //NB: checking agent, service and remote UUIDs/names in the conditions below is a way to ensure they have
    //not been destroyed while we are iterating.
    if (agent->context && agent->uuid && !agent->is_virtual) {
        zlistx_t *local_agents = zhashx_values(agent->context->agents);
        igsagent_t *local_agent = zlistx_first(local_agents);
        while (local_agent && local_agent->uuid) {
            if((streq (local_agent->definition->name, agent_name_or_uuid) || streq (local_agent->uuid, agent_name_or_uuid))) {
                // we found a matching agent
                assert(local_agent->definition && local_agent->definition->services_table);
                found = true;
                igs_service_t *service = zhashx_lookup(local_agent->definition->services_table, service_name);
                if (service && service->name){
                    size_t nb_arguments = 0;
                    if (list && *list){
                        igs_service_arg_t *arg = *list;
                        while (arg) {
                            nb_arguments++;
                            arg = arg->next;
                        }
                    }
                    size_t defined_nb_arguments = 0;
                    igs_service_arg_t *count_arg = service->arguments;
                    while (count_arg) {
                        defined_nb_arguments++;
                        count_arg = count_arg->next;
                    }
                    if (nb_arguments != defined_nb_arguments) {
                        igsagent_error (agent, "passed number of arguments is not correct (received: %zu / expected: %zu) : service will not be called", nb_arguments, defined_nb_arguments);
                        continue;
                    }else {
                        agent->rt_current_timestamp_microseconds = current_microseconds;
                        agent->rt_current_timestamp_microseconds = INT64_MIN;
                        if (core_context->enable_service_logging)
                            service_log_received_service (local_agent, agent->definition->name, agent->uuid, service_name,
                                                          (list)?*list:NULL, current_microseconds);
                        s_lock_zyre_peer (__FUNCTION__, __LINE__);
                        if (core_context->node)
                            zyre_shouts (agent->context->node, agent->igs_channel, "SERVICE %s(%s) called %s.%s(%s)",
                                         agent->definition->name, agent->uuid, local_agent->definition->name, service_name, local_agent->uuid);
                        s_unlock_zyre_peer (__FUNCTION__, __LINE__);
                        
                        if (core_context->enable_service_logging)
                            s_service_log_sent_service (agent, local_agent->definition->name, local_agent->uuid,
                                                        service_name, *list, current_microseconds);
                        else
                            igsagent_debug (agent, "calling %s.%s(%s) locally", local_agent->definition->name, service_name, local_agent->uuid);
                        model_read_write_unlock(__FUNCTION__, __LINE__);
                        if (local_agent->uuid && agent->uuid && service->service_cb)
                            (service->service_cb) (local_agent, agent->definition->name, agent->uuid, service_name,
                                                   *list, nb_arguments, token, service->cb_data);
                        model_read_write_lock(__FUNCTION__, __LINE__);
                    }
                }else
                    igsagent_error (agent, "could not find service named %s for %s (%s) : service will not be called",
                                    service_name, local_agent->definition->name,local_agent->uuid);
            }
            local_agent = zlistx_next(local_agents);
        }
        zlistx_destroy(&local_agents);
    }
    
    if (list && *list)
        igs_service_args_destroy(list);
    
    igs_result_t res = IGS_SUCCESS;
    if (!agent->context){
        igsagent_debug (agent, "agent is not activated, service was not called");
        res = IGS_FAILURE;
    } else if (!found) {
        igsagent_debug (agent, "could not find an agent with name or UUID %s. Agent is missing or deactivated.", agent_name_or_uuid);
        res = IGS_FAILURE;
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

size_t igsagent_service_count (igsagent_t *agent)
{
    assert(agent);
    if (!agent->uuid)
        return 0;
    assert (agent->definition);
    model_read_write_lock(__FUNCTION__, __LINE__);
    size_t res = zhashx_size(agent->definition->services_table);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

bool igsagent_service_exists (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return false;
    assert (agent->definition);
    assert(name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_service_t *s = zhashx_lookup(agent->definition->services_table, name);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return s;
}

char **igsagent_service_list (igsagent_t *agent, size_t *nb_of_elements)
{
    assert (agent);
    if (!agent->uuid){
        *nb_of_elements = 0;
        return NULL;
    }
    assert (agent->definition);
    model_read_write_lock(__FUNCTION__, __LINE__);
    size_t nb = zhashx_size(agent->definition->services_table);
    if (nb == 0) {
        *nb_of_elements = 0;
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return NULL;
    }
    *nb_of_elements = nb;
    char **res = (char **) zmalloc (nb * sizeof (char *));
    size_t i = 0;
    const char* service_name = zlist_first(agent->definition->services_names_ordered);
    while (service_name) {
        res[i++] = strdup (service_name);
        service_name = zlist_next(agent->definition->services_names_ordered);
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
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
    if (!agent->uuid)
        return NULL;
    assert(service_name);
    assert(agent->definition);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_service_t *s = zhashx_lookup(agent->definition->services_table, service_name);
    if (!s) {
        igsagent_debug (agent, "could not find service with name %s", service_name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return NULL;
    }
    igs_service_arg_t *res = s->arguments;
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

size_t igsagent_service_args_count (igsagent_t *agent,
                                    const char *service_name)
{
    assert(agent);
    if (!agent->uuid)
        return 0;
    assert(service_name);
    assert(agent->definition);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_service_t *s = zhashx_lookup(agent->definition->services_table, service_name);
    if (!s) {
        igsagent_debug (agent, "could not find service with name %s", service_name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return 0;
    }
    size_t nb = 0;
    igs_service_arg_t *count_arg = s->arguments;
    while (count_arg) {
        nb++;
        count_arg = count_arg->next;
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return nb;
}

bool igsagent_service_arg_exists (igsagent_t *agent,
                                  const char *service_name,
                                  const char *arg_name)
{
    assert(agent);
    if (!agent->uuid)
        return false;
    assert(service_name);
    assert(arg_name);
    assert(agent->definition);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_service_t *s = zhashx_lookup(agent->definition->services_table, service_name);
    if (!s){
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return false;
    }
    igs_service_arg_t *a = s->arguments;
    while (a) {
        if (streq (a->name, arg_name)){
            model_read_write_unlock(__FUNCTION__, __LINE__);
            return true;
        }
        a = a->next;
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return false;
}

bool igsagent_service_has_replies(igsagent_t *agent, const char *service_name){
    assert(agent);
    if (!agent->uuid)
        return false;
    assert(service_name);
    assert(agent->definition);
    model_read_write_lock(__FUNCTION__, __LINE__);
    bool res = false;
    igs_service_t *s = zhashx_lookup(agent->definition->services_table, service_name);
    if (!s) {
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return false;
    }
    if (zhashx_size(s->replies))
        res = true;
    else
        res = false;
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

bool igsagent_service_has_reply(igsagent_t *agent, const char *service_name, const char *reply_name){
    assert(agent);
    if (!agent->uuid)
        return false;
    assert(service_name);
    assert(reply_name);
    assert(agent->definition);
    model_read_write_lock(__FUNCTION__, __LINE__);
    bool res = false;
    igs_service_t *s = zhashx_lookup(agent->definition->services_table, service_name);
    if (!s){
        model_read_write_unlock(__FUNCTION__, __LINE__);
    return false;
    }
    igs_service_t *r = zhashx_lookup(s->replies, reply_name);
    if (r)
        res = true;
    else
        res = false;
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

char ** igsagent_service_reply_names(igsagent_t *agent, const char *service_name, size_t *service_replies_nbr){
    assert(agent);
    if (!agent->uuid){
        *service_replies_nbr = 0;
        return NULL;
    }
    assert(service_name);
    assert(service_replies_nbr);
    assert(agent->definition);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_service_t *s = zhashx_lookup(agent->definition->services_table, service_name);
    if (!s) {
        igsagent_debug (agent, "could not find service with name %s", service_name);
        *service_replies_nbr = 0;
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return NULL;
    }
    *service_replies_nbr = zhashx_size(s->replies);
    if (*service_replies_nbr == 0){
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return NULL;
    }
    char ** names = (char**)calloc(*service_replies_nbr, sizeof(char*));
    size_t index = 0;
    const char* r_name = zlist_first(s->replies_names_ordered);
    while (r_name) {
        names[index] = strdup(r_name);
        index++;
        r_name = zlist_next(s->replies_names_ordered);
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return names;
}

igs_service_arg_t * igsagent_service_reply_args_first(igsagent_t *agent, const char *service_name, const char *reply_name){
    assert(agent);
    if (!agent->uuid)
        return NULL;
    assert(service_name);
    assert(reply_name);
    assert(agent->definition);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_service_t *s = zhashx_lookup(agent->definition->services_table, service_name);
    if (!s) {
        igsagent_debug (agent, "could not find service with name %s", service_name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return NULL;
    }
    igs_service_t *r = zhashx_lookup(s->replies, reply_name);
    if (r){
        igs_service_arg_t *res = r->arguments;
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return res;
    }else {
        igsagent_debug (agent, "could not find service with name %s and reply %s", service_name, reply_name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return NULL;
    }
}

size_t igsagent_service_reply_args_count(igsagent_t *agent, const char *service_name, const char *reply_name){
    assert(agent);
    if (!agent->uuid)
        return 0;
    assert(service_name);
    assert(reply_name);
    assert(agent->definition);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_service_t *s = zhashx_lookup(agent->definition->services_table, service_name);
    if (!s) {
        igsagent_debug (agent, "could not find service with name %s", service_name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return 0;
    }
    igs_service_t *r = zhashx_lookup(s->replies, reply_name);
    if (r){
        size_t res = 0;
        igs_service_arg_t *arg_count = r->arguments;
        while (arg_count) {
            res++;
            arg_count = arg_count->next;
        }
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return res;
    }else {
        igsagent_debug (agent, "could not find service with name %s and reply %s", service_name, reply_name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return 0;
    }
}

bool igsagent_service_reply_arg_exists(igsagent_t *agent, const char *service_name, const char *reply_name, const char *arg_name){
    assert(agent);
    if (!agent->uuid)
        return false;
    assert(service_name);
    assert(reply_name);
    assert(arg_name);
    assert(agent->definition);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_service_t *s = zhashx_lookup(agent->definition->services_table, service_name);
    if (!s) {
        igsagent_debug (agent, "could not find service with name %s", service_name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return 0;
    }
    igs_service_t *r = zhashx_lookup(s->replies, reply_name);
    if (r){
        igs_service_arg_t *a = r->arguments;
        while (a) {
            if (streq (a->name, arg_name)){
                model_read_write_unlock(__FUNCTION__, __LINE__);
                return true;
            }
            a = a->next;
        }
    }else
        igsagent_debug (agent, "could not find service with name %s and reply named %s", service_name, reply_name);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return false;
}
