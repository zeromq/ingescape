//
//  call.c
//  ingescape
//
//  Created by Stephane Vales on 29/05/2018.
//  Copyright © 2018 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <zyre.h>
#include "ingescape.h"
#include "ingescape_advanced.h"
#include "ingescape_private.h"
#include "uthash/uthash.h"
#include "uthash/utlist.h"
#if (defined WIN32 || defined _WIN32)
#include "unixfunctions.h"
#endif

////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
void call_freeCallArguments(igs_callArgument_t *args){
    igs_callArgument_t *arg, *tmp;
    LL_FOREACH_SAFE(args, arg, tmp){
        LL_DELETE(args, arg);
        if (arg->name != NULL){
            free(arg->name);
        }
        if (arg->type == IGS_DATA_T && arg->data != NULL){
            free(arg->data);
        } else if (arg->type == IGS_STRING_T && arg->c != NULL){
            free(arg->c);
        }
        free(arg);
    }
}

void call_freeCall(igs_call_t *t){
    if (t != NULL){
        if (t->name != NULL)
            free(t->name);
        call_freeCallArguments(t->arguments);
        if (t->reply != NULL){
            if (t->reply->name != NULL)
                free(t->reply->name);
            call_freeCallArguments(t->reply->arguments);
        }
        free(t);
    }
}

igs_result_t call_addValuesToArgumentsFromMessage(const char *name, igs_callArgument_t *arg, zmsg_t *msg){
    size_t nbFrames = zmsg_size(msg);
    size_t nbArgs = 0;
    igs_callArgument_t *tmp = NULL;
    DL_COUNT(arg, tmp, nbArgs);
    if (nbFrames != nbArgs){
        igs_error("arguments count do not match in received message for call %s (%zu vs. %zu expected)", name, nbFrames, nbArgs);
        return IGS_FAILURE;
    }
    igs_callArgument_t *current = NULL;
    DL_FOREACH(arg, current){
        zframe_t *f = zmsg_pop(msg);
        size_t size = zframe_size(f);
        switch (current->type) {
            case IGS_BOOL_T:
                memcpy(&(current->b), zframe_data(f), sizeof(bool));
                break;
            case IGS_INTEGER_T:
                memcpy(&(current->i), zframe_data(f), sizeof(int));
                break;
            case IGS_DOUBLE_T:
                memcpy(&(current->d), zframe_data(f), sizeof(double));
                break;
            case IGS_STRING_T:
                if (current->c)
                    free(current->c);
                current->c = calloc(1, size);
                memcpy(current->c, zframe_data(f), size);
                break;
            case IGS_DATA_T:
                if (current->data)
                    free(current->data);
                current->data = calloc(1, size);
                memcpy(current->data, zframe_data(f), size);
                break;

            default:
                break;
        }
        current->size = size;
        zframe_destroy(&f);
    }
    return IGS_SUCCESS;
}

igs_result_t call_copyArguments(igs_callArgument_t *source, igs_callArgument_t *destination){
    assert(source);
    assert(destination);
    size_t nbArgsSource = 0;
    size_t nbArgsDesintation = 0;
    igs_callArgument_t *tmp = NULL;
    DL_COUNT(source, tmp, nbArgsSource);
    DL_COUNT(destination, tmp, nbArgsDesintation);
    
    if (nbArgsSource != nbArgsDesintation){
        igs_error("number of elements must be the same in source and destination");
        return IGS_FAILURE;
    }
    
    igs_callArgument_t *currentS = NULL;
    igs_callArgument_t *currentD = NULL;
    DL_FOREACH(destination, currentD){

        //init for source if needed
        if (currentS == NULL)
            currentS = source;
        
        size_t size = currentS->size;
        switch (currentD->type) {
            case IGS_BOOL_T:
                memcpy(&(currentD->b), &(currentS->b), sizeof(bool));
                break;
            case IGS_INTEGER_T:
                memcpy(&(currentD->i), &(currentS->i), sizeof(int));
                break;
            case IGS_DOUBLE_T:
                memcpy(&(currentD->d), &(currentS->d), sizeof(double));
                break;
            case IGS_STRING_T:
                if (currentD->c)
                    free(currentD->c);
                currentD->c = calloc(1, size+1);
                memcpy(currentD->c, currentS->c, size);
                break;
            case IGS_DATA_T:
                if (currentD->data)
                    free(currentD->data);
                currentD->data = calloc(1, size);
                memcpy(currentD->data, currentS->data, size);
                break;

            default:
                break;
        }
        currentD->size = size;
        currentS = currentS->next;
    }
    return IGS_SUCCESS;
}

