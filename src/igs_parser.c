/*  =========================================================================
    parser - parse/generate mapping and definition

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include <stdio.h>
#include <stdlib.h>

#include "ingescape_private.h"
#include "yajl_gen.h"
#include "yajl_tree.h"

#define STR_DEFINITION "definition"
#define STR_NAME "name"
#define STR_FAMILY "family"
#define STR_DESCRIPTION "description"
#define STR_VERSION "version"
#define STR_PARAMETERS "parameters"
#define STR_OUTPUTS "outputs"
#define STR_INPUTS "inputs"
#define STR_SERVICES "services"
#define STR_SERVICES_DEPRECATED "calls" // deprecated since ingescape 3.0 or greater
#define STR_ARGUMENTS "arguments"
#define STR_REPLIES "replies"
#define STR_TYPE "type"
#define STR_VALUE "value"
#define STR_CONSTRAINT "constraint"

#define STR_MAPPINGS "mappings"
#define STR_SPLITS "splits"
#define STR_FROM_INPUT "fromInput"
#define STR_TO_AGENT "toAgent"
#define STR_TO_OUTPUT "toOutput"

//deprecated
#define STR_LEGACY_MAPPING "mapping"
#define STR_LEGACY_MAPPINGS "mapping_out"
#define STR_LEGACY_FROM_INPUT "input_name"
#define STR_LEGACY_TO_AGENT "agent_name"
#define STR_LEGACY_TO_OUTPUT "output_name"

igs_iop_value_type_t s_string_to_value_type (const char *str)
{
    if (str) {
        if (streq (str, "INTEGER"))
            return IGS_INTEGER_T;
        if (streq (str, "DOUBLE"))
            return IGS_DOUBLE_T;
        if (streq (str, "STRING"))
            return IGS_STRING_T;
        if (streq (str, "BOOL"))
            return IGS_BOOL_T;
        if (streq (str, "IMPULSION"))
            return IGS_IMPULSION_T;
        if (streq (str, "DATA"))
            return IGS_DATA_T;
        if (streq (str, "UNKNOWN"))
            return IGS_UNKNOWN_T;
    }
    igs_error ("unknown value type \"%s\" to convert, returned IGS_UNKNOWN_T",
               str);
    return IGS_UNKNOWN_T;
}

bool s_string_to_boolean (const char *str)
{
    if (str) {
        if (streq (str, "true"))
            return true;
        if (streq (str, "false"))
            return false;
    }
    igs_warn ("unknown string \"%s\" to convert, returned false by default",
              str);
    return false;
}

const char *s_value_type_to_string (igs_iop_value_type_t type)
{
    switch (type) {
        case IGS_INTEGER_T:
            return "INTEGER";
        case IGS_DOUBLE_T:
            return "DOUBLE";
        case IGS_STRING_T:
            return "STRING";
        case IGS_BOOL_T:
            return "BOOL";
        case IGS_IMPULSION_T:
            return "IMPULSION";
        case IGS_DATA_T:
            return "DATA";
        case IGS_UNKNOWN_T:
            return "UNKNOWN";
        default:
            igs_error ("unknown igs_iop_value_type_t to convert");
            break;
    }
    return NULL;
}

//
// Definition parsing
//
igs_definition_t *parser_parse_definition_from_node (igs_json_node_t **json)
{
    assert (json);
    assert (*json);
    igs_definition_t *definition = NULL;
    const char *description_path[] = {STR_DEFINITION, STR_DESCRIPTION, NULL};
    const char *version_path[] = {STR_DEFINITION, STR_VERSION, NULL};
    const char *inputs_path[] = {STR_DEFINITION, STR_INPUTS, NULL};
    const char *outputs_path[] = {STR_DEFINITION, STR_OUTPUTS, NULL};
    const char *parameters_path[] = {STR_DEFINITION, STR_PARAMETERS, NULL};
    const char *services_path[] = {STR_DEFINITION, STR_SERVICES, NULL};
    const char *alternate_service_path[] = {STR_DEFINITION,
                                             STR_SERVICES_DEPRECATED, NULL};
    const char *arguments_path[] = {STR_ARGUMENTS, NULL};
    const char *agent_name_path[] = {STR_DEFINITION, STR_NAME, NULL};
    const char *name_path[] = {STR_NAME, NULL};
    const char *constraint_path[] = {STR_CONSTRAINT, NULL};
    const char *iop_description_path[] = {STR_DESCRIPTION, NULL};
    const char *family_path[] = {STR_DEFINITION, STR_FAMILY, NULL};
    const char *type_path[] = {STR_TYPE, NULL};
    const char *value_path[] = {STR_VALUE, NULL};
    const char *replies_path[] = {STR_REPLIES, NULL};

    // name is mandatory
    igs_json_node_t *name = igs_json_node_find (*json, agent_name_path);
    if (name && name->type == IGS_JSON_STRING && name->u.string) {
        char *n = s_strndup (name->u.string, IGS_MAX_AGENT_NAME_LENGTH);
        if (strlen (name->u.string) > IGS_MAX_AGENT_NAME_LENGTH)
            igs_warn ("definition name '%s' exceeds maximum size and will be "
                      "truncated to '%s'",
                      name->u.string, n);
        bool space_in_name = false;
        size_t length_ofn = strlen (n);
        size_t i = 0;
        for (i = 0; i < length_ofn; i++) {
            if (n[i] == ' ') {
                n[i] = '_';
                space_in_name = true;
            }
        }
        if (space_in_name)
            igs_warn (
              "spaces are not allowed in definition name: '%s' has been "
              "changed to '%s'",
              name->u.string, n);
        definition = (igs_definition_t *) zmalloc (sizeof (igs_definition_t));
        definition->name = n;
    }
    else
        return NULL;

    // family
    igs_json_node_t *family = igs_json_node_find (*json, family_path);
    if (family && family->type == IGS_JSON_STRING && family->u.string)
        definition->family = strdup (family->u.string);

    // agent description
    igs_json_node_t *description = igs_json_node_find (*json, description_path);
    if (description && description->type == IGS_JSON_STRING && description->u.string)
        definition->description = s_strndup (description->u.string, IGS_MAX_DESCRIPTION_LENGTH);

    // version
    igs_json_node_t *version = igs_json_node_find (*json, version_path);
    if (version && version->type == IGS_JSON_STRING && version->u.string)
        definition->version = strdup (version->u.string);

    // inputs
    igs_json_node_t *inputs = igs_json_node_find (*json, inputs_path);
    if (inputs && inputs->type == IGS_JSON_ARRAY) {
        for (size_t i = 0; i < inputs->u.array.len; i++) {
            igs_json_node_t *iop_name = igs_json_node_find (inputs->u.array.values[i], name_path);
            if (iop_name && iop_name->type == IGS_JSON_STRING && iop_name->u.string) {
                igs_iop_t *iop = NULL;
                char *corrected_name = s_strndup (iop_name->u.string, IGS_MAX_IOP_NAME_LENGTH);
                bool space_in_name = false;
                size_t length_ofn = strlen (corrected_name);
                size_t k = 0;
                for (k = 0; k < length_ofn; k++) {
                    if (corrected_name[k] == ' ') {
                        corrected_name[k] = '_';
                        space_in_name = true;
                    }
                }
                if (space_in_name)
                    igs_warn ("Spaces are not allowed in IOP name: %s has been "
                              "renamed to %s",
                              iop_name->u.string, corrected_name);

                HASH_FIND_STR (definition->inputs_table, corrected_name, iop);
                if (iop) {
                    igs_warn (
                      "input with name '%s' already exists : ignoring new one",
                      corrected_name);
                    free (corrected_name);
                    continue; // iop with this name already exists
                }

                iop = (igs_iop_t *) zmalloc (sizeof (igs_iop_t));
                iop->type = IGS_INPUT_T;
                iop->value_type = IGS_UNKNOWN_T;
                iop->name = corrected_name;

                igs_json_node_t *iop_type = igs_json_node_find (inputs->u.array.values[i], type_path);
                if (iop_type && iop_type->type == IGS_JSON_STRING && iop_type->u.string)
                    iop->value_type = s_string_to_value_type (iop_type->u.string);
                
                igs_json_node_t *constraint = igs_json_node_find (inputs->u.array.values[i], constraint_path);
                if (constraint && constraint->type == IGS_JSON_STRING && constraint->u.string){
                    char *error = NULL;
                    iop->constraint = s_model_parse_constraint(iop->value_type, constraint->u.string, &error);
                    if (error)
                        igs_error ("%s", error);
                }
                
                igs_json_node_t *iop_description = igs_json_node_find (inputs->u.array.values[i], iop_description_path);
                if (iop_description && iop_description->type == IGS_JSON_STRING && iop_description->u.string){
                    if (iop->description)
                        free(iop->description);
                    iop->description = s_strndup(iop_description->u.string, IGS_MAX_LOG_LENGTH);
                }

                //NB: inputs do not have initial value in definition => nothing to do here
                HASH_ADD_STR (definition->inputs_table, name, iop);
            }
        }
    }else if(inputs)
        igs_error ("inputs are not an array : ignoring");

    // outputs
    igs_json_node_t *outputs = igs_json_node_find (*json, outputs_path);
    if (outputs && outputs->type == IGS_JSON_ARRAY) {
        for (size_t i = 0; i < outputs->u.array.len; i++) {
            igs_json_node_t *iop_name =
              igs_json_node_find (outputs->u.array.values[i], name_path);
            if (iop_name && iop_name->type == IGS_JSON_STRING && iop_name->u.string) {
                igs_iop_t *iop = NULL;
                char *corrected_name = s_strndup (iop_name->u.string, IGS_MAX_IOP_NAME_LENGTH);
                bool space_in_name = false;
                size_t length_ofn = strlen (corrected_name);
                size_t k = 0;
                for (k = 0; k < length_ofn; k++) {
                    if (corrected_name[k] == ' ') {
                        corrected_name[k] = '_';
                        space_in_name = true;
                    }
                }
                if (space_in_name)
                    igs_warn ("Spaces are not allowed in IOP name: %s has been "
                              "renamed to %s",
                              iop_name->u.string, corrected_name);

                HASH_FIND_STR (definition->outputs_table, corrected_name, iop);
                if (iop) {
                    igs_warn (
                      "output with name '%s' already exists : ignoring new one",
                      corrected_name);
                    free (corrected_name);
                    continue; // iop with this name already exists
                }

                iop = (igs_iop_t *) zmalloc (sizeof (igs_iop_t));
                iop->type = IGS_OUTPUT_T;
                iop->value_type = IGS_UNKNOWN_T;
                iop->name = corrected_name;

                igs_json_node_t *iop_type =
                  igs_json_node_find (outputs->u.array.values[i], type_path);
                if (iop_type && iop_type->type == IGS_JSON_STRING && iop_type->u.string)
                    iop->value_type = s_string_to_value_type (iop_type->u.string);
                
                igs_json_node_t *constraint = igs_json_node_find (outputs->u.array.values[i], constraint_path);
                if (constraint && constraint->type == IGS_JSON_STRING && constraint->u.string){
                    char *error = NULL;
                    iop->constraint = s_model_parse_constraint(iop->value_type, constraint->u.string, &error);
                }
                
                igs_json_node_t *iop_description = igs_json_node_find (outputs->u.array.values[i], iop_description_path);
                if (iop_description && iop_description->type == IGS_JSON_STRING && iop_description->u.string){
                    if (iop->description)
                        free(iop->description);
                    iop->description = s_strndup(iop_description->u.string, IGS_MAX_LOG_LENGTH);
                }

                igs_json_node_t *iop_value = igs_json_node_find (outputs->u.array.values[i], value_path);
                if (iop_value) {
                    switch (iop->value_type) {
                        case IGS_INTEGER_T:
                            iop->value.i =
                              (int) IGSYAJL_GET_INTEGER (iop_value);
                            break;
                        case IGS_DOUBLE_T:
                            iop->value.d = IGSYAJL_GET_DOUBLE (iop_value);
                            break;
                        case IGS_BOOL_T:
                            if (iop_value->type == IGS_JSON_TRUE)
                                iop->value.b = true;
                            else
                            if (iop_value->type == IGS_JSON_TRUE)
                                iop->value.b = false;
                            else
                            if (iop_value->type == IGS_JSON_STRING)
                                iop->value.b =
                                  s_string_to_boolean (iop_value->u.string);
                            break;
                        case IGS_STRING_T:
                            iop->value.s = (IGSYAJL_IS_STRING (iop_value)
                                              ? strdup (iop_value->u.string)
                                              : NULL);
                            break;
                        case IGS_IMPULSION_T:
                            // IMPULSION has no value
                            break;
                        case IGS_DATA_T:
                            // we store data as hexa string but we convert it to actual bytes
                            iop->value.data =
                              (IGSYAJL_IS_STRING (iop_value)
                                 ? s_model_string_to_bytes (iop_value->u.string)
                                 : NULL);
                            iop->value_size =
                              (iop->value.data)
                                ? strlen (iop_value->u.string) / 2
                                : 0;
                            break;
                        default:
                            break;
                    }
                }
                HASH_ADD_STR (definition->outputs_table, name, iop);
            }
        }
    }
    else
    if (outputs)
        igs_error ("outputs are not an array : ignoring");

    // parameters
    igs_json_node_t *parameters = igs_json_node_find (*json, parameters_path);
    if (parameters && parameters->type == IGS_JSON_ARRAY) {
        for (size_t i = 0; i < parameters->u.array.len; i++) {
            igs_json_node_t *iop_name =
              igs_json_node_find (parameters->u.array.values[i], name_path);
            if (iop_name && iop_name->type == IGS_JSON_STRING && iop_name->u.string) {
                igs_iop_t *iop = NULL;
                char *corrected_name = s_strndup (iop_name->u.string, IGS_MAX_IOP_NAME_LENGTH);
                bool space_in_name = false;
                size_t length_ofn = strlen (corrected_name);
                size_t k = 0;
                for (k = 0; k < length_ofn; k++) {
                    if (corrected_name[k] == ' ') {
                        corrected_name[k] = '_';
                        space_in_name = true;
                    }
                }
                if (space_in_name)
                    igs_warn ("Spaces are not allowed in IOP name: %s has been "
                              "renamed to %s",
                              iop_name->u.string, corrected_name);

                HASH_FIND_STR (definition->params_table, corrected_name, iop);
                if (iop) {
                    igs_warn ("parameter with name '%s' already exists : "
                              "ignoring new one",
                              corrected_name);
                    free (corrected_name);
                    continue; // iop with this name already exists
                }

                iop = (igs_iop_t *) zmalloc (sizeof (igs_iop_t));
                iop->type = IGS_PARAMETER_T;
                iop->value_type = IGS_UNKNOWN_T;
                iop->name = corrected_name;

                igs_json_node_t *iop_type =
                  igs_json_node_find (parameters->u.array.values[i], type_path);
                if (iop_type && iop_type->type == IGS_JSON_STRING && iop_type->u.string)
                    iop->value_type = s_string_to_value_type (iop_type->u.string);
                
                igs_json_node_t *constraint = igs_json_node_find (parameters->u.array.values[i], constraint_path);
                if (constraint && constraint->type == IGS_JSON_STRING && constraint->u.string){
                    char *error = NULL;
                    iop->constraint = s_model_parse_constraint(iop->value_type, constraint->u.string, &error);
                }
                
                igs_json_node_t *iop_description = igs_json_node_find (parameters->u.array.values[i], iop_description_path);
                if (iop_description && iop_description->type == IGS_JSON_STRING && iop_description->u.string){
                    if (iop->description)
                        free(iop->description);
                    iop->description = s_strndup(iop_description->u.string, IGS_MAX_LOG_LENGTH);
                }

                igs_json_node_t *iop_value = igs_json_node_find (
                        parameters->u.array.values[i], value_path);
                if (iop_value) {
                    switch (iop->value_type) {
                        case IGS_INTEGER_T:
                            iop->value.i =
                              (int) IGSYAJL_GET_INTEGER (iop_value);
                            break;
                        case IGS_DOUBLE_T:
                            iop->value.d = IGSYAJL_GET_DOUBLE (iop_value);
                            break;
                        case IGS_BOOL_T:
                            if (iop_value->type == IGS_JSON_TRUE)
                                iop->value.b = true;
                            else
                            if (iop_value->type == IGS_JSON_TRUE)
                                iop->value.b = false;
                            else
                            if (iop_value->type == IGS_JSON_STRING)
                                iop->value.b =
                                  s_string_to_boolean (iop_value->u.string);
                            break;
                        case IGS_STRING_T:
                            iop->value.s = (IGSYAJL_IS_STRING (iop_value)
                                              ? strdup (iop_value->u.string)
                                              : NULL);
                            break;
                        case IGS_IMPULSION_T:
                            // IMPULSION has no value
                            break;
                        case IGS_DATA_T:
                            // we store data as hexa string but we convert it to actual bytes
                            iop->value.data =
                              (IGSYAJL_IS_STRING (iop_value)
                                 ? s_model_string_to_bytes (iop_value->u.string)
                                 : NULL);
                            break;
                        default:
                            break;
                    }
                }
                HASH_ADD_STR (definition->params_table, name, iop);
            }
        }
    }
    else
    if (parameters)
        igs_error ("parameters are not an array : ignoring");

    // services
    igs_json_node_t *services = igs_json_node_find (*json, services_path);
    if (services == NULL)
        services = igs_json_node_find (*json, alternate_service_path);
    if (services && services->type == IGS_JSON_ARRAY) {
        for (size_t i = 0; i < services->u.array.len; i++) {
            igs_json_node_t *service_name =
              igs_json_node_find (services->u.array.values[i], name_path);
            if (service_name && service_name->type == IGS_JSON_STRING && service_name->u.string) {
                igs_service_t *service = NULL;
                char *corrected_name = s_strndup (service_name->u.string, IGS_MAX_IOP_NAME_LENGTH);
                bool space_in_name = false;
                size_t length_ofn = strlen (corrected_name);
                size_t k = 0;
                for (k = 0; k < length_ofn; k++) {
                    if (corrected_name[k] == ' ') {
                        corrected_name[k] = '_';
                        space_in_name = true;
                    }
                }
                if (space_in_name)
                    igs_warn (
                      "Spaces are not allowed in service name: %s has been "
                      "renamed to %s",
                      service_name->u.string, corrected_name);
                HASH_FIND_STR (definition->services_table, corrected_name,
                               service);
                if (service) {
                    igs_warn ("service with name '%s' already exists : "
                              "ignoring new one",
                              corrected_name);
                    free (corrected_name);
                    continue; // service with this name already exists
                }

                service = (igs_service_t *) zmalloc (sizeof (igs_service_t));
                service->name = corrected_name;

                description = igs_json_node_find (services->u.array.values[i],
                                                  description_path);
                if (description && description->type == IGS_JSON_STRING && description->u.string)
                    service->description = strdup (description->u.string);

                igs_json_node_t *arguments = igs_json_node_find (
                  services->u.array.values[i], arguments_path);
                if (arguments && arguments->type == IGS_JSON_ARRAY) {
                    for (size_t j = 0; j < arguments->u.array.len; j++) {
                        if (arguments->u.array.values[j]
                            && arguments->u.array.values[j]->type
                                 == IGS_JSON_MAP) {
                            igs_json_node_t *arg_name = igs_json_node_find (
                              arguments->u.array.values[j], name_path);
                            if (arg_name && arg_name->type == IGS_JSON_STRING
                                && arg_name->u.string) {
                                char *corrected_arg_name = s_strndup (
                                  arg_name->u.string, IGS_MAX_IOP_NAME_LENGTH);
                                bool space_in_arg_name = false;
                                size_t arg_name_length =
                                  strlen (corrected_arg_name);
                                size_t arg_name_idx = 0;
                                for (arg_name_idx = 0;
                                     arg_name_idx < arg_name_length;
                                     arg_name_idx++) {
                                    if (corrected_arg_name[arg_name_idx]
                                        == ' ') {
                                        corrected_arg_name[arg_name_idx] = '_';
                                        space_in_arg_name = true;
                                    }
                                }
                                if (space_in_arg_name)
                                    igs_warn ("Spaces are not allowed in "
                                              "service argument name: "
                                              "%s has been renamed to %s",
                                              arg_name->u.string,
                                              corrected_arg_name);

                                igs_service_arg_t *new_arg =
                                  (igs_service_arg_t *) zmalloc (
                                    sizeof (igs_service_arg_t));
                                new_arg->name = corrected_arg_name;
                                igs_json_node_t *arg_type = igs_json_node_find (
                                  arguments->u.array.values[j], type_path);
                                if (arg_type
                                    && arg_type->type == IGS_JSON_STRING
                                    && arg_type->u.string)
                                    new_arg->type = s_string_to_value_type (
                                      arg_type->u.string);
                                LL_APPEND (service->arguments, new_arg);
                            }
                        }
                    }
                }

                igs_json_node_t *replies = igs_json_node_find (services->u.array.values[i], replies_path);
                if (replies && replies->type == IGS_JSON_ARRAY) {
                    size_t replies_nb = replies->u.array.len;
                    for (size_t j = 0; j < replies_nb; j++){
                        igs_json_node_t *reply_name = igs_json_node_find (replies->u.array.values[j], name_path);
                        if (reply_name && reply_name->type == IGS_JSON_STRING && reply_name->u.string) {
                            char *corrected_reply_name = s_strndup (reply_name->u.string, IGS_MAX_IOP_NAME_LENGTH);
                            bool space_in_reply_name = false;
                            size_t reply_name_length = strlen (corrected_reply_name);
                            size_t reply_name_idx = 0;
                            for (reply_name_idx = 0; reply_name_idx < reply_name_length; reply_name_idx++) {
                                if (corrected_reply_name[reply_name_idx] == ' ') {
                                    corrected_reply_name[reply_name_idx] = '_';
                                    space_in_reply_name = true;
                                }
                            }
                            if (space_in_reply_name)
                                igs_warn ("Spaces are not allowed in service argument name: %s has been renamed to %s", reply_name->u.string, corrected_reply_name);
                            igs_service_t *my_reply = (igs_service_t *) zmalloc (sizeof (igs_service_t));
                            my_reply->name = corrected_reply_name;

                            arguments = igs_json_node_find (replies->u.array.values[j], arguments_path);
                            if (arguments && arguments->type == IGS_JSON_ARRAY) {
                                for (size_t len = 0; len < arguments->u.array.len; len++) {
                                    if (arguments->u.array.values[len] && arguments->u.array.values[len]->type == IGS_JSON_MAP) {
                                        igs_json_node_t *arg_name = igs_json_node_find (arguments->u.array.values[len], name_path);
                                        if (arg_name && arg_name->type == IGS_JSON_STRING && arg_name->u.string) {
                                            char *corrected_reply_arg_name = s_strndup (arg_name->u.string, IGS_MAX_IOP_NAME_LENGTH);
                                            bool space_in_reply_arg_name = false;
                                            size_t reply_arg_name_length = strlen (corrected_reply_arg_name);
                                            size_t reply_arg_name_idx = 0;
                                            for (reply_arg_name_idx = 0; reply_arg_name_idx < reply_arg_name_length; reply_arg_name_idx++) {
                                                if (corrected_reply_arg_name[reply_arg_name_idx] == ' ') {
                                                    corrected_reply_arg_name[reply_arg_name_idx] = '_';
                                                    space_in_reply_arg_name = true;
                                                }
                                            }
                                            if (space_in_reply_arg_name)
                                                igs_warn ("Spaces are not allowed in service argument name: %s has been renamed to %s",
                                                  arg_name->u.string, corrected_reply_arg_name);

                                            igs_service_arg_t *new_arg = (igs_service_arg_t *) zmalloc (sizeof (igs_service_arg_t));
                                            new_arg->name = corrected_reply_arg_name;
                                            igs_json_node_t *arg_type = igs_json_node_find (arguments->u.array.values[len], type_path);
                                            if (arg_type && arg_type->type == IGS_JSON_STRING && arg_type->u.string)
                                                new_arg->type = s_string_to_value_type (arg_type->u.string);
                                            LL_APPEND (my_reply->arguments,new_arg);
                                        }
                                    }
                                }
                            }
                            HASH_ADD_STR(service->replies, name, my_reply);
                        }
                    }
                }
                HASH_ADD_STR (definition->services_table, name, service);
            }
        }
    }
    else
    if (services)
        igs_error ("services are not an array : ignoring");

    igs_json_node_destroy (json);
    return definition;
}

//
// Mapping parsing
//
igs_mapping_t *parser_parse_mapping_from_node (igs_json_node_t **json)
{
    assert (json);
    assert (*json);
    igs_mapping_t *mapping = NULL;
    const char *mappings_path[] = {STR_MAPPINGS, NULL};
    const char *splits_path[] = {STR_SPLITS, NULL};
    // const char *from_agent_path[] = {"fromAgent",NULL};
    const char *from_input_path[] = {STR_FROM_INPUT, NULL};
    const char *to_agent_path[] = {STR_TO_AGENT, NULL};
    const char *to_output_path[] = {STR_TO_OUTPUT, NULL};
    const char *alternate_mapping_path[] = {STR_LEGACY_MAPPING,
                                             STR_LEGACY_MAPPINGS, NULL};
    const char *alternate_from_input_path[] = {STR_LEGACY_FROM_INPUT, NULL};
    const char *alternate_to_agent_path[] = {STR_LEGACY_TO_AGENT, NULL};
    const char *alternate_to_output_path[] = {STR_LEGACY_TO_OUTPUT, NULL};

    bool use_alternate = false;
    igs_json_node_t *mappings = igs_json_node_find (*json, mappings_path);
    igs_json_node_t *splits = igs_json_node_find (*json, splits_path);
    if (mappings == NULL) {
        mappings = igs_json_node_find (*json, alternate_mapping_path);
        if (mappings && mappings->type == IGS_JSON_ARRAY)
            use_alternate = true;
        else
        if (!(mappings && mappings->type == IGS_JSON_ARRAY)
            && splits == NULL) {
            igs_json_node_destroy (json);
            return NULL;
        }
    }

    mapping = (igs_mapping_t *) zmalloc (sizeof (igs_mapping_t));

    // FIXME: we will not use from_agent in parsing because the received
    // mappings should all imply our inputs. In the future, we could
    // check from_agent to ensure this is us but this requires changing the
    // internal API to attach parsing to a specific agent instance.
    if (mappings) {
        for (size_t i = 0; i < mappings->u.array.len; i++) {
            if (mappings->u.array.values[i]->type != IGS_JSON_MAP)
                continue;
            char *from_input = NULL;
            char *to_agent = NULL;
            char *to_output = NULL;
            igs_json_node_t *from_input_node = NULL;
            igs_json_node_t *to_agent_node = NULL;
            igs_json_node_t *to_output_node = NULL;
            if (!use_alternate) {
                from_input_node = igs_json_node_find (
                  mappings->u.array.values[i], from_input_path);
                to_agent_node = igs_json_node_find (mappings->u.array.values[i],
                                                    to_agent_path);
                to_output_node = igs_json_node_find (
                  mappings->u.array.values[i], to_output_path);
            }
            else {
                from_input_node = igs_json_node_find (
                  mappings->u.array.values[i], alternate_from_input_path);
                to_agent_node = igs_json_node_find (mappings->u.array.values[i],
                                                    alternate_to_agent_path);
                to_output_node = igs_json_node_find (
                  mappings->u.array.values[i], alternate_to_output_path);
            }
            if (from_input_node && from_input_node->type == IGS_JSON_STRING
                && from_input_node->u.string) {
                char *corrected_name = s_strndup (from_input_node->u.string,
                                                  IGS_MAX_IOP_NAME_LENGTH);
                bool space_in_name = false;
                size_t length_ofn = strlen (corrected_name);
                size_t k = 0;
                for (k = 0; k < length_ofn; k++) {
                    if (corrected_name[k] == ' ') {
                        corrected_name[k] = '_';
                        space_in_name = true;
                    }
                }
                if (space_in_name)
                    igs_warn (
                      "Spaces are not allowed in mapping element name: %s has "
                      "been renamed to %s",
                      from_input_node->u.string, corrected_name);
                from_input = corrected_name;
            }
            if (to_agent_node && to_agent_node->type == IGS_JSON_STRING
                && to_agent_node->u.string) {
                char *corrected_name =
                  s_strndup (to_agent_node->u.string, IGS_MAX_IOP_NAME_LENGTH);
                bool space_in_name = false;
                size_t length_ofn = strlen (corrected_name);
                size_t k = 0;
                for (k = 0; k < length_ofn; k++) {
                    if (corrected_name[k] == ' ') {
                        corrected_name[k] = '_';
                        space_in_name = true;
                    }
                }
                if (space_in_name)
                    igs_warn (
                      "Spaces are not allowed in mapping element name: %s has "
                      "been renamed to %s",
                      to_agent_node->u.string, corrected_name);
                to_agent = corrected_name;
            }
            if (to_output_node && to_output_node->type == IGS_JSON_STRING
                && to_output_node->u.string) {
                char *corrected_name =
                  s_strndup (to_output_node->u.string, IGS_MAX_IOP_NAME_LENGTH);
                bool space_in_name = false;
                size_t length_ofn = strlen (corrected_name);
                size_t k = 0;
                for (k = 0; k < length_ofn; k++) {
                    if (corrected_name[k] == ' ') {
                        corrected_name[k] = '_';
                        space_in_name = true;
                    }
                }
                if (space_in_name)
                    igs_warn (
                      "Spaces are not allowed in mapping element name: %s has "
                      "been renamed to %s",
                      to_output_node->u.string, corrected_name);
                to_output = corrected_name;
            }
            if (from_input && to_agent && to_output) {
                size_t len = strlen (from_input) + strlen (to_agent)
                             + strlen (to_output) + 3 + 1;
                char *mashup = (char *) zmalloc (len * sizeof (char));
                strcpy (mashup, from_input);
                strcat (mashup, "."); // separator
                strcat (mashup, to_agent);
                strcat (mashup, "."); // separator
                strcat (mashup, to_output);
                mashup[len - 1] = '\0';
                uint64_t h = s_djb2_hash ((unsigned char *) mashup);
                free (mashup);

                igs_map_t *tmp = NULL;
                HASH_FIND (hh, mapping->map_elements, &h,
                           sizeof (uint64_t), tmp);
                if (tmp == NULL) {
                    // element does not exist yet : create and register it
                    igs_map_t *new = mapping_create_mapping_element (
                      from_input, to_agent, to_output);
                    new->id = h;
                    HASH_ADD (hh, mapping->map_elements, id,
                              sizeof (uint64_t), new);
                }
                else
                    igs_error ("hash already exists for %s->%s.%s", from_input,
                               to_agent, to_output);
            }
            if (from_input)
                free (from_input);
            if (to_agent)
                free (to_agent);
            if (to_output)
                free (to_output);
        }
    }
    if (splits) {
        for (size_t i = 0; i < splits->u.array.len; i++) {
            if (splits->u.array.values[i]->type != IGS_JSON_MAP)
                continue;
            char *from_input = NULL;
            char *to_agent = NULL;
            char *to_output = NULL;
            igs_json_node_t *from_input_node = NULL;
            igs_json_node_t *to_agent_node = NULL;
            igs_json_node_t *to_output_node = NULL;
            from_input_node = igs_json_node_find (splits->u.array.values[i],
                                                  from_input_path);
            to_agent_node =
              igs_json_node_find (splits->u.array.values[i], to_agent_path);
            to_output_node =
              igs_json_node_find (splits->u.array.values[i], to_output_path);

            if (from_input_node && from_input_node->type == IGS_JSON_STRING
                && from_input_node->u.string) {
                char *corrected_name = s_strndup (from_input_node->u.string,
                                                  IGS_MAX_IOP_NAME_LENGTH);
                bool space_in_name = false;
                size_t length_ofn = strlen (corrected_name);
                size_t k = 0;
                for (k = 0; k < length_ofn; k++) {
                    if (corrected_name[k] == ' ') {
                        corrected_name[k] = '_';
                        space_in_name = true;
                    }
                }
                if (space_in_name)
                    igs_warn (
                      "Spaces are not allowed in split element name: %s has "
                      "been renamed to %s",
                      from_input_node->u.string, corrected_name);
                from_input = corrected_name;
            }
            if (to_agent_node && to_agent_node->type == IGS_JSON_STRING
                && to_agent_node->u.string) {
                char *corrected_name =
                  s_strndup (to_agent_node->u.string, IGS_MAX_IOP_NAME_LENGTH);
                bool space_in_name = false;
                size_t length_ofn = strlen (corrected_name);
                size_t k = 0;
                for (k = 0; k < length_ofn; k++) {
                    if (corrected_name[k] == ' ') {
                        corrected_name[k] = '_';
                        space_in_name = true;
                    }
                }
                if (space_in_name)
                    igs_warn (
                      "Spaces are not allowed in split element name: %s has "
                      "been renamed to %s",
                      to_agent_node->u.string, corrected_name);
                to_agent = corrected_name;
            }
            if (to_output_node && to_output_node->type == IGS_JSON_STRING
                && to_output_node->u.string) {
                char *corrected_name =
                  s_strndup (to_output_node->u.string, IGS_MAX_IOP_NAME_LENGTH);
                bool space_in_name = false;
                size_t length_ofn = strlen (corrected_name);
                size_t k = 0;
                for (k = 0; k < length_ofn; k++) {
                    if (corrected_name[k] == ' ') {
                        corrected_name[k] = '_';
                        space_in_name = true;
                    }
                }
                if (space_in_name)
                    igs_warn (
                      "Spaces are not allowed in split element name: %s has "
                      "been renamed to %s",
                      to_output_node->u.string, corrected_name);
                to_output = corrected_name;
            }
            if (from_input && to_agent && to_output) {
                size_t len = strlen (from_input) + strlen (to_agent)
                             + strlen (to_output) + 3 + 1;
                char *mashup = (char *) zmalloc (len * sizeof (char));
                strcpy (mashup, from_input);
                strcat (mashup, "."); // separator
                strcat (mashup, to_agent);
                strcat (mashup, "."); // separator
                strcat (mashup, to_output);
                mashup[len - 1] = '\0';
                uint64_t h = s_djb2_hash ((unsigned char *) mashup);
                free (mashup);

                igs_split_t *tmp = NULL;
                HASH_FIND (hh, mapping->split_elements, &h,
                           sizeof (uint64_t), tmp);
                if (tmp == NULL) {
                    // element does not exist yet : create and register it
                    igs_split_t *new = split_create_split_element (
                      from_input, to_agent, to_output);
                    new->id = h;
                    HASH_ADD (hh, mapping->split_elements, id,
                              sizeof (uint64_t), new);
                }
                else
                    igs_error ("hash already exists for %s->%s.%s", from_input,
                               to_agent, to_output);
            }
            if (from_input)
                free (from_input);
            if (to_agent)
                free (to_agent);
            if (to_output)
                free (to_output);
        }
    }
    igs_json_node_destroy (json);
    return mapping;
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
igs_definition_t *parser_load_definition (const char *json_str)
{
    assert (json_str);
    igs_json_node_t *json = igs_json_node_parse_from_str (json_str);
    if (!json) {
        igs_error ("could not parse JSON string : '%s'", json_str);
        return NULL;
    }
    if (json->type != IGS_JSON_MAP) {
        igs_json_node_destroy (&json);
        igs_error ("parsed JSON is not an array : '%s'", json_str);
        return NULL;
    }
    return parser_parse_definition_from_node (&json); // will free json tree node
}

igs_definition_t *parser_load_definition_from_path (const char *path)
{
    assert (path);
    igs_json_node_t *json = igs_json_node_parse_from_file (path);
    if (!json) {
        igs_error ("could not parse JSON file '%s'", path);
        return NULL;
    }
    if (json->type != IGS_JSON_MAP) {
        igs_json_node_destroy (&json);
        igs_error ("parsed JSON at '%s' is not a map", path);
        return NULL;
    }
    return parser_parse_definition_from_node (&json); // will free json tree node
}

igs_mapping_t *parser_load_mapping (const char *json_str)
{
    assert (json_str);
    igs_json_node_t *json = igs_json_node_parse_from_str (json_str);
    if (!json) {
        igs_error ("could not parse JSON string : '%s'", json_str);
        return NULL;
    }
    if (json->type != IGS_JSON_MAP) {
        igs_json_node_destroy (&json);
        igs_error ("parsed JSON is not an array : '%s'", json_str);
        return NULL;
    }
    return parser_parse_mapping_from_node (&json); // will free json tree node
}

igs_mapping_t *parser_load_mapping_from_path (const char *path)
{
    assert (path);
    igs_json_node_t *json = igs_json_node_parse_from_file (path);
    if (!json) {
        igs_error ("could not parse JSON file '%s'", path);
        return NULL;
    }
    if (json->type != IGS_JSON_MAP) {
        igs_json_node_destroy (&json);
        igs_error ("parsed JSON at '%s' is not an array", path);
        return NULL;
    }
    return parser_parse_mapping_from_node (&json); // will free json tree node
}

char *parser_export_definition (igs_definition_t *def)
{
    assert (def);
    igs_json_t *json = igs_json_new ();
    igs_json_open_map (json);
    igs_json_add_string (json, STR_DEFINITION);
    igs_json_open_map (json);
    if (def->name) {
        igs_json_add_string (json, STR_NAME);
        igs_json_add_string (json, def->name);
    }
    if (def->family) {
        igs_json_add_string (json, STR_FAMILY);
        igs_json_add_string (json, def->family);
    }
    if (def->description) {
        igs_json_add_string (json, STR_DESCRIPTION);
        igs_json_add_string (json, def->description);
    }
    if (def->version) {
        igs_json_add_string (json, STR_VERSION);
        igs_json_add_string (json, def->version);
    }

    igs_json_add_string (json, STR_INPUTS);
    igs_json_open_array (json);
    igs_iop_t *iop, *tmp_iop;
    HASH_ITER (hh, def->inputs_table, iop, tmp_iop)
    {
        igs_json_open_map (json);
        if (iop->name) {
            igs_json_add_string (json, STR_NAME);
            igs_json_add_string (json, iop->name);
        }
        char constraint_expression[IGS_MAX_LOG_LENGTH] = "";
        if (iop->constraint){
            if (iop->constraint->type == IGS_CONSTRAINT_MIN){
                if (iop->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "min %d",
                             iop->constraint->min_int.min);
                    igs_json_add_string(json, constraint_expression);
                }else if (iop->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "min %f",
                             iop->constraint->min_double.min);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (iop->constraint->type == IGS_CONSTRAINT_MAX){
                if (iop->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "max %d",
                             iop->constraint->max_int.max);
                    igs_json_add_string(json, constraint_expression);
                }else if (iop->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "max %f",
                             iop->constraint->max_double.max);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (iop->constraint->type == IGS_CONSTRAINT_RANGE){
                if (iop->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "[%d, %d]",
                             iop->constraint->range_int.min,
                             iop->constraint->range_int.max);
                    igs_json_add_string(json, constraint_expression);
                }else if (iop->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "[%f, %f]",
                             iop->constraint->range_double.min,
                             iop->constraint->range_double.max);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (iop->constraint->type == IGS_CONSTRAINT_REGEXP){
                igs_json_add_string (json, STR_CONSTRAINT);
                snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "~ %s",
                         iop->constraint->regexp.string);
                igs_json_add_string(json, constraint_expression);
            }
        }
        if (iop->description){
            igs_json_add_string (json, STR_DESCRIPTION);
            igs_json_add_string (json, iop->description);
        }
        igs_json_add_string (json, STR_TYPE);
        igs_json_add_string (json, s_value_type_to_string (iop->value_type));
        //NB: inputs do not have intial values
        igs_json_close_map (json);
    }
    igs_json_close_array (json);

    igs_json_add_string (json, STR_OUTPUTS);
    igs_json_open_array (json);
    HASH_ITER (hh, def->outputs_table, iop, tmp_iop)
    {
        igs_json_open_map (json);
        if (iop->name) {
            igs_json_add_string (json, STR_NAME);
            igs_json_add_string (json, iop->name);
        }
        igs_json_add_string (json, STR_TYPE);
        igs_json_add_string (json, s_value_type_to_string (iop->value_type));
        igs_json_add_string (json, STR_VALUE);
        switch (iop->value_type) {
            case IGS_INTEGER_T:
                igs_json_add_int (json, iop->value.i);
                break;
            case IGS_DOUBLE_T:
                igs_json_add_double (json, iop->value.d);
                break;
            case IGS_BOOL_T:
                igs_json_add_bool (json, iop->value.b);
                break;
            case IGS_STRING_T:
                igs_json_add_string (json, iop->value.s);
                break;
            case IGS_IMPULSION_T:
                igs_json_add_null (json);
                break;
            case IGS_DATA_T: {
                if (iop->value_size){
                    char *data_to_store = (char *) zmalloc ((2 * iop->value_size + 1) * sizeof (char));
                    for (size_t i = 0; i < iop->value_size; i++)
                        sprintf (data_to_store + 2 * i, "%02X",
                                 *((uint8_t *) ((char *) iop->value.data + i)));
                    igs_json_add_string (json, data_to_store);
                    free (data_to_store);
                }else{
                    igs_json_add_null(json);
                }
                break;
            }
            default:
                igs_json_add_string (json, "");
                break;
        }
        char constraint_expression[IGS_MAX_LOG_LENGTH] = "";
        if (iop->constraint){
            if (iop->constraint->type == IGS_CONSTRAINT_MIN){
                if (iop->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "min %d",
                             iop->constraint->min_int.min);
                    igs_json_add_string(json, constraint_expression);
                }else if (iop->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "min %f",
                             iop->constraint->min_double.min);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (iop->constraint->type == IGS_CONSTRAINT_MAX){
                if (iop->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "max %d",
                             iop->constraint->max_int.max);
                    igs_json_add_string(json, constraint_expression);
                }else if (iop->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "max %f",
                             iop->constraint->max_double.max);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (iop->constraint->type == IGS_CONSTRAINT_RANGE){
                if (iop->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "[%d, %d]",
                             iop->constraint->range_int.min,
                             iop->constraint->range_int.max);
                    igs_json_add_string(json, constraint_expression);
                }else if (iop->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "[%f, %f]",
                             iop->constraint->range_double.min,
                             iop->constraint->range_double.max);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (iop->constraint->type == IGS_CONSTRAINT_REGEXP){
                igs_json_add_string (json, STR_CONSTRAINT);
                snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "~ %s",
                         iop->constraint->regexp.string);
                igs_json_add_string(json, constraint_expression);
            }
        }
        if (iop->description){
            igs_json_add_string (json, STR_DESCRIPTION);
            igs_json_add_string (json, iop->description);
        }
        igs_json_close_map (json);
    }
    igs_json_close_array (json);

    igs_json_add_string (json, STR_PARAMETERS);
    igs_json_open_array (json);
    HASH_ITER (hh, def->params_table, iop, tmp_iop)
    {
        igs_json_open_map (json);
        if (iop->name) {
            igs_json_add_string (json, STR_NAME);
            igs_json_add_string (json, iop->name);
        }
        igs_json_add_string (json, STR_TYPE);
        igs_json_add_string (json, s_value_type_to_string (iop->value_type));
        igs_json_add_string (json, STR_VALUE);
        switch (iop->value_type) {
            case IGS_INTEGER_T:
                igs_json_add_int (json, iop->value.i);
                break;
            case IGS_DOUBLE_T:
                igs_json_add_double (json, iop->value.d);
                break;
            case IGS_BOOL_T:
                igs_json_add_bool (json, iop->value.b);
                break;
            case IGS_STRING_T:
                igs_json_add_string (json, iop->value.s);
                break;
            case IGS_IMPULSION_T:
                igs_json_add_null (json);
                break;
            case IGS_DATA_T: {
                if (iop->value_size){
                    char *data_to_store = (char *) zmalloc ((2 * iop->value_size + 1) * sizeof (char));
                    for (size_t i = 0; i < iop->value_size; i++)
                        sprintf (data_to_store + 2 * i, "%02X",
                                 *((uint8_t *) ((char *) iop->value.data + i)));
                    igs_json_add_string (json, data_to_store);
                    free (data_to_store);
                }else{
                    igs_json_add_null(json);
                }
                break;
            }
            default:
                igs_json_add_string (json, "");
                break;
        }
        char constraint_expression[IGS_MAX_LOG_LENGTH] = "";
        if (iop->constraint){
            if (iop->constraint->type == IGS_CONSTRAINT_MIN){
                if (iop->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "min %d",
                             iop->constraint->min_int.min);
                    igs_json_add_string(json, constraint_expression);
                }else if (iop->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "min %f",
                             iop->constraint->min_double.min);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (iop->constraint->type == IGS_CONSTRAINT_MAX){
                if (iop->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "max %d",
                             iop->constraint->max_int.max);
                    igs_json_add_string(json, constraint_expression);
                }else if (iop->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "max %f",
                             iop->constraint->max_double.max);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (iop->constraint->type == IGS_CONSTRAINT_RANGE){
                if (iop->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "[%d, %d]",
                             iop->constraint->range_int.min,
                             iop->constraint->range_int.max);
                    igs_json_add_string(json, constraint_expression);
                }else if (iop->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "[%f, %f]",
                             iop->constraint->range_double.min,
                             iop->constraint->range_double.max);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (iop->constraint->type == IGS_CONSTRAINT_REGEXP){
                igs_json_add_string (json, STR_CONSTRAINT);
                snprintf(constraint_expression, IGS_MAX_LOG_LENGTH, "~ %s",
                         iop->constraint->regexp.string);
                igs_json_add_string(json, constraint_expression);
            }
        }
        if (iop->description){
            igs_json_add_string (json, STR_DESCRIPTION);
            igs_json_add_string (json, iop->description);
        }
        igs_json_close_map (json);
    }
    igs_json_close_array (json);

    igs_json_add_string (json, STR_SERVICES);
    igs_json_open_array (json);
    igs_service_t *service, *tmp_service;
    HASH_ITER (hh, def->services_table, service, tmp_service)
    {
        igs_json_open_map (json);
        if (service->name) {
            igs_json_add_string (json, STR_NAME);
            igs_json_add_string (json, service->name);
            if (service->description) {
                igs_json_add_string (json, STR_DESCRIPTION);
                igs_json_add_string (json, service->description);
            }

            if (service->arguments) {
                igs_json_add_string (json, STR_ARGUMENTS);
                igs_json_open_array (json);
                igs_service_arg_t *argument = NULL;
                LL_FOREACH (service->arguments, argument)
                {
                    if (argument->name) {
                        igs_json_open_map (json);
                        igs_json_add_string (json, STR_NAME);
                        igs_json_add_string (json, argument->name);
                        igs_json_add_string (json, STR_TYPE);
                        igs_json_add_string (
                          json, s_value_type_to_string (argument->type));
                        igs_json_close_map (json);
                    }
                }
                igs_json_close_array (json);
            }

            if (service->replies) {
                igs_service_t *r, *r_tmp;
                igs_json_add_string (json, STR_REPLIES);
                igs_json_open_array (json);
                HASH_ITER(hh, service->replies, r, r_tmp){
                    if (r->name) {
                        igs_json_open_map (json);
                        igs_json_add_string (json, STR_NAME);
                        igs_json_add_string (json, r->name);
                        if (r->description) {
                            igs_json_add_string (json, STR_DESCRIPTION);
                            igs_json_add_string (json, r->description);
                        }
                        if (r->arguments) {
                            igs_json_add_string (json, STR_ARGUMENTS);
                            igs_json_open_array (json);
                            igs_service_arg_t *argument = NULL;
                            LL_FOREACH (r->arguments, argument){
                                if (argument->name) {
                                    igs_json_open_map (json);
                                    igs_json_add_string (json, STR_NAME);
                                    igs_json_add_string (json, argument->name);
                                    igs_json_add_string (json, STR_TYPE);
                                    igs_json_add_string (
                                      json,
                                      s_value_type_to_string (argument->type));
                                    igs_json_close_map (json);
                                }
                            }
                            igs_json_close_array (json);
                        }
                        igs_json_close_map (json);
                    }
                }
                igs_json_close_array (json);
            }
        }
        igs_json_close_map (json);
    }
    igs_json_close_array (json);

    igs_json_close_map (json);
    igs_json_close_map (json);
    char *res = igs_json_dump (json);
    igs_json_destroy (&json);
    return res;
}

char *parser_export_definition_legacy (igs_definition_t *def)
{
    assert (def);
    igs_json_t *json = igs_json_new ();
    igs_json_open_map (json);
    igs_json_add_string (json, STR_DEFINITION);
    igs_json_open_map (json);
    if (def->name) {
        igs_json_add_string (json, STR_NAME);
        igs_json_add_string (json, def->name);
    }
    if (def->family) {
        igs_json_add_string (json, STR_FAMILY);
        igs_json_add_string (json, def->family);
    }
    if (def->description) {
        igs_json_add_string (json, STR_DESCRIPTION);
        igs_json_add_string (json, def->description);
    }
    if (def->version) {
        igs_json_add_string (json, STR_VERSION);
        igs_json_add_string (json, def->version);
    }

    igs_json_add_string (json, STR_INPUTS);
    igs_json_open_array (json);
    igs_iop_t *iop, *tmp_iop;
    HASH_ITER (hh, def->inputs_table, iop, tmp_iop)
    {
        igs_json_open_map (json);
        if (iop->name) {
            igs_json_add_string (json, STR_NAME);
            igs_json_add_string (json, iop->name);
        }
        igs_json_add_string (json, STR_TYPE);
        igs_json_add_string (json, s_value_type_to_string (iop->value_type));
        // NB: inputs do not have intial values
        igs_json_close_map (json);
    }
    igs_json_close_array (json);

    igs_json_add_string (json, STR_OUTPUTS);
    igs_json_open_array (json);
    HASH_ITER (hh, def->outputs_table, iop, tmp_iop)
    {
        igs_json_open_map (json);
        if (iop->name) {
            igs_json_add_string (json, STR_NAME);
            igs_json_add_string (json, iop->name);
        }
        igs_json_add_string (json, STR_TYPE);
        igs_json_add_string (json, s_value_type_to_string (iop->value_type));
        igs_json_add_string (json, STR_VALUE);
        switch (iop->value_type) {
            case IGS_INTEGER_T:
                igs_json_add_int (json, iop->value.i);
                break;
            case IGS_DOUBLE_T:
                igs_json_add_double (json, iop->value.d);
                break;
            case IGS_BOOL_T:
                igs_json_add_bool (json, iop->value.b);
                break;
            case IGS_STRING_T:
                igs_json_add_string (json, iop->value.s);
                break;
            case IGS_IMPULSION_T:
                igs_json_add_null (json);
                break;
            case IGS_DATA_T: {
                if (iop->value_size){
                    char *data_to_store = (char *) zmalloc ((2 * iop->value_size + 1) * sizeof (char));
                    for (size_t i = 0; i < iop->value_size; i++)
                        sprintf (data_to_store + 2 * i, "%02X",
                                 *((uint8_t *) ((char *) iop->value.data + i)));
                    igs_json_add_string (json, data_to_store);
                    free (data_to_store);
                }else{
                    igs_json_add_null(json);
                }
                break;
            }
            default:
                igs_json_add_string (json, "");
                break;
        }
        igs_json_close_map (json);
    }
    igs_json_close_array (json);

    igs_json_add_string (json, STR_PARAMETERS);
    igs_json_open_array (json);
    HASH_ITER (hh, def->params_table, iop, tmp_iop)
    {
        igs_json_open_map (json);
        if (iop->name) {
            igs_json_add_string (json, STR_NAME);
            igs_json_add_string (json, iop->name);
        }
        igs_json_add_string (json, STR_TYPE);
        igs_json_add_string (json, s_value_type_to_string (iop->value_type));
        igs_json_add_string (json, STR_VALUE);
        switch (iop->value_type) {
            case IGS_INTEGER_T:
                igs_json_add_int (json, iop->value.i);
                break;
            case IGS_DOUBLE_T:
                igs_json_add_double (json, iop->value.d);
                break;
            case IGS_BOOL_T:
                igs_json_add_bool (json, iop->value.b);
                break;
            case IGS_STRING_T:
                igs_json_add_string (json, iop->value.s);
                break;
            case IGS_IMPULSION_T:
                igs_json_add_null (json);
                break;
            case IGS_DATA_T: {
                if (iop->value_size){
                    char *data_to_store = (char *) zmalloc ((2 * iop->value_size + 1) * sizeof (char));
                    for (size_t i = 0; i < iop->value_size; i++)
                        sprintf (data_to_store + 2 * i, "%02X",
                                 *((uint8_t *) ((char *) iop->value.data + i)));
                    igs_json_add_string (json, data_to_store);
                    free (data_to_store);
                }else{
                    igs_json_add_null(json);
                }
                break;
            }
            default:
                igs_json_add_string (json, "");
                break;
        }
        igs_json_close_map (json);
    }
    igs_json_close_array (json);

    igs_json_add_string (json, STR_SERVICES_DEPRECATED);
    igs_json_open_array (json);
    igs_service_t *service, *tmp_service;
    HASH_ITER (hh, def->services_table, service, tmp_service)
    {
        igs_json_open_map (json);
        if (service->name) {
            igs_json_add_string (json, STR_NAME);
            igs_json_add_string (json, service->name);
            if (service->description) {
                igs_json_add_string (json, STR_DESCRIPTION);
                igs_json_add_string (json, service->description);
            }

            if (service->arguments) {
                igs_json_add_string (json, STR_ARGUMENTS);
                igs_json_open_array (json);
                igs_service_arg_t *argument = NULL;
                LL_FOREACH (service->arguments, argument)
                {
                    if (argument->name) {
                        igs_json_open_map (json);
                        igs_json_add_string (json, STR_NAME);
                        igs_json_add_string (json, argument->name);
                        igs_json_add_string (json, STR_TYPE);
                        igs_json_add_string (
                          json, s_value_type_to_string (argument->type));
                        igs_json_close_map (json);
                    }
                }
                igs_json_close_array (json);
            }

            if (service->replies) {
                igs_service_t *r, *r_tmp;
                igs_json_add_string (json, STR_REPLIES);
                igs_json_open_array (json);
                HASH_ITER(hh, service->replies, r, r_tmp){
                    if (r->name) {
                        igs_json_open_map (json);
                        igs_json_add_string (json, STR_NAME);
                        igs_json_add_string (json, r->name);
                        if (r->description) {
                            igs_json_add_string (json, STR_DESCRIPTION);
                            igs_json_add_string (json, r->description);
                        }
                        if (r->arguments) {
                            igs_json_add_string (json, STR_ARGUMENTS);
                            igs_json_open_array (json);
                            igs_service_arg_t *argument = NULL;
                            LL_FOREACH (r->arguments, argument){
                                if (argument->name) {
                                    igs_json_open_map (json);
                                    igs_json_add_string (json, STR_NAME);
                                    igs_json_add_string (json, argument->name);
                                    igs_json_add_string (json, STR_TYPE);
                                    igs_json_add_string (
                                      json,
                                      s_value_type_to_string (argument->type));
                                    igs_json_close_map (json);
                                }
                            }
                            igs_json_close_array (json);
                        }
                        igs_json_close_map (json);
                    }
                }
                igs_json_close_array (json);
            }
        }
        igs_json_close_map (json);
    }
    igs_json_close_array (json);

    igs_json_close_map (json);
    igs_json_close_map (json);
    char *res = igs_json_dump (json);
    igs_json_destroy (&json);
    return res;
}

char *parser_export_mapping (igs_mapping_t *mapping)
{
    assert (mapping);
    igs_json_t *json = igs_json_new ();
    igs_json_open_map (json);
    igs_json_add_string (json, STR_MAPPINGS);
    igs_json_open_array (json);
    igs_map_t *elmt, *tmp;
    HASH_ITER (hh, mapping->map_elements, elmt, tmp)
    {
        igs_json_open_map (json);
        if (elmt->from_input) {
            igs_json_add_string (json, STR_FROM_INPUT);
            igs_json_add_string (json, elmt->from_input);
        }
        if (elmt->to_agent) {
            igs_json_add_string (json, STR_TO_AGENT);
            igs_json_add_string (json, elmt->to_agent);
        }
        if (elmt->to_output) {
            igs_json_add_string (json, STR_TO_OUTPUT);
            igs_json_add_string (json, elmt->to_output);
        }
        igs_json_close_map (json);
    }
    igs_json_close_array (json);

    igs_json_add_string (json, STR_SPLITS);
    igs_json_open_array (json);
    igs_split_t *elmt_split, *tmp_split;
    HASH_ITER (hh, mapping->split_elements, elmt_split, tmp_split)
    {
        igs_json_open_map (json);
        if (elmt_split->from_input) {
            igs_json_add_string (json, STR_FROM_INPUT);
            igs_json_add_string (json, elmt_split->from_input);
        }
        if (elmt_split->to_agent) {
            igs_json_add_string (json, STR_TO_AGENT);
            igs_json_add_string (json, elmt_split->to_agent);
        }
        if (elmt_split->to_output) {
            igs_json_add_string (json, STR_TO_OUTPUT);
            igs_json_add_string (json, elmt_split->to_output);
        }
        igs_json_close_map (json);
    }
    igs_json_close_array (json);

    igs_json_close_map (json);
    char *res = igs_json_dump (json);
    igs_json_destroy (&json);
    return res;
}

// legacy mapping export
char *parser_export_mapping_legacy (igs_mapping_t *mapping)
{
    assert (mapping);
    igs_json_t *json = igs_json_new ();
    igs_json_open_map (json);
    igs_json_add_string (json, STR_LEGACY_MAPPING);
    igs_json_open_map (json);
    igs_json_add_string (json, STR_LEGACY_MAPPINGS);
    igs_json_open_array (json);
    igs_map_t *elmt, *tmp;
    HASH_ITER (hh, mapping->map_elements, elmt, tmp)
    {
        igs_json_open_map (json);
        if (elmt->from_input) {
            igs_json_add_string (json, STR_LEGACY_FROM_INPUT);
            igs_json_add_string (json, elmt->from_input);
        }
        if (elmt->to_agent) {
            igs_json_add_string (json, STR_LEGACY_TO_AGENT);
            igs_json_add_string (json, elmt->to_agent);
        }
        if (elmt->to_output) {
            igs_json_add_string (json, STR_LEGACY_TO_OUTPUT);
            igs_json_add_string (json, elmt->to_output);
        }
        igs_json_close_map (json);
    }
    igs_json_close_array (json);
    igs_json_close_map (json);
    igs_json_close_map (json);

    char *res = igs_json_dump (json);
    igs_json_destroy (&json);
    return res;
}

////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////
igs_result_t igsagent_definition_load_str (igsagent_t *agent,
                                           const char *json_str)
{
    assert (agent);
    assert (json_str);
    // Try to load definition
    igs_definition_t *tmp = parser_load_definition (json_str);
    if (tmp == NULL) {
        igsagent_debug (agent, "json string caused an error and was ignored");
        return IGS_FAILURE;
    }
    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    igsagent_set_name (agent, tmp->name);
    definition_free_definition (&agent->definition);
    agent->definition = tmp;
    agent->network_need_to_send_definition_update = true;
    model_read_write_unlock (__FUNCTION__, __LINE__);
    return IGS_SUCCESS;
}

igs_result_t igsagent_definition_load_file (igsagent_t *agent,
                                            const char *file_path)
{
    assert (agent);
    assert (file_path);
    // Try to load definition
    igs_definition_t *tmp = parser_load_definition_from_path (file_path);
    if (tmp == NULL) {
        igsagent_debug (
          agent, "json file content at '%s' caused an error and was ignored",
          file_path);
        return IGS_FAILURE;
    }
    model_read_write_lock (__FUNCTION__, __LINE__);
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    igsagent_set_name (agent, tmp->name);
    definition_free_definition (&agent->definition);
    agent->definition_path = s_strndup (file_path, IGS_MAX_PATH_LENGTH - 1);
    agent->definition = tmp;
    agent->network_need_to_send_definition_update = true;
    model_read_write_unlock (__FUNCTION__, __LINE__);
    return IGS_SUCCESS;
}
