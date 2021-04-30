//
//  igs_parser.c
//
//  Created by Stephane Vales
//  Modified by Patxi Berard
//  Copyright © 2017 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#if (defined WIN32 || defined _WIN32)
#include "unixfunctions.h"
#endif

#include "yajl_tree.h"
#include "yajl_gen.h"
#include "ingescape_private.h"

#define STR_DEFINITION "definition"
#define STR_NAME "name"
#define STR_FAMILY "family"
#define STR_DESCRIPTION "description"
#define STR_VERSION "version"
#define STR_PARAMETERS "parameters"
#define STR_OUTPUTS "outputs"
#define STR_INPUTS "inputs"
#define STR_CALLS "calls"
#define STR_ARGUMENTS "arguments"
#define STR_REPLY "reply"
#define STR_TYPE "type"
#define STR_VALUE "value"

iopType_t string_to_value_type(const char* str) {
    if (str){
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
        if (!strcmp(str, "UNKNOWN"))
            return IGS_UNKNOWN_T;
    }
    igs_error("unknown value type \"%s\" to convert, returned IGS_UNKNOWN_T", str);
    return IGS_UNKNOWN_T;
}

bool string_to_boolean(const char* str) {
    if(str){
        if (!strcmp(str, "true"))
            return true;
        if (!strcmp(str, "false"))
            return false;
    }
    igs_warn("unknown string \"%s\" to convert, returned false by default", str);
    return false;
}

const char* value_type_to_string (iopType_t type) {
    switch (type) {
        case IGS_INTEGER_T:
            return "INTEGER";
            break;
        case IGS_DOUBLE_T:
            return "DOUBLE";
            break;
        case IGS_STRING_T:
            return "STRING";
            break;
        case IGS_BOOL_T:
            return "BOOL";
            break;
        case IGS_IMPULSION_T:
            return "IMPULSION";
            break;
        case IGS_DATA_T:
            return "DATA";
            break;
        case IGS_UNKNOWN_T:
            return "UNKNOWN";
            break;
        default:
            igs_error("unknown iopType_t to convert");
            break;
    }
    return NULL;
}