void call_freeValuesInArguments(igs_callArgument_t *arg){
    if (arg != NULL){
        igs_callArgument_t *tmp = NULL;
        DL_FOREACH(arg, tmp){
            if (tmp->type == IGS_DATA_T && tmp->data != NULL){
                free(tmp->data);
            } else if (tmp->type == IGS_STRING_T && tmp->data != NULL){
                free(tmp->c);
            }
            tmp->data = NULL;
            tmp->size = 0;
        }
    }
}

////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////
void igs_destroyArgumentsList(igs_callArgument_t **list){
    if (list != NULL && *list != NULL){
        call_freeCallArguments(*list);
        *list = NULL;
    }
}

igs_callArgument_t *igs_cloneArgumentsList(igs_callArgument_t *list){
    assert(list);
    igs_callArgument_t *res = NULL;
    igs_callArgument_t *arg = NULL;
    LL_FOREACH(list, arg){
        if (arg != NULL){
            igs_callArgument_t *new = calloc(1, sizeof(igs_callArgument_t));
            new->type = arg->type;
            if (arg->name != NULL){
                new->name = strdup(arg->name);
            }
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
                    new->c = strdup(arg->c);
                    break;
                case IGS_DATA_T:
                    new->data = calloc(1, arg->size);
                    memcpy(new->data, arg->data, arg->size);
                    break;

                default:
                    break;
            }
            LL_APPEND(res, new);
        }
    }
    return res;
}

void igs_addIntToArgumentsList(igs_callArgument_t **list, int value){
    igs_callArgument_t *new = calloc(1, sizeof(igs_callArgument_t));
    new->type = IGS_INTEGER_T;
    new->i = value;
    new->size = sizeof(int);
    LL_APPEND(*list, new);
}

void igs_addBoolToArgumentsList(igs_callArgument_t **list, bool value){
    igs_callArgument_t *new = calloc(1, sizeof(igs_callArgument_t));
    new->type = IGS_BOOL_T;
    new->b = value;
    new->size = sizeof(bool);
    LL_APPEND(*list, new);
}

void igs_addDoubleToArgumentsList(igs_callArgument_t **list, double value){
    igs_callArgument_t *new = calloc(1, sizeof(igs_callArgument_t));
    new->type = IGS_DOUBLE_T;
    new->d = value;
    new->size = sizeof(double);
    LL_APPEND(*list, new);
}

void igs_addStringToArgumentsList(igs_callArgument_t **list, const char *value){
    igs_callArgument_t *new = calloc(1, sizeof(igs_callArgument_t));
    new->type = IGS_STRING_T;
    if (value != NULL){
        new->c = strdup(value);
        new->size = strlen(value) + 1;
    }else{
        new->c = NULL;
        new->size = 0;
    }

    LL_APPEND(*list, new);
}

void igs_addDataToArgumentsList(igs_callArgument_t **list, void *value, size_t size){
    igs_callArgument_t *new = calloc(1, sizeof(igs_callArgument_t));
    new->type = IGS_DATA_T;
    new->data = calloc(1, size);
    memcpy(new->data, value, size);
    new->size = size;
    LL_APPEND(*list, new);
}


igs_result_t igsAgent_initCall(igs_agent_t *agent, const char *name, igsAgent_callFunction cb, void *myData){
    assert(agent);
    assert(name && strlen(name) > 0);
    assert(cb);
    igs_call_t *t = NULL;
    if(agent->definition == NULL){
        agent->definition = calloc(1, sizeof(igs_definition_t));
    }
    HASH_FIND_STR(agent->definition->calls_table, name, t);
    if (t != NULL && t->cb != NULL){
        igsAgent_error(agent, "call with name %s already exists and has a callback", name);
        return IGS_FAILURE;
    }else{
        if (cb == NULL){
            igsAgent_error(agent, "non-NULL callback is mandatory at call creation");
            return IGS_FAILURE;
        }else{
            if (t == NULL){
                //call is completely new: allocate it
                t = calloc(1, sizeof(igs_call_t));
                if (strnlen(name, MAX_STRING_MSG_LENGTH) == MAX_STRING_MSG_LENGTH){
                    t->name = strndup(name, MAX_STRING_MSG_LENGTH);
                    igsAgent_warn(agent, "call name has been shortened to %s", t->name);
                }else{
                    t->name = strndup(name, MAX_STRING_MSG_LENGTH);
                }
                HASH_ADD_STR(agent->definition->calls_table, name, t);
                agent->network_needToSendDefinitionUpdate = true;
            }
            t->cb = cb;
            t->cbData = myData;
        }
    }
    return IGS_SUCCESS;
}

