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
// PRIVATE API
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

void split_remove_worker (igs_core_context_t *context, char *uuid, char *input_name)
{
    assert(uuid);
    assert(context);
    igs_splitter_t *splitter;
    igs_splitter_t *tmp;
    LL_FOREACH_SAFE(context->splitters, splitter, tmp){
        igs_worker_t *worker, *tmpWorker;
        LL_FOREACH_SAFE(splitter->workers_list, worker, tmpWorker){
            if (streq(uuid, worker->agent_uuid) &&
                (!input_name || (input_name && streq(input_name, worker->input_name)))){
                LL_DELETE(splitter->workers_list, worker);
                free(worker->agent_uuid);
                worker->agent_uuid = NULL;
                free(worker->input_name);
                worker->input_name = NULL;
                free(worker);
            }
        }
        if(splitter->workers_list == NULL){
            LL_DELETE(context->splitters, splitter);
            free(splitter->agent_uuid);
            splitter->agent_uuid = NULL;
            free(splitter->output_name);
            splitter->output_name = NULL;
            igs_queued_work_t *work_elt, *work_tmp;
            LL_FOREACH_SAFE(splitter->queued_works, work_elt, work_tmp){
                LL_DELETE(splitter->queued_works, work_elt);
                if(work_elt->value_type == IGS_DATA_T){
                    free(work_elt->value.data);
                    work_elt->value.data = NULL;
                }else if(work_elt->value_type == IGS_STRING_T){
                    free(work_elt->value.s);
                    work_elt->value.s = NULL;
                }
                free(work_elt);
            }
            free(splitter->queued_works);
            splitter->queued_works = NULL;
            free(splitter);
        }
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
    new_split_elmt->from_input = strdup (from_input);
    new_split_elmt->to_agent = strdup (to_agent);
    new_split_elmt->to_output = strdup (to_output);
    return new_split_elmt;
}

void s_split_trigger_send_message_to_worker (igs_core_context_t *context, char *agent_uuid, const igs_iop_t *output)
{
    assert(context);
    assert(agent_uuid);
    assert(output);

    igs_splitter_t *splitter = NULL;
    //FIXME: splitters should be stored in a hash and not a list, and searched by uuid
    LL_FOREACH(context->splitters, splitter){
        if(streq(splitter->agent_uuid, agent_uuid) && streq(splitter->output_name, output->name)){
            igs_worker_t *worker = NULL;
            igs_worker_t *max_credit_worker = NULL;
            LL_FOREACH(splitter->workers_list, worker){
                if(!max_credit_worker //init with 1st worker in list
                   || max_credit_worker->credit < worker->credit //take worker with more credits
                   || (max_credit_worker->credit == worker->credit //take worker with same credits but less uses
                       && worker->uses < max_credit_worker->uses))
                    max_credit_worker = worker;
            }
            if(max_credit_worker && max_credit_worker->credit > 0){
                igs_queued_work_t *work = splitter->queued_works;
                if(work){
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
                    if (context->node) {
                        igsagent_t *local_agent, *atmp;
                        HASH_ITER(hh, context->agents, local_agent, atmp){
                            if(streq(local_agent->uuid, agent_uuid)){
                                igs_remote_agent_t *remote_agent, *rtmp;
                                HASH_ITER(hh, context->remote_agents, remote_agent, rtmp){
                                    if(streq(remote_agent->uuid, max_credit_worker->agent_uuid)){
                                        zyre_shouts (context->node, local_agent->igs_channel,
                                                     "SPLIT %s(%s).%s to %s(%s).%s",
                                                     local_agent->definition->name,
                                                     splitter->agent_uuid,
                                                     output->name,
                                                     remote_agent->definition->name,
                                                     max_credit_worker->agent_uuid,
                                                     max_credit_worker->input_name);
                                    }
                                }
                            }
                        }
                    }

                    igs_channel_whisper_zmsg(max_credit_worker->agent_uuid, &readyMessage);
                    
                    LL_DELETE(splitter->queued_works, work);
                    switch(work->value_type){
                        case IGS_STRING_T:
                            free(work->value.s);
                            work->value.s = NULL;
                            break;
                        case IGS_DATA_T:
                            free(work->value.data);
                            work->value.data = NULL;
                            break;
                        default:
                            break;
                    }
                    free(work);
                    max_credit_worker->uses++;
                    max_credit_worker->credit--;
                }
            }
            break;
        }
    }
}

void split_add_work_to_queue (igs_core_context_t *context, char* agent_uuid, const igs_iop_t *output)
{
    assert(context);
    assert(agent_uuid);
    assert(output);
    assert(output->name);

    if(context->splitters){
        igs_splitter_t *splitter = NULL;
        LL_FOREACH(context->splitters, splitter){
            if(splitter->workers_list
               && streq(splitter->agent_uuid, agent_uuid)
               && streq(splitter->output_name, output->name)){
                igs_queued_work_t *new_work = (igs_queued_work_t *) zmalloc (sizeof(igs_queued_work_t));
                new_work->value_size = output->value_size;
                new_work->value_type = output->value_type;
                new_work->next = NULL;
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
                LL_APPEND(splitter->queued_works, new_work);
            }
        }
        s_split_trigger_send_message_to_worker(context, agent_uuid, output);
    }
}

////////////////////////////////////////////////////////////////////////
// Handler for message from worker or splitter
////////////////////////////////////////////////////////////////////////


void s_split_add_credit_to_worker (igs_core_context_t *context, char* agent_uuid, igs_iop_t* output,
                                   char* worker_uuid, char* input_name, int credit, bool new_worker)
{
    assert(context);
    assert(agent_uuid);
    assert(worker_uuid);
    assert(input_name);
    assert(output);
    assert(output->name);

    bool worker_found = false;
    bool splitter_found = false;
    igs_splitter_t *splitter;
    LL_FOREACH(context->splitters, splitter){
        if(streq(splitter->agent_uuid, agent_uuid)
           && streq(splitter->output_name, output->name)){
            igs_worker_t *worker = NULL;
            int maxUses = 0;
            LL_FOREACH(splitter->workers_list, worker){
                if(streq(worker_uuid, worker->agent_uuid)
                   && streq(input_name, worker->input_name)){
                    worker->credit += credit;
                    worker_found = true;
                }
                if (maxUses < worker->uses)
                    maxUses = worker->uses;
            }
            if(!worker_found && new_worker){
                igs_worker_t *new_w = (igs_worker_t *) zmalloc (sizeof(igs_worker_t));
                new_w->agent_uuid = s_strndup(worker_uuid, strlen(worker_uuid));
                new_w->input_name = s_strndup(input_name, strlen(input_name));
                new_w->credit = credit;
                new_w->uses = maxUses;
                LL_APPEND(splitter->workers_list, new_w);
            }
            splitter_found = true;
        }
    }
    if(!splitter_found && new_worker){
        igs_splitter_t *new_splitter = (igs_splitter_t *)zmalloc(sizeof(igs_splitter_t));
        new_splitter->agent_uuid = s_strndup(agent_uuid, strlen(agent_uuid));
        new_splitter->output_name = s_strndup(output->name, strlen(output->name));
        LL_APPEND(context->splitters, new_splitter);
        igs_worker_t *new_w = (igs_worker_t *)zmalloc(sizeof(igs_worker_t));
        new_w->agent_uuid = s_strndup(worker_uuid, strlen(worker_uuid));
        new_w->input_name = s_strndup(input_name, strlen(input_name));
        new_w->credit = credit;
        LL_APPEND(new_splitter->workers_list, new_w);
    }
    s_split_trigger_send_message_to_worker(context, agent_uuid, output);
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
        igsagent_t *agent, *tmpAgent;
        HASH_ITER(hh, context->agents, agent, tmpAgent){
            if(streq(agent_uuid, agent->uuid)){
                igs_iop_t *iop, *tmpIop;
                HASH_ITER(hh, agent->definition->outputs_table, iop, tmpIop){
                    if(!iop || !iop->name)
                        continue;
                    if (streq(outputName, iop->name)){
                        s_split_add_credit_to_worker(context, agent->uuid, iop, worker_uuid, inputName, credit, true);
                        break;
                    }
                }
            }
        }
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
        igsagent_t *agent, *tmpAgent;
        HASH_ITER(hh, context->agents, agent, tmpAgent){
            if(streq(agent_uuid, agent->uuid)){
                igs_iop_t *iop, *tmpIop;
                HASH_ITER(hh, agent->definition->outputs_table, iop, tmpIop){
                    if(!iop || !iop->name)
                        continue;
                    if (streq(outputName, iop->name)){
                        s_split_add_credit_to_worker (context, agent->uuid, iop, worker_uuid, inputName, 1, false);
                        break;
                    }
                }
            }
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
    igs_iop_value_type_t valueType = atoi(vType);

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
        if (value == NULL){
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
    
    igsagent_t *tmp, *elt;
    HASH_ITER(hh, core_context->agents, elt, tmp){
        if (streq(elt->uuid, worker_uuid)){
            if (valueType == IGS_STRING_T)
                model_write_iop(elt, inputName, IGS_INPUT_T, valueType, value, strlen(value)+1);
            else
                model_write_iop(elt, inputName, IGS_INPUT_T, valueType, data, size);
            break;
        }
    }
    if(frame)
        zframe_destroy(&frame);
    if(elt && elt->uuid){
        zmsg_t *readyMessage = zmsg_new();
        zmsg_addstr(readyMessage, WORKER_READY_MSG);
        zmsg_addstr(readyMessage, worker_uuid);
        zmsg_addstr(readyMessage, inputName);
        zmsg_addstr(readyMessage, outputName);
        igs_channel_whisper_zmsg(agent_uuid, &readyMessage);
    }
    free(worker_uuid);
    free(agent_uuid);
    free(inputName);
    free(outputName);
    return 0;
}

////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////

size_t igsagent_split_count (igsagent_t *agent)
{
    assert (agent);
    assert (agent->mapping);
    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return 0;
    }
    size_t res = HASH_COUNT (agent->mapping->split_elements);
    model_read_write_unlock (__FUNCTION__, __LINE__);
    return res;
}

uint64_t igsagent_split_add (igsagent_t *agent,
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
          agent, "split inputs to outputs of the same agent will not work "
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

    // Add the new split element if not already there
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

    igs_split_t *tmp = NULL;
    if (agent->mapping->split_elements)
        HASH_FIND (hh, agent->mapping->split_elements, &hash,
                   sizeof (uint64_t), tmp);
    if (tmp == NULL) {
        // element does not exist yet : create and register it
        // check input against definition and reject if input does not exist in
        // definition
        if (!igsagent_input_exists (agent, reviewed_from_our_input)) {
            igsagent_warn (agent,
                            "input %s does not exist in our definition (will "
                            "be stored anyway)",
                            reviewed_from_our_input);
        }
        igs_split_t *new = split_create_split_element (
          reviewed_from_our_input, reviewed_to_agent, reviewed_with_output);
        new->id = hash;
        HASH_ADD (hh, agent->mapping->split_elements, id,
                  sizeof (uint64_t), new);
        agent->network_need_to_send_mapping_update = true;

        // If agent is already known send HELLO message immediately
        igs_remote_agent_t *elt_agent, *tmp_agent;
        HASH_ITER (hh, core_context->remote_agents, elt_agent, tmp_agent)
        {
            if (streq (elt_agent->definition->name, to_agent)) {
                zmsg_t *ready_message = zmsg_new ();
                zmsg_addstr (ready_message, WORKER_HELLO_MSG);
                zmsg_addstr (ready_message, agent->uuid);
                zmsg_addstr (ready_message, from_our_input);
                zmsg_addstr (ready_message, with_output);
                zmsg_addstrf (ready_message, "%i", IGS_DEFAULT_WORKER_CREDIT);
                igs_channel_whisper_zmsg (elt_agent->uuid, &ready_message);
            }
        }
    }
    else
        igsagent_warn (agent, "split combination %s->%s.%s already exists : will not "
                       "be duplicated", reviewed_from_our_input, reviewed_to_agent, reviewed_with_output);

    free (reviewed_from_our_input);
    free (reviewed_to_agent);
    free (reviewed_with_output);
    model_read_write_unlock (__FUNCTION__, __LINE__);
    return hash;
}

igs_result_t igsagent_split_remove_with_id (igsagent_t *agent,
                                            uint64_t the_id)
{
    assert (agent);
    assert (the_id > 0);
    assert (agent->mapping);
    igs_split_t *el = NULL;
    if (agent->mapping->split_elements == NULL) {
        igsagent_error (agent, "no split elements defined yet");
        return IGS_FAILURE;
    }
    HASH_FIND (hh, agent->mapping->split_elements, &the_id,
               sizeof (uint64_t), el);
    if (el == NULL) {
        igsagent_error (agent, "id %llu is not part of the current split", the_id);
        return IGS_FAILURE;
    }
    else {
        model_read_write_lock (__FUNCTION__, __LINE__);
        // check that this agent has not been destroyed when we were locked
        if (!agent || !(agent->uuid)) {
            model_read_write_unlock (__FUNCTION__, __LINE__);
            return IGS_SUCCESS;
        }
        HASH_DEL (agent->mapping->split_elements, el);
        zmsg_t *goodbye_message = zmsg_new ();
        zmsg_addstr (goodbye_message, WORKER_GOODBYE_MSG);
        zmsg_addstr (goodbye_message, agent->uuid);
        zmsg_addstr (goodbye_message, el->from_input);
        zmsg_addstr (goodbye_message, el->to_output);
        igs_channel_whisper_zmsg (el->to_agent, &goodbye_message);
        split_free_split_element(&el);
        agent->network_need_to_send_mapping_update = true;
        model_read_write_unlock (__FUNCTION__, __LINE__);
    }
    return IGS_SUCCESS;
}

igs_result_t igsagent_split_remove_with_name (igsagent_t *agent,
                                               const char *from_our_input,
                                               const char *to_agent,
                                               const char *with_output)
{
    assert (agent);
    assert (from_our_input);
    assert (to_agent);
    assert (with_output);
    assert (agent->mapping);
    if (agent->mapping->split_elements == NULL) {
        igsagent_error (agent, "no split elements defined yet");
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

    igs_split_t *tmp = NULL;
    if (agent->mapping->split_elements)
        HASH_FIND (hh, agent->mapping->split_elements, &h,
                   sizeof (uint64_t), tmp);
    if (tmp == NULL) {
        igsagent_error (agent, "split combination %s->%s.%s does NOT exist",
                         from_our_input, to_agent, with_output);
        return IGS_FAILURE;
    }

    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return IGS_SUCCESS;
    }
    HASH_DEL (agent->mapping->split_elements, tmp);
    zmsg_t *goodbye_message = zmsg_new ();
    zmsg_addstr (goodbye_message, WORKER_GOODBYE_MSG);
    zmsg_addstr (goodbye_message, agent->uuid);
    zmsg_addstr (goodbye_message, tmp->from_input);
    zmsg_addstr (goodbye_message, tmp->to_output);
    igs_channel_whisper_zmsg (tmp->to_agent, &goodbye_message);
    split_free_split_element (&tmp);
    agent->network_need_to_send_mapping_update = true;
    model_read_write_unlock (__FUNCTION__, __LINE__);
    return IGS_SUCCESS;
}