//
//Definition parsing
//
igs_definition_t* parser_parseDefinitionFromNode(igsJSONTreeNode_t **json){
    assert(json);
    assert(*json);
    igs_definition_t *definition = NULL;
    const char *descriptionPath[] = {"definition","description",NULL};
    const char *versionPath[] = {"definition","version",NULL};
    const char *inputsPath[] = {"definition","inputs",NULL};
    const char *outputsPath[] = {"definition","outputs",NULL};
    const char *parametersPath[] = {"definition","parameters",NULL};
    const char *callsPath[] = {"definition","calls",NULL};
    const char *argumentsPath[] = {"arguments",NULL};
    const char *agentNamePath[] = {"definition","name",NULL};
    const char *namePath[] = {"name",NULL};
    const char *familyPath[] = {"definition","family",NULL};
    const char *typePath[] = {"type",NULL};
    const char *valuePath[] = {"value",NULL};
    const char *replyPath[] = {"reply",NULL};
    
    //name is mandatory
    igsJSONTreeNode_t *name = igs_JSONTreeGetNodeAtPath(*json, agentNamePath);
    if (name && name->type == IGS_JSON_STRING & name->u.string != NULL){
        char *n = strndup(name->u.string, IGS_MAX_AGENT_NAME_LENGTH);
        if (strlen(name->u.string) > IGS_MAX_AGENT_NAME_LENGTH){
            igs_warn("definition name '%s' exceeds maximum size and will be truncated to '%s'", name->u.string, n);
        }
        bool spaceInName = false;
        size_t lengthOfN = strlen(n);
        size_t i = 0;
        for (i = 0; i < lengthOfN; i++){
            if (n[i] == ' '){
                n[i] = '_';
                spaceInName = true;
            }
        }
        if (spaceInName)
            igs_warn("spaces are not allowed in definition name: '%s' has been changed to '%s'", name->u.string, n);
        definition = (igs_definition_t*) calloc (1, sizeof(igs_definition_t));
        definition->name = n;
        
    }else
        return NULL;
    
    //family
    igsJSONTreeNode_t *family = igs_JSONTreeGetNodeAtPath(*json, familyPath);
    if (family && family->type == IGS_JSON_STRING & family->u.string != NULL)
        definition->family = strdup(family->u.string);
        
    //description
    igsJSONTreeNode_t *description = igs_JSONTreeGetNodeAtPath(*json, descriptionPath);
    if (description && description->type == IGS_JSON_STRING & description->u.string != NULL)
        definition->description = strndup(description->u.string, IGS_MAX_DESCRIPTION_LENGTH);
    
    //version
    igsJSONTreeNode_t *version = igs_JSONTreeGetNodeAtPath(*json, versionPath);
    if (version && version->type == IGS_JSON_STRING & version->u.string != NULL)
        definition->version = strdup(version->u.string);
    
    //inputs
    igsJSONTreeNode_t *inputs = igs_JSONTreeGetNodeAtPath(*json, inputsPath);
    if (inputs && inputs->type == IGS_JSON_ARRAY){
        for (size_t i = 0; i < inputs->u.array.len; i++){
            igsJSONTreeNode_t *iop_name = igs_JSONTreeGetNodeAtPath(inputs->u.array.values[i], namePath);
            if (iop_name && iop_name->type == IGS_JSON_STRING & iop_name->u.string != NULL){
                igs_iop_t *iop = NULL;
                char *correctedName = strndup(iop_name->u.string, IGS_MAX_IOP_NAME_LENGTH);
                bool spaceInName = false;
                size_t lengthOfN = strlen(correctedName);
                size_t k = 0;
                for (k = 0; k < lengthOfN; k++){
                    if (correctedName[k] == ' '){
                        correctedName[k] = '_';
                        spaceInName = true;
                    }
                }
                if (spaceInName){
                    igs_warn("Spaces are not allowed in IOP name: %s has been renamed to %s",
                             iop_name->u.string, correctedName);
                }
                HASH_FIND_STR(definition->inputs_table, correctedName, iop);
                if (iop){
                    igs_warn("input with name '%s' already exists : ignoring new one", correctedName);
                    free(correctedName);
                    continue; //iop with this name already exists
                }
                
                iop = (igs_iop_t *) calloc(1, sizeof(igs_iop_t));
                iop->type = IGS_INPUT_T;
                iop->value_type = IGS_UNKNOWN_T;
                iop->name = correctedName;
                
                igsJSONTreeNode_t *iop_type = igs_JSONTreeGetNodeAtPath(inputs->u.array.values[i], typePath);
                if (iop_type && iop_type->type == IGS_JSON_STRING & iop_type->u.string != NULL){
                    iop->value_type = string_to_value_type(iop_type->u.string);
                }
                //NB: inputs do not have initial value in definition => nothing to do here
                HASH_ADD_STR(definition->inputs_table, name, iop);
            }
        }
    } else if (inputs){
        igs_error("inputs are not an array : ignoring");
    }
    
    //outputs
    igsJSONTreeNode_t *outputs = igs_JSONTreeGetNodeAtPath(*json, outputsPath);
    if (outputs && outputs->type == IGS_JSON_ARRAY){
        for (size_t i = 0; i < outputs->u.array.len; i++){
            igsJSONTreeNode_t *iop_name = igs_JSONTreeGetNodeAtPath(outputs->u.array.values[i], namePath);
            if (iop_name && iop_name->type == IGS_JSON_STRING & iop_name->u.string != NULL){
                igs_iop_t *iop = NULL;
                char *correctedName = strndup(iop_name->u.string, IGS_MAX_IOP_NAME_LENGTH);
                bool spaceInName = false;
                size_t lengthOfN = strlen(correctedName);
                size_t k = 0;
                for (k = 0; k < lengthOfN; k++){
                    if (correctedName[k] == ' '){
                        correctedName[k] = '_';
                        spaceInName = true;
                    }
                }
                if (spaceInName){
                    igs_warn("Spaces are not allowed in IOP name: %s has been renamed to %s",
                             iop_name->u.string, correctedName);
                }
                HASH_FIND_STR(definition->outputs_table, correctedName, iop);
                if (iop){
                    igs_warn("output with name '%s' already exists : ignoring new one", correctedName);
                    free(correctedName);
                    continue; //iop with this name already exists
                }
                
                iop = (igs_iop_t *) calloc(1, sizeof(igs_iop_t));
                iop->type = IGS_OUTPUT_T;
                iop->value_type = IGS_UNKNOWN_T;
                iop->name = correctedName;
                
                igsJSONTreeNode_t *iop_type = igs_JSONTreeGetNodeAtPath(outputs->u.array.values[i], typePath);
                if (iop_type && iop_type->type == IGS_JSON_STRING & iop_type->u.string != NULL){
                    iop->value_type = string_to_value_type(iop_type->u.string);
                }
                
                igsJSONTreeNode_t *iop_value = igs_JSONTreeGetNodeAtPath(outputs->u.array.values[i], valuePath);
                if (iop_value){
                    switch (iop->value_type) {
                        case IGS_INTEGER_T:
                            iop->value.i =(int) IGSYAJL_GET_INTEGER (iop_value);
                            break;
                        case IGS_DOUBLE_T:
                            iop->value.d = IGSYAJL_GET_DOUBLE (iop_value);
                            break;
                        case IGS_BOOL_T:
                            if(iop_value->type == IGS_JSON_TRUE)
                                iop->value.b = true;
                            else if (iop_value->type == IGS_JSON_TRUE)
                                iop->value.b = false;
                            else if (iop_value->type == IGS_JSON_STRING)
                                iop->value.b = string_to_boolean(iop_value->u.string);
                            break;
                        case IGS_STRING_T:
                            iop->value.s =  (IGSYAJL_IS_STRING(iop_value) ? strdup(iop_value->u.string) : NULL);
                            break;
                        case IGS_IMPULSION_T:
                            //IMPULSION has no value
                            break;
                        case IGS_DATA_T:
                            //we store data as hexa string but we convert it to actual bytes
                            iop->value.data = (IGSYAJL_IS_STRING(iop_value) ? model_stringToBytes(iop_value->u.string) : NULL);
                            iop->valueSize = (iop->value.data)? strlen(iop_value->u.string)/2 : 0;
                            break;
                        default:
                            break;
                    }
                }
                HASH_ADD_STR(definition->outputs_table, name, iop);
            }
        }
    } else if (outputs){
        igs_error("outputs are not an array : ignoring");
    }
    
    //parameters
    igsJSONTreeNode_t *parameters = igs_JSONTreeGetNodeAtPath(*json, parametersPath);
    if (parameters && parameters->type == IGS_JSON_ARRAY){
        for (size_t i = 0; i < parameters->u.array.len; i++){
            igsJSONTreeNode_t *iop_name = igs_JSONTreeGetNodeAtPath(parameters->u.array.values[i], namePath);
            if (iop_name && iop_name->type == IGS_JSON_STRING & iop_name->u.string != NULL){
                igs_iop_t *iop = NULL;
                char *correctedName = strndup(iop_name->u.string, IGS_MAX_IOP_NAME_LENGTH);
                bool spaceInName = false;
                size_t lengthOfN = strlen(correctedName);
                size_t k = 0;
                for (k = 0; k < lengthOfN; k++){
                    if (correctedName[k] == ' '){
                        correctedName[k] = '_';
                        spaceInName = true;
                    }
                }
                if (spaceInName){
                    igs_warn("Spaces are not allowed in IOP name: %s has been renamed to %s",
                             iop_name->u.string, correctedName);
                }
                HASH_FIND_STR(definition->params_table, correctedName, iop);
                if (iop){
                    igs_warn("parameter with name '%s' already exists : ignoring new one", correctedName);
                    free(correctedName);
                    continue; //iop with this name already exists
                }
                
                iop = (igs_iop_t *) calloc(1, sizeof(igs_iop_t));
                iop->type = IGS_PARAMETER_T;
                iop->value_type = IGS_UNKNOWN_T;
                iop->name = correctedName;
                
                igsJSONTreeNode_t *iop_type = igs_JSONTreeGetNodeAtPath(parameters->u.array.values[i], typePath);
                if (iop_type && iop_type->type == IGS_JSON_STRING & iop_type->u.string != NULL){
                    iop->value_type = string_to_value_type(iop_type->u.string);
                }
                
                igsJSONTreeNode_t *iop_value = igs_JSONTreeGetNodeAtPath(parameters->u.array.values[i], valuePath);
                if (iop_value){
                    switch (iop->value_type) {
                        case IGS_INTEGER_T:
                            iop->value.i =(int) IGSYAJL_GET_INTEGER (iop_value);
                            break;
                        case IGS_DOUBLE_T:
                            iop->value.d = IGSYAJL_GET_DOUBLE (iop_value);
                            break;
                        case IGS_BOOL_T:
                            if(iop_value->type == IGS_JSON_TRUE)
                                iop->value.b = true;
                            else if (iop_value->type == IGS_JSON_TRUE)
                                iop->value.b = false;
                            else if (iop_value->type == IGS_JSON_STRING)
                                iop->value.b = string_to_boolean(iop_value->u.string);
                            break;
                        case IGS_STRING_T:
                            iop->value.s =  (IGSYAJL_IS_STRING(iop_value) ? strdup(iop_value->u.string) : NULL);
                            break;
                        case IGS_IMPULSION_T:
                            //IMPULSION has no value
                            break;
                        case IGS_DATA_T:
                            //we store data as hexa string but we convert it to actual bytes
                            iop->value.data = (IGSYAJL_IS_STRING(iop_value) ? model_stringToBytes(iop_value->u.string) : NULL);
                            break;
                        default:
                            break;
                    }
                }
                HASH_ADD_STR(definition->params_table, name, iop);
            }
        }
    } else if (parameters){
        igs_error("parameters are not an array : ignoring");
    }
    
    //calls
    igsJSONTreeNode_t *calls = igs_JSONTreeGetNodeAtPath(*json, callsPath);
    if (calls && calls->type == IGS_JSON_ARRAY){
        for (size_t i = 0; i < calls->u.array.len; i++){
            igsJSONTreeNode_t *call_name = igs_JSONTreeGetNodeAtPath(calls->u.array.values[i], namePath);
            if (call_name && call_name->type == IGS_JSON_STRING & call_name->u.string != NULL){
                igs_call_t *myCall = NULL;
                char *correctedName = strndup(call_name->u.string, IGS_MAX_IOP_NAME_LENGTH);
                bool spaceInName = false;
                size_t lengthOfN = strlen(correctedName);
                size_t k = 0;
                for (k = 0; k < lengthOfN; k++){
                    if (correctedName[k] == ' '){
                        correctedName[k] = '_';
                        spaceInName = true;
                    }
                }
                if (spaceInName){
                    igs_warn("Spaces are not allowed in call name: %s has been renamed to %s",
                             call_name->u.string, correctedName);
                }
                HASH_FIND_STR(definition->calls_table, correctedName, myCall);
                if (myCall){
                    igs_warn("call with name '%s' already exists : ignoring new one", correctedName);
                    free(correctedName);
                    continue; //call with this name already exists
                }
                
                myCall = (igs_call_t *) calloc(1, sizeof(igs_call_t));
                myCall->name = correctedName;
                
                description = igs_JSONTreeGetNodeAtPath(calls->u.array.values[i], descriptionPath);
                if (description && description->type == IGS_JSON_STRING & description->u.string != NULL){
                    myCall->description = strdup(description->u.string);
                }
                
                igsJSONTreeNode_t *arguments = igs_JSONTreeGetNodeAtPath(calls->u.array.values[i], argumentsPath);
                if (arguments && arguments->type == IGS_JSON_ARRAY){
                    for (size_t j = 0; j < arguments->u.array.len; j++){
                        if (arguments->u.array.values[j] && arguments->u.array.values[j]->type == IGS_JSON_MAP){
                            igsJSONTreeNode_t *argName = igs_JSONTreeGetNodeAtPath(arguments->u.array.values[j], namePath);
                            if (argName && argName->type == IGS_JSON_STRING && argName->u.string){
                                char *correctedName = strndup(argName->u.string, IGS_MAX_IOP_NAME_LENGTH);
                                bool spaceInName = false;
                                size_t lengthOfN = strlen(correctedName);
                                size_t k = 0;
                                for (k = 0; k < lengthOfN; k++){
                                    if (correctedName[k] == ' '){
                                        correctedName[k] = '_';
                                        spaceInName = true;
                                    }
                                }
                                if (spaceInName){
                                    igs_warn("Spaces are not allowed in call argument name: %s has been renamed to %s",
                                             argName->u.string, correctedName);
                                }
                                igs_callArgument_t *newArg = (igs_callArgument_t*) calloc(1, sizeof(igs_callArgument_t));
                                newArg->name = correctedName;
                                igsJSONTreeNode_t *argType = igs_JSONTreeGetNodeAtPath(arguments->u.array.values[j], typePath);
                                if (argType && argType->type == IGS_JSON_STRING && argType->u.string){
                                    newArg->type = string_to_value_type(argType->u.string);
                                }
                                LL_APPEND(myCall->arguments, newArg);
                            }
                        }
                    }
                }
                
                igsJSONTreeNode_t *reply = igs_JSONTreeGetNodeAtPath(calls->u.array.values[i], replyPath);
                if (reply && reply->type == IGS_JSON_MAP){
                    igsJSONTreeNode_t *replyName = igs_JSONTreeGetNodeAtPath(reply, namePath);
                    if (replyName && replyName->type == IGS_JSON_STRING && replyName->u.string){
                        char *correctedName = strndup(replyName->u.string, IGS_MAX_IOP_NAME_LENGTH);
                        bool spaceInName = false;
                        size_t lengthOfN = strlen(correctedName);
                        size_t k = 0;
                        for (k = 0; k < lengthOfN; k++){
                            if (correctedName[k] == ' '){
                                correctedName[k] = '_';
                                spaceInName = true;
                            }
                        }
                        if (spaceInName){
                            igs_warn("Spaces are not allowed in call argument name: %s has been renamed to %s",
                                     replyName->u.string, correctedName);
                        }
                        igs_call_t *myReply = (igs_call_t *) calloc(1, sizeof(igs_call_t));
                        myReply->name = correctedName;
                        
                        arguments = igs_JSONTreeGetNodeAtPath(reply, argumentsPath);
                        if (arguments && arguments->type == IGS_JSON_ARRAY){
                            for (size_t j = 0; j < arguments->u.array.len; j++){
                                if (arguments->u.array.values[j] && arguments->u.array.values[j]->type == IGS_JSON_MAP){
                                    igsJSONTreeNode_t *argName = igs_JSONTreeGetNodeAtPath(arguments->u.array.values[j], namePath);
                                    if (argName && argName->type == IGS_JSON_STRING && argName->u.string){
                                        char *correctedName = strndup(argName->u.string, IGS_MAX_IOP_NAME_LENGTH);
                                        bool spaceInName = false;
                                        size_t lengthOfN = strlen(correctedName);
                                        size_t k = 0;
                                        for (k = 0; k < lengthOfN; k++){
                                            if (correctedName[k] == ' '){
                                                correctedName[k] = '_';
                                                spaceInName = true;
                                            }
                                        }
                                        if (spaceInName){
                                            igs_warn("Spaces are not allowed in call argument name: %s has been renamed to %s",
                                                     argName->u.string, correctedName);
                                        }
                                        igs_callArgument_t *newArg = (igs_callArgument_t*) calloc(1, sizeof(igs_callArgument_t));
                                        newArg->name = correctedName;
                                        igsJSONTreeNode_t *argType = igs_JSONTreeGetNodeAtPath(arguments->u.array.values[j], typePath);
                                        if (argType && argType->type == IGS_JSON_STRING && argType->u.string){
                                            newArg->type = string_to_value_type(argType->u.string);
                                        }
                                        LL_APPEND(myReply->arguments, newArg);
                                    }
                                }
                            }
                        }
                        myCall->reply = myReply;
                    }
                }

                HASH_ADD_STR(definition->calls_table, name, myCall);
            }
        }
    } else if (calls){
        igs_error("calls are not an array : ignoring");
    }
    
    igs_JSONTreeFree(json);
    return definition;
}

