/*  =========================================================================
    split - parallel computation

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
#pragma mark INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////
void s_split_trigger_send_message_to_worker (igs_core_context_t *context,
                                             char *agent_uuid,
                                             const igs_io_t *output)
{
    assert(context);
    assert(agent_uuid);
    assert(output);

    zlist_t *splitters = zlist_dup(context->splitters);
    igs_splitter_t *splitter = zlist_first(splitters);
    while (splitter) {
        if(streq(splitter->agent_uuid, agent_uuid) && streq(splitter->output_name, output->name)){
            igs_worker_t *max_credit_worker = NULL;
            zlist_t *workers = zlist_dup(splitter->workers);
            igs_worker_t *worker = max_credit_worker = zlist_first(workers);
            while (worker) {
                if(max_credit_worker->credit < worker->credit //take worker with more credits
                   || (max_credit_worker->credit == worker->credit //take worker with same credits but less uses
                       && worker->uses < max_credit_worker->uses))
                    max_credit_worker = worker;
                worker = zlist_next(workers);
            }
            zlist_destroy(&workers);
            if(max_credit_worker && max_credit_worker->credit > 0){
                igs_queued_work_t *work = zlist_first(splitter->queued_works);
                if(work){
                    char *max_credit_worker_uuid = strdup(max_credit_worker->agent_uuid);
                    zmsg_t *readyMessage = zmsg_new();
                    zmsg_addstr(readyMessage, SPLITTER_WORK_MSG);
                    zmsg_addstr(readyMessage, splitter->agent_uuid );
                    zmsg_addstr(readyMessage, max_credit_worker->input_name);
                    zmsg_addstr(readyMessage, output->name);
                    zmsg_addstrf(readyMessage, "%d", output->value_type);
                    switch (output->value_type) {
                        case IGS_INTEGER_T:
                            zmsg_addmem(readyMessage, &(work->value.i), sizeof(int));
                            break;
                        case IGS_DOUBLE_T:
                            zmsg_addmem(readyMessage, &(work->value.d), sizeof(double));
                            break;
                        case IGS_BOOL_T:
                            zmsg_addmem(readyMessage, &(work->value.b), sizeof(bool));
                            break;
                        case IGS_STRING_T:
                            zmsg_addstr(readyMessage, work->value.s);
                            break;
                        case IGS_IMPULSION_T:
                            zmsg_addmem(readyMessage, NULL, 0);
                            break;
                        case IGS_DATA_T:{
                            zframe_t *frame = zframe_new (work->value.data, work->value_size);
                            zmsg_append(readyMessage, &frame);}
                            break;
                        default:
                            break;
                    }
                    
                    zlist_remove(splitter->queued_works, work);
                    if(work->value_type == IGS_STRING_T)
                        free(work->value.s);
                    else if (work->value_type == IGS_DATA_T)
                        free(work->value.data);
                    free(work);
                    max_credit_worker->uses++;
                    max_credit_worker->credit--;
                    
                    if (context->node) {
                        zlistx_t *agents = zhashx_values(context->agents);
                        igsagent_t *local_agent = zlistx_first(agents);
                        while (local_agent) {
                            if(streq(local_agent->uuid, agent_uuid)){
                                zlistx_t *remote_agents = zhashx_values(context->remote_agents);
                                igs_remote_agent_t *remote_agent = zlistx_first(remote_agents);
                                while (remote_agent) {
                                    if(streq(remote_agent->uuid, max_credit_worker->agent_uuid)){
                                        s_lock_zyre_peer(__FUNCTION__, __LINE__);
                                        zyre_shouts (context->node, local_agent->igs_channel,
                                                     "SPLIT %s(%s).%s to %s(%s).%s",
                                                     local_agent->definition->name,
                                                     splitter->agent_uuid,
                                                     output->name,
                                                     remote_agent->definition->name,
                                                     max_credit_worker->agent_uuid,
                                                     max_credit_worker->input_name);
                                        s_unlock_zyre_peer(__FUNCTION__, __LINE__);
                                    }
                                    remote_agent = zlistx_next(remote_agents);
                                }
                                zlistx_destroy(&remote_agents);
                            }
                            local_agent = zlistx_next(agents);
                        }
                        zlistx_destroy(&agents);
                    }
                    model_read_write_unlock(__FUNCTION__, __LINE__);
                    igs_channel_whisper_zmsg(max_credit_worker_uuid, &readyMessage);
                    free(max_credit_worker_uuid);
                    model_read_write_lock(__FUNCTION__, __LINE__);
                }
            }
            break;
        }
        splitter = zlist_next(splitters);
    }
    zlist_destroy(&splitters);
}

void s_split_add_credit_to_worker (igs_core_context_t *context, char* agent_uuid, igs_io_t* output,
                                   char* worker_uuid, char* input_name, int credit, bool is_new_worker)
{
    assert(context);
    assert(agent_uuid);
    assert(worker_uuid);
    assert(input_name);
    assert(output);
    assert(output->name);

    bool worker_found = false;
    bool splitter_found = false;
    igs_splitter_t *splitter = zlist_first(context->splitters);
    while (splitter) {
        if(streq(splitter->agent_uuid, agent_uuid)
           && streq(splitter->output_name, output->name)){
            int maxUses = 0;
            igs_worker_t *worker = zlist_first(splitter->workers);
            while (worker) {
                if(streq(worker_uuid, worker->agent_uuid)
                   && streq(input_name, worker->input_name)){
                    worker->credit += credit;
                    worker_found = true;
                }
                if (maxUses < worker->uses)
                    maxUses = worker->uses;
                worker = zlist_next(splitter->workers);
            }
            if(!worker_found && is_new_worker){
                igs_worker_t *new_worker = (igs_worker_t *) zmalloc (sizeof(igs_worker_t));
                new_worker->agent_uuid = s_strndup(worker_uuid, strlen(worker_uuid));
                new_worker->input_name = s_strndup(input_name, strlen(input_name));
                new_worker->credit = credit;
                new_worker->uses = maxUses;
                zlist_append(splitter->workers, new_worker);
            }
            splitter_found = true;
        }
        splitter = zlist_next(context->splitters);
    }
    if(!splitter_found && is_new_worker){
        igs_splitter_t *new_splitter = (igs_splitter_t *)zmalloc(sizeof(igs_splitter_t));
        new_splitter->agent_uuid = s_strndup(agent_uuid, strlen(agent_uuid));
        new_splitter->output_name = strdup(output->name);
        new_splitter->workers = zlist_new();
        new_splitter->queued_works = zlist_new();
        zlist_append(context->splitters, new_splitter);
        igs_worker_t *new_worker = (igs_worker_t *)zmalloc(sizeof(igs_worker_t));
        new_worker->agent_uuid = s_strndup(worker_uuid, strlen(worker_uuid));
        new_worker->input_name = s_strndup(input_name, strlen(input_name));
        new_worker->credit = credit;
        zlist_append(new_splitter->workers, new_worker);
    }
    s_split_trigger_send_message_to_worker(context, agent_uuid, output);
}


////////////////////////////////////////////////////////////////////////
#pragma mark PRIVATE API
////////////////////////////////////////////////////////////////////////

void split_free_split_element (igs_split_t **split_elmt)
{
    assert (split_elmt);
    assert (*split_elmt);

    if ((*split_elmt)->from_input)
        free ((*split_elmt)->from_input);
    if ((*split_elmt)->to_agent)
        free ((*split_elmt)->to_agent);
    if ((*split_elmt)->to_output)
        free ((*split_elmt)->to_output);
    free (*split_elmt);
    *split_elmt = NULL;
}

void split_free_splitter (igs_splitter_t **splitter)
{
    assert (splitter);
    assert (*splitter);
    if ((*splitter)->agent_uuid)
        free((*splitter)->agent_uuid);
    if ((*splitter)->output_name)
        free((*splitter)->output_name);
    igs_worker_t *worker = zlist_first((*splitter)->workers);
    while (worker) {
        if (worker->input_name)
            free(worker->input_name);
        if (worker->agent_uuid)
            free(worker->agent_uuid);
        free(worker);
        worker = zlist_next((*splitter)->workers);
    }
    zlist_destroy(&(*splitter)->workers);
    igs_queued_work_t *work = zlist_first((*splitter)->queued_works);
    while (work) {
        if (work->value_type == IGS_STRING_T && work->value.s)
            free(work->value.s);
        else if (work->value_type == IGS_DATA_T && work->value.data)
            free(work->value.data);
        free(work);
        work = zlist_next((*splitter)->queued_works);
    }
    zlist_destroy(&(*splitter)->queued_works);
    free(*splitter);
    *splitter = NULL;
}

void split_remove_worker (igs_core_context_t *context, char *uuid, char *input_name)
{
    assert(uuid);
    assert(context);
    igs_splitter_t *splitter = zlist_first(context->splitters);
    while (splitter) {
        igs_worker_t *worker = zlist_first(splitter->workers);
        while (worker) {
            if (streq(uuid, worker->agent_uuid) &&
                (!input_name || (input_name && streq(input_name, worker->input_name)))){
                zlist_remove(splitter->workers, worker);
                free(worker->agent_uuid);
                free(worker->input_name);
                free(worker);
            }
            worker = zlist_next(splitter->workers);
        }
        if(zlist_size(splitter->workers) == 0){
            zlist_destroy(&splitter->workers);
            zlist_remove(context->splitters, splitter);
            free(splitter->agent_uuid);
            free(splitter->output_name);
            igs_queued_work_t *work_elt = zlist_first(splitter->queued_works);
            while (work_elt) {
                zlist_remove(splitter->queued_works, work_elt);
                if(work_elt->value_type == IGS_DATA_T)
                    free(work_elt->value.data);
                else if(work_elt->value_type == IGS_STRING_T)
                    free(work_elt->value.s);
                free(work_elt);
                work_elt = zlist_next(splitter->queued_works);
            }
            zlist_destroy(&splitter->queued_works);
            free(splitter);
        }
        splitter = zlist_next(context->splitters);
    }
}

igs_split_t *split_create_split_element (const char *from_input,
                                         const char *to_agent,
                                         const char *to_output)
{
    assert (from_input);
    assert(to_agent);
    assert(to_output);
    igs_split_t *new_split_elmt = (igs_split_t *) zmalloc (sizeof (igs_split_t));
    new_split_elmt->from_input = s_strndup (from_input, IGS_MAX_IO_NAME_LENGTH);
    new_split_elmt->to_agent = s_strndup (to_agent, IGS_MAX_AGENT_NAME_LENGTH);
    new_split_elmt->to_output = s_strndup (to_output, IGS_MAX_IO_NAME_LENGTH);
    return new_split_elmt;
}

void split_add_work_to_queue (igs_core_context_t *context, 
                              char* agent_uuid,
                              const igs_io_t *output)
{
    assert(context);
    assert(agent_uuid);
    assert(output);
    assert(output->name);
    zlist_t *splitters = zlist_dup(context->splitters);
    igs_splitter_t *splitter = zlist_first(splitters);
    while (splitter) {
        assert(splitter->workers);
        if(streq(splitter->agent_uuid, agent_uuid)
           && streq(splitter->output_name, output->name)){
            igs_queued_work_t *new_work = (igs_queued_work_t *) zmalloc (sizeof(igs_queued_work_t));
            new_work->value_size = output->value_size;
            new_work->value_type = output->value_type;
            switch (output->value_type) {
                case IGS_INTEGER_T:
                    new_work->value.i = output->value.i;
                    break;
                case IGS_DOUBLE_T:
                    new_work->value.d = output->value.d;
                    break;
                case IGS_BOOL_T:
                    new_work->value.b = output->value.b;
                    break;
                case IGS_STRING_T:
                    new_work->value.s = strdup(output->value.s);
                    break;
                case IGS_IMPULSION_T:
                    break;
                case IGS_DATA_T:
                    new_work->value.data = (void *)zmalloc( output->value_size);
                    memcpy(new_work->value.data, output->value.data, output->value_size);
                    break;
                default:
                    break;
            }
            zlist_append(splitter->queued_works, new_work);
        }
        s_split_trigger_send_message_to_worker(context, agent_uuid, output);
        splitter = zlist_next(splitters);
    }
    zlist_destroy(&splitters);
}

int split_message_from_worker (char *command, zmsg_t *msg, igs_core_context_t *context)
{
    assert(command);
    assert(context);
    assert(msg);
    char * worker_uuid = zmsg_popstr(msg);
    if(!worker_uuid){
        igs_error ("no valid worker uuid in message %s from worker : rejecting", command);
        return 1;
    }
    char * inputName = zmsg_popstr(msg);
    if(!inputName){
        igs_error ("no valid input name in message %s from worker %s : rejecting", command, worker_uuid);
        free(worker_uuid);
        return 1;
    }
    char * outputName = zmsg_popstr(msg);
    if(!outputName){
        igs_error ("no valid output name in message %s from worker %s : rejecting", command, worker_uuid);
        free(worker_uuid);
        free(outputName);
        return 1;
    }
    
    if(streq(command, WORKER_HELLO_MSG)){
        char *creditStr = zmsg_popstr(msg);
        int credit = atoi(creditStr);
        char *agent_uuid = zmsg_popstr(msg);
        if(!agent_uuid){
            igs_error ("no valid splitter uuid in message %s from worker %s : rejecting", command, worker_uuid);
            free(worker_uuid);
            free(inputName);
            free(outputName);
            return 1;
        }
        igsagent_t *agent = zhashx_lookup(context->agents, agent_uuid);
        if (agent) {
            igs_io_t *io = zhashx_first(agent->definition->outputs_table);
            while (io) {
                if (streq(outputName, io->name)){
                    s_split_add_credit_to_worker(context, agent->uuid, io, worker_uuid, inputName, credit, true);
                    break;
                }
                io = zhashx_next(agent->definition->outputs_table);
            }
        } else
            igs_error("%s is not a known UUID for our agents", agent_uuid);
        free(creditStr);
        free(agent_uuid);
    }else if(streq(command, WORKER_READY_MSG)){
        char * agent_uuid = zmsg_popstr(msg);
        if(agent_uuid == NULL){
            igs_error ("no valid splitter uuid in message %s from worker %s : rejecting", command, worker_uuid);
            free(worker_uuid);
            free(inputName);
            free(outputName);
            return 1;
        }
        igsagent_t *agent = zhashx_first(context->agents);
        while (agent) {
            if(streq(agent_uuid, agent->uuid)){
                igs_io_t *io = zhashx_first(agent->definition->outputs_table);
                while (io) {
                    if (streq(outputName, io->name)){
                        s_split_add_credit_to_worker (context, agent->uuid, io, worker_uuid, inputName, 1, false);
                        break;
                    }
                    io = zhashx_next(agent->definition->outputs_table);
                }
            }
            agent = zhashx_next(context->agents);
        }
        free(agent_uuid);
    }else if(streq(command, WORKER_GOODBYE_MSG))
        split_remove_worker(context, worker_uuid, inputName);
    free(worker_uuid);
    free(inputName);
    free(outputName);
    return 0;
}

int split_message_from_splitter (zmsg_t *msg, igs_core_context_t *context)
{
    assert(msg);
    assert(context);
    char * agent_uuid = zmsg_popstr(msg);
    if(!agent_uuid){
        igs_error ("no valid splitter uuid in work message from splitter : rejecting");
        return 1;
    }
    char * inputName = zmsg_popstr(msg);
    if(!inputName){
        igs_error ("no valid input name in work message from splitter %s : rejecting", agent_uuid);
        free(agent_uuid);
        return 1;
    }
    char * outputName = zmsg_popstr(msg);
    if(!outputName){
        igs_error ("no valid output name in work message from splitter %s : rejecting", agent_uuid);
        free(agent_uuid);
        free(inputName);
        return 1;
    }
    char * vType = zmsg_popstr(msg);
    if(vType == NULL){
        igs_error ("no valid value type in work message from splitter %s : rejecting", agent_uuid);
        free(agent_uuid);
        free(inputName);
        free(outputName);
        return 1;
    }
    igs_io_value_type_t valueType = atoi(vType);

    free(vType);
    vType = NULL;
    if (valueType < IGS_INTEGER_T || valueType > IGS_DATA_T){
        igs_error("input type is not valid (%d) in received publication : rejecting", valueType);
        free(agent_uuid);
        free(inputName);
        free(outputName);
        return 1;
    }

    zframe_t *frame = NULL;
    void *data = NULL;
    size_t size = 0;
    char * value = NULL;
    if (valueType == IGS_STRING_T){
        value = zmsg_popstr(msg);
        if (!value){
            igs_error("value is NULL in received publication : rejecting");
            free(agent_uuid);
            free(inputName);
            free(outputName);
            return 1;
        }
    }else{
        frame = zmsg_pop(msg);
        if (!frame){
            igs_error("value is NULL in received publication : rejecting");
            free(agent_uuid);
            free(inputName);
            free(outputName);
            return 1;
        }
        data = zframe_data(frame);
        size = zframe_size(frame);
    }
    
    char * worker_uuid = zmsg_popstr(msg);
    if(!worker_uuid){
        igs_error ("no valid worker uuid in work message from splitter %s : rejecting", agent_uuid);
        free(agent_uuid);
        free(inputName);
        free(outputName);
        if (data)
            free(data);
        if (value)
            free(value);
        return 1;
    }
    
    zlistx_t *agents = zhashx_values(context->agents);
    igsagent_t *agent = zlistx_first(agents);
    while (agent && agent->uuid) {
        if (streq(agent->uuid, worker_uuid)){
            igs_io_t *io = NULL;
            if (valueType == IGS_STRING_T)
                io = model_write(agent, inputName, IGS_INPUT_T, valueType, value, strlen(value)+1);
            else
                io = model_write(agent, inputName, IGS_INPUT_T, valueType, data, size);
            model_read_write_unlock(__FUNCTION__, __LINE__);
            if (io && io->name)
                model_LOCKED_handle_io_callbacks(agent, io);
            model_read_write_lock(__FUNCTION__, __LINE__);
            break;
        }
        agent = zlistx_next(agents);
    }
    zlistx_destroy(&agents);
    if(frame)
        zframe_destroy(&frame);
    zmsg_t *readyMessage = zmsg_new();
    zmsg_addstr(readyMessage, WORKER_READY_MSG);
    zmsg_addstr(readyMessage, worker_uuid);
    zmsg_addstr(readyMessage, inputName);
    zmsg_addstr(readyMessage, outputName);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    igs_channel_whisper_zmsg(agent_uuid, &readyMessage);
    model_read_write_lock(__FUNCTION__, __LINE__);
    free(worker_uuid);
    free(agent_uuid);
    free(inputName);
    free(outputName);
    return 0;
}

////////////////////////////////////////////////////////////////////////
#pragma mark PUBLIC API
////////////////////////////////////////////////////////////////////////

size_t igsagent_split_count (igsagent_t *agent)
{
    assert (agent);
    if (!agent->uuid)
        return 0;
    assert (agent->mapping);
    model_read_write_lock(__FUNCTION__, __LINE__);
    size_t res = zlist_size(agent->mapping->split_elements);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

uint64_t igsagent_split_add (igsagent_t *agent,
                             const char *from_our_input,
                             const char *to_agent,
                             const char *with_output)
{
    assert (agent);
    if (!agent->uuid)
        return 0;
    assert(from_our_input && strlen (from_our_input) > 0);
    assert(model_check_string(from_our_input, IGS_MAX_IO_NAME_LENGTH));
    assert(to_agent && strlen (to_agent) > 0);
    assert(model_check_string(to_agent, IGS_MAX_AGENT_NAME_LENGTH));
    assert(with_output && strlen (with_output) > 0);
    assert(model_check_string(with_output, IGS_MAX_IO_NAME_LENGTH));
    
    model_read_write_lock(__FUNCTION__, __LINE__);
    assert (agent->mapping);
    // Add the new split element if not already there
    size_t len = strlen (from_our_input) + strlen (to_agent)
                 + strlen (with_output) + 3 + 1;
    char *mashup = (char *) zmalloc (len * sizeof (char));
    strcpy (mashup, from_our_input);
    strcat (mashup, "."); // separator
    strcat (mashup, to_agent);
    strcat (mashup, "."); // separator
    strcat (mashup, with_output);
    mashup[len - 1] = '\0';
    uint64_t hash = mapping_djb2_hash ((unsigned char *) mashup);
    free (mashup);

    igs_split_t *split = NULL;
    igs_split_t *tmp = zlist_first(agent->mapping->split_elements);
    while (tmp) {
        if (tmp->id == hash){
            split = tmp;
            break;
        }
        tmp = zlist_next(agent->mapping->split_elements);
    }
    if (!split) {
        // element does not exist yet : create and register it
        igs_split_t *new_split = split_create_split_element (from_our_input, to_agent, with_output);
        new_split->id = hash;
        zlist_append(agent->mapping->split_elements, new_split);
        mapping_update_json(agent->mapping);
        agent->network_need_to_send_mapping_update = true;

        // If agent is already known send WORKER_HELLO_MSG immediately
        igs_remote_agent_t *remote = zhashx_first(core_context->remote_agents);
        while (remote && remote->uuid) {
            if (streq (remote->definition->name, to_agent)) {
                zmsg_t *ready_message = zmsg_new ();
                zmsg_addstr (ready_message, WORKER_HELLO_MSG);
                zmsg_addstr (ready_message, agent->uuid);
                zmsg_addstr (ready_message, from_our_input);
                zmsg_addstr (ready_message, with_output);
                zmsg_addstrf (ready_message, "%i", IGS_DEFAULT_WORKER_CREDIT);
                char *remote_uuid = strdup(remote->uuid);
                model_read_write_unlock(__FUNCTION__, __LINE__);
                igs_channel_whisper_zmsg (remote_uuid, &ready_message);
                model_read_write_lock(__FUNCTION__, __LINE__);
                free(remote_uuid);
            }
            remote = zhashx_next(core_context->remote_agents);
        }
    }
        
    else
        igsagent_warn (agent, "split combination %s->%s.%s already exists and will not be duplicated",
                       from_our_input, to_agent, with_output);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return hash;
}

igs_result_t igsagent_split_remove_with_id (igsagent_t *agent,
                                            uint64_t the_id)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (the_id > 0);
    assert (agent->mapping);
    assert(agent->mapping->split_elements);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_split_t *el = NULL;
    igs_split_t *tmp = zlist_first(agent->mapping->split_elements);
    while (tmp) {
        if (tmp->id == the_id){
            el = tmp;
            break;
        }
        tmp = zlist_next(agent->mapping->split_elements);
    }
    if (!el) {
        igsagent_error (agent, "id %llu is not part of the current split", the_id);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    } else {
        zlist_remove(agent->mapping->split_elements, el);
        mapping_update_json(agent->mapping);
        agent->network_need_to_send_mapping_update = true;
        zmsg_t *goodbye_message = zmsg_new ();
        zmsg_addstr (goodbye_message, WORKER_GOODBYE_MSG);
        zmsg_addstr (goodbye_message, agent->uuid);
        zmsg_addstr (goodbye_message, el->from_input);
        zmsg_addstr (goodbye_message, el->to_output);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        if (el->to_agent)
            igs_channel_whisper_zmsg (el->to_agent, &goodbye_message);
        if (el->to_agent)
            split_free_split_element(&el);
    }
    return IGS_SUCCESS;
}

igs_result_t igsagent_split_remove_with_name (igsagent_t *agent,
                                               const char *from_our_input,
                                               const char *to_agent,
                                               const char *with_output)
{
    assert(agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert(from_our_input);
    assert(to_agent);
    assert(with_output);
    assert(agent->mapping);
    assert(agent->mapping->split_elements);
    model_read_write_lock(__FUNCTION__, __LINE__);
    size_t len = strlen (from_our_input) + strlen (to_agent) + strlen (with_output) + 3 + 1;
    char *mashup = (char *) zmalloc (len * sizeof (char));
    strcpy (mashup, from_our_input);
    strcat (mashup, "."); // separator
    strcat (mashup, to_agent);
    strcat (mashup, "."); // separator
    strcat (mashup, with_output);
    mashup[len - 1] = '\0';
    uint64_t h = mapping_djb2_hash ((unsigned char *) mashup);
    free (mashup);

    igs_split_t *el = NULL;
    igs_split_t *tmp = zlist_first(agent->mapping->split_elements);
    while (tmp) {
        if (tmp->id == h){
            el = tmp;
            break;
        }
        tmp = zlist_next(agent->mapping->split_elements);
    }
    if (!el) {
        igsagent_error (agent, "split combination %s->%s.%s does NOT exist", from_our_input, to_agent, with_output);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }else{
        zlist_remove(agent->mapping->split_elements, el);
        split_free_split_element (&el);
        mapping_update_json(agent->mapping);
        agent->network_need_to_send_mapping_update = true;
        zmsg_t *goodbye_message = zmsg_new ();
        zmsg_addstr (goodbye_message, WORKER_GOODBYE_MSG);
        zmsg_addstr (goodbye_message, agent->uuid);
        zmsg_addstr (goodbye_message, from_our_input);
        zmsg_addstr (goodbye_message, with_output);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        igs_channel_whisper_zmsg (to_agent, &goodbye_message);
    }
    return IGS_SUCCESS;
}
