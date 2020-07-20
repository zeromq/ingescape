//
//  call.c
//  ingeprobe
//
//  Created by Stephane Vales on 19/07/2020.
//  Copyright © 2020 Ingenuity i/o. All rights reserved.
//

#include "call.h"
#include "yajl_tree.h"
#include <czmq.h>


iopType_t stringTypeToNumberType(const char *str){
    if (str != NULL){
        if (!strcmp(str, "INTEGER"))
            return IGS_INTEGER_T;
        if (!strcmp(str, "DOUBLE"))
            return IGS_DOUBLE_T;
        if (!strcmp(str, "STRING"))
            return IGS_STRING_T;
        if (!strcmp(str, "BOOL"))
            return IGS_BOOL_T;
        if (!strcmp(str, "IMPULSION"))
            return IGS_IMPULSION_T;
        if (!strcmp(str, "DATA"))
            return IGS_DATA_T;
    }
    return 0;
}

void parseCallsFromDefinition(const char *definition, call_t **calls){
//    zfile_t *f = zfile_new(NULL, "/Users/steph/code/i2gitlab/ingescape/ingescape/resources/example_definition.json");
//    zfile_input(f);
//    zchunk_t *chunk = zfile_read(f, zfile_size("/Users/steph/code/i2gitlab/ingescape/ingescape/resources/example_definition.json"), 0);
//    definition = zchunk_strdup(chunk);
//    zchunk_destroy(&chunk);
//    zfile_destroy(&f);
    
    igsyajl_val def = igsyajl_tree_parse(definition, NULL, 0);
    if (def != NULL){
        const char *path[] = {"definition", "calls", NULL};
        igsyajl_val mycalls = igsyajl_tree_get(def, path, igsyajl_t_array);
        for (size_t i = 0; mycalls != NULL && i < mycalls->u.array.len; i++){
            call_t *c = calloc(1, sizeof(call_t));
            const char *namePath[] = {"name", NULL};
            const char *typePath[] = {"type", NULL};
            const char *descriptionPath[] = {"description", NULL};
            const char *argsPath[] = {"arguments", NULL};
            
            igsyajl_val name = igsyajl_tree_get(mycalls->u.array.values[i], namePath, igsyajl_t_string);
            igsyajl_val description = igsyajl_tree_get(mycalls->u.array.values[i], descriptionPath, igsyajl_t_string);
            c->name = strdup(name->u.string);
            if (c->name != NULL){
                if (description == NULL){
                    c->description = NULL;
                }else{
                    strdup(description->u.string);
                }
                HASH_ADD_STR(*calls, name, c);
                printf("call %s ", c->name);
                igsyajl_val arguments = igsyajl_tree_get(mycalls->u.array.values[i], argsPath, igsyajl_t_array);
                for (size_t j = 0; arguments != NULL && j < arguments->u.array.len; j++){
                    name = igsyajl_tree_get(arguments->u.array.values[j], namePath, igsyajl_t_string);
                    if (name->u.string != NULL){
                        callArgument_t *arg = calloc(1, sizeof(callArgument_t));
                        arg->name = strdup(name->u.string);
                        igsyajl_val type = igsyajl_tree_get(arguments->u.array.values[j], typePath, igsyajl_t_string);
                        arg->type = stringTypeToNumberType(type->u.string);
                        if (arg->name != NULL && arg->type > 0){
                            LL_APPEND(c->arguments, arg);
                            printf("| %s (%d)", arg->name, arg->type);
                        }else{
                            if (arg->name != NULL)
                                free(arg->name);
                            free(arg);
                        }
                    }
                }
                printf("\n");
            }else{
                free(c);
            }
            
        }
    }
    igsyajl_tree_free(def);
}

void freeCalls(call_t **calls){
    if (!calls || !(*calls)){
        return;
    }
    call_t *call, *tmp;
    HASH_ITER(hh, *calls, call, tmp){
        free(call->name);
        if (call->description != NULL)
            free(call->description);
        callArgument_t *argu, *argtmp;
        DL_FOREACH_SAFE(call->arguments, argu, argtmp){
            free(argu->name);
            free(argu);
        }
        free(call);
    }
    *calls = NULL;
}

bool addArgumentsToCallMessage(zmsg_t *msg, call_t *call, const char *arguments){
    assert(msg);
    assert(call);
    assert(arguments);
    assert(strncmp(arguments, call->name, strlen(call->name)) == 0); //check call name consistency
    zmsg_addstr(msg, call->name);
    
    void *limit = (void *)(arguments + strlen(arguments));
    //set arguments after call name
    arguments = arguments + strlen(call->name) + 1;
    callArgument_t *arg = NULL;
    char buffer[4096] = "";
    int myInt = 0;
    double myDouble = 0;
    LL_FOREACH(call->arguments, arg){
        zframe_t *frame = NULL;
        size_t offset = 0;
        while (arguments[offset] != ' ' && arguments[offset] != '\0') {
            offset++;
        }
        strncpy(buffer, arguments, offset);
        buffer[offset] = '\0';
        if (strlen(buffer) == 0){
            printf("error: call misses arguments\n");
            return false;
        }
        switch (arg->type) {
            case IGS_BOOL_T:
                myInt = atoi(buffer);
                frame = zframe_new(&myInt, sizeof(int));
                break;
            case IGS_INTEGER_T:
                myInt = atoi(buffer);
                frame = zframe_new(&myInt, sizeof(int));
                break;
            case IGS_DOUBLE_T:
                myDouble = atof(buffer);
                frame = zframe_new(&myDouble, sizeof(double));
                break;
            case IGS_STRING_T:{
                if (strlen(buffer) > 0){
                    frame = zframe_new(buffer, strlen(buffer)+1);
                }else{
                    frame = zframe_new(NULL, 0);
                }
                break;
            }
                
            case IGS_DATA_T:
                frame = zframe_new(NULL, 0);
                break;
                
            default:
                break;
        }
        if (frame != NULL){
            zmsg_add(msg, frame);
        }
        if ((void *)(arguments + offset) > limit){
            printf("error: call misses arguments\n");
            return false;
        }else{
            arguments = arguments + offset + 1;
        }
    }
    if ((void *) arguments == limit || (void *) arguments == limit + 1){
        //we consumed the whole args string
        //printf("limit reached\n");
    }else{
        //we did not consume the whole args string
        printf("passed arguments are too long : '%s' was not used\n", arguments);
    }
    return true;
}