//
// Mapping parsing
//
igs_mapping_t* parser_parseMappingFromNode(igsJSONTreeNode_t **json){
    assert(json);
    assert(*json);
    igs_mapping_t *mapping = NULL;
    const char *mappingsPath[] = {"mappings",NULL};
    //const char *fromAgentPath[] = {"fromAgent",NULL};
    const char *fromInputPath[] = {"fromInput",NULL};
    const char *toAgentPath[] = {"toAgent",NULL};
    const char *toOutputPath[] = {"toOutput",NULL};
    const char *alternateMappingsPath[] = {"mapping","mapping_out",NULL};
    const char *alternateFromInputPath[] = {"input_name",NULL};
    const char *alternateToAgentPath[] = {"agent_name",NULL};
    const char *alternateToOutputPath[] = {"output_name",NULL};
    
    bool useAlternate = false;
    igsJSONTreeNode_t *mappings = igs_JSONTreeGetNodeAtPath(*json, mappingsPath);
    if (mappings == NULL){
        mappings = igs_JSONTreeGetNodeAtPath(*json, alternateMappingsPath);
        if (mappings && mappings->type == IGS_JSON_ARRAY)
            useAlternate = true;
        else
            return NULL;
    }
    
    mapping = (igs_mapping_t*) calloc(1, sizeof(igs_mapping_t));
    
    //FIXME: we will not use fromAgent in parsing because the received
    //mappings should all imply our inputs. In the future, we could
    //check fromAgent to ensure this is us but this requires changing the
    //internal API to attach parsing to a specific agent instance.
    for (size_t i = 0; i < mappings->u.array.len; i++){
        if (mappings->u.array.values[i]->type != IGS_JSON_MAP)
            continue;
        char *fromInput = NULL;
        char *toAgent = NULL;
        char *toOutput = NULL;
        igsJSONTreeNode_t *fromInputNode = NULL;
        igsJSONTreeNode_t *toAgentNode = NULL;
        igsJSONTreeNode_t *toOutputNode = NULL;
        if (!useAlternate){
            fromInputNode = igs_JSONTreeGetNodeAtPath(mappings->u.array.values[i], fromInputPath);
            toAgentNode = igs_JSONTreeGetNodeAtPath(mappings->u.array.values[i], toAgentPath);
            toOutputNode = igs_JSONTreeGetNodeAtPath(mappings->u.array.values[i], toOutputPath);
        }else{
            fromInputNode = igs_JSONTreeGetNodeAtPath(mappings->u.array.values[i], alternateFromInputPath);
            toAgentNode = igs_JSONTreeGetNodeAtPath(mappings->u.array.values[i], alternateToAgentPath);
            toOutputNode = igs_JSONTreeGetNodeAtPath(mappings->u.array.values[i], alternateToOutputPath);
        }
        if (fromInputNode && fromInputNode->type == IGS_JSON_STRING && fromInputNode->u.string){
            char *correctedName = strndup(fromInputNode->u.string, IGS_MAX_IOP_NAME_LENGTH);
            bool spaceInName = false;
            size_t lengthOfN = strlen(correctedName);
            size_t k = 0;
            for (k = 0; k < lengthOfN; k++){
                if (correctedName[k] == ' '){
                    correctedName[k] = '_';
                    spaceInName = true;
                }
            }
            if (spaceInName){
                igs_warn("Spaces are not allowed in mapping element name: %s has been renamed to %s",
                         fromInputNode->u.string, correctedName);
            }
            fromInput = correctedName;
        }
        if (toAgentNode && toAgentNode->type == IGS_JSON_STRING && toAgentNode->u.string){
            char *correctedName = strndup(toAgentNode->u.string, IGS_MAX_IOP_NAME_LENGTH);
            bool spaceInName = false;
            size_t lengthOfN = strlen(correctedName);
            size_t k = 0;
            for (k = 0; k < lengthOfN; k++){
                if (correctedName[k] == ' '){
                    correctedName[k] = '_';
                    spaceInName = true;
                }
            }
            if (spaceInName){
                igs_warn("Spaces are not allowed in mapping element name: %s has been renamed to %s",
                         toAgentNode->u.string, correctedName);
            }
            toAgent = correctedName;
        }
        if (toOutputNode && toOutputNode->type == IGS_JSON_STRING && toOutputNode->u.string){
            char *correctedName = strndup(toOutputNode->u.string, IGS_MAX_IOP_NAME_LENGTH);
            bool spaceInName = false;
            size_t lengthOfN = strlen(correctedName);
            size_t k = 0;
            for (k = 0; k < lengthOfN; k++){
                if (correctedName[k] == ' '){
                    correctedName[k] = '_';
                    spaceInName = true;
                }
            }
            if (spaceInName){
                igs_warn("Spaces are not allowed in mapping element name: %s has been renamed to %s",
                         toOutputNode->u.string, correctedName);
            }
            toOutput = correctedName;
        }
        if (fromInput && toAgent && toOutput){
            
            size_t len = strlen(fromInput)+strlen(toAgent)+strlen(toOutput)+3+1;
            char *mashup = calloc(1, len*sizeof(char));
            strcpy(mashup, fromInput);
            strcat(mashup, ".");//separator
            strcat(mashup, toAgent);
            strcat(mashup, ".");//separator
            strcat(mashup, toOutput);
            mashup[len -1] = '\0';
            unsigned long h = djb2_hash((unsigned char *)mashup);
            free(mashup);
            
            igs_mapping_element_t *tmp = NULL;
            HASH_FIND(hh, mapping->map_elements, &h, sizeof(unsigned long), tmp);
            if (tmp == NULL){
                //element does not exist yet : create and register it
                igs_mapping_element_t *new = mapping_createMappingElement(fromInput, toAgent, toOutput);
                new->id = h;
                HASH_ADD(hh, mapping->map_elements, id, sizeof(unsigned long), new);
            }else{
                igs_error("hash already exists for %s->%s.%s", fromInput, toAgent, toOutput);
            }
        }
        if (fromInput)
            free(fromInput);
        if (toAgent)
            free(toAgent);
        if (toOutput)
            free(toOutput);
    }
    
    igs_JSONTreeFree(json);
    return mapping;
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
igs_definition_t* parser_loadDefinition (const char* json_str) {
    assert(json_str);
    igsJSONTreeNode_t *json = igs_JSONTreeParseFromString(json_str);
    if (!json){
        igs_error("could not parse JSON string : '%s'", json_str);
        return NULL;
    }
    if (json->type != IGS_JSON_MAP){
        igs_error("parsed JSON is not an array : '%s'", json_str);
        return NULL;
    }
    return parser_parseDefinitionFromNode(&json);
}


igs_definition_t * parser_loadDefinitionFromPath (const char* path) {
    assert(path);
    igsJSONTreeNode_t *json = igs_JSONTreeParseFromFile(path);
    if (!json){
        igs_error("could not parse JSON file '%s'", path);
        return NULL;
    }
    if (json->type != IGS_JSON_MAP){
        igs_error("parsed JSON at '%s' is not an array", path);
        return NULL;
    }
    return parser_parseDefinitionFromNode(&json);
}


igs_mapping_t* parser_loadMapping(const char* json_str){
    assert(json_str);
    igsJSONTreeNode_t *json = igs_JSONTreeParseFromString(json_str);
    if (!json){
        igs_error("could not parse JSON string : '%s'", json_str);
        return NULL;
    }
    if (json->type != IGS_JSON_MAP){
        igs_error("parsed JSON is not an array : '%s'", json_str);
        return NULL;
    }
    return parser_parseMappingFromNode(&json);
}


igs_mapping_t* parser_loadMappingFromPath (const char* path){
    assert(path);
    igsJSONTreeNode_t *json = igs_JSONTreeParseFromFile(path);
    if (!json){
        igs_error("could not parse JSON file '%s'", path);
        return NULL;
    }
    if (json->type != IGS_JSON_MAP){
        igs_error("parsed JSON at '%s' is not an array", path);
        return NULL;
    }
    return parser_parseMappingFromNode(&json);
}


char* parser_exportDefinition(igs_definition_t* def){
    assert(def);
    igsJSON_t json = igs_JSONinit();
    igs_JSONopenMap(json);
    igs_JSONaddString(json, STR_DEFINITION);
    igs_JSONopenMap(json);
    if (def->name){
        igs_JSONaddString(json, STR_NAME);
        igs_JSONaddString(json, def->name);
    }
    if (def->family){
        igs_JSONaddString(json, STR_FAMILY);
        igs_JSONaddString(json, def->family);
    }
    if (def->description){
        igs_JSONaddString(json, STR_DESCRIPTION);
        igs_JSONaddString(json, def->description);
    }
    if (def->version){
        igs_JSONaddString(json, STR_VERSION);
        igs_JSONaddString(json, def->version);
    }
    
    igs_JSONaddString(json, STR_INPUTS);
    igs_JSONopenArray(json);
    igs_iop_t *iop, *tmpIop;
    HASH_ITER(hh, def->inputs_table, iop, tmpIop){
        igs_JSONopenMap(json);
        if (iop->name){
            igs_JSONaddString(json, STR_NAME);
            igs_JSONaddString(json, iop->name);
        }
        igs_JSONaddString(json, STR_TYPE);
        igs_JSONaddString(json, value_type_to_string(iop->value_type));
        //NB: inputs do not have intial values
        igs_JSONcloseMap(json);
    }
    igs_JSONcloseArray(json);
    
    igs_JSONaddString(json, STR_OUTPUTS);
    igs_JSONopenArray(json);
    HASH_ITER(hh, def->outputs_table, iop, tmpIop){
        igs_JSONopenMap(json);
        if (iop->name){
            igs_JSONaddString(json, STR_NAME);
            igs_JSONaddString(json, iop->name);
        }
        igs_JSONaddString(json, STR_TYPE);
        igs_JSONaddString(json, value_type_to_string(iop->value_type));
        igs_JSONaddString(json, STR_VALUE);
        switch (iop->value_type) {
            case IGS_INTEGER_T:
                igs_JSONaddInt(json, iop->value.i);
                break;
            case IGS_DOUBLE_T:
                igs_JSONaddDouble(json, iop->value.d);
                break;
            case IGS_BOOL_T:
                igs_JSONaddBool(json, iop->value.b);
                break;
            case IGS_STRING_T:
                igs_JSONaddString(json, iop->value.s);
                break;
            case IGS_IMPULSION_T:
                igs_JSONaddNULL(json);
                break;
            case IGS_DATA_T:{
                char *dataToStore = (char *)calloc(2 * iop->valueSize + 1, sizeof(char));
                for (size_t i = 0; i < iop->valueSize; i++)
                    sprintf(dataToStore + 2*i, "%02X", *((uint8_t *)((char *)iop->value.data +i)));
                igs_JSONaddString(json, dataToStore);
                free(dataToStore);
                break;
            }
            default:
                igs_JSONaddString(json, "");
                break;
        }
        igs_JSONcloseMap(json);
    }
    igs_JSONcloseArray(json);
    
    igs_JSONaddString(json, STR_PARAMETERS);
    igs_JSONopenArray(json);
    HASH_ITER(hh, def->params_table, iop, tmpIop){
        igs_JSONopenMap(json);
        if (iop->name){
            igs_JSONaddString(json, STR_NAME);
            igs_JSONaddString(json, iop->name);
        }
        igs_JSONaddString(json, STR_TYPE);
        igs_JSONaddString(json, value_type_to_string(iop->value_type));
        igs_JSONaddString(json, STR_VALUE);
        switch (iop->value_type) {
            case IGS_INTEGER_T:
                igs_JSONaddInt(json, iop->value.i);
                break;
            case IGS_DOUBLE_T:
                igs_JSONaddDouble(json, iop->value.d);
                break;
            case IGS_BOOL_T:
                igs_JSONaddBool(json, iop->value.b);
                break;
            case IGS_STRING_T:
                igs_JSONaddString(json, iop->value.s);
                break;
            case IGS_IMPULSION_T:
                igs_JSONaddNULL(json);
                break;
            case IGS_DATA_T:{
                char *dataToStore = (char *)calloc(2 * iop->valueSize + 1, sizeof(char));
                for (size_t i = 0; i < iop->valueSize; i++)
                    sprintf(dataToStore + 2*i, "%02X", *((uint8_t *)((char *)iop->value.data +i)));
                igs_JSONaddString(json, dataToStore);
                free(dataToStore);
                break;
            }
            default:
                igs_JSONaddString(json, "");
                break;
        }
        igs_JSONcloseMap(json);
    }
    igs_JSONcloseArray(json);
    
    igs_JSONaddString(json, STR_CALLS);
    igs_JSONopenArray(json);
    igs_call_t *call, *tmpCall;
    HASH_ITER(hh, def->calls_table, call, tmpCall){
        igs_JSONopenMap(json);
        if (call->name){
            igs_JSONaddString(json, STR_NAME);
            igs_JSONaddString(json, call->name);
            if (call->description){
                igs_JSONaddString(json, STR_DESCRIPTION);
                igs_JSONaddString(json, call->description);
            }
            
            if (call->arguments){
                igs_JSONaddString(json, STR_ARGUMENTS);
                igs_JSONopenArray(json);
                igs_callArgument_t *argument = NULL;
                LL_FOREACH(call->arguments, argument){
                    if (argument->name){
                        igs_JSONopenMap(json);
                        igs_JSONaddString(json, STR_NAME);
                        igs_JSONaddString(json, argument->name);
                        igs_JSONaddString(json, STR_TYPE);
                        igs_JSONaddString(json, value_type_to_string(argument->type));
                        igs_JSONcloseMap(json);
                    }
                }
                igs_JSONcloseArray(json);
            }
            
            if (call->reply){
                if (call->reply->name){
                    igs_JSONaddString(json, STR_REPLY);
                    igs_JSONopenMap(json);
                    igs_JSONaddString(json, STR_NAME);
                    igs_JSONaddString(json, call->reply->name);
                    if (call->reply->description){
                        igs_JSONaddString(json, STR_DESCRIPTION);
                        igs_JSONaddString(json, call->reply->description);
                    }
                    
                    if (call->reply->arguments){
                        igs_JSONaddString(json, STR_ARGUMENTS);
                        igs_JSONopenArray(json);
                        igs_callArgument_t *argument = NULL;
                        LL_FOREACH(call->reply->arguments, argument){
                            if (argument->name){
                                igs_JSONopenMap(json);
                                igs_JSONaddString(json, STR_NAME);
                                igs_JSONaddString(json, argument->name);
                                igs_JSONaddString(json, STR_TYPE);
                                igs_JSONaddString(json, value_type_to_string(argument->type));
                                igs_JSONcloseMap(json);
                            }
                        }
                        igs_JSONcloseArray(json);
                    }
                    igs_JSONcloseMap(json);
                }
            }
        }
        igs_JSONcloseMap(json);
    }
    igs_JSONcloseArray(json);
    
    
    igs_JSONcloseMap(json);
    igs_JSONcloseMap(json);
    char *res = igs_JSONdump(json);
    igs_JSONfree(&json);
    return res;
}

char* parser_exportMapping(igs_mapping_t *mapping){
    assert(mapping);
    igsJSON_t json = igs_JSONinit();
    igs_JSONopenMap(json);
    igs_JSONaddString(json, "mappings");
    igs_JSONopenArray(json);
    
    igs_mapping_element_t *elmt, *tmp;
    HASH_ITER(hh, mapping->map_elements, elmt, tmp){
        igs_JSONopenMap(json);
        if (elmt->fromInput){
            igs_JSONaddString(json, "fromInput");
            igs_JSONaddString(json, elmt->fromInput);
        }
        if (elmt->toAgent){
            igs_JSONaddString(json, "toAgent");
            igs_JSONaddString(json, elmt->toAgent);
        }
        if (elmt->toOutput){
            igs_JSONaddString(json, "toOutput");
            igs_JSONaddString(json, elmt->toOutput);
        }
        igs_JSONcloseMap(json);
    }
    
    igs_JSONcloseArray(json);
    igs_JSONcloseMap(json);
    char *res = igs_JSONdump(json);
    igs_JSONfree(&json);
    return res;
}

//legacy mapping export
char* parser_exportMapping_v2(igs_mapping_t *mapping){
    assert(mapping);
    igsJSON_t json = igs_JSONinit();
    igs_JSONopenMap(json);
    igs_JSONaddString(json, "mapping");
    igs_JSONopenMap(json);
    igs_JSONaddString(json, "mapping_out");
    igs_JSONopenArray(json);
    igs_mapping_element_t *elmt, *tmp;
    HASH_ITER(hh, mapping->map_elements, elmt, tmp){
        igs_JSONopenMap(json);
        if (elmt->fromInput){
            igs_JSONaddString(json, "input_name");
            igs_JSONaddString(json, elmt->fromInput);
        }
        if (elmt->toAgent){
            igs_JSONaddString(json, "agent_name");
            igs_JSONaddString(json, elmt->toAgent);
        }
        if (elmt->toOutput){
            igs_JSONaddString(json, "output_name");
            igs_JSONaddString(json, elmt->toOutput);
        }
        igs_JSONcloseMap(json);
    }
    igs_JSONcloseArray(json);
    igs_JSONcloseMap(json);
    igs_JSONcloseMap(json);
    
    char *res = igs_JSONdump(json);
    igs_JSONfree(&json);
    return res;
}


////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////
igs_result_t igsAgent_loadDefinition (igs_agent_t *agent, const char* json_str){
    assert(agent);
    assert(json_str);
    //Try to load definition
    igs_definition_t *tmp = parser_loadDefinition(json_str);
    if(tmp == NULL){
        igsAgent_debug(agent, "json string caused an error and was ignored");
        return IGS_FAILURE;
    }else{
        model_readWriteLock();
        //check that this agent has not been destroyed when we were locked
        if (!agent || !(agent->uuid)){
            model_readWriteUnlock();
            return IGS_FAILURE;
        }
        
        if (agent->definition != NULL){
            if (strneq(agent->definition->name, tmp->name))
                igsAgent_warn(agent, "agent will change name from %s to %s after loading definition ",
                              agent->definition->name, tmp->name);
            definition_freeDefinition(&agent->definition);
        }
        agent->definition = tmp;
        agent->network_needToSendDefinitionUpdate = true;
        model_readWriteUnlock();
    }
    return IGS_SUCCESS;
}


igs_result_t igsAgent_loadDefinitionFromPath (igs_agent_t *agent, const char* file_path){
    assert(agent);
    assert(file_path);
    //Try to load definition
    igs_definition_t *tmp = parser_loadDefinitionFromPath(file_path);
    if(tmp == NULL){
        igsAgent_debug(agent, "json file content at '%s' caused an error and was ignored", file_path);
        return IGS_FAILURE;
    }else{
        model_readWriteLock();
        //check that this agent has not been destroyed when we were locked
        if (!agent || !(agent->uuid)){
            model_readWriteUnlock();
            return IGS_FAILURE;
        }
        
        if (agent->definition != NULL){
            if (strneq(agent->definition->name, tmp->name))
                igsAgent_warn(agent, "agent will change name from %s to %s after loading definition ",
                              agent->definition->name, tmp->name);
            definition_freeDefinition(&agent->definition);
        }
        agent->definitionPath = strndup(file_path, IGS_MAX_PATH_LENGTH - 1);
        agent->definition = tmp;
        agent->network_needToSendDefinitionUpdate = true;
        model_readWriteUnlock();
    }
    return IGS_SUCCESS;
}