igs_result_t igsAgent_removeCall(igs_agent_t *agent, const char *name){
    assert(agent);
    assert(name);
    igs_call_t *t = NULL;
    if(agent->definition == NULL){
        igsAgent_error(agent, "No definition available yet");
        return IGS_FAILURE;
    }
    HASH_FIND_STR(agent->definition->calls_table, name, t);
    if (t == NULL){
        igsAgent_error(agent, "call with name '%s' does not exist", name);
        return IGS_FAILURE;
    }else{
        HASH_DEL(agent->definition->calls_table, t);
        call_freeCall(t);
        agent->network_needToSendDefinitionUpdate = true;
    }
    return IGS_SUCCESS;
}

igs_result_t igsAgent_addArgumentToCall(igs_agent_t *agent, const char *callName, const char *argName, iopType_t type){
    assert(agent);
    assert(callName);
    assert(argName && strlen(argName) > 0);
    igs_call_t *t = NULL;
    if(agent->definition == NULL){
        igsAgent_error(agent, "No definition available yet");
        return IGS_FAILURE;
    }
    HASH_FIND_STR(agent->definition->calls_table, callName, t);
    if (type == IGS_IMPULSION_T){
        igsAgent_error(agent, "impulsion type is not allowed as a call argument");
        return IGS_FAILURE;
    }
    if (type == IGS_UNKNOWN_T){
        igsAgent_error(agent, "unknown type is not allowed as a call argument");
        return IGS_FAILURE;
    }
    if (t == NULL){
        igsAgent_error(agent, "call with name %s does not exist", callName);
        return IGS_FAILURE;
    }else{
        igs_callArgument_t *a = calloc(1, sizeof(igs_callArgument_t));
        if (strnlen(argName, MAX_STRING_MSG_LENGTH) == MAX_STRING_MSG_LENGTH){
            a->name = strndup(argName, MAX_STRING_MSG_LENGTH);
            igsAgent_warn(agent, "call argument name has been shortened to %s", a->name);
        }else{
            a->name = strndup(argName, MAX_STRING_MSG_LENGTH);
        }
        switch (type) {
            case IGS_BOOL_T:
                a->size = sizeof(bool);
                break;
            case IGS_INTEGER_T:
                a->size = sizeof(int);
                break;
            case IGS_DOUBLE_T:
                a->size = sizeof(double);
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
        LL_APPEND(t->arguments, a);
        agent->network_needToSendDefinitionUpdate = true;
    }
    return IGS_SUCCESS;
}

igs_result_t igsAgent_removeArgumentFromCall(igs_agent_t *agent, const char *callName, const char *argName){
    assert(agent);
    assert(callName);
    assert(argName);
    igs_call_t *t = NULL;
    if(agent->definition == NULL){
        igsAgent_error(agent, "No definition available yet");
        return IGS_FAILURE;
    }
    HASH_FIND_STR(agent->definition->calls_table, callName, t);
    if (t == NULL){
        igsAgent_error(agent, "call with name %s does not exist", callName);
        return IGS_FAILURE;
    }else{
        igs_callArgument_t *arg = NULL, *tmp = NULL;
        bool found = false;
        LL_FOREACH_SAFE(t->arguments, arg, tmp){
            if (strcmp(argName, arg->name) == 0){
                LL_DELETE(t->arguments, arg);
                free(arg->name);
                if (arg->type == IGS_DATA_T && arg->data != NULL){
                    free(arg->data);
                } else if (arg->type == IGS_STRING_T && arg->data != NULL){
                    free(arg->c);
                }
                free(arg);
                found = true;
                agent->network_needToSendDefinitionUpdate = true;
                break;
            }
        }
        if (!found){
            igsAgent_debug(agent, "no argument named %s for call %s", argName, callName);
        }
    }
    return IGS_SUCCESS;
}

igs_result_t igsAgent_sendCall(igs_agent_t *agent, const char *agentNameOrUUID, const char *callName, igs_callArgument_t **list){
    assert(agent);
    assert(agentNameOrUUID);
    assert(callName);
    assert(list);
    assert(*list);
    
    bool found = false;
    
    //1- iteration on remote agents
    igs_remote_agent_t *remoteAgent = NULL, *tmp = NULL;
    HASH_ITER(hh, agent->context->remoteAgents, remoteAgent, tmp){
        if (streq(remoteAgent->name, agentNameOrUUID) || streq(remoteAgent->uuid, agentNameOrUUID)){
            //we found a matching agent
            igs_callArgument_t *arg = NULL;
            found = true;
            if (remoteAgent->definition == NULL){
                igsAgent_warn(agent, "definition is unknown for %s(%s) : cannot verify call before sending it",
                              remoteAgent->name, agentNameOrUUID);
                //continue; //commented to allow sending the message anyway
            }else{
                igs_call_t *call = NULL;
                HASH_FIND_STR(remoteAgent->definition->calls_table, callName, call);
                if (call != NULL){
                    size_t nbArguments = 0;
                    if (list != NULL && *list != NULL)
                        LL_COUNT(*list, arg, nbArguments);
                    size_t definedNbArguments = 0;
                    LL_COUNT(call->arguments, arg, definedNbArguments);
                    if (nbArguments != definedNbArguments){
                        igsAgent_error(agent, "passed number of arguments is not correct (received: %zu / expected: %zu) : call will not be sent",
                                  nbArguments, definedNbArguments);
                        continue;
                    }
                }else{
                    igsAgent_error(agent, "could not find call named %s for %s  : call will not be sent", callName, agentNameOrUUID);
                    continue;
                }
            }
            zmsg_t *msg = zmsg_new();
            zmsg_addstr(msg, "CALL");
            zmsg_addstr(msg, agent->uuid);
            zmsg_addstr(msg, remoteAgent->uuid);
            zmsg_addstr(msg, callName);
            if (list != NULL){
                LL_FOREACH(*list, arg){
                    zframe_t *frame = NULL;
                    switch (arg->type) {
                        case IGS_BOOL_T:
                            frame = zframe_new(&arg->b, sizeof(int));
                            break;
                        case IGS_INTEGER_T:
                            frame = zframe_new(&arg->i, sizeof(int));
                            break;
                        case IGS_DOUBLE_T:
                            frame = zframe_new(&arg->d, sizeof(double));
                            break;
                        case IGS_STRING_T:{
                            if (arg->c != NULL){
                                frame = zframe_new(arg->c, strlen(arg->c)+1);
                            }else{
                                frame = zframe_new(NULL, 0);
                            }
                            break;
                        }
                        case IGS_DATA_T:
                            frame = zframe_new(arg->data, arg->size);
                            break;
                        default:
                            break;
                    }
                    assert(frame);
                    zmsg_add(msg, frame);
                }
            }
            bus_zyreLock();
            zyre_shouts(agent->context->node, agent->context->callsChannel, "%s(%s) calls %s.%s(%s)",
                        agent->name, agent->uuid, remoteAgent->name, callName, remoteAgent->uuid);
            zyre_whisper(agent->context->node, remoteAgent->peer->peerId, &msg);
            bus_zyreUnlock();
            igsAgent_debug(agent, "sent call %s to %s(%s)", callName, remoteAgent->name, remoteAgent->uuid);

        }
    }
    
    //2- iteration on local agents
    igs_agent_t *localAgent, *atmp;
    HASH_ITER(hh, agent->context->agents, localAgent, atmp){
        if (streq(localAgent->name, agentNameOrUUID) || streq(localAgent->uuid, agentNameOrUUID)){
            //we found a matching agent
            igs_callArgument_t *arg = NULL;
            found = true;
            if (localAgent->definition == NULL){
                igsAgent_warn(agent, "definition is unknown for %s(%s) : cannot verify call before sending it",
                              localAgent->name, agentNameOrUUID);
                //continue; //commented to allow sending the message anyway
            }else{
                igs_call_t *call = NULL;
                HASH_FIND_STR(localAgent->definition->calls_table, callName, call);
                if (call != NULL){
                    size_t nbArguments = 0;
                    if (list != NULL && *list != NULL)
                        LL_COUNT(*list, arg, nbArguments);
                    size_t definedNbArguments = 0;
                    LL_COUNT(call->arguments, arg, definedNbArguments);
                    if (nbArguments != definedNbArguments){
                        igsAgent_error(agent, "passed number of arguments is not correct (received: %zu / expected: %zu) : call will not be sent",
                                       nbArguments, definedNbArguments);
                        continue;
                    }else{
                        //update call arguments values with new ones
                        if(call->arguments){
                            call_copyArguments(*list, call->arguments);
                        }
                        if (call->cb != NULL){
                            (call->cb)(localAgent, agent->name, agent->uuid, callName, call->arguments, nbArguments, call->cbData);
                            call_freeValuesInArguments(call->arguments);
                        }else{
                            igsAgent_error(agent, "no defined callback to handle received call %s", callName);
                        }
                    }
                }else{
                    igsAgent_error(agent, "could not find call named %s for %s  : call will not be sent", callName, agentNameOrUUID);
                    continue;
                }
            }

            bus_zyreLock();
            zyre_shouts(agent->context->node, agent->context->callsChannel, "%s(%s) calls %s.%s(%s)",
                        agent->name, agent->uuid, localAgent->name, callName, localAgent->uuid);
            bus_zyreUnlock();
            igsAgent_debug(agent, "sent call %s to %s(%s)", callName, localAgent->name, localAgent->uuid);

        }
    }
    
    call_freeCallArguments(*list);
    *list = NULL;
    
    if (!found){
        igsAgent_error(agent, "could not find an agent with name or UUID : %s", agentNameOrUUID);
        return IGS_FAILURE;
    }
    return IGS_SUCCESS;
}

size_t igsAgent_getNumberOfCalls(igs_agent_t *agent){
    if (agent->definition == NULL){
        igsAgent_warn(agent, "definition is NULL");
        return 0;
    }
    return HASH_COUNT(agent->definition->calls_table);
}

bool igsAgent_checkCallExistence(igs_agent_t *agent, const char *name){
    assert(agent);
    if (agent->definition == NULL)
        return false;
    igs_call_t *t = NULL;
    HASH_FIND_STR(agent->definition->calls_table, name, t);
    if (t == NULL){
        return false;
    }else{
        return true;
    }
}

char** igsAgent_getCallsList(igs_agent_t *agent, size_t *nbOfElements){
    assert(agent);
    if (agent->definition == NULL){
        *nbOfElements = 0;
        return NULL;
    }
    size_t nb = HASH_COUNT(agent->definition->calls_table);
    if (nb == 0){
        *nbOfElements = 0;
        return NULL;
    }
    *nbOfElements = nb;
    char **res = calloc(nb, sizeof(char*));
    igs_call_t *el, *tmp;
    size_t i = 0;
    HASH_ITER(hh, agent->definition->calls_table, el, tmp){
        res[i++] = strdup(el->name);
    }
    return res;
}

void igs_freeCallsList(char ***list, size_t nbOfCalls){
    if (list == NULL || *list == NULL)
        return;
    size_t i = 0;
    for (i = 0; i < nbOfCalls; i++){
        if ((*list)[i] != NULL)
            free((*list)[i]);
    }
    free((*list));
    *list = NULL;
}

igs_callArgument_t* igsAgent_getFirstArgumentForCall(igs_agent_t *agent, const char *callName){
    if (callName == NULL || strlen(callName) == 0){
        igsAgent_error(agent, "call name cannot be NULL or empty");
        return NULL;
    }
    if (agent->definition == NULL){
        igsAgent_error(agent, "definition is NULL");
        return NULL;
    }
    igs_call_t *t = NULL;
    HASH_FIND_STR(agent->definition->calls_table, callName, t);
    if (t == NULL){
        igsAgent_debug(agent, "could not find call with name %s", callName);
        return NULL;
    }
    return t->arguments;
}

size_t igsAgent_getNumberOfArgumentsForCall(igs_agent_t *agent, const char *callName){
    if (callName == NULL || strlen(callName) == 0){
        igsAgent_error(agent, "call name cannot be NULL or empty");
        return 0;
    }
    if (agent->definition == NULL){
        igsAgent_error(agent, "definition is NULL");
        return 0;
    }
    igs_call_t *t = NULL;
    HASH_FIND_STR(agent->definition->calls_table, callName, t);
    if (t == NULL){
        igsAgent_debug(agent, "could not find call with name %s", callName);
        return 0;
    }
    size_t nb = 0;
    igs_callArgument_t *a = NULL;
    LL_COUNT(t->arguments, a, nb);
    return nb;
}

bool igsAgent_checkCallArgumentExistence(igs_agent_t *agent, const char *callName, const char *argName){
    if (callName == NULL || strlen(callName) == 0){
        igsAgent_error(agent, "call name cannot be NULL or empty");
        return false;
    }
    if (agent->definition == NULL){
        igsAgent_error(agent, "definition is NULL");
        return false;
    }
    igs_call_t *t = NULL;
    HASH_FIND_STR(agent->definition->calls_table, callName, t);
    if (t == NULL){
        igsAgent_debug(agent, "could not find call with name %s", callName);
        return false;
    }
    igs_callArgument_t *a = NULL;
    LL_FOREACH(t->arguments, a){
        if (strcmp(a->name, argName) == 0)
            return true;
    }
    return false;
}
