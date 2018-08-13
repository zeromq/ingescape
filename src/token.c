//
//  token.c
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

////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
void token_freeTokenArguments(igs_tokenArgument_t *args){
    igs_tokenArgument_t *arg, *tmp;
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

void token_freeToken(igs_token_t *t){
    if (t != NULL){
        free(t->name);
        token_freeTokenArguments(t->arguments);
        free(t);
    }
}

bool token_addValuesToArgumentsFromMessage(const char *name, igs_tokenArgument_t *arg, zmsg_t *msg){
    size_t nbFrames = zmsg_size(msg);
    size_t nbArgs = 0;
    igs_tokenArgument_t *tmp = NULL;
    DL_COUNT(arg, tmp, nbArgs);
    if (nbFrames != nbArgs){
        igs_error("arguments count do not match in received message for token %s (%zu vs. %zu expected)", name, nbFrames, nbArgs);
        return 0;
    }
    igs_tokenArgument_t *current = NULL;
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

int token_freeValuesInArguments(igs_tokenArgument_t *arg){
    if (arg != NULL){
        igs_tokenArgument_t *tmp = NULL;
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
void igs_destroyArgumentsList(igs_tokenArgument_t **list){
    token_freeTokenArguments(*list);
    *list = NULL;
}

void igs_addIntToArgumentsList(igs_tokenArgument_t **list, int value){
    igs_tokenArgument_t *new = calloc(1, sizeof(igs_tokenArgument_t));
    new->type = IGS_INTEGER_T;
    new->i = value;
    new->size = sizeof(int);
    LL_APPEND(*list, new);
}

void igs_addBoolToArgumentsList(igs_tokenArgument_t **list, bool value){
    igs_tokenArgument_t *new = calloc(1, sizeof(igs_tokenArgument_t));
    new->type = IGS_BOOL_T;
    new->b = value;
    new->size = sizeof(bool);
    LL_APPEND(*list, new);
}

void igs_addDoubleToArgumentsList(igs_tokenArgument_t **list, double value){
    igs_tokenArgument_t *new = calloc(1, sizeof(igs_tokenArgument_t));
    new->type = IGS_DOUBLE_T;
    new->d = value;
    new->size = sizeof(double);
    LL_APPEND(*list, new);
}

void igs_addStringToArgumentsList(igs_tokenArgument_t **list, char *value){
    igs_tokenArgument_t *new = calloc(1, sizeof(igs_tokenArgument_t));
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

void igs_addDataToArgumentsList(igs_tokenArgument_t **list, void *value, size_t size){
    igs_tokenArgument_t *new = calloc(1, sizeof(igs_tokenArgument_t));
    new->type = IGS_DATA_T;
    new->data = calloc(1, size);
    memcpy(new->data, value, size);
    new->size = size;
    LL_APPEND(*list, new);
}


int igs_initToken(const char *name, igs_tokenCallback cb, void *myData){
    igs_token_t *t = NULL;
    if(igs_internal_definition == NULL){
        igs_internal_definition = calloc(1, sizeof(definition));
    }
    HASH_FIND_STR(igs_internal_definition->tokens_table, name, t);
    if (t != NULL && t->cb != NULL){
        igs_error("token with name %s already exists and has a callback", name);
        return 0;
    }else{
        if (cb == NULL){
            igs_error("non-NULL callback is mandatory at token creation");
            return 0;
        }else{
            if (t == NULL){
                //token is completely new: allocate it
                t = calloc(1, sizeof(igs_token_t));
                if (strnlen(name, MAX_STRING_MSG_LENGTH) == MAX_STRING_MSG_LENGTH){
                    t->name = strndup(name, MAX_STRING_MSG_LENGTH);
                    igs_warn("token name has been shortened to %s", t->name);
                }else{
                    t->name = strndup(name, MAX_STRING_MSG_LENGTH);
                }
                HASH_ADD_STR(igs_internal_definition->tokens_table, name, t);
            }
            t->cb = cb;
            t->cbData = myData;
        }
    }
    return 1;
}

int igs_removeToken(const char *name){
    igs_token_t *t = NULL;
    if(igs_internal_definition == NULL){
        igs_error("No definition available yet");
        return -1;
    }
    HASH_FIND_STR(igs_internal_definition->tokens_table, name, t);
    if (t == NULL){
        igs_error("token with name %s does not exist", name);
    }else{
        HASH_DEL(igs_internal_definition->tokens_table, t);
        token_freeToken(t);
    }
    return 1;
}

int igs_addArgumentToToken(const char *tokenName, const char *argName, iopType_t type){
    igs_token_t *t = NULL;
    if(igs_internal_definition == NULL){
        igs_error("No definition available yet");
        return -1;
    }
    HASH_FIND_STR(igs_internal_definition->tokens_table, tokenName, t);
    if (type == IGS_IMPULSION_T){
        igs_error("impulsion type is not allowed as a token argument");
        return -1;
    }
    if (type == IGS_UNKNOWN_T){
        igs_error("unknown type is not allowed as a token argument");
        return -1;
    }
    if (t == NULL){
        igs_error("token with name %s does not exist", tokenName);
        return -1;
    }else{
        igs_tokenArgument_t *a = calloc(1, sizeof(igs_tokenArgument_t));
        if (strnlen(argName, MAX_STRING_MSG_LENGTH) == MAX_STRING_MSG_LENGTH){
            a->name = strndup(argName, MAX_STRING_MSG_LENGTH);
            igs_warn("token argument name has been shortened to %s", a->name);
        }else{
            a->name = strndup(argName, MAX_STRING_MSG_LENGTH);
        }
        a->size = 0;
        a->type = type;
        LL_APPEND(t->arguments, a);
    }
    return 1;
}

int igs_removeArgumentFromToken(const char *tokenName, const char *argName){
    igs_token_t *t = NULL;
    if(igs_internal_definition == NULL){
        igs_error("No definition available yet");
        return -1;
    }
    HASH_FIND_STR(igs_internal_definition->tokens_table, tokenName, t);
    if (t == NULL){
        igs_error("token with name %s does not exist", tokenName);
        return -1;
    }else{
        igs_tokenArgument_t *arg = NULL, *tmp = NULL;
        bool found = false;
        LL_FOREACH_SAFE(t->arguments, arg, tmp){
            if (strcmp(argName, arg->name) == 0){
                LL_DELETE(t->arguments, arg);
                free(arg->name);
                if (tmp->type == IGS_DATA_T && arg->data != NULL){
                    free(arg->data);
                } else if (arg->type == IGS_STRING_T && arg->data != NULL){
                    free(arg->c);
                }
                free(arg);
                found = true;
                break;
            }
        }
        if (!found){
            igs_info("no argument named %s for token %s", argName, tokenName);
        }
    }
    return 1;
}

int igs_sendToken(const char *agentNameOrUUID, const char *tokenName, igs_tokenArgument_t *list){
    if (agentNameOrUUID == NULL || strlen(agentNameOrUUID) == 0){
        igs_error("agent name or UUID must not be NULL or empty");
        return -1;
    }
    zyreAgent_t *agent = NULL, *tmp = NULL;
    bool found = false;
    HASH_ITER(hh, zyreAgents, agent, tmp){
        if (strcmp(agent->name, agentNameOrUUID) == 0 || strcmp(agent->peerId, agentNameOrUUID) == 0){
            found = true;
            //we found a matching agent
            igs_token_t *token = NULL;
            if (agent->subscriber == NULL || agent->subscriber->definition == NULL){
                igs_error("subscriber or definition is empty for %s", agentNameOrUUID);
                break;
            }
            HASH_FIND_STR(agent->subscriber->definition->tokens_table, tokenName, token);
            if (token != NULL){
                igs_tokenArgument_t *arg = NULL;
                zmsg_t *msg = zmsg_new();
                zmsg_addstr(msg, "TOKEN");
                zmsg_addstr(msg, tokenName);
                
                size_t nbArguments = 0;
                LL_COUNT(list, arg, nbArguments);
                size_t definedNbArguments = 0;
                LL_COUNT(token->arguments, arg, definedNbArguments);
                if (nbArguments != definedNbArguments){
                    igs_error("passed number of arguments is not correct (received: %zu / expected: %zu)",
                              nbArguments, definedNbArguments);
                    break;
                }
                LL_FOREACH(list, arg){
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
                zyre_whisper(agentElements->node, agent->peerId, &msg);
                igs_info("sent token %s to %s", tokenName, agentNameOrUUID);
            }else{
                igs_error("could not find token named %s for %s", tokenName, agentNameOrUUID);
            }
        }
    }
    if (!found){
        igs_error("could not find agent name or UUID: %s", agentNameOrUUID);
    }
    return 1;
}

size_t igs_getNumberOfTokens(void){
    if (igs_internal_definition == NULL)
        return 0;
    return HASH_COUNT(igs_internal_definition->tokens_table);
}

bool igs_checkTokenExistence(const char *name){
    if (igs_internal_definition == NULL)
        return false;
    igs_token_t *t = NULL;
    HASH_FIND_STR(igs_internal_definition->tokens_table, name, t);
    if (t == NULL){
        return false;
    }else{
        return true;
    }
}

char** igs_getTokensList(size_t *nbOfElements){
    if (igs_internal_definition == NULL){
        *nbOfElements = 0;
        return NULL;
    }
    size_t nb = HASH_COUNT(igs_internal_definition->tokens_table);
    if (nb == 0){
        *nbOfElements = 0;
        return NULL;
    }
    *nbOfElements = nb;
    char **res = calloc(nb, sizeof(char*));
    igs_token_t *el, *tmp;
    size_t i = 0;
    HASH_ITER(hh, igs_internal_definition->tokens_table, el, tmp){
        res[i++] = strdup(el->name);
    }
    return res;
}

void igs_freeTokensList(char **list, size_t nbOfTokens){
    if (list == NULL)
        return;
    for (size_t i = 0; i < nbOfTokens; i++){
        if (list[i] != NULL)
            free(list[i]);
    }
    free(list);
}

igs_tokenArgument_t* igs_getFirstArgumentForToken(const char *tokenName){
    if (tokenName == NULL || strlen(tokenName) == 0){
        igs_error("token name cannot be NULL or empty");
        return NULL;
    }
    if (igs_internal_definition == NULL){
        igs_error("definition is NULL");
        return NULL;
    }
    igs_token_t *t = NULL;
    HASH_FIND_STR(igs_internal_definition->tokens_table, tokenName, t);
    if (t == NULL){
        igs_info("could not find token with name %s", tokenName);
        return NULL;
    }
    return t->arguments;
}

size_t igs_getNumberOfArgumentsForToken(const char *tokenName){
    if (tokenName == NULL || strlen(tokenName) == 0){
        igs_error("token name cannot be NULL or empty");
        return 0;
    }
    if (igs_internal_definition == NULL){
        igs_error("definition is NULL");
        return 0;
    }
    igs_token_t *t = NULL;
    HASH_FIND_STR(igs_internal_definition->tokens_table, tokenName, t);
    if (t == NULL){
        igs_info("could not find token with name %s", tokenName);
        return 0;
    }
    size_t nb = 0;
    igs_tokenArgument_t *a = NULL;
    LL_COUNT(t->arguments, a, nb);
    return nb;
}

bool igs_checkTokenArgumentExistence(const char *tokenName, const char *argName){
    if (tokenName == NULL || strlen(tokenName) == 0){
        igs_error("token name cannot be NULL or empty");
        return false;
    }
    if (igs_internal_definition == NULL){
        igs_error("definition is NULL");
        return false;
    }
    igs_token_t *t = NULL;
    HASH_FIND_STR(igs_internal_definition->tokens_table, tokenName, t);
    if (t == NULL){
        igs_info("could not find token with name %s", tokenName);
        return false;
    }
    igs_tokenArgument_t *a = NULL;
    LL_FOREACH(t->arguments, a){
        if (strcmp(a->name, argName) == 0)
            return true;
    }
    return false;
}
