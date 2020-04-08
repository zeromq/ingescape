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

bool call_addValuesToArgumentsFromMessage(const char *name, igs_callArgument_t *arg, zmsg_t *msg){
    size_t nbFrames = zmsg_size(msg);
    size_t nbArgs = 0;
    igs_callArgument_t *tmp = NULL;
    DL_COUNT(arg, tmp, nbArgs);
    if (nbFrames != nbArgs){
        igs_error("arguments count do not match in received message for call %s (%zu vs. %zu expected)", name, nbFrames, nbArgs);
        return 0;
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
                current->c = calloc(1, size);
                memcpy(current->c, zframe_data(f), size);
                break;
            case IGS_DATA_T:
                current->data = calloc(1, size);
                memcpy(current->data, zframe_data(f), size);
                break;
                
            default:
                break;
        }
        current->size = size;
        zframe_destroy(&f);
    }
    return 1;
}

int call_freeValuesInArguments(igs_callArgument_t *arg){
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
    return 1;
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


int igsAgent_initCall(igsAgent_t *agent, const char *name, igsAgent_callFunction cb, void *myData){
    igs_call_t *t = NULL;
    if(agent->definition == NULL){
        agent->definition = calloc(1, sizeof(igs_definition_t));
    }
    HASH_FIND_STR(agent->definition->calls_table, name, t);
    if (t != NULL && t->cb != NULL){
        igsAgent_error(agent, "call with name %s already exists and has a callback", name);
        return 0;
    }else{
        if (cb == NULL){
            igsAgent_error(agent, "non-NULL callback is mandatory at call creation");
            return 0;
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
    return 1;
}

int igsAgent_removeCall(igsAgent_t *agent, const char *name){
    igs_call_t *t = NULL;
    if(agent->definition == NULL){
        igsAgent_error(agent, "No definition available yet");
        return -1;
    }
    HASH_FIND_STR(agent->definition->calls_table, name, t);
    if (t == NULL){
        igsAgent_error(agent, "call with name %s does not exist", name);
    }else{
        HASH_DEL(agent->definition->calls_table, t);
        call_freeCall(t);
        agent->network_needToSendDefinitionUpdate = true;
    }
    return 1;
}

int igsAgent_addArgumentToCall(igsAgent_t *agent, const char *callName, const char *argName, iopType_t type){
    igs_call_t *t = NULL;
    if(agent->definition == NULL){
        igsAgent_error(agent, "No definition available yet");
        return -1;
    }
    HASH_FIND_STR(agent->definition->calls_table, callName, t);
    if (type == IGS_IMPULSION_T){
        igsAgent_error(agent, "impulsion type is not allowed as a call argument");
        return -1;
    }
    if (type == IGS_UNKNOWN_T){
        igsAgent_error(agent, "unknown type is not allowed as a call argument");
        return -1;
    }
    if (t == NULL){
        igsAgent_error(agent, "call with name %s does not exist", callName);
        return -1;
    }else{
        igs_callArgument_t *a = calloc(1, sizeof(igs_callArgument_t));
        if (strnlen(argName, MAX_STRING_MSG_LENGTH) == MAX_STRING_MSG_LENGTH){
            a->name = strndup(argName, MAX_STRING_MSG_LENGTH);
            igsAgent_warn(agent, "call argument name has been shortened to %s", a->name);
        }else{
            a->name = strndup(argName, MAX_STRING_MSG_LENGTH);
        }
        a->size = 0;
        a->type = type;
        LL_APPEND(t->arguments, a);
        agent->network_needToSendDefinitionUpdate = true;
    }
    return 1;
}

int igsAgent_removeArgumentFromCall(igsAgent_t *agent, const char *callName, const char *argName){
    igs_call_t *t = NULL;
    if(agent->definition == NULL){
        igsAgent_error(agent, "No definition available yet");
        return -1;
    }
    HASH_FIND_STR(agent->definition->calls_table, callName, t);
    if (t == NULL){
        igsAgent_error(agent, "call with name %s does not exist", callName);
        return -1;
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
    return 1;
}

int igsAgent_sendCall(igsAgent_t *agent, const char *agentNameOrUUID, const char *callName, igs_callArgument_t **list){
    if (agentNameOrUUID == NULL || strlen(agentNameOrUUID) == 0){
        igsAgent_error(agent, "agent name or UUID must not be NULL or empty");
        return -1;
    }
    zyreAgent_t *agt = NULL, *tmp = NULL;
    bool found = false;
    HASH_ITER(hh, agent->zyreAgents, agt, tmp){
        if (strcmp(agt->name, agentNameOrUUID) == 0 || strcmp(agt->peerId, agentNameOrUUID) == 0){
            //we found a matching agent
            igs_callArgument_t *arg = NULL;
            if (agt->subscriber == NULL || agt->subscriber->definition == NULL){
                igsAgent_warn(agent, "definition is unknown for %s : cannot verify call before sending it", agentNameOrUUID);
                //continue; //commented to allow sending the message anyway
            }else{
                igs_call_t *call = NULL;
                HASH_FIND_STR(agt->subscriber->definition->calls_table, callName, call);
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
            found = true;
            zmsg_t *msg = zmsg_new();
            zmsg_addstr(msg, "CALL");
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
                    if (frame != NULL){
                        zmsg_add(msg, frame);
                    }
                }
            }
            bus_zyreLock();
            zyre_shouts(agent->loopElements->node, agent->callsChannel, "%s to %s", callName, agentNameOrUUID);
            zyre_whisper(agent->loopElements->node, agt->peerId, &msg);
            bus_zyreUnlock();
            igsAgent_debug(agent, "sent call %s to %s", callName, agentNameOrUUID);
            
        }
    }
    if (!found){
        igsAgent_error(agent, "could not find an agent with name or UUID : %s", agentNameOrUUID);
    }
    if (list != NULL && *list != NULL){
        call_freeCallArguments(*list);
        *list = NULL;
    }
    return 1;
}

size_t igsAgent_getNumberOfCalls(igsAgent_t *agent){
    if (agent->definition == NULL){
        igsAgent_warn(agent, "definition is NULL");
        return 0;
    }
    return HASH_COUNT(agent->definition->calls_table);
}

bool igsAgent_checkCallExistence(igsAgent_t *agent, const char *name){
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

char** igsAgent_getCallsList(igsAgent_t *agent, size_t *nbOfElements){
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

void igs_freeCallsList(char **list, size_t nbOfCalls){
    if (list == NULL)
        return;
    size_t i = 0;
    for (i = 0; i < nbOfCalls; i++){
        if (list[i] != NULL)
            free(list[i]);
    }
    free(list);
}

igs_callArgument_t* igsAgent_getFirstArgumentForCall(igsAgent_t *agent, const char *callName){
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

size_t igsAgent_getNumberOfArgumentsForCall(igsAgent_t *agent, const char *callName){
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

bool igsAgent_checkCallArgumentExistence(igsAgent_t *agent, const char *callName, const char *argName){
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
