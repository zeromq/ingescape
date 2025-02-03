/*  =========================================================================
    parser - parse/generate mapping and definition

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "ingescape_private.h"
#include "yajl_gen.h"
#include "yajl_tree.h"

#define STR_DEFINITION "definition"
#define STR_NAME "name"
#define STR_CLASS "class"
#define STR_PACKAGE "package"
#define STR_FAMILY "family"
#define STR_DESCRIPTION "description"
#define STR_DETAILED_TYPE "detailed_type"
#define STR_SPECIFICATION "specification"
#define STR_VERSION "version"
#define STR_ATTRIBUTES "attributes"
#define STR_ATTRIBUTES_DEPRECATED "parameters" // deprecated since ingescape 4.0 or greater
#define STR_OUTPUTS "outputs"
#define STR_INPUTS "inputs"
#define STR_SERVICES "services"
#define STR_SERVICES_DEPRECATED "calls" // deprecated since ingescape 3.0 or greater
#define STR_ARGUMENTS "arguments"
#define STR_REPLIES "replies"
#define STR_TYPE "type"
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


igs_io_value_type_t s_string_to_value_type (const char *str)
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

const char *s_value_type_to_string (igs_io_value_type_t type)
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
            igs_error ("unknown igs_io_value_type_t %d to convert", type);
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
    const char *attributes_path[] = {STR_DEFINITION, STR_ATTRIBUTES, NULL};
    const char *attributes_path_deprecated[] = {STR_DEFINITION, STR_ATTRIBUTES_DEPRECATED, NULL};
    const char *services_path[] = {STR_DEFINITION, STR_SERVICES, NULL};
    const char *service_path_deprecated[] = {STR_DEFINITION, STR_SERVICES_DEPRECATED, NULL};
    const char *service_description_path[] = {STR_DESCRIPTION, NULL};
    const char *arguments_path[] = {STR_ARGUMENTS, NULL};
    const char *argument_description_path[] = {STR_DESCRIPTION, NULL};
    const char *agent_name_path[] = {STR_DEFINITION, STR_NAME, NULL};
    const char *name_path[] = {STR_NAME, NULL};
    const char *class_path[] = {STR_DEFINITION, STR_CLASS, NULL};
    const char *package_path[] = {STR_DEFINITION, STR_PACKAGE, NULL};
    const char *constraint_path[] = {STR_CONSTRAINT, NULL};
    const char *io_description_path[] = {STR_DESCRIPTION, NULL};
    const char *io_detailed_type_path[] = {STR_DETAILED_TYPE, NULL};
    const char *io_specification_path[] = {STR_SPECIFICATION, NULL};
    const char *family_path[] = {STR_DEFINITION, STR_FAMILY, NULL};
    const char *type_path[] = {STR_TYPE, NULL};
    const char *replies_path[] = {STR_REPLIES, NULL};

    // name is mandatory
    igs_json_node_t *name = igs_json_node_find (*json, agent_name_path);
    if (name && name->type == IGS_JSON_STRING && name->u.string) {
        char *n = s_strndup (name->u.string, IGS_MAX_AGENT_NAME_LENGTH);
        size_t changes = model_clean_string(n, IGS_MAX_AGENT_NAME_LENGTH);
        if (changes)
            igs_warn ("definition name '%s' has been changed to '%s'", name->u.string, n);
        //FIXME: Use a definition method to create the definition
        definition = (igs_definition_t *) zmalloc (sizeof (igs_definition_t));
        definition->name = n;
        definition->inputs_names_ordered = zlist_new();
        zlist_comparefn(definition->inputs_names_ordered, (zlist_compare_fn*) strcmp);
        zlist_autofree(definition->inputs_names_ordered);
        definition->inputs_table = zhashx_new();
        definition->outputs_names_ordered = zlist_new();
        zlist_comparefn(definition->outputs_names_ordered, (zlist_compare_fn*) strcmp);
        zlist_autofree(definition->outputs_names_ordered);
        definition->outputs_table = zhashx_new();
        definition->attributes_names_ordered = zlist_new();
        zlist_comparefn(definition->attributes_names_ordered, (zlist_compare_fn*) strcmp);
        zlist_autofree(definition->attributes_names_ordered);
        definition->attributes_table = zhashx_new();
        definition->services_names_ordered = zlist_new();
        zlist_comparefn(definition->services_names_ordered, (zlist_compare_fn*) strcmp);
        zlist_autofree(definition->services_names_ordered);
        definition->services_table = zhashx_new();
    } else {
        igs_json_node_destroy (json);
        return NULL;
    }

    // class
    igs_json_node_t *class = igs_json_node_find (*json, class_path);
    if (class && class->type == IGS_JSON_STRING && class->u.string){
        definition->my_class = strdup (class->u.string);
        size_t changes = model_clean_string(definition->my_class, IGS_MAX_AGENT_CLASS_LENGTH);
        if (changes)
            igs_warn ("definition class '%s' has been changed to '%s'", class->u.string, definition->my_class);
    }

    // package
    igs_json_node_t *package = igs_json_node_find (*json, package_path);
    if (package && package->type == IGS_JSON_STRING && package->u.string){
        definition->package = strdup (package->u.string);
        size_t changes = model_clean_string(definition->package, IGS_MAX_AGENT_PACKAGE_LENGTH);
        if (changes)
            igs_warn ("definition package '%s' has been changed to '%s'", package->u.string, definition->package);
    }

    // family
    igs_json_node_t *family = igs_json_node_find (*json, family_path);
    if (family && family->type == IGS_JSON_STRING && family->u.string){
        definition->family = s_strndup(family->u.string, IGS_MAX_FAMILY_LENGTH);
        size_t changes = model_clean_string(definition->family, IGS_MAX_FAMILY_LENGTH);
        if (changes)
            igs_warn ("definition family '%s' has been changed to '%s'", family->u.string, definition->family);
    }

    // agent description
    igs_json_node_t *description = igs_json_node_find (*json, description_path);
    if (description && description->type == IGS_JSON_STRING && description->u.string)
        definition->description = s_strndup (description->u.string, IGS_MAX_DESCRIPTION_LENGTH);

    // version
    igs_json_node_t *version = igs_json_node_find (*json, version_path);
    if (version && version->type == IGS_JSON_STRING && version->u.string){
        definition->version = s_strndup(version->u.string, IGS_MAX_VERSION_LENGTH);
        size_t changes = model_clean_string(definition->version, IGS_MAX_VERSION_LENGTH);
        if (changes)
            igs_warn ("definition version '%s' has been changed to '%s'", version->u.string, definition->version);
    }

    // inputs
    igs_json_node_t *inputs = igs_json_node_find (*json, inputs_path);
    if (inputs && inputs->type == IGS_JSON_ARRAY) {
        for (size_t i = 0; i < inputs->u.array.len; i++) {
            igs_json_node_t *io_name = igs_json_node_find (inputs->u.array.values[i], name_path);
            if (io_name && io_name->type == IGS_JSON_STRING && io_name->u.string) {
                char *corrected_name = s_strndup (io_name->u.string, IGS_MAX_IO_NAME_LENGTH);
                size_t changes = model_clean_string(corrected_name, IGS_MAX_IO_NAME_LENGTH);
                if (changes)
                    igs_warn ("input name '%s' has been changed to '%s'", io_name->u.string, corrected_name);
                igs_io_t *io = zhashx_lookup(definition->inputs_table, corrected_name);
                if (io) {
                    igs_warn ("input with name '%s' already exists : ignoring new one", corrected_name);
                    free (corrected_name);
                    continue;
                }

                io = (igs_io_t *) zmalloc (sizeof (igs_io_t));
                io->type = IGS_INPUT_T;
                io->value_type = IGS_UNKNOWN_T;
                io->name = corrected_name;
                io->io_callbacks = zlist_new();

                igs_json_node_t *io_type = igs_json_node_find (inputs->u.array.values[i], type_path);
                if (io_type && io_type->type == IGS_JSON_STRING && io_type->u.string)
                    io->value_type = s_string_to_value_type (io_type->u.string);

                igs_json_node_t *constraint = igs_json_node_find (inputs->u.array.values[i], constraint_path);
                if (constraint && constraint->type == IGS_JSON_STRING && constraint->u.string){
                    char *error = NULL;
                    io->constraint = model_parse_constraint(io->value_type, constraint->u.string, &error);
                    if (error)
                        igs_error ("%s", error);
                }

                igs_json_node_t *io_description = igs_json_node_find (inputs->u.array.values[i], io_description_path);
                if (io_description && io_description->type == IGS_JSON_STRING && io_description->u.string){
                    if (io->description)
                        free(io->description);
                    io->description = s_strndup(io_description->u.string, IGS_MAX_DESCRIPTION_LENGTH);
                }

                igs_json_node_t *io_detailed_type = igs_json_node_find (inputs->u.array.values[i], io_detailed_type_path);
                if (io_detailed_type && io_detailed_type->type == IGS_JSON_STRING && io_detailed_type->u.string){
                    if (io->detailed_type)
                        free(io->detailed_type);
                    io->detailed_type = s_strndup(io_detailed_type->u.string, IGS_MAX_DETAILED_TYPE_LENGTH);
                    size_t changes = model_clean_string(io->detailed_type, IGS_MAX_DETAILED_TYPE_LENGTH);
                    if (changes)
                        igs_warn ("input detailed type '%s' has been changed to '%s'", io_detailed_type->u.string, io->detailed_type);
                }

                igs_json_node_t *io_specification = igs_json_node_find (inputs->u.array.values[i], io_specification_path);
                if (io_specification && io_specification->type == IGS_JSON_STRING && io_specification->u.string){
                    if (io->specification)
                        free(io->specification);
                    io->specification = s_strndup(io_specification->u.string, IGS_MAX_SPECIFICATION_LENGTH);
                }
                zlist_append(definition->inputs_names_ordered, strdup(io->name));
                zhashx_insert(definition->inputs_table, io->name, io);
            }
        }
    }else if(inputs)
        igs_error ("inputs are not an array : ignoring");

    // outputs
    igs_json_node_t *outputs = igs_json_node_find (*json, outputs_path);
    if (outputs && outputs->type == IGS_JSON_ARRAY) {
        for (size_t i = 0; i < outputs->u.array.len; i++) {
            igs_json_node_t *io_name = igs_json_node_find (outputs->u.array.values[i], name_path);
            if (io_name && io_name->type == IGS_JSON_STRING && io_name->u.string) {
                char *corrected_name = s_strndup (io_name->u.string, IGS_MAX_IO_NAME_LENGTH);
                size_t changes = model_clean_string(corrected_name, IGS_MAX_IO_NAME_LENGTH);
                if (changes)
                    igs_warn ("output name '%s' has been changed to '%s'", io_name->u.string, corrected_name);
                igs_io_t *io = zhashx_lookup(definition->outputs_table, corrected_name);
                if (io) {
                    igs_warn ("output with name '%s' already exists : ignoring new one", corrected_name);
                    free (corrected_name);
                    continue;
                }

                io = (igs_io_t *) zmalloc (sizeof (igs_io_t));
                io->type = IGS_OUTPUT_T;
                io->value_type = IGS_UNKNOWN_T;
                io->name = corrected_name;
                io->io_callbacks = zlist_new();

                igs_json_node_t *io_type = igs_json_node_find (outputs->u.array.values[i], type_path);
                if (io_type && io_type->type == IGS_JSON_STRING && io_type->u.string)
                    io->value_type = s_string_to_value_type (io_type->u.string);

                igs_json_node_t *constraint = igs_json_node_find (outputs->u.array.values[i], constraint_path);
                if (constraint && constraint->type == IGS_JSON_STRING && constraint->u.string){
                    char *error = NULL;
                    io->constraint = model_parse_constraint(io->value_type, constraint->u.string, &error);
                }

                igs_json_node_t *io_description = igs_json_node_find (outputs->u.array.values[i], io_description_path);
                if (io_description && io_description->type == IGS_JSON_STRING && io_description->u.string){
                    if (io->description)
                        free(io->description);
                    io->description = s_strndup(io_description->u.string, IGS_MAX_DESCRIPTION_LENGTH);
                }

                igs_json_node_t *io_detailed_type = igs_json_node_find (outputs->u.array.values[i], io_detailed_type_path);
                if (io_detailed_type && io_detailed_type->type == IGS_JSON_STRING && io_detailed_type->u.string){
                    if (io->detailed_type)
                        free(io->detailed_type);
                    io->detailed_type = s_strndup(io_detailed_type->u.string, IGS_MAX_DETAILED_TYPE_LENGTH);
                    size_t changes = model_clean_string(io->detailed_type, IGS_MAX_DETAILED_TYPE_LENGTH);
                    if (changes)
                        igs_warn ("output detailed type '%s' has been changed to '%s'", io_detailed_type->u.string, io->detailed_type);
                }

                igs_json_node_t *io_specification = igs_json_node_find (outputs->u.array.values[i], io_specification_path);
                if (io_specification && io_specification->type == IGS_JSON_STRING && io_specification->u.string){
                    if (io->specification)
                        free(io->specification);
                    io->specification = s_strndup(io_specification->u.string, IGS_MAX_SPECIFICATION_LENGTH);
                }

                zlist_append(definition->outputs_names_ordered, strdup(io->name));
                zhashx_insert(definition->outputs_table, io->name, io);
            }
        }
    } else if (outputs)
        igs_error ("outputs are not an array : ignoring");

    // attributes
    igs_json_node_t *attributes = igs_json_node_find (*json, attributes_path);
    if (attributes == NULL)
        attributes = igs_json_node_find (*json, attributes_path_deprecated);
    if (attributes && attributes->type == IGS_JSON_ARRAY) {
        for (size_t i = 0; i < attributes->u.array.len; i++) {
            igs_json_node_t *io_name = igs_json_node_find (attributes->u.array.values[i], name_path);
            if (io_name && io_name->type == IGS_JSON_STRING && io_name->u.string) {
                char *corrected_name = s_strndup (io_name->u.string, IGS_MAX_IO_NAME_LENGTH);
                size_t changes = model_clean_string(corrected_name, IGS_MAX_IO_NAME_LENGTH);
                if (changes)
                    igs_warn ("attribute name '%s' has been changed to '%s'", io_name->u.string, corrected_name);
                igs_io_t *io = zhashx_lookup(definition->attributes_table, corrected_name);
                if (io) {
                    igs_warn ("attribute with name '%s' already exists : ignoring new one", corrected_name);
                    free (corrected_name);
                    continue;
                }

                io = (igs_io_t *) zmalloc (sizeof (igs_io_t));
                io->type = IGS_ATTRIBUTE_T;
                io->value_type = IGS_UNKNOWN_T;
                io->name = corrected_name;
                io->io_callbacks = zlist_new();

                igs_json_node_t *io_type = igs_json_node_find (attributes->u.array.values[i], type_path);
                if (io_type && io_type->type == IGS_JSON_STRING && io_type->u.string)
                    io->value_type = s_string_to_value_type (io_type->u.string);

                igs_json_node_t *constraint = igs_json_node_find (attributes->u.array.values[i], constraint_path);
                if (constraint && constraint->type == IGS_JSON_STRING && constraint->u.string){
                    char *error = NULL;
                    io->constraint = model_parse_constraint(io->value_type, constraint->u.string, &error);
                }

                igs_json_node_t *io_description = igs_json_node_find (attributes->u.array.values[i], io_description_path);
                if (io_description && io_description->type == IGS_JSON_STRING && io_description->u.string){
                    if (io->description)
                        free(io->description);
                    io->description = s_strndup(io_description->u.string, IGS_MAX_DESCRIPTION_LENGTH);
                }

                igs_json_node_t *io_detailed_type = igs_json_node_find (attributes->u.array.values[i], io_detailed_type_path);
                if (io_detailed_type && io_detailed_type->type == IGS_JSON_STRING && io_detailed_type->u.string){
                    if (io->detailed_type)
                        free(io->detailed_type);
                    io->detailed_type = s_strndup(io_detailed_type->u.string, IGS_MAX_DETAILED_TYPE_LENGTH);
                    size_t changes = model_clean_string(io->detailed_type, IGS_MAX_DETAILED_TYPE_LENGTH);
                    if (changes)
                        igs_warn ("attribute detailed type '%s' has been changed to '%s'", io_detailed_type->u.string, io->detailed_type);
                }

                igs_json_node_t *io_specification = igs_json_node_find (attributes->u.array.values[i], io_specification_path);
                if (io_specification && io_specification->type == IGS_JSON_STRING && io_specification->u.string){
                    if (io->specification)
                        free(io->specification);
                    io->specification = s_strndup(io_specification->u.string, IGS_MAX_SPECIFICATION_LENGTH);
                }
                zlist_append(definition->attributes_names_ordered, strdup(io->name));
                zhashx_insert(definition->attributes_table, io->name, io);
            }
        }
    } else if (attributes)
        igs_error ("attributes are not an array : ignoring");

    // services
    igs_json_node_t *services = igs_json_node_find (*json, services_path);
    if (services == NULL)
        services = igs_json_node_find (*json, service_path_deprecated);
    if (services && services->type == IGS_JSON_ARRAY) {
        for (size_t i = 0; i < services->u.array.len; i++) {
            igs_json_node_t *service_name =
              igs_json_node_find (services->u.array.values[i], name_path);
            if (service_name && service_name->type == IGS_JSON_STRING && service_name->u.string) {
                char *corrected_name = s_strndup (service_name->u.string, IGS_MAX_SERVICE_NAME_LENGTH);
                size_t changes = model_clean_string(corrected_name, IGS_MAX_SERVICE_NAME_LENGTH);
                if (changes)
                    igs_warn ("service name '%s' has been changed to '%s'", service_name->u.string, corrected_name);
                igs_service_t *service = zhashx_lookup(definition->services_table, corrected_name);
                if (service) {
                    igs_warn ("service with name '%s' already exists : ignoring new one", corrected_name);
                    free (corrected_name);
                    continue;
                }

                service = (igs_service_t *) zmalloc (sizeof (igs_service_t));
                service->name = corrected_name;
                service->replies_names_ordered = zlist_new();
                zlist_comparefn(service->replies_names_ordered, (zlist_compare_fn*) strcmp);
                zlist_autofree(service->replies_names_ordered);
                service->replies = zhashx_new();

                description = igs_json_node_find (services->u.array.values[i], service_description_path);
                if (description && description->type == IGS_JSON_STRING && description->u.string)
                    service->description = s_strndup (description->u.string, IGS_MAX_DESCRIPTION_LENGTH);

                igs_json_node_t *arguments = igs_json_node_find (services->u.array.values[i], arguments_path);
                if (arguments && arguments->type == IGS_JSON_ARRAY) {
                    for (size_t j = 0; j < arguments->u.array.len; j++) {
                        if (arguments->u.array.values[j] && arguments->u.array.values[j]->type == IGS_JSON_MAP) {
                            igs_json_node_t *arg_name = igs_json_node_find (arguments->u.array.values[j], name_path);
                            if (arg_name && arg_name->type == IGS_JSON_STRING && arg_name->u.string) {
                                char *corrected_arg_name = s_strndup (arg_name->u.string, IGS_MAX_SERVICE_ARG_NAME_LENGTH);
                                changes = model_clean_string(corrected_arg_name, IGS_MAX_SERVICE_ARG_NAME_LENGTH);
                                if (changes)
                                    igs_warn ("argument name '%s' has been changed to '%s'", arg_name->u.string, corrected_arg_name);
                                igs_service_arg_t *new_arg = (igs_service_arg_t *) zmalloc (sizeof (igs_service_arg_t));
                                new_arg->name = corrected_arg_name;
                                igs_json_node_t *arg_type = igs_json_node_find (arguments->u.array.values[j], type_path);
                                if (arg_type && arg_type->type == IGS_JSON_STRING && arg_type->u.string)
                                    new_arg->type = s_string_to_value_type (arg_type->u.string);

                                description = igs_json_node_find (arguments->u.array.values[j], argument_description_path);
                                if (description && description->type == IGS_JSON_STRING && description->u.string)
                                    new_arg->description = s_strndup (description->u.string, IGS_MAX_DESCRIPTION_LENGTH);

                                igs_service_arg_t *last_arg = service->arguments;
                                while (last_arg && last_arg->next) {
                                    last_arg = last_arg->next;
                                }
                                if (last_arg)
                                    last_arg->next = new_arg;
                                else
                                    service->arguments = new_arg;
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
                            char *corrected_reply_name = s_strndup (reply_name->u.string, IGS_MAX_SERVICE_NAME_LENGTH);
                            changes = model_clean_string(corrected_reply_name, IGS_MAX_SERVICE_NAME_LENGTH);
                            if (changes)
                                igs_warn ("reply name '%s' has been changed to '%s'", reply_name->u.string, corrected_reply_name);
                            igs_service_t *my_reply = (igs_service_t *) zmalloc (sizeof (igs_service_t));
                            my_reply->name = corrected_reply_name;
                            
                            igs_json_node_t *replyDescription = igs_json_node_find (replies->u.array.values[j], service_description_path);
                            if (replyDescription && replyDescription->type == IGS_JSON_STRING && replyDescription->u.string)
                                my_reply->description = s_strndup (replyDescription->u.string, IGS_MAX_DESCRIPTION_LENGTH);
                            
                            my_reply->replies_names_ordered = zlist_new();
                            zlist_comparefn(my_reply->replies_names_ordered, (zlist_compare_fn*) strcmp);
                            zlist_autofree(my_reply->replies_names_ordered);
                            my_reply->replies = zhashx_new();

                            arguments = igs_json_node_find (replies->u.array.values[j], arguments_path);
                            if (arguments && arguments->type == IGS_JSON_ARRAY) {
                                for (size_t len = 0; len < arguments->u.array.len; len++) {
                                    if (arguments->u.array.values[len] && arguments->u.array.values[len]->type == IGS_JSON_MAP) {
                                        igs_json_node_t *arg_name = igs_json_node_find (arguments->u.array.values[len], name_path);
                                        if (arg_name && arg_name->type == IGS_JSON_STRING && arg_name->u.string) {
                                            char *corrected_reply_arg_name = s_strndup (arg_name->u.string, IGS_MAX_SERVICE_ARG_NAME_LENGTH);
                                            changes = model_clean_string(corrected_reply_arg_name, IGS_MAX_SERVICE_ARG_NAME_LENGTH);
                                            if (changes)
                                                igs_warn ("reply argument name '%s' has been changed to '%s'", arg_name->u.string, corrected_reply_arg_name);
                                            igs_service_arg_t *new_arg = (igs_service_arg_t *) zmalloc (sizeof (igs_service_arg_t));
                                            new_arg->name = corrected_reply_arg_name;
                                            
                                            description = igs_json_node_find (arguments->u.array.values[j], argument_description_path);
                                            if (description && description->type == IGS_JSON_STRING && description->u.string)
                                                new_arg->description = s_strndup (description->u.string, IGS_MAX_DESCRIPTION_LENGTH);
                                            
                                            igs_json_node_t *arg_type = igs_json_node_find (arguments->u.array.values[len], type_path);
                                            if (arg_type && arg_type->type == IGS_JSON_STRING && arg_type->u.string)
                                                new_arg->type = s_string_to_value_type (arg_type->u.string);
                                            igs_service_arg_t *last_arg = my_reply->arguments;
                                            while (last_arg && last_arg->next) {
                                                last_arg = last_arg->next;
                                            }
                                            if (last_arg)
                                                last_arg->next = new_arg;
                                            else
                                                my_reply->arguments = new_arg;
                                        }
                                    }
                                }
                            }
                            zlist_append(service->replies_names_ordered, strdup(my_reply->name));
                            zhashx_insert(service->replies, my_reply->name, my_reply);
                        }
                    }
                }
                zlist_append(definition->services_names_ordered, strdup(service->name));
                zhashx_insert(definition->services_table, service->name, service);
            }
        }
    }
    else
    if (services)
        igs_error ("services are not an array : ignoring");

    igs_json_node_destroy(json);
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
    mapping->map_elements = zlist_new();
    mapping->split_elements = zlist_new();

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
                from_input_node = igs_json_node_find (mappings->u.array.values[i], from_input_path);
                to_agent_node = igs_json_node_find (mappings->u.array.values[i], to_agent_path);
                to_output_node = igs_json_node_find (mappings->u.array.values[i], to_output_path);
            }else{
                from_input_node = igs_json_node_find (mappings->u.array.values[i], alternate_from_input_path);
                to_agent_node = igs_json_node_find (mappings->u.array.values[i], alternate_to_agent_path);
                to_output_node = igs_json_node_find (mappings->u.array.values[i], alternate_to_output_path);
            }
            if (from_input_node && from_input_node->type == IGS_JSON_STRING && from_input_node->u.string) {
                char *corrected_name = s_strndup (from_input_node->u.string, IGS_MAX_IO_NAME_LENGTH);
                size_t changes = model_clean_string(corrected_name, IGS_MAX_IO_NAME_LENGTH);
                if (changes)
                    igs_warn("mapping input name '%s' has been changed to '%s'", from_input_node->u.string, corrected_name);
                from_input = corrected_name;
            }
            if (to_agent_node && to_agent_node->type == IGS_JSON_STRING && to_agent_node->u.string) {
                char *corrected_name = s_strndup (to_agent_node->u.string, IGS_MAX_AGENT_NAME_LENGTH);
                size_t changes = model_clean_string(corrected_name, IGS_MAX_AGENT_NAME_LENGTH);
                if (changes)
                    igs_warn("mapping agent name '%s' has been changed to '%s'", to_agent_node->u.string, corrected_name);
                to_agent = corrected_name;
            }
            if (to_output_node && to_output_node->type == IGS_JSON_STRING && to_output_node->u.string) {
                char *corrected_name = s_strndup (to_output_node->u.string, IGS_MAX_IO_NAME_LENGTH);
                size_t changes = model_clean_string(corrected_name, IGS_MAX_IO_NAME_LENGTH);
                if (changes)
                    igs_warn("mapping output name '%s' has been changed to '%s'", to_output_node->u.string, corrected_name);
                to_output = corrected_name;
            }
            if (from_input && to_agent && to_output) {
                size_t len = strlen (from_input) + strlen (to_agent) + strlen (to_output) + 3 + 1;
                char *mashup = (char *) zmalloc (len * sizeof (char));
                strcpy (mashup, from_input);
                strcat (mashup, "."); // separator
                strcat (mashup, to_agent);
                strcat (mashup, "."); // separator
                strcat (mashup, to_output);
                mashup[len - 1] = '\0';
                uint64_t h = mapping_djb2_hash ((unsigned char *) mashup);
                free (mashup);

                igs_map_t *map_emlt = NULL;
                igs_map_t *map_tmp = zlist_first(mapping->map_elements);
                while (map_tmp) {
                    if (map_tmp->id == h){
                        map_emlt = map_tmp;
                        break;
                    }
                    map_tmp = zlist_next(mapping->map_elements);
                }

                if (!map_emlt) {
                    // element does not exist yet : create and register it
                    igs_map_t *new = mapping_create_mapping_element (from_input, to_agent, to_output);
                    new->id = h;
                    zlist_append(mapping->map_elements, new);
                }
                else
                    igs_error ("mapping hash already exists for %s->%s.%s", from_input, to_agent, to_output);
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
            from_input_node = igs_json_node_find (splits->u.array.values[i], from_input_path);
            to_agent_node = igs_json_node_find (splits->u.array.values[i], to_agent_path);
            to_output_node = igs_json_node_find (splits->u.array.values[i], to_output_path);

            if (from_input_node && from_input_node->type == IGS_JSON_STRING && from_input_node->u.string) {
                char *corrected_name = s_strndup (from_input_node->u.string, IGS_MAX_IO_NAME_LENGTH);
                size_t changes = model_clean_string(corrected_name, IGS_MAX_IO_NAME_LENGTH);
                if (changes)
                    igs_warn("split input name '%s' has been changed to '%s'", from_input_node->u.string, corrected_name);
                from_input = corrected_name;
            }
            if (to_agent_node && to_agent_node->type == IGS_JSON_STRING && to_agent_node->u.string) {
                char *corrected_name = s_strndup (to_agent_node->u.string, IGS_MAX_IO_NAME_LENGTH);
                size_t changes = model_clean_string(corrected_name, IGS_MAX_AGENT_NAME_LENGTH);
                if (changes)
                    igs_warn("split agent name '%s' has been changed to '%s'", to_agent_node->u.string, corrected_name);
                to_agent = corrected_name;
            }
            if (to_output_node && to_output_node->type == IGS_JSON_STRING && to_output_node->u.string) {
                char *corrected_name = s_strndup (to_output_node->u.string, IGS_MAX_IO_NAME_LENGTH);
                size_t changes = model_clean_string(corrected_name, IGS_MAX_IO_NAME_LENGTH);
                if (changes)
                    igs_warn("split output name '%s' has been changed to '%s'", to_output_node->u.string, corrected_name);
                to_output = corrected_name;
            }
            if (from_input && to_agent && to_output) {
                size_t len = strlen (from_input) + strlen (to_agent) + strlen (to_output) + 3 + 1;
                char *mashup = (char *) zmalloc (len * sizeof (char));
                strcpy (mashup, from_input);
                strcat (mashup, "."); // separator
                strcat (mashup, to_agent);
                strcat (mashup, "."); // separator
                strcat (mashup, to_output);
                mashup[len - 1] = '\0';
                uint64_t h = mapping_djb2_hash ((unsigned char *) mashup);
                free (mashup);

                igs_map_t *map_emlt = NULL;
                igs_map_t *map_tmp = zlist_first(mapping->split_elements);
                while (map_tmp) {
                    if (map_tmp->id == h){
                        map_emlt = map_tmp;
                        break;
                    }
                    map_tmp = zlist_next(mapping->split_elements);
                }
                if (!map_emlt) {
                    // element does not exist yet : create and register it
                    igs_split_t *new = split_create_split_element (from_input, to_agent, to_output);
                    new->id = h;
                    zlist_append(mapping->split_elements, new);
                }
                else
                    igs_error ("split hash already exists for %s->%s.%s", from_input, to_agent, to_output);
            }
            if (from_input)
                free (from_input);
            if (to_agent)
                free (to_agent);
            if (to_output)
                free (to_output);
        }
    }
    igs_json_node_destroy(json);
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
        igs_error ("parsed JSON is not a map : '%s'", json_str);
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
    if (def->my_class) {
        igs_json_add_string (json, STR_CLASS);
        igs_json_add_string (json, def->my_class);
    }
    if (def->package) {
        igs_json_add_string (json, STR_PACKAGE);
        igs_json_add_string (json, def->package);
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
    
    //FIXME: Code for I/O/A could be factored
    const char* io_name = zlist_first(def->inputs_names_ordered);
    while (io_name) {
        igs_io_t *io = (igs_io_t*) zhashx_lookup (def->inputs_table, io_name);
        assert(io);
        igs_json_open_map (json);
        if (io->name) {
            igs_json_add_string (json, STR_NAME);
            igs_json_add_string (json, io->name);
        }
        igs_json_add_string (json, STR_TYPE);
        igs_json_add_string (json, s_value_type_to_string (io->value_type));
        char constraint_expression[IGS_MAX_CONSTRAINT_LENGTH] = "";
        if (io->constraint){
            if (io->constraint->type == IGS_CONSTRAINT_MIN){
                if (io->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "min %d",
                             io->constraint->min_int.min);
                    igs_json_add_string(json, constraint_expression);
                }else if (io->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "min %f",
                             io->constraint->min_double.min);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (io->constraint->type == IGS_CONSTRAINT_MAX){
                if (io->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "max %d",
                             io->constraint->max_int.max);
                    igs_json_add_string(json, constraint_expression);
                }else if (io->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "max %f",
                             io->constraint->max_double.max);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (io->constraint->type == IGS_CONSTRAINT_RANGE){
                if (io->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "[%d, %d]",
                             io->constraint->range_int.min,
                             io->constraint->range_int.max);
                    igs_json_add_string(json, constraint_expression);
                }else if (io->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "[%f, %f]",
                             io->constraint->range_double.min,
                             io->constraint->range_double.max);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (io->constraint->type == IGS_CONSTRAINT_REGEXP){
                igs_json_add_string (json, STR_CONSTRAINT);
                snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "~ %s",
                         io->constraint->regexp.string);
                igs_json_add_string(json, constraint_expression);
            }
        }
        if (io->description){
            igs_json_add_string (json, STR_DESCRIPTION);
            igs_json_add_string (json, io->description);
        }
        if (io->detailed_type){
            igs_json_add_string (json, STR_DETAILED_TYPE);
            igs_json_add_string (json, io->detailed_type);
        }
        if (io->specification){
            igs_json_add_string (json, STR_SPECIFICATION);
            igs_json_add_string (json, io->specification);
        }
        igs_json_close_map (json);
        io_name = zlist_next(def->inputs_names_ordered);
    }
    igs_json_close_array (json);
    
    igs_json_add_string (json, STR_OUTPUTS);
    igs_json_open_array (json);
    io_name = zlist_first(def->outputs_names_ordered);
    while (io_name) {
        igs_io_t *io = (igs_io_t*) zhashx_lookup (def->outputs_table, io_name);
        assert(io);
        igs_json_open_map (json);
        if (io->name) {
            igs_json_add_string (json, STR_NAME);
            igs_json_add_string (json, io->name);
        }
        igs_json_add_string (json, STR_TYPE);
        igs_json_add_string (json, s_value_type_to_string (io->value_type));
        
        char constraint_expression[IGS_MAX_CONSTRAINT_LENGTH] = "";
        if (io->constraint){
            if (io->constraint->type == IGS_CONSTRAINT_MIN){
                if (io->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "min %d",
                             io->constraint->min_int.min);
                    igs_json_add_string(json, constraint_expression);
                }else if (io->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "min %f",
                             io->constraint->min_double.min);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (io->constraint->type == IGS_CONSTRAINT_MAX){
                if (io->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "max %d",
                             io->constraint->max_int.max);
                    igs_json_add_string(json, constraint_expression);
                }else if (io->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "max %f",
                             io->constraint->max_double.max);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (io->constraint->type == IGS_CONSTRAINT_RANGE){
                if (io->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "[%d, %d]",
                             io->constraint->range_int.min,
                             io->constraint->range_int.max);
                    igs_json_add_string(json, constraint_expression);
                }else if (io->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "[%f, %f]",
                             io->constraint->range_double.min,
                             io->constraint->range_double.max);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (io->constraint->type == IGS_CONSTRAINT_REGEXP){
                igs_json_add_string (json, STR_CONSTRAINT);
                snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "~ %s",
                         io->constraint->regexp.string);
                igs_json_add_string(json, constraint_expression);
            }
        }
        if (io->description){
            igs_json_add_string (json, STR_DESCRIPTION);
            igs_json_add_string (json, io->description);
        }
        if (io->detailed_type){
            igs_json_add_string (json, STR_DETAILED_TYPE);
            igs_json_add_string (json, io->detailed_type);
        }
        if (io->specification){
            igs_json_add_string (json, STR_SPECIFICATION);
            igs_json_add_string (json, io->specification);
        }
        igs_json_close_map (json);
        io_name = zlist_next(def->outputs_names_ordered);
    }
    igs_json_close_array (json);
    
    igs_json_add_string (json, STR_ATTRIBUTES);
    igs_json_open_array (json);
    io_name = zlist_first(def->attributes_names_ordered);
    while (io_name) {
        igs_io_t *io = (igs_io_t*) zhashx_lookup (def->attributes_table, io_name);
        assert(io);
        igs_json_open_map (json);
        if (io->name) {
            igs_json_add_string (json, STR_NAME);
            igs_json_add_string (json, io->name);
        }
        igs_json_add_string (json, STR_TYPE);
        igs_json_add_string (json, s_value_type_to_string (io->value_type));
        char constraint_expression[IGS_MAX_CONSTRAINT_LENGTH] = "";
        if (io->constraint){
            if (io->constraint->type == IGS_CONSTRAINT_MIN){
                if (io->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "min %d",
                             io->constraint->min_int.min);
                    igs_json_add_string(json, constraint_expression);
                }else if (io->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "min %f",
                             io->constraint->min_double.min);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (io->constraint->type == IGS_CONSTRAINT_MAX){
                if (io->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "max %d",
                             io->constraint->max_int.max);
                    igs_json_add_string(json, constraint_expression);
                }else if (io->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "max %f",
                             io->constraint->max_double.max);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (io->constraint->type == IGS_CONSTRAINT_RANGE){
                if (io->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "[%d, %d]",
                             io->constraint->range_int.min,
                             io->constraint->range_int.max);
                    igs_json_add_string(json, constraint_expression);
                }else if (io->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "[%f, %f]",
                             io->constraint->range_double.min,
                             io->constraint->range_double.max);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (io->constraint->type == IGS_CONSTRAINT_REGEXP){
                igs_json_add_string (json, STR_CONSTRAINT);
                snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "~ %s",
                         io->constraint->regexp.string);
                igs_json_add_string(json, constraint_expression);
            }
        }
        if (io->description){
            igs_json_add_string (json, STR_DESCRIPTION);
            igs_json_add_string (json, io->description);
        }
        if (io->detailed_type){
            igs_json_add_string (json, STR_DETAILED_TYPE);
            igs_json_add_string (json, io->detailed_type);
        }
        if (io->specification){
            igs_json_add_string (json, STR_SPECIFICATION);
            igs_json_add_string (json, io->specification);
        }
        igs_json_close_map (json);
        io_name = zlist_next(def->attributes_names_ordered);
    }
    igs_json_close_array (json);

    igs_json_add_string (json, STR_SERVICES);
    igs_json_open_array (json);
    const char* service_name = zlist_first(def->services_names_ordered);
    while (service_name) {
        igs_service_t* service = zhashx_lookup(def->services_table, service_name);
        assert(service);
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
                igs_service_arg_t *argument = service->arguments;
                while (argument) {
                    if (argument->name) {
                        igs_json_open_map (json);
                        igs_json_add_string (json, STR_NAME);
                        igs_json_add_string (json, argument->name);
                        if (argument->description) {
                            igs_json_add_string (json, STR_DESCRIPTION);
                            igs_json_add_string (json, argument->description);
                        }
                        igs_json_add_string (json, STR_TYPE);
                        igs_json_add_string (
                                                json, s_value_type_to_string (argument->type));
                        igs_json_close_map (json);
                    }
                    argument = argument->next;
                }
                igs_json_close_array (json);
            }

            if (service->replies && service->replies_names_ordered) {
                igs_json_add_string (json, STR_REPLIES);
                igs_json_open_array (json);
                const char* r_name = zlist_first(service->replies_names_ordered);
                while (r_name) {
                    igs_service_t* r = zhashx_lookup(service->replies, r_name);
                    assert(r);
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
                            igs_service_arg_t *argument = r->arguments;
                            while (argument) {
                                if (argument->name) {
                                    igs_json_open_map (json);
                                    igs_json_add_string (json, STR_NAME);
                                    igs_json_add_string (json, argument->name);
                                    if (argument->description) {
                                        igs_json_add_string (json, STR_DESCRIPTION);
                                        igs_json_add_string (json, argument->description);
                                    }
                                    igs_json_add_string (json, STR_TYPE);
                                    igs_json_add_string (
                                                            json,
                                                            s_value_type_to_string (argument->type));
                                    igs_json_close_map (json);
                                }
                                argument = argument->next;
                            }
                            igs_json_close_array (json);
                        }
                        igs_json_close_map (json);
                    }
                    r_name = zlist_next(service->replies_names_ordered);
                }
                igs_json_close_array (json);
            }
        }
        igs_json_close_map (json);
        service_name = zlist_next(def->services_names_ordered);
    }
    igs_json_close_array (json);

    igs_json_close_map (json);
    igs_json_close_map (json);
    char *res = igs_json_dump (json);
    igs_json_destroy (&json);
    return res;
}

char *parser_export_definition_legacy_v4 (igs_definition_t *def)
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
    if (def->my_class) {
        igs_json_add_string (json, STR_CLASS);
        igs_json_add_string (json, def->my_class);
    }
    if (def->package) {
        igs_json_add_string (json, STR_PACKAGE);
        igs_json_add_string (json, def->package);
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
    const char* io_name = zlist_first(def->inputs_names_ordered);
    while (io_name) {
        igs_io_t* io = zhashx_lookup(def->inputs_table, io_name);
        assert(io);
        igs_json_open_map (json);
        if (io->name) {
            igs_json_add_string (json, STR_NAME);
            igs_json_add_string (json, io->name);
        }
        igs_json_add_string (json, STR_TYPE);
        igs_json_add_string (json, s_value_type_to_string (io->value_type));
        char constraint_expression[IGS_MAX_CONSTRAINT_LENGTH] = "";
        if (io->constraint){
            if (io->constraint->type == IGS_CONSTRAINT_MIN){
                if (io->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "min %d",
                                io->constraint->min_int.min);
                    igs_json_add_string(json, constraint_expression);
                }else if (io->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "min %f",
                                io->constraint->min_double.min);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (io->constraint->type == IGS_CONSTRAINT_MAX){
                if (io->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "max %d",
                                io->constraint->max_int.max);
                    igs_json_add_string(json, constraint_expression);
                }else if (io->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "max %f",
                                io->constraint->max_double.max);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (io->constraint->type == IGS_CONSTRAINT_RANGE){
                if (io->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "[%d, %d]",
                                io->constraint->range_int.min,
                                io->constraint->range_int.max);
                    igs_json_add_string(json, constraint_expression);
                }else if (io->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "[%f, %f]",
                                io->constraint->range_double.min,
                                io->constraint->range_double.max);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (io->constraint->type == IGS_CONSTRAINT_REGEXP){
                igs_json_add_string (json, STR_CONSTRAINT);
                snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "~ %s",
                            io->constraint->regexp.string);
                igs_json_add_string(json, constraint_expression);
            }
        }
        if (io->description){
            igs_json_add_string (json, STR_DESCRIPTION);
            igs_json_add_string (json, io->description);
        }
        if (io->detailed_type){
            igs_json_add_string (json, STR_DETAILED_TYPE);
            igs_json_add_string (json, io->detailed_type);
        }
        if (io->specification){
            igs_json_add_string (json, STR_SPECIFICATION);
            igs_json_add_string (json, io->specification);
        }
        igs_json_close_map (json);
        io_name = zlist_next(def->inputs_names_ordered);
    }
    igs_json_close_array (json);

    igs_json_add_string (json, STR_OUTPUTS);
    igs_json_open_array (json);
    io_name = zlist_first(def->outputs_names_ordered);
    while (io_name) {
        igs_io_t* io = zhashx_lookup(def->outputs_table, io_name);
        assert(io);
        igs_json_open_map (json);
        if (io->name) {
            igs_json_add_string (json, STR_NAME);
            igs_json_add_string (json, io->name);
        }
        igs_json_add_string (json, STR_TYPE);
        igs_json_add_string (json, s_value_type_to_string (io->value_type));

        char constraint_expression[IGS_MAX_CONSTRAINT_LENGTH] = "";
        if (io->constraint){
            if (io->constraint->type == IGS_CONSTRAINT_MIN){
                if (io->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "min %d",
                                io->constraint->min_int.min);
                    igs_json_add_string(json, constraint_expression);
                }else if (io->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "min %f",
                                io->constraint->min_double.min);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (io->constraint->type == IGS_CONSTRAINT_MAX){
                if (io->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "max %d",
                                io->constraint->max_int.max);
                    igs_json_add_string(json, constraint_expression);
                }else if (io->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "max %f",
                                io->constraint->max_double.max);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (io->constraint->type == IGS_CONSTRAINT_RANGE){
                if (io->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "[%d, %d]",
                                io->constraint->range_int.min,
                                io->constraint->range_int.max);
                    igs_json_add_string(json, constraint_expression);
                }else if (io->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "[%f, %f]",
                                io->constraint->range_double.min,
                                io->constraint->range_double.max);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (io->constraint->type == IGS_CONSTRAINT_REGEXP){
                igs_json_add_string (json, STR_CONSTRAINT);
                snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "~ %s",
                            io->constraint->regexp.string);
                igs_json_add_string(json, constraint_expression);
            }
        }
        if (io->description){
            igs_json_add_string (json, STR_DESCRIPTION);
            igs_json_add_string (json, io->description);
        }
        if (io->detailed_type){
            igs_json_add_string (json, STR_DETAILED_TYPE);
            igs_json_add_string (json, io->detailed_type);
        }
        if (io->specification){
            igs_json_add_string (json, STR_SPECIFICATION);
            igs_json_add_string (json, io->specification);
        }
        igs_json_close_map (json);
        io_name = zlist_next(def->outputs_names_ordered);
    }
    igs_json_close_array (json);

    igs_json_add_string (json, STR_ATTRIBUTES_DEPRECATED);
    igs_json_open_array (json);

    io_name = zlist_first(def->attributes_names_ordered);
    while (io_name) {
        igs_io_t* io = zhashx_lookup(def->attributes_table, io_name);
        assert(io);
        igs_json_open_map (json);
        if (io->name) {
            igs_json_add_string (json, STR_NAME);
            igs_json_add_string (json, io->name);
        }
        igs_json_add_string (json, STR_TYPE);
        igs_json_add_string (json, s_value_type_to_string (io->value_type));
        char constraint_expression[IGS_MAX_CONSTRAINT_LENGTH] = "";
        if (io->constraint){
            if (io->constraint->type == IGS_CONSTRAINT_MIN){
                if (io->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "min %d",
                                io->constraint->min_int.min);
                    igs_json_add_string(json, constraint_expression);
                }else if (io->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "min %f",
                                io->constraint->min_double.min);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (io->constraint->type == IGS_CONSTRAINT_MAX){
                if (io->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "max %d",
                                io->constraint->max_int.max);
                    igs_json_add_string(json, constraint_expression);
                }else if (io->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "max %f",
                                io->constraint->max_double.max);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (io->constraint->type == IGS_CONSTRAINT_RANGE){
                if (io->value_type == IGS_INTEGER_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "[%d, %d]",
                                io->constraint->range_int.min,
                                io->constraint->range_int.max);
                    igs_json_add_string(json, constraint_expression);
                }else if (io->value_type == IGS_DOUBLE_T){
                    igs_json_add_string (json, STR_CONSTRAINT);
                    snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "[%f, %f]",
                                io->constraint->range_double.min,
                                io->constraint->range_double.max);
                    igs_json_add_string(json, constraint_expression);
                }
            }else if (io->constraint->type == IGS_CONSTRAINT_REGEXP){
                igs_json_add_string (json, STR_CONSTRAINT);
                snprintf(constraint_expression, IGS_MAX_CONSTRAINT_LENGTH, "~ %s",
                            io->constraint->regexp.string);
                igs_json_add_string(json, constraint_expression);
            }
        }
        if (io->description){
            igs_json_add_string (json, STR_DESCRIPTION);
            igs_json_add_string (json, io->description);
        }
        if (io->detailed_type){
            igs_json_add_string (json, STR_DETAILED_TYPE);
            igs_json_add_string (json, io->detailed_type);
        }
        if (io->specification){
            igs_json_add_string (json, STR_SPECIFICATION);
            igs_json_add_string (json, io->specification);
        }
        igs_json_close_map (json);
        io_name = zlist_next(def->attributes_names_ordered);
    }
    igs_json_close_array (json);

    igs_json_add_string (json, STR_SERVICES);
    igs_json_open_array (json);
    const char* service_name = zlist_first(def->services_names_ordered);
    while (service_name) {
        igs_service_t* service = zhashx_lookup(def->services_table, service_name);
        assert(service);
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
                igs_service_arg_t *argument = service->arguments;
                while (argument) {
                    if (argument->name) {
                        igs_json_open_map (json);
                        igs_json_add_string (json, STR_NAME);
                        igs_json_add_string (json, argument->name);
                        igs_json_add_string (json, STR_TYPE);
                        igs_json_add_string (
                                                json, s_value_type_to_string (argument->type));
                        igs_json_close_map (json);
                    }
                    argument = argument->next;
                }
                igs_json_close_array (json);
            }

            if (service->replies) {
                igs_json_add_string (json, STR_REPLIES);
                igs_json_open_array (json);
                const char* r_name = zlist_first(service->replies_names_ordered);
                while (r_name) {
                    igs_service_t* r = zhashx_lookup(service->replies, r_name);
                    if (r && r->name) {
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
                            igs_service_arg_t *argument = r->arguments;
                            while (argument) {
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
                                argument = argument->next;
                            }
                            igs_json_close_array (json);
                        }
                        igs_json_close_map (json);
                    }
                    r_name = zlist_next(service->replies_names_ordered);
                }
                igs_json_close_array (json);
            }
            igs_json_close_map (json);
        }
        service_name = zlist_next(def->services_names_ordered);
    }
    igs_json_close_array (json);

    igs_json_close_map (json);
    igs_json_close_map (json);
    char *res = igs_json_dump (json);
    igs_json_destroy (&json);
    return res;
}
char *parser_export_definition_legacy_v3 (igs_definition_t *def)
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
    const char* io_name = zlist_first(def->inputs_names_ordered);
    while (io_name) {
        igs_io_t* io = zhashx_lookup(def->inputs_table, io_name);
        assert(io);
        igs_json_open_map (json);
        if (io->name) {
            igs_json_add_string (json, STR_NAME);
            igs_json_add_string (json, io->name);
        }
        igs_json_add_string (json, STR_TYPE);
        igs_json_add_string (json, s_value_type_to_string (io->value_type));
        // NB: inputs do not have intial values
        igs_json_close_map (json);
        io_name = zlist_next(def->inputs_names_ordered);
    }
    igs_json_close_array (json);

    igs_json_add_string (json, STR_OUTPUTS);
    igs_json_open_array (json);
    io_name = zlist_first(def->outputs_names_ordered);
    while (io_name) {
        igs_io_t* io = zhashx_lookup(def->outputs_table, io_name);
        assert(io);
        igs_json_open_map (json);
        if (io->name) {
            igs_json_add_string (json, STR_NAME);
            igs_json_add_string (json, io->name);
        }
        igs_json_add_string (json, STR_TYPE);
        igs_json_add_string (json, s_value_type_to_string (io->value_type));
        igs_json_close_map (json);
        io_name = zlist_next(def->outputs_names_ordered);
    }
    igs_json_close_array (json);

    igs_json_add_string (json, STR_ATTRIBUTES_DEPRECATED);
    igs_json_open_array (json);
    io_name = zlist_first(def->attributes_names_ordered);
    while (io_name) {
        igs_io_t* io = zhashx_lookup(def->attributes_table, io_name);
        assert(io);
        igs_json_open_map (json);
        if (io->name) {
            igs_json_add_string (json, STR_NAME);
            igs_json_add_string (json, io->name);
        }
        igs_json_add_string (json, STR_TYPE);
        igs_json_add_string (json, s_value_type_to_string (io->value_type));
        igs_json_close_map (json);
        io_name = zlist_next(def->attributes_names_ordered);
    }
    igs_json_close_array (json);

    igs_json_add_string (json, STR_SERVICES_DEPRECATED);
    igs_json_open_array (json);
    const char* service_name = zlist_first(def->services_names_ordered);
    while (service_name) {
        igs_service_t* service = zhashx_lookup(def->services_table, service_name);
        assert(service);
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
                igs_service_arg_t *argument = service->arguments;
                while (argument) {
                    if (argument->name) {
                        igs_json_open_map (json);
                        igs_json_add_string (json, STR_NAME);
                        igs_json_add_string (json, argument->name);
                        igs_json_add_string (json, STR_TYPE);
                        igs_json_add_string (
                                                json, s_value_type_to_string (argument->type));
                        igs_json_close_map (json);
                    }
                    argument = argument->next;
                }
                igs_json_close_array (json);
            }

            if (service->replies) {
                igs_json_add_string (json, STR_REPLIES);
                igs_json_open_array (json);
                const char* r_name = zlist_first(service->replies_names_ordered);
                while (r_name) {
                    igs_service_t* r = zhashx_lookup(service->replies, r_name);
                    assert(r);
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
                            igs_service_arg_t *argument = r->arguments;
                            while (argument) {
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
                                argument = argument->next;
                            }
                            igs_json_close_array (json);
                        }
                        igs_json_close_map (json);
                    }
                    r_name = zlist_next(service->replies_names_ordered);
                }
                igs_json_close_array (json);
            }
        }
        igs_json_close_map (json);
        service_name = zlist_next(def->services_names_ordered);
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
    igs_map_t *elmt = zlist_first(mapping->map_elements);
    while (elmt) {
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
        elmt = zlist_next(mapping->map_elements);
    }
    igs_json_close_array (json);

    igs_json_add_string (json, STR_SPLITS);
    igs_json_open_array (json);
    igs_split_t *elmt_split = zlist_first(mapping->split_elements);
    while (elmt_split) {
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
        elmt_split = zlist_next(mapping->split_elements);
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
    igs_map_t *elmt = zlist_first(mapping->map_elements);
    while (elmt) {
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
        elmt = zlist_next(mapping->map_elements);
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
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (json_str);
    // Try to load definition
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_definition_t *tmp = parser_load_definition (json_str);
    if (tmp == NULL) {
        igsagent_error (agent, "json string caused an error and was ignored");
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    definition_free_definition (&agent->definition);
    agent->definition = tmp;
    definition_update_json (agent->definition);
    agent->network_need_to_send_definition_update = true;
    model_read_write_unlock(__FUNCTION__, __LINE__);
    igsagent_set_name (agent, tmp->name);
    return IGS_SUCCESS;
}

igs_result_t igsagent_definition_load_file (igsagent_t *agent,
                                            const char *file_path)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (file_path);
    // Try to load definition
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_definition_t *tmp = parser_load_definition_from_path (file_path);
    if (tmp == NULL) {
        igsagent_error (agent, "json file content at '%s' caused an error and was ignored", file_path);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    definition_free_definition (&agent->definition);
    agent->definition_path = s_strndup (file_path, IGS_MAX_PATH_LENGTH);
    agent->definition = tmp;
    definition_update_json (agent->definition);
    agent->network_need_to_send_definition_update = true;
    model_read_write_unlock(__FUNCTION__, __LINE__);
    igsagent_set_name (agent, tmp->name);
    return IGS_SUCCESS;
}
