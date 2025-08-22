/*  =========================================================================
 model - read/write/observe inputs, outputs and attributes

 Copyright (c) the Contributors as noted in the AUTHORS file.
 This file is part of Ingescape, see https://github.com/zeromq/ingescape.

 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
 =========================================================================
 */

#include "ingescape_classes.h"
#include "ingescape_private.h"
#include <czmq.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef W_OK
#define W_OK 02
#endif

#define NUMBER_TO_STRING_MAX_LENGTH 255
#define BOOL_TO_STRING_MAX_LENGTH 6
#define MAX_IOP_VALUE_LOG_BUFFER_LENGTH 256

////////////////////////////////////////////////////////////////////////
#pragma mark INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////
igs_io_t *s_model_find_input_by_name (igsagent_t *agent, const char *name)
{
    assert(agent);
    assert(name);
    igs_io_t *found = NULL;
    if (name && agent->definition)
        found = zhashx_lookup(agent->definition->inputs_table, name);
    return found;
}

igs_io_t *s_model_find_output_by_name (igsagent_t *agent, const char *name)
{
    assert(agent);
    assert(name);
    igs_io_t *found = NULL;
    if (name && agent->definition)
        found = zhashx_lookup(agent->definition->outputs_table, name);
    return found;
}

igs_io_t *s_model_find_attribute_by_name (igsagent_t *agent, const char *name)
{
    assert(agent);
    assert(name);
    igs_io_t *found = NULL;
    if (name && agent->definition)
        found = zhashx_lookup(agent->definition->attributes_table, name);
    return found;
}

void *
s_model_get_value_for (igsagent_t *agent, const char *name, igs_io_type_t type)
{
    igs_io_t *io = model_find_io_by_name (agent, name, type);
    if (io == NULL) {
        igsagent_error (agent, "%s not found", name);
        return NULL;
    }
    switch (io->value_type) {
        case IGS_INTEGER_T:
            return &io->value.i;
        case IGS_DOUBLE_T:
            return &io->value.d;
        case IGS_BOOL_T:
            return &io->value.b;
        case IGS_STRING_T:
            return io->value.s;
        case IGS_IMPULSION_T:
            return NULL;
        case IGS_DATA_T:
            return io->value.data;
        default:
            igsagent_error (agent, "Unknown value type for %s", name);
            break;
    }
    return NULL;
}

igs_result_t s_read_io (igsagent_t *agent,
                        const char *name,
                        igs_io_type_t type,
                        void **value,
                        size_t *size)
{
    igs_io_t *io = model_find_io_by_name (agent, name, type);
    if (io == NULL) {
        igsagent_error (agent, "%s not found", name);
        return IGS_FAILURE;
    }
    if (io->value_type == IGS_IMPULSION_T
        || (io->value_type == IGS_STRING_T && io->value.s == NULL)
        || (io->value_type == IGS_DATA_T && io->value.data == NULL)) {
        *value = NULL;
        *size = 0;
    }
    else {
        *value = (void *) zmalloc (io->value_size);
        memcpy (*value, s_model_get_value_for (agent, name, type),
                io->value_size);
        *size = io->value_size;
    }
    return IGS_SUCCESS;
}

bool s_model_read_io_as_bool (igsagent_t *agent,
                              const char *name,
                              igs_io_type_t type)
{
    bool res = false;
    //NB: we lock this function because it is called directly by wrapping functions
    igs_io_t *io = model_find_io_by_name (agent, name, type);
    if (io == NULL) {
        igsagent_error (agent, "%s not found", name);
        return false;
    }
    switch (io->value_type) {
        case IGS_BOOL_T:
            res = io->value.b;
            break;
        case IGS_INTEGER_T:
            igsagent_warn (agent, "Implicit conversion from int to bool for %s", name);
            res = (io->value.i == 0) ? false : true;
            break;
        case IGS_DOUBLE_T:
            igsagent_warn (agent, "Implicit conversion from double to bool for %s", name);
            res = (io->value.d >= 0 && io->value.d <= 0) ? false : true;
            break;
        case IGS_STRING_T:
            if (!io->value.s){
                igsagent_warn (agent,"Implicit conversion from NULL string to bool for %s (false was returned)", name);
                res = false;
            }else if (streq (io->value.s, "true")) {
                igsagent_warn (agent, "Implicit conversion from string to bool for %s", name);
                res = true;
            }else if (streq (io->value.s, "false")) {
                igsagent_warn (agent, "Implicit conversion from string to bool for %s", name);
                res = false;
            }else {
                igsagent_warn (agent,"Implicit conversion from string to bool for %s (string value is %s and false was returned)", name, io->value.s);
                res = false;
            }
            break;
        default:
            igsagent_error (agent, "No implicit conversion possible for %s (false was returned)", name);
            res = false;
    }
    return res;
}

int s_model_read_io_as_int (igsagent_t *agent,
                            const char *name,
                            igs_io_type_t type)
{
    int res = 0;
    igs_io_t *io = model_find_io_by_name (agent, name, type);
    if (io == NULL) {
        igsagent_error (agent, "%s not found", name);
        return 0;
    }
    switch (io->value_type) {
        case IGS_BOOL_T:
            igsagent_warn (agent, "Implicit conversion from bool to int for %s", name);
            res = (io->value.b) ? 1 : 0;
            break;
        case IGS_INTEGER_T:
            res = io->value.i;
            break;
        case IGS_DOUBLE_T:
            igsagent_warn (agent, "Implicit conversion from double to int for %s", name);
            if (io->value.d < 0)
                res = (int) (io->value.d - 0.5);
            else
                res = (int) (io->value.d + 0.5);
            break;
        case IGS_STRING_T:
            if (!io->value.s){
                igsagent_warn (agent,"Implicit conversion from NULL string to int for %s (false was returned)", name);
                res = false;
            }else
                igsagent_warn (agent, "Implicit conversion from string %s to int for %s", io->value.s, name);
            res = atoi (io->value.s);
            break;
        default:
            igsagent_error (agent, "No implicit conversion possible for %s (0 was returned)", name);
            res = 0;
    }
    return res;
}

double s_model_read_io_as_double (igsagent_t *agent,
                                  const char *name,
                                  igs_io_type_t type)
{
    double res = 0;
    igs_io_t *io = model_find_io_by_name (agent, name, type);
    if (io == NULL) {
        igsagent_error (agent, "%s not found", name);
        return 0;
    }
    switch (io->value_type) {
        case IGS_BOOL_T:
            igsagent_warn (agent, "Implicit conversion from bool to double for %s", name);
            res = (io->value.b) ? 1 : 0;
            break;
        case IGS_INTEGER_T:
            igsagent_warn (agent, "Implicit conversion from int to double for %s", name);
            res = io->value.i;
            break;
        case IGS_DOUBLE_T:
            res = io->value.d;
            break;
        case IGS_STRING_T:
            if (!io->value.s){
                igsagent_warn (agent,"Implicit conversion from NULL string to double for %s (0 was returned)", name);
                res = 0;
            }else
                igsagent_warn (agent, "Implicit conversion from string %s to double for %s", io->value.s, name);
            res = atof (io->value.s);
            break;
        default:
            igsagent_error (agent, "No implicit conversion possible for %s (0 was returned)", name);
            res = 0;
    }
    return res;
}

char *s_model_int_to_string (const int value)
{
    int length = snprintf (NULL, 0, "%d", value);
    if (length == 0)
        return NULL;
    char *str = (char *) zmalloc (length + 1);
    snprintf (str, length + 1, "%d", value);
    return str;
}

char *s_model_double_to_string (const double value)
{
    int length = snprintf (NULL, 0, "%lf", value);
    if (length == 0)
        return NULL;
    char *str = (char *) zmalloc (length + 1);
    snprintf (str, length + 1, "%lf", value);
    return str;
}

char *s_model_read_io_as_string (igsagent_t *agent,
                                 const char *name,
                                 igs_io_type_t type)
{
    char *res = NULL;
    igs_io_t *io = model_find_io_by_name (agent, name, type);
    if (io == NULL) {
        igsagent_error (agent, "%s not found", name);
        return NULL;
    }
    switch (io->value_type) {
        case IGS_STRING_T:
            if (io->value.s)
                res = strdup (io->value.s);
            break;
        case IGS_BOOL_T:
            igsagent_warn (agent, "Implicit conversion from bool to string for %s", name);
            res = io->value.b ? strdup ("true") : strdup ("false");
            break;
        case IGS_INTEGER_T:
            igsagent_warn (agent, "Implicit conversion from int to string for %s", name);
            res = s_model_int_to_string (io->value.i);
            break;
        case IGS_DOUBLE_T:
            igsagent_warn (agent, "Implicit conversion from double to string for %s", name);
            res = s_model_double_to_string (io->value.d);
            break;
        default:
            igsagent_error (agent, "No implicit conversion possible for %s (NULL was returned)", name);
    }
    return res;
}

igs_result_t s_model_read_io_as_data (igsagent_t *agent,
                                      const char *name,
                                      igs_io_type_t type,
                                      void **value,
                                      size_t *size)
{
    assert (agent);
    assert (value);
    assert (size);
    igs_io_t *io = model_find_io_by_name (agent, name, type);
    if (io == NULL) {
        igsagent_error (agent, "%s not found", name);
        *value = NULL;
        *size = 0;
        return IGS_FAILURE;
    }
    if (io->value_type == IGS_IMPULSION_T || io->value_type == IGS_UNKNOWN_T
        || (io->value_type == IGS_DATA_T && io->value.data == NULL)) {
        *value = NULL;
        *size = 0;
    }else{
        *size = io->value_size;
        *value = (void *) zmalloc (io->value_size);
        memcpy (*value, s_model_get_value_for (agent, name, type), *size);
    }
    return IGS_SUCCESS;
}

igs_constraint_t* model_parse_constraint(igs_io_value_type_t type,
                                           const char *expression,char **error){
    assert(expression);
    assert(error);
    const char *min_exp = "min ([+-]?(\\d*[.])?\\d+)";
    const char *max_exp = "max ([+-]?(\\d*[.])?\\d+)";
    const char *range_exp = "\\[([+-]?(\\d*[.])?\\d+)\\s*,\\s*([+-]?(\\d*[.])?\\d+)\\]";
    const char *regexp = "~ ([^\n]+)";
    const char *exp1 = NULL;
    const char *exp2 = NULL;
    igs_constraint_t *c = NULL;
    zrex_t *rex = zrex_new(min_exp);
    if (zrex_matches(rex, expression)){
        zrex_fetch(rex, &exp1, NULL);
        if (type == IGS_INTEGER_T){
            c = (igs_constraint_t *)calloc(1, sizeof(igs_constraint_t));
            c->type = IGS_CONSTRAINT_MIN;
            c->min_int.min = atoi(exp1);
        }else if (type == IGS_DOUBLE_T){
            c = (igs_constraint_t *)calloc(1, sizeof(igs_constraint_t));
            c->type = IGS_CONSTRAINT_MIN;
            c->min_double.min = atof(exp1);
        }else
            *error = strdup("min constraint is allowed on integer and double IOPs only");
    }else if (zrex_eq(rex, expression, max_exp)){
        zrex_fetch(rex, &exp1, NULL);
        if (type == IGS_INTEGER_T){
            c = (igs_constraint_t *)calloc(1, sizeof(igs_constraint_t));
            c->type = IGS_CONSTRAINT_MAX;
            c->max_int.max = atoi(exp1);
        }else if (type == IGS_DOUBLE_T){
            c = (igs_constraint_t *)calloc(1, sizeof(igs_constraint_t));
            c->type = IGS_CONSTRAINT_MAX;
            c->max_double.max = atof(exp1);
        }else
            *error = strdup("max constraint is allowed on integer and double IOPs only");
    }else if (zrex_eq(rex, expression, range_exp)){
        //FIXME: check that min <= max
        exp1 = zrex_hit(rex, 1);
        exp2 = zrex_hit(rex, 3);
        if (type == IGS_INTEGER_T){
            c = (igs_constraint_t *)calloc(1, sizeof(igs_constraint_t));
            c->type = IGS_CONSTRAINT_RANGE;
            c->range_int.min = atoi(exp1);
            c->range_int.max = atoi(exp2);
            if (c->range_int.max < c->range_int.min){
                char error_msg[IGS_MAX_LOG_LENGTH] = "";
                snprintf(error_msg, IGS_MAX_LOG_LENGTH, "range min is superior to range max in %s", expression);
                *error = strdup(error_msg);
                definition_free_constraint(&c);
            }
        }else if (type == IGS_DOUBLE_T){
            c = (igs_constraint_t *)calloc(1, sizeof(igs_constraint_t));
            c->type = IGS_CONSTRAINT_RANGE;
            c->range_double.min = atof(exp1);
            c->range_double.max = atof(exp2);
        }else
            *error = strdup("range constraint is allowed on integer and double IOPs only");
    }else if (zrex_eq(rex, expression, regexp)){
        exp1 = zrex_hit(rex, 1);
        if (type == IGS_STRING_T){
            c = (igs_constraint_t *)calloc(1, sizeof(igs_constraint_t));
            c->type = IGS_CONSTRAINT_REGEXP;
            c->regexp.rex = zrex_new(exp1);
            if(!zrex_valid(c->regexp.rex)){
                char error_msg[IGS_MAX_LOG_LENGTH] = "";
                snprintf(error_msg, IGS_MAX_LOG_LENGTH, "regular expression '%s' is invalid", exp1);
                *error = strdup(error_msg);
                zrex_destroy(&c->regexp.rex);
                definition_free_constraint(&c);
            }else
                c->regexp.string = strdup(exp1);
        }else
            *error = strdup("regexp constraint is allowed on string IOPs only");
    }else{
        char error_msg[IGS_MAX_LOG_LENGTH] = "";
        snprintf(error_msg, IGS_MAX_LOG_LENGTH, "expression '%s' did not match the allowed syntax", expression);
        *error = strdup(error_msg);
    }
    zrex_destroy(&rex);
    return c;
}

igs_result_t s_model_add_constraint (igsagent_t *self, igs_io_type_t type,
                                     const char *name,
                                     const char *constraint)
{
    assert(self);
    assert(name);
    assert(constraint);
    igs_io_t *io = NULL;
    if (type == IGS_INPUT_T) {
        io = zhashx_lookup(self->definition->inputs_table, name);
        if (!io) {
            igsagent_error (self, "Input %s cannot be found", name);
            return IGS_FAILURE;
        }
    }
    else if (type == IGS_OUTPUT_T) {
        io = zhashx_lookup (self->definition->outputs_table, name);
        if (!io) {
            igsagent_error (self, "Output %s cannot be found", name);
            return IGS_FAILURE;
        }
    }
    else if (type == IGS_ATTRIBUTE_T) {
        io = zhashx_lookup (self->definition->attributes_table, name);
        if (!io) {
            igsagent_error (self, "Attribute %s cannot be found", name);
            return IGS_FAILURE;
        }
    }
    else {
        igsagent_error (self, "Unknown IOP type %d", type);
        return IGS_FAILURE;
    }
    if (io->constraint){
        igsagent_warn (self, "%s already has a constraint that will be removed", name);
        definition_free_constraint(&io->constraint);
    }
    char *error = NULL;
    io->constraint = model_parse_constraint(io->value_type, constraint, &error);
    if (!io->constraint || error){
        if (error){
            igsagent_error (self, "%s", error);
            free(error);
        }
        return IGS_FAILURE;
    }
    definition_update_json(self->definition);
    self->network_need_to_send_definition_update = true;
    
    return IGS_SUCCESS;
}

igs_result_t s_model_set_description(igsagent_t *self, igs_io_type_t type,
                                     const char *name,
                                     const char *description)
{
    assert(self);
    assert(name);
    assert(description);
    igs_io_t *io = NULL;
    if (type == IGS_INPUT_T) {
        io = zhashx_lookup (self->definition->inputs_table, name);
        if (!io) {
            igsagent_error (self, "Input %s cannot be found", name);
            return IGS_FAILURE;
        }
    }
    else
        if (type == IGS_OUTPUT_T) {
            io = zhashx_lookup (self->definition->outputs_table, name);
            if (!io) {
                igsagent_error (self, "Output %s cannot be found", name);
                return IGS_FAILURE;
            }
        }
        else
            if (type == IGS_ATTRIBUTE_T) {
                io = zhashx_lookup (self->definition->attributes_table, name);
                if (!io) {
                    igsagent_error (self, "Parameter %s cannot be found", name);
                    return IGS_FAILURE;
                }
            }
            else {
                igsagent_error (self, "Unknown IOP type %d", type);
                return IGS_FAILURE;
            }
    if (io->description)
        free(io->description);
    io->description = s_strndup(description, IGS_MAX_DESCRIPTION_LENGTH);
    definition_update_json(self->definition);
    self->network_need_to_send_definition_update = true;
    return IGS_SUCCESS;
}

char * s_model_get_description(igsagent_t *self, igs_io_type_t type,
                                     const char *name)
{
    assert(self);
    assert(name);
    igs_io_t *io = NULL;
    if (type == IGS_INPUT_T) {
        io = zhashx_lookup (self->definition->inputs_table, name);
        if (!io) {
            igsagent_error (self, "Input %s cannot be found", name);
            return NULL;
        }
    }
    else
        if (type == IGS_OUTPUT_T) {
            io = zhashx_lookup (self->definition->outputs_table, name);
            if (!io) {
                igsagent_error (self, "Output %s cannot be found", name);
                return NULL;
            }
        }
        else
            if (type == IGS_ATTRIBUTE_T) {
                io = zhashx_lookup (self->definition->attributes_table, name);
                if (!io) {
                    igsagent_error (self, "Parameter %s cannot be found", name);
                    return NULL;
                }
            }
            else {
                igsagent_error (self, "Unknown IOP type %d", type);
                return NULL;
            }
    return (io && io->description) ? strdup(io->description) : NULL;
}

igs_result_t s_model_set_detailed_type(igsagent_t *self, igs_io_type_t type,
                                       const char *name, const char *type_name,
                                       const char *specification)
{
    assert(self);
    assert(name);
    assert(type_name);
    assert(model_check_string(type_name, IGS_MAX_DETAILED_TYPE_LENGTH));
    assert(specification);
    igs_io_t *io = NULL;
    if (type == IGS_INPUT_T) {
        io = zhashx_lookup (self->definition->inputs_table, name);
        if (!io) {
            igsagent_error (self, "Input %s cannot be found", name);
            return IGS_FAILURE;
        }
    }
    else
        if (type == IGS_OUTPUT_T) {
            io = zhashx_lookup (self->definition->outputs_table, name);
            if (!io) {
                igsagent_error (self, "Output %s cannot be found", name);
                return IGS_FAILURE;
            }
        }
        else
            if (type == IGS_ATTRIBUTE_T) {
                io = zhashx_lookup (self->definition->attributes_table, name);
                if (!io) {
                    igsagent_error (self, "Parameter %s cannot be found", name);
                    return IGS_FAILURE;
                }
            }
            else {
                igsagent_error (self, "Unknown IOP type %d", type);
                return IGS_FAILURE;
            }
    if (io->detailed_type)
        free(io->detailed_type);
    io->detailed_type = s_strndup(type_name, IGS_MAX_DETAILED_TYPE_LENGTH);
    if (io->specification)
        free(io->specification);
    io->specification = s_strndup(specification, IGS_MAX_SPECIFICATION_LENGTH);
    definition_update_json(self->definition);
    self->network_need_to_send_definition_update = true;
    return IGS_SUCCESS;
}

igs_io_value_type_t s_model_get_type_for_io (igsagent_t *agent,
                                             const char *name,
                                             igs_io_type_t type)
{
    assert (agent && agent->definition);
    assert (name && strlen (name) > 0);
    igs_io_t *io = NULL;
    if (type == IGS_INPUT_T) {
        io = zhashx_lookup (agent->definition->inputs_table, name);
        if (io == NULL) {
            igsagent_error (agent, "Input %s cannot be found", name);
            return IGS_UNKNOWN_T;
        }
    }
    else
        if (type == IGS_OUTPUT_T) {
            io = zhashx_lookup (agent->definition->outputs_table, name);
            if (io == NULL) {
                igsagent_error (agent, "Output %s cannot be found", name);
                return IGS_UNKNOWN_T;
            }
        }
        else
            if (type == IGS_ATTRIBUTE_T) {
                io = zhashx_lookup (agent->definition->attributes_table, name);
                if (io == NULL) {
                    igsagent_error (agent, "Parameter %s cannot be found", name);
                    return IGS_UNKNOWN_T;
                }
            }
            else {
                igsagent_error (agent, "Unknown IOP type %d", type);
                return IGS_UNKNOWN_T;
            }
    igs_io_value_type_t t = io->value_type;
    return t;
}

bool s_model_check_io_existence (igsagent_t *agent,
                                 const char *name,
                                 zhashx_t *hash)
{

    if (agent->definition == NULL) {
        igsagent_error (agent, "Definition is NULL");
        return false;
    }
    igs_io_t *io = zhashx_lookup (hash, name);
    return (io?true:false);
}

char **s_model_get_io_list (igsagent_t *agent,
                            size_t *nb_of_elements,
                            igs_io_type_t type)
{
    assert(agent);
    if (agent->definition == NULL) {
        igsagent_warn (agent, "Definition is NULL");
        if (nb_of_elements)
            *nb_of_elements = 0;
        return NULL;
    }
    zlist_t *list = NULL;
    switch (type) {
        case IGS_INPUT_T:
            list = agent->definition->inputs_names_ordered;
            break;
        case IGS_OUTPUT_T:
            list = agent->definition->outputs_names_ordered;
            break;
        case IGS_ATTRIBUTE_T:
            list = agent->definition->attributes_names_ordered;
            break;
        default:
            break;
    }
    if (list == NULL) {
        if (nb_of_elements)
            *nb_of_elements = 0;
        return NULL;
    }

    size_t N = zlist_size (list);
    if (nb_of_elements)
        *nb_of_elements = N;
    if (N < 1)
        return NULL;

    char **names_list = (char **) malloc (N * sizeof (char *));
    const char *io_name = (const char*) zlist_first (list);
    int index = 0;
    while (io_name) {
        names_list[index] = strdup (io_name);
        index++;
        io_name = zlist_next(list);
    }
    return names_list;
}

static void s_model_observe (igsagent_t *agent,
                             const char *name,
                             igs_io_type_t value_type,
                             igsagent_io_fn cb,
                             void *my_data)
{
    igs_io_t *io = model_find_io_by_name (agent, name, value_type);
    // Check if the input has been returned.
    if (io == NULL) {
        const char *t = NULL;
        switch (value_type) {
            case IGS_INPUT_T:
                t = "input";
                break;
            case IGS_OUTPUT_T:
                t = "output";
                break;
            case IGS_ATTRIBUTE_T:
                t = "attribute";
                break;
            default:
                break;
        }
        igsagent_error (agent, "Cannot find %s %s", t, name);
        return;
    }
    igs_observe_io_wrapper_t *new_callback = (igs_observe_io_wrapper_t *) zmalloc (sizeof (igs_observe_io_wrapper_t));
    new_callback->callback_ptr = cb;
    new_callback->data = my_data;
    zlist_append (io->io_callbacks, new_callback);
}

void s_model_clear_io (igsagent_t *agent, const char *name, igs_io_type_t type)
{
    assert (agent);
    assert (name);
    igs_io_t *io = model_find_io_by_name (agent, name, type);
    if (!io)
        return;
    switch (io->value_type) {
        case IGS_IMPULSION_T:
            break;
        case IGS_DATA_T:
            if (io->value.data) {
                free (io->value.data);
                io->value.data = NULL;
                io->value_size = 0;
            }
            break;
        case IGS_STRING_T:
            if (io->value.s) {
                free (io->value.s);
                io->value.s = NULL;
                io->value_size = 0;
            }
            break;
        case IGS_DOUBLE_T:
            io->value.d = 0;
            break;
        case IGS_INTEGER_T:
            io->value.i = 0;
            break;
        case IGS_BOOL_T:
            io->value.b = false;
            break;
        case IGS_UNKNOWN_T:
            igsagent_error (agent, "%s cannot be reset (unknown type)", name);
            break;
        default:
            break;
    }
}

//FIXME: apparently unused
char *s_model_get_io_value_as_string (igs_io_t *io)
{
    assert (io);
    char *str_value = NULL;
    if (io) {
        switch (io->value_type) {
            case IGS_INTEGER_T:
                str_value = (char *) zmalloc (NUMBER_TO_STRING_MAX_LENGTH + 1);
                snprintf (str_value, NUMBER_TO_STRING_MAX_LENGTH, "%i",
                          io->value.i);
                break;
            case IGS_DOUBLE_T:
                str_value = (char *) zmalloc (NUMBER_TO_STRING_MAX_LENGTH + 1);
                snprintf (str_value, NUMBER_TO_STRING_MAX_LENGTH, "%lf",
                          io->value.d);
                break;
            case IGS_BOOL_T:
                str_value = (char *) zmalloc (BOOL_TO_STRING_MAX_LENGTH);
                if (io->value.b == true)
                    snprintf (str_value, BOOL_TO_STRING_MAX_LENGTH, "%s",
                              "true");
                else
                    snprintf (str_value, BOOL_TO_STRING_MAX_LENGTH, "%s",
                              "false");
                break;
            case IGS_STRING_T:
                if (io->value.s){
                    str_value = (char *) zmalloc (strlen (io->value.s) + 1);
                    snprintf (str_value, strlen (io->value.s) + 1, "%s", io->value.s);
                }
                break;
            case IGS_IMPULSION_T:
                break;
            case IGS_DATA_T:
                str_value = (char *) zmalloc (io->value_size + 1);
                snprintf (str_value, io->value_size + 1, "%s",
                          (char *) io->value.data);
                break;
            default:
                break;
        }
    }
    return str_value;
}


////////////////////////////////////////////////////////////////////////
#pragma mark PRIVATE API
////////////////////////////////////////////////////////////////////////
uint8_t *model_string_to_bytes (char *string)
{
    assert (string);
    size_t slength = strlen (string);
    if (slength < 2 || (slength % 2) != 0) // must be even with at least two chars
        return NULL;
    size_t dlength = slength / 2;
    uint8_t *data = (uint8_t *) zmalloc (dlength);
    size_t index = 0;
    while (index < slength) {
        char c = string[index];
        int value = 0;
        if (c >= '0' && c <= '9')
            value = (c - '0');
        else if (c >= 'A' && c <= 'F')
            value = (10 + (c - 'A'));
        else if (c >= 'a' && c <= 'f')
            value = (10 + (c - 'a'));
        else {
            free (data);
            return NULL;
        }
        data[(index / 2)] += value << (((index + 1) % 2) * 4);
        index++;
    }
    return data;
}

size_t model_clean_string(char *string, int64_t max){
    assert(string);
    if (max <= 0)
        max = INT64_MAX;
    char *char_index = string;
    size_t offset = 0;
    int64_t write_index = 0;
    while (1) {
        while (*(char_index+offset) == '\t'
            || *(char_index+offset) == '\v'
            || *(char_index+offset) == '\r'
            || *(char_index+offset) == '\n'
            || *(char_index+offset) == '\f'){
            offset++;
        }
        if (offset)
            string[write_index] = *(char_index + offset);
        if (string[write_index] == '\0')
            break;
        write_index++;
        char_index++;
        if (write_index == max){
            string[write_index] = '\0';
            offset++; //to signal a change
            break;
        }
    }
    return offset;
}

bool model_check_string(const char *string, int64_t max){
    assert(string);
    if (max <= 0)
        max = INT64_MAX;
    int64_t offset = 0;
    while (1) {
        if (string[offset] == '\t'
            || string[offset] == '\v'
            || string[offset] == '\r'
            || string[offset] == '\n'
            || string[offset] == '\f'){
            igs_error("string '%s' contains invalid characters", string);
            return false;
        }
        if (string[offset] == '\0')
            break;
        if (offset >= max)
            return false;
        offset++;
    }
    return true;
}

igs_mutex_t s_model_read_write_mutex;
static bool s_model_read_write_mutex_initialized = false;
static int s_model_lock_counter = 0;
void model_read_write_lock (const char *function, int line)
{
    if (IGS_MODEL_READ_WRITE_MUTEX_DEBUG){
        printf("---model_read_write_lock from %s (line %d)\n", function, line);
        if (s_model_lock_counter++)
            printf("---model_read_write_lock ACTIVE\n");
    }
    if (!s_model_read_write_mutex_initialized) {
        IGS_MUTEX_INIT (s_model_read_write_mutex);
        s_model_read_write_mutex_initialized = true;
    }
    IGS_MUTEX_LOCK (s_model_read_write_mutex);
}

void model_read_write_unlock (const char *function, int line)
{
    if (IGS_MODEL_READ_WRITE_MUTEX_DEBUG){
        printf("-model_read_write_unlock from %s (line %d)\n", function, line);
        s_model_lock_counter--;
    }
    assert (s_model_read_write_mutex_initialized);
    IGS_MUTEX_UNLOCK (s_model_read_write_mutex);
}

igs_io_t *model_write (igsagent_t *agent, const char *name,
                       igs_io_type_t type, igs_io_value_type_t value_type,
                       void *value, size_t size)
{
    assert (agent);
    assert (name);
    igs_io_t *io = model_find_io_by_name (agent, name, type);
    if (!io) {
        igsagent_error (agent, "%s not found for writing", name);
        return NULL;
    }
    int ret = 1;
    char buf[NUMBER_TO_STRING_MAX_LENGTH + 1] = "";

    //apply constraint if any
    if (io->constraint && agent->enforce_constraints){
        if (io->value_type == IGS_INTEGER_T){
            int converted_value = 0;
            if (value && size > 0){
                switch (value_type) {
                    case IGS_STRING_T:
                        converted_value = atoi((char *)value);
                        break;
                    case IGS_DATA_T:{
                        if (size == sizeof(int)){
                            converted_value = *(int *)value;
                            break;
                        }else{
                            igsagent_error(agent, "constraint type error for %s: value is data with wrong size (%zu bytes)", io->name, size);
                            return NULL;
                        }
                    }
                    case IGS_DOUBLE_T:
                        converted_value = (int)(*(double*)value);
                        break;
                    default:
                        converted_value = *(int*)value;
                        break;
                }
            }

            switch (io->constraint->type) {
                case IGS_CONSTRAINT_MIN:
                    if (converted_value < io->constraint->min_int.min){
                        igsagent_error(agent, "constraint error for %s (too low)", io->name);
                        return NULL;
                    }
                    break;
                case IGS_CONSTRAINT_MAX:
                    if (converted_value > io->constraint->max_int.max){
                        igsagent_error(agent, "constraint error for %s (too high)", io->name);
                        return NULL;
                    }
                    break;
                case IGS_CONSTRAINT_RANGE:
                    if (converted_value > io->constraint->range_int.max){
                        igsagent_error(agent, "constraint error for %s (too high)", io->name);
                        return NULL;
                    }else if (converted_value < io->constraint->range_int.min){
                        igsagent_error(agent, "constraint error for %s (too low)", io->name);
                        return NULL;
                    }
                    break;

                default:
                    break;
            }
        }else if(io->value_type == IGS_DOUBLE_T){
            double converted_value = 0;
            if (value && size > 0){
                switch (value_type) {
                    case IGS_STRING_T:
                        converted_value = atof((char *)value);
                        break;
                    case IGS_DATA_T:{
                        if (size == sizeof(double)){
                            converted_value = *(double *)value;
                            break;
                        }else{
                            igsagent_error(agent, "constraint type error for %s: value is data with wrong size (%zu bytes)", io->name, size);
                            return NULL;
                        }
                    }
                    case IGS_INTEGER_T:
                    case IGS_BOOL_T:
                        converted_value = (double)(*(int*)value);
                        break;
                    default:
                        converted_value = *(double*)value;
                        break;
                }
            }

            switch (io->constraint->type) {
                case IGS_CONSTRAINT_MIN:
                    if (converted_value < io->constraint->min_double.min){
                        igsagent_error(agent, "constraint error for %s (too low)", io->name);
                        return NULL;
                    }
                    break;
                case IGS_CONSTRAINT_MAX:
                    if (converted_value > io->constraint->max_double.max){
                        igsagent_error(agent, "constraint error for %s (too high)", io->name);
                        return NULL;
                    }
                    break;
                case IGS_CONSTRAINT_RANGE:
                    if (converted_value > io->constraint->range_double.max){
                        igsagent_error(agent, "constraint error for %s (too high)", io->name);
                        return NULL;
                    }else if (converted_value < io->constraint->range_double.min){
                        igsagent_error(agent, "constraint error for %s (too low)", io->name);
                        return NULL;
                    }
                    break;

                default:
                    break;
            }
        }else if (io->value_type == IGS_STRING_T){
            char *converted_value = NULL;
            if (value && size > 0){
                switch (value_type) {
                    case IGS_STRING_T:
                        converted_value = (char *)value;
                        break;
                    case IGS_DATA_T:{
                        if (((char*)value)[size - 1] == '\0'){
                            //NULL terminated data can be interpreted as string
                            converted_value = (char*)value;
                        }else{
                            igsagent_error(agent, "constraint type error for %s (value is data and IOP is string)", io->name);
                            return NULL;
                        }
                    }
                    case IGS_INTEGER_T:
                    case IGS_BOOL_T:
                        snprintf (buf, NUMBER_TO_STRING_MAX_LENGTH + 1, "%d", *(int *) (value));
                        converted_value = buf;
                        break;
                    case IGS_DOUBLE_T:
                        snprintf (buf, NUMBER_TO_STRING_MAX_LENGTH + 1, "%f", *(double *) (value));
                        converted_value = buf;
                        break;
                    default:
                        converted_value = "";
                        break;
                }
            }
            if (!converted_value){
                igsagent_error(agent, "constraint error for %s (value is NULL)", io->name);
                return NULL;
            }
            if (!zrex_matches(io->constraint->regexp.rex, converted_value)){
                igsagent_error(agent, "constraint error for %s (not matching regexp)", io->name);
                return NULL;
            }
        }
    }

    
    if (!value || size == 0){
        switch (io->value_type) {
            case IGS_INTEGER_T:
                io->value_size = sizeof (int);
                io->value.i = 0;
                break;
            case IGS_DOUBLE_T:
                io->value_size = sizeof (double);
                io->value.d = 0;
                break;
            case IGS_BOOL_T:
                io->value_size = sizeof (bool);
                io->value.b = false;
                break;
            case IGS_STRING_T: {
                if (io->value.s)
                    free (io->value.s);
                io->value.s = NULL;
                io->value_size = 0;
            } break;
            case IGS_IMPULSION_T:
                io->value_size = 0;
                break;
            case IGS_DATA_T: {
                if (io->value.data)
                    free (io->value.data);
                io->value.data = NULL;
                io->value_size = 0;
            } break;
            default:
                igsagent_error(agent, "%s has an invalid value type %d",
                               name, io->value_type);
                ret = 0;
                break;
        }
    }else{
        switch (value_type) {
            case IGS_INTEGER_T: {
                switch (io->value_type) {
                    case IGS_INTEGER_T:
                        io->value_size = sizeof (int);
                        io->value.i = *(int *) (value);
                        break;
                    case IGS_DOUBLE_T:
                        io->value_size = sizeof (double);
                        io->value.d = (double) *(int *) (value);
                        break;
                    case IGS_BOOL_T:
                        io->value_size = sizeof (bool);
                        io->value.b = (bool) *(int *) (value);
                        break;
                    case IGS_STRING_T: {
                        if (io->value.s)
                            free (io->value.s);
                        snprintf (buf, NUMBER_TO_STRING_MAX_LENGTH + 1, "%d", *(int *) (value));
                        io->value.s = strdup (buf);
                        io->value_size = (strlen (io->value.s) + 1) * sizeof (char);
                    } break;
                    case IGS_IMPULSION_T:
                        io->value_size = 0;
                        break;
                    case IGS_DATA_T: {
                        if (io->value.data)
                            free (io->value.data);
                        io->value.data = (void *) zmalloc (sizeof (int));
                        assert(io->value.data);
                        memcpy (io->value.data, value, sizeof (int));
                        io->value_size = sizeof (int);
                    } break;
                    default:
                        igsagent_error (agent, "%s has an invalid value type %d",
                                        name, io->value_type);
                        ret = 0;
                        break;
                }
            } break;
            case IGS_DOUBLE_T: {
                switch (io->value_type) {
                    case IGS_INTEGER_T:
                        io->value_size = sizeof (int);
                        io->value.i = (int) (*(double *) (value));
                        break;
                    case IGS_DOUBLE_T:
                        io->value_size = sizeof (double);
                        io->value.d = (*(double *) (value));
                        break;
                    case IGS_BOOL_T:
                        io->value_size = sizeof (bool);
                        io->value.b = (bool) (*(double *) (value));
                        break;
                    case IGS_STRING_T: {
                        if (io->value.s)
                            free (io->value.s);
                        snprintf (buf, NUMBER_TO_STRING_MAX_LENGTH + 1, "%lf", *(double *) (value));
                        io->value.s = strdup (buf);
                        io->value_size = (strlen (io->value.s) + 1) * sizeof (char);
                    } break;
                    case IGS_IMPULSION_T:
                        io->value_size = 0;
                        break;
                    case IGS_DATA_T: {
                        if (io->value.data)
                            free (io->value.data);
                        io->value.data = (void *) zmalloc (sizeof (double));
                        assert(io->value.data);
                        memcpy (io->value.data, value, sizeof (double));
                        io->value_size = sizeof (double);
                    } break;
                    default:
                        igsagent_error(agent, "%s has an invalid value type %d",
                                       name, io->value_type);
                        ret = 0;
                        break;
                }
            } break;
            case IGS_BOOL_T: {
                switch (io->value_type) {
                    case IGS_INTEGER_T:
                        io->value_size = sizeof (int);
                        io->value.i = (int) *(bool *) (value);
                        break;
                    case IGS_DOUBLE_T:
                        io->value_size = sizeof (double);
                        io->value.d = (double) *(bool *) (value);
                        break;
                    case IGS_BOOL_T:
                        io->value_size = sizeof (bool);
                        io->value.b = *(bool *) value;
                        break;
                    case IGS_STRING_T: {
                        if (io->value.s)
                            free (io->value.s);
                        snprintf (buf, NUMBER_TO_STRING_MAX_LENGTH + 1, "%d", (*(bool *) value) ? true : false);
                        io->value.s = strdup (buf);
                        io->value_size = (strlen (io->value.s) + 1) * sizeof (char);
                    } break;
                    case IGS_IMPULSION_T:
                        io->value_size = 0;
                        break;
                    case IGS_DATA_T: {
                        if (io->value.data)
                            free (io->value.data);
                        io->value.data = (void *) zmalloc (sizeof (bool));
                        assert(io->value.data);
                        memcpy (io->value.data, value, sizeof (bool));
                        io->value_size = sizeof (bool);
                    } break;
                    default:
                        igsagent_error(agent, "%s has an invalid value type %d", name, io->value_type);
                        ret = 0;
                        break;
                }
            } break;
            case IGS_STRING_T: {
                switch (io->value_type) {
                    case IGS_INTEGER_T:
                        io->value_size = sizeof (int);
                        io->value.i = atoi ((char *) value);
                        break;
                    case IGS_DOUBLE_T:
                        io->value_size = sizeof (double);
                        io->value.d = atof ((char *) value);
                        break;
                    case IGS_BOOL_T: {
                        char *v = (char *) value;
                        if (streq (v, "false") || streq (v, "False") || streq (v, "FALSE"))
                            io->value.b = false;
                        else if (streq (v, "true") || streq (v, "True") || streq (v, "TRUE"))
                            io->value.b = true;
                        else
                            io->value.b = atoi (v) ? true : false;
                    }
                        io->value_size = sizeof (bool);
                        break;
                    case IGS_STRING_T: {
                        if (io->value.s)
                            free (io->value.s);
                        io->value.s = strdup ((char *) value);
                        io->value_size = (strlen (io->value.s) + 1) * sizeof (char);
                    } break;
                    case IGS_IMPULSION_T:
                        io->value_size = 0;
                        break;
                    case IGS_DATA_T: {
                        if (io->value.data)
                            free (io->value.data);
                        uint8_t *converted = model_string_to_bytes (value);
                        if (converted){
                            io->value.data = converted;
                            io->value_size = strlen (value) / 2;
                        }else {
                            igs_warn ("string %s is not a valid hexadecimal-encoded string and will be encoded as raw data", (char *) value);
                            size_t value_size = (strlen ((char*)value) + 1) * sizeof (char);
                            io->value.data = (void *) zmalloc (value_size);
                            assert(io->value.data);
                            memcpy (io->value.data, value, value_size);
                            io->value_size = value_size;
                        }
                    } break;
                    default:
                        igsagent_error(agent, "%s has an invalid value type %d", name, io->value_type);
                        ret = 0;
                        break;
                }
            } break;
            case IGS_IMPULSION_T: {
                switch (io->value_type) {
                    case IGS_INTEGER_T:
                        io->value_size = sizeof (int);
                        io->value.i = 0;
                        break;
                    case IGS_DOUBLE_T:
                        io->value_size = sizeof (double);
                        io->value.d = 0.0;
                        break;
                    case IGS_BOOL_T:
                        io->value_size = sizeof (bool);
                        io->value.b = false;
                        break;
                    case IGS_STRING_T: {
                        if (io->value.s)
                            free (io->value.s);
                        io->value.s = NULL;
                        io->value_size = 0;
                    } break;
                    case IGS_IMPULSION_T:
                        io->value_size = 0;
                        break;
                    case IGS_DATA_T: {
                        if (io->value.data)
                            free (io->value.data);
                        io->value.data = NULL;
                        io->value_size = 0;
                    } break;
                    default:
                        igsagent_error(agent, "%s has an invalid value type %d", name, io->value_type);
                        ret = 0;
                        break;
                }
            } break;
            case IGS_DATA_T: {
                switch (io->value_type) {
                    case IGS_INTEGER_T:
                        if (size == sizeof(int)){
                            io->value.i = *(int*)value;
                            io->value_size = sizeof(int);
                        }else{
                            igsagent_error(agent, "Raw data does not have proper size for integer IOP %s (%zu bytes vs %zu bytes expected)",
                                           name, size, sizeof(int));
                            ret = 0;
                        }
                        break;
                    case IGS_DOUBLE_T:
                        if (size == sizeof(double)){
                            io->value.d = *(double*)value;
                            io->value_size = sizeof(double);
                        }else{
                            igsagent_error(agent, "Raw data does not have proper size for double IOP %s (%zu bytes vs %zu bytes expected)",
                                           name, size, sizeof(double));
                            ret = 0;
                        }
                        break;
                    case IGS_BOOL_T:
                        if (size == sizeof(bool)){
                            io->value.b = *(bool*)value;
                            io->value_size = sizeof(bool);
                        }else{
                            igsagent_error(agent, "Raw data does not have proper size for bool IOP %s (%zu bytes vs %zu bytes expected)",
                                           name, size, sizeof(bool));
                            ret = 0;
                        }
                        break;
                        break;
                    case IGS_STRING_T:
                        if (((char*)value)[size-1] == '\0'){
                            io->value.s = (void *) zmalloc (size);
                            assert(io->value.s);
                            memcpy (io->value.s, value, size);
                            io->value_size = size;
                        }else{
                            igsagent_error(agent, "Non NULL-terminated data is not allowed into string IOP %s",name);
                            ret = 0;
                        }
                        break;
                    case IGS_IMPULSION_T:
                        io->value_size = 0;
                        break;
                    case IGS_DATA_T: {
                        if (io->value.data)
                            free (io->value.data);
                        io->value.data = (void *) zmalloc (size);
                        assert(io->value.data);
                        memcpy (io->value.data, value, size);
                        io->value_size = size;
                    } break;
                    default:
                        igsagent_error(agent, "%s has an invalid value type %d", name, io->value_type);
                        ret = 0;
                        break;
                }
            } break;
            default:
                break;
        }
    }

    if (ret) {
        // compose log entry
        const char *log_io_type = NULL;
        switch (type) {
            case IGS_INPUT_T:
                log_io_type = "input";
                break;
            case IGS_OUTPUT_T:
                log_io_type = "output";
                break;
            case IGS_ATTRIBUTE_T:
                log_io_type = "attribute";
                break;
            default:
                break;
        }
        char log_io_value_buffer[MAX_IOP_VALUE_LOG_BUFFER_LENGTH] = "";
        char *log_io_value = NULL; //FIXME: try to use log_io_value_buffer only
        switch (io->value_type) {
            case IGS_IMPULSION_T:
                log_io_value = strdup ("impulsion (no value)");
                break;
            case IGS_BOOL_T:
                snprintf (log_io_value_buffer, MAX_IOP_VALUE_LOG_BUFFER_LENGTH,
                          "bool %d", io->value.b);
                log_io_value = strdup (log_io_value_buffer);
                break;
            case IGS_INTEGER_T:
                snprintf (log_io_value_buffer, MAX_IOP_VALUE_LOG_BUFFER_LENGTH,
                          "int %d", io->value.i);
                log_io_value = strdup (log_io_value_buffer);
                break;
            case IGS_DOUBLE_T:
                snprintf (log_io_value_buffer, MAX_IOP_VALUE_LOG_BUFFER_LENGTH,
                          "double %f", io->value.d);
                log_io_value = strdup (log_io_value_buffer);
                break;
            case IGS_STRING_T:
                if (io->value.s){
                    log_io_value = zmalloc ((strlen (io->value.s) + strlen ("string ") + 1) * sizeof (char));
                    sprintf (log_io_value, "string %s", io->value.s);
                } else {
                    log_io_value = zmalloc ((strlen ("null string") + 1) * sizeof (char));
                    sprintf (log_io_value, "null string");
                }
                break;
            case IGS_DATA_T: {
                if (core_context->enable_data_logging) {
                    if (io->value_size > 0) {
                        zchunk_t *chunk = zchunk_new (io->value.data, io->value_size);
                        char *hex_chunk = zchunk_strhex (chunk);
                        log_io_value = zmalloc ((strlen (hex_chunk) + strlen ("data ") + 1) * sizeof (char));
                        sprintf (log_io_value, "data %s", hex_chunk);
                        free (hex_chunk);
                        zchunk_destroy (&chunk);
                    } else {
                        log_io_value = (void *) zmalloc ((strlen ("data 00") + 1) * sizeof (char));
                        sprintf (log_io_value, "data 00");
                    }
                }
                else {
                    snprintf (log_io_value_buffer,
                              MAX_IOP_VALUE_LOG_BUFFER_LENGTH,
                              "data | size: %zu bytes", io->value_size);
                    log_io_value = strdup (log_io_value_buffer);
                }
            } break;
            default:
                break;
        }
        igsagent_debug (agent, "set %s %s to %s", log_io_type, name, log_io_value);
        if (log_io_value)
            free (log_io_value);
        return io;
    }
    return NULL;
}

void model_LOCKED_handle_io_callbacks (igsagent_t *agent, igs_io_t *io){
    assert(agent);
    if (!agent->uuid) //protection against concurrent agent destruction
        return;
    assert(io);
    if (!io->io_callbacks)
        return;
    if (!agent->uuid)
        return;
    model_read_write_lock(__FUNCTION__, __LINE__);
    zlist_t *callbacks = zlist_dup(io->io_callbacks);
    igs_observe_io_wrapper_t *cb = zlist_first(callbacks);
    igs_io_type_t io_type = io->type;
    char *name = strdup(io->name);
    igs_io_value_type_t value_type = io->value_type;
    while (cb && cb->callback_ptr && io->name) {
        switch (io->value_type) {
            case IGS_IMPULSION_T:
                model_read_write_unlock(__FUNCTION__, __LINE__);
                if (agent->uuid)
                    cb->callback_ptr (agent, io_type, name, value_type, NULL, 0, cb->data);
                model_read_write_lock(__FUNCTION__, __LINE__);
                break;
            case IGS_BOOL_T:
                model_read_write_unlock(__FUNCTION__, __LINE__);
                if (agent->uuid)
                    cb->callback_ptr (agent, io_type, name, value_type, &io->value.b, io->value_size, cb->data);
                model_read_write_lock(__FUNCTION__, __LINE__);
                break;
            case IGS_INTEGER_T:
                model_read_write_unlock(__FUNCTION__, __LINE__);
                if (agent->uuid)
                    cb->callback_ptr (agent, io_type, name, value_type, &io->value.i, io->value_size, cb->data);
                model_read_write_lock(__FUNCTION__, __LINE__);
                break;
            case IGS_DOUBLE_T:
                model_read_write_unlock(__FUNCTION__, __LINE__);
                if (agent->uuid)
                    cb->callback_ptr (agent, io_type, name, value_type, &io->value.d, io->value_size, cb->data);
                model_read_write_lock(__FUNCTION__, __LINE__);
                break;
            case IGS_STRING_T:
                model_read_write_unlock(__FUNCTION__, __LINE__);
                if (agent->uuid)
                    cb->callback_ptr (agent, io_type, name, value_type, io->value.s, io->value_size, cb->data);
                model_read_write_lock(__FUNCTION__, __LINE__);
                break;
            case IGS_DATA_T:
                model_read_write_unlock(__FUNCTION__, __LINE__);
                if (agent->uuid)
                    cb->callback_ptr (agent, io_type, name, value_type, io->value.data, io->value_size, cb->data);
                model_read_write_lock(__FUNCTION__, __LINE__);
                break;
            default:
                break;
        }
        cb = zlist_next(callbacks);
    }
    free(name);
    zlist_destroy(&callbacks);
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

igs_io_t *model_find_io_by_name (igsagent_t *agent,
                                 const char *name,
                                 igs_io_type_t type)
{
    switch (type) {
        case IGS_INPUT_T:
            return s_model_find_input_by_name (agent, name);
        case IGS_OUTPUT_T:
            return s_model_find_output_by_name (agent, name);
        case IGS_ATTRIBUTE_T:
            return s_model_find_attribute_by_name (agent, name);
        default:
            igsagent_error (agent, "Unknown IOP type %d", type);
            break;
    }
    return NULL;
}

////////////////////////////////////////////////////////////////////////
#pragma mark PUBLIC API
////////////////////////////////////////////////////////////////////////

void igs_log_include_data (bool enable)
{
    core_init_agent ();
    core_context->enable_data_logging = enable;
}

void igs_log_include_services (bool enable)
{
    core_init_agent ();
    core_context->enable_service_logging = enable;
}

void igs_log_no_warning_if_undefined_service(bool enable){
    core_init_agent ();
    core_context->allow_undefined_services = enable;
}

igs_result_t igsagent_input_buffer (igsagent_t *agent,
                                    const char *name,
                                    void **value,
                                    size_t *size)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_result_t res = s_read_io (agent, name, IGS_INPUT_T, value, size);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

igs_result_t igsagent_output_buffer (igsagent_t *agent,
                                     const char *name,
                                     void **value,
                                     size_t *size)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_result_t res = s_read_io (agent, name, IGS_OUTPUT_T, value, size);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

igs_result_t igsagent_attribute_buffer (igsagent_t *agent,
                                        const char *name,
                                        void **value,
                                        size_t *size)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_result_t res = s_read_io (agent, name, IGS_ATTRIBUTE_T, value, size);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

bool igsagent_input_bool (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return false;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    bool res = s_model_read_io_as_bool (agent, name, IGS_INPUT_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

int igsagent_input_int (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return 0;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    int res = s_model_read_io_as_int (agent, name, IGS_INPUT_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

double igsagent_input_double (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return 0;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    double res = s_model_read_io_as_double (agent, name, IGS_INPUT_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

char *igsagent_input_string (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return NULL;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    char *res = s_model_read_io_as_string (agent, name, IGS_INPUT_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

igs_result_t igsagent_input_data (igsagent_t *agent,
                                  const char *name,
                                  void **data,
                                  size_t *size)
{
    assert (agent);
    if (!agent->uuid){
        *data = NULL;
        *size = 0;
        return IGS_FAILURE;
    }
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_result_t res = s_model_read_io_as_data (agent, name, IGS_INPUT_T, data, size);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

igs_result_t
igsagent_input_zmsg (igsagent_t *agent, const char *name, zmsg_t **msg)
{
    assert (agent);
    if (!agent->uuid){
        *msg = NULL;
        return IGS_FAILURE;
    }
    assert (name);
    void *data = NULL;
    size_t size = 0;
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_result_t ret = s_model_read_io_as_data (agent, name, IGS_INPUT_T, &data, &size);
    zframe_t *frame = zframe_new (data, size);
    free (data);
    *msg = zmsg_decode (frame);
    zframe_destroy (&frame);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return ret;
}

bool igsagent_output_bool (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return false;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    bool res = s_model_read_io_as_bool (agent, name, IGS_OUTPUT_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

int igsagent_output_int (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return 0;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    int res = s_model_read_io_as_int (agent, name, IGS_OUTPUT_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

double igsagent_output_double (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return 0;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    double res = s_model_read_io_as_double (agent, name, IGS_OUTPUT_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

char *igsagent_output_string (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return NULL;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    char *res = s_model_read_io_as_string (agent, name, IGS_OUTPUT_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

igs_result_t igsagent_output_data (igsagent_t *agent,
                                   const char *name,
                                   void **data,
                                   size_t *size)
{
    assert (agent);
    if (!agent->uuid){
        *data = NULL;
        *size = 0;
        return IGS_FAILURE;
    }
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_result_t res = s_model_read_io_as_data (agent, name, IGS_OUTPUT_T, data, size);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

bool igsagent_attribute_bool (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return false;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    bool res = s_model_read_io_as_bool (agent, name, IGS_ATTRIBUTE_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

int igsagent_attribute_int (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return 0;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    int res = s_model_read_io_as_int (agent, name, IGS_ATTRIBUTE_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

double igsagent_attribute_double (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return 0;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    double res = s_model_read_io_as_double (agent, name, IGS_ATTRIBUTE_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

char *igsagent_attribute_string (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return NULL;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    char *res = s_model_read_io_as_string (agent, name, IGS_ATTRIBUTE_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

igs_result_t igsagent_attribute_data (igsagent_t *agent,
                                      const char *name,
                                      void **data,
                                      size_t *size)
{
    assert (agent);
    if (!agent->uuid){
        *data = NULL;
        *size = 0;
        return IGS_FAILURE;
    }
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_result_t res = s_model_read_io_as_data (agent, name, IGS_ATTRIBUTE_T, data, size);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

// --------------------------------  WRITE
// ------------------------------------//

igs_result_t
igsagent_input_set_bool (igsagent_t *agent, const char *name, bool value)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_t *io = model_write (agent, name, IGS_INPUT_T, IGS_BOOL_T, &value, sizeof (bool));
    model_read_write_unlock(__FUNCTION__, __LINE__);
    if (io){
        model_LOCKED_handle_io_callbacks(agent, io);
        return IGS_SUCCESS;
    } else
        return IGS_FAILURE;
}

igs_result_t
igsagent_input_set_int (igsagent_t *agent, const char *name, int value)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_t *io = model_write (agent, name, IGS_INPUT_T, IGS_INTEGER_T, &value, sizeof (int));
    model_read_write_unlock(__FUNCTION__, __LINE__);
    if (io){
        model_LOCKED_handle_io_callbacks(agent, io);
        return IGS_SUCCESS;
    } else
        return IGS_FAILURE;
}

igs_result_t
igsagent_input_set_double (igsagent_t *agent, const char *name, double value)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_t *io = model_write (agent, name, IGS_INPUT_T, IGS_DOUBLE_T, &value, sizeof (double));
    model_read_write_unlock(__FUNCTION__, __LINE__);
    if (io){
        model_LOCKED_handle_io_callbacks(agent, io);
        return IGS_SUCCESS;
    } else
        return IGS_FAILURE;
}

igs_result_t igsagent_input_set_string (igsagent_t *agent,
                                        const char *name,
                                        const char *value)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    size_t value_length = (value == NULL) ? 0 : strlen (value) + 1;
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_t *io = model_write (agent, name, IGS_INPUT_T, IGS_STRING_T, (char *) value, value_length);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    if (io){
        model_LOCKED_handle_io_callbacks(agent, io);
        return IGS_SUCCESS;
    } else
        return IGS_FAILURE;
}

igs_result_t igsagent_input_set_impulsion (igsagent_t *agent,
                                           const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_t *io = model_write (agent, name, IGS_INPUT_T, IGS_IMPULSION_T, NULL, 0);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    if (io){
        model_LOCKED_handle_io_callbacks(agent, io);
        return IGS_SUCCESS;
    } else
        return IGS_FAILURE;
}

igs_result_t igsagent_input_set_data (igsagent_t *agent,
                                      const char *name,
                                      void *value,
                                      size_t size)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_t *io = model_write (agent, name, IGS_INPUT_T, IGS_DATA_T, value, size);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    if (io){
        model_LOCKED_handle_io_callbacks(agent, io);
        return IGS_SUCCESS;
    } else
        return IGS_FAILURE;
}

igs_result_t
igsagent_output_set_bool (igsagent_t *agent, const char *name, bool value)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_t *io = model_write (agent, name, IGS_OUTPUT_T, IGS_BOOL_T, &value, sizeof (bool));
    if (io && !agent->rt_synchronous_mode_enabled)
        network_publish_output (agent, io);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    if (io){
        model_LOCKED_handle_io_callbacks(agent, io);
        return IGS_SUCCESS;
    } else
        return IGS_FAILURE;
}

igs_result_t
igsagent_output_set_int (igsagent_t *agent, const char *name, int value)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_t *io = model_write (agent, name, IGS_OUTPUT_T, IGS_INTEGER_T, &value, sizeof (int));
    if (io && !agent->rt_synchronous_mode_enabled)
        network_publish_output (agent, io);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    if (io){
        model_LOCKED_handle_io_callbacks(agent, io);
        return IGS_SUCCESS;
    } else
        return IGS_FAILURE;
}

igs_result_t
igsagent_output_set_double (igsagent_t *agent, const char *name, double value)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_t *io = model_write (agent, name, IGS_OUTPUT_T, IGS_DOUBLE_T, &value, sizeof (double));
    if (io && !agent->rt_synchronous_mode_enabled)
        network_publish_output (agent, io);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    if (io){
        model_LOCKED_handle_io_callbacks(agent, io);
        return IGS_SUCCESS;
    } else
        return IGS_FAILURE;
}

igs_result_t igsagent_output_set_string (igsagent_t *agent,
                                         const char *name,
                                         const char *value)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    size_t length = (value == NULL) ? 0 : strlen (value) + 1;
    igs_io_t *io = model_write (agent, name, IGS_OUTPUT_T, IGS_STRING_T, (char *) value, length);
    if (io && !agent->rt_synchronous_mode_enabled)
        network_publish_output (agent, io);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    if (io){
        model_LOCKED_handle_io_callbacks(agent, io);
        return IGS_SUCCESS;
    } else
        return IGS_FAILURE;
}

igs_result_t igsagent_output_set_impulsion (igsagent_t *agent,
                                            const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_t *io = model_write (agent, name, IGS_OUTPUT_T, IGS_IMPULSION_T, NULL, 0);
    if (io && !agent->rt_synchronous_mode_enabled)
        network_publish_output (agent, io);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    if (io){
        model_LOCKED_handle_io_callbacks(agent, io);
        return IGS_SUCCESS;
    } else
        return IGS_FAILURE;
}

igs_result_t igsagent_output_set_data (igsagent_t *agent,
                                       const char *name,
                                       void *value,
                                       size_t size)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_t *io = model_write (agent, name, IGS_OUTPUT_T, IGS_DATA_T, value, size);
    if (io && !agent->rt_synchronous_mode_enabled)
        network_publish_output (agent, io);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    if (io){
        model_LOCKED_handle_io_callbacks(agent, io);
        return IGS_SUCCESS;
    } else
        return IGS_FAILURE;
}

igs_result_t
igsagent_output_set_zmsg (igsagent_t *agent, const char *name, zmsg_t *msg)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    assert (msg);
    model_read_write_lock(__FUNCTION__, __LINE__);
    zframe_t *frame = zmsg_encode (msg);
    assert(frame);
    void *value = zframe_data (frame);
    size_t size = zframe_size (frame);
    igs_io_t *io = model_write (agent, name, IGS_OUTPUT_T, IGS_DATA_T, value, size);
    if (io && !agent->rt_synchronous_mode_enabled)
        network_publish_output (agent, io);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    if (io){
        model_LOCKED_handle_io_callbacks(agent, io);
        return IGS_SUCCESS;
    } else
        return IGS_FAILURE;
}

igs_result_t
igsagent_attribute_set_bool (igsagent_t *agent, const char *name, bool value)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_t *io = model_write (agent, name, IGS_ATTRIBUTE_T, IGS_BOOL_T, &value, sizeof (bool));
    model_read_write_unlock(__FUNCTION__, __LINE__);
    if (io){
        model_LOCKED_handle_io_callbacks(agent, io);
        return IGS_SUCCESS;
    } else
        return IGS_FAILURE;
}

igs_result_t
igsagent_attribute_set_int (igsagent_t *agent, const char *name, int value)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_t *io = model_write (agent, name, IGS_ATTRIBUTE_T, IGS_INTEGER_T, &value, sizeof (int));
    model_read_write_unlock(__FUNCTION__, __LINE__);
    if (io){
        model_LOCKED_handle_io_callbacks(agent, io);
        return IGS_SUCCESS;
    } else
        return IGS_FAILURE;
}

igs_result_t igsagent_attribute_set_double (igsagent_t *agent,
                                            const char *name,
                                            double value)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_t *io = model_write (agent, name, IGS_ATTRIBUTE_T, IGS_DOUBLE_T, &value, sizeof (double));
    model_read_write_unlock(__FUNCTION__, __LINE__);
    if (io){
        model_LOCKED_handle_io_callbacks(agent, io);
        return IGS_SUCCESS;
    } else
        return IGS_FAILURE;
}

igs_result_t igsagent_attribute_set_string (igsagent_t *agent,
                                            const char *name,
                                            const char *value)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    size_t value_length = (value == NULL) ? 0 : strlen (value) + 1;
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_t *io = model_write (agent, name, IGS_ATTRIBUTE_T, IGS_STRING_T, (char *) value, value_length);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    if (io){
        model_LOCKED_handle_io_callbacks(agent, io);
        return IGS_SUCCESS;
    } else
        return IGS_FAILURE;
}

igs_result_t igsagent_attribute_set_data (igsagent_t *agent,
                                          const char *name,
                                          void *value,
                                          size_t size)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_t *io = model_write (agent, name, IGS_ATTRIBUTE_T, IGS_DATA_T, value, size);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    if (io){
        model_LOCKED_handle_io_callbacks(agent, io);
        return IGS_SUCCESS;
    } else
        return IGS_FAILURE;
}

void igsagent_constraints_enforce(igsagent_t *self, bool enforce)
{
    assert (self);
    if (!self->uuid)
        return;
    self->enforce_constraints = enforce;
}

igs_result_t igsagent_input_add_constraint (igsagent_t *self, const char *name,
                                            const char *constraint)
{
    assert (self);
    if (!self->uuid)
        return IGS_FAILURE;
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_result_t res =  s_model_add_constraint(self, IGS_INPUT_T, name, constraint);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

igs_result_t igsagent_output_add_constraint (igsagent_t *self, const char *name,
                                             const char *constraint)
{
    assert (self);
    if (!self->uuid)
        return IGS_FAILURE;
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_result_t res =  s_model_add_constraint(self, IGS_OUTPUT_T, name, constraint);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

igs_result_t igsagent_attribute_add_constraint (igsagent_t *self, const char *name,
                                                const char *constraint)
{
    assert (self);
    if (!self->uuid)
        return IGS_FAILURE;
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_result_t res =  s_model_add_constraint(self, IGS_ATTRIBUTE_T, name, constraint);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

igs_result_t igsagent_input_set_description(igsagent_t *self, const char *name, const char *description)
{
    assert (self);
    if (!self->uuid)
        return IGS_FAILURE;
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_result_t res =  s_model_set_description(self, IGS_INPUT_T, name, description);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

char * igsagent_input_description(igsagent_t *self, const char *name)
{
    assert (self);
    if (!self->uuid)
        return NULL;
    model_read_write_lock(__FUNCTION__, __LINE__);
    char * description =  s_model_get_description(self, IGS_INPUT_T, name);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return description;
}

igs_result_t igsagent_output_set_description(igsagent_t *self, const char *name, const char *description)
{
    assert (self);
    if (!self->uuid)
        return IGS_FAILURE;
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_result_t res =  s_model_set_description(self, IGS_OUTPUT_T, name, description);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

char * igsagent_output_description(igsagent_t *self, const char *name)
{
    assert (self);
    if (!self->uuid)
        return NULL;
    model_read_write_lock(__FUNCTION__, __LINE__);
    char * description =  s_model_get_description(self, IGS_OUTPUT_T, name);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return description;
}

igs_result_t igsagent_attribute_set_description(igsagent_t *self, const char *name, const char *description)
{
    assert (self);
    if (!self->uuid)
        return IGS_FAILURE;
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_result_t res =  s_model_set_description(self, IGS_ATTRIBUTE_T, name, description);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

char * igsagent_attribute_description(igsagent_t *self, const char *name)
{
    assert (self);
    if (!self->uuid)
        return NULL;
    model_read_write_lock(__FUNCTION__, __LINE__);
    char * description =  s_model_get_description(self, IGS_ATTRIBUTE_T, name);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return description;
}

igs_result_t igsagent_input_set_detailed_type(igsagent_t *self, const char *name,
                                              const char *type_name, const char *specification)
{
    assert (self);
    if (!self->uuid)
        return IGS_FAILURE;
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_result_t res =  s_model_set_detailed_type(self, IGS_INPUT_T, name, type_name, specification);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

igs_result_t igsagent_output_set_detailed_type(igsagent_t *self, const char *name,
                                               const char *type_name, const char *specification)
{
    assert (self);
    if (!self->uuid)
        return IGS_FAILURE;
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_result_t res =  s_model_set_detailed_type(self, IGS_OUTPUT_T, name, type_name, specification);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

igs_result_t igsagent_attribute_set_detailed_type(igsagent_t *self, const char *name,
                                                  const char *type_name, const char *specification)
{
    assert (self);
    if (!self->uuid)
        return IGS_FAILURE;
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_result_t res =  s_model_set_detailed_type(self, IGS_ATTRIBUTE_T, name, type_name, specification);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

void igsagent_clear_input (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    s_model_clear_io (agent, name, IGS_INPUT_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

void igsagent_clear_output (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    s_model_clear_io (agent, name, IGS_OUTPUT_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

void igsagent_clear_attribute (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return;
    assert (name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    s_model_clear_io (agent, name, IGS_ATTRIBUTE_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

// --------------------------------  INTROSPECTION

igs_io_value_type_t igsagent_input_type (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_UNKNOWN_T;
    assert (name);
    assert (strlen(name));
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_value_type_t res = s_model_get_type_for_io (agent, name, IGS_INPUT_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

igs_io_value_type_t igsagent_output_type (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_UNKNOWN_T;
    assert (name);
    assert (strlen(name));
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_value_type_t res = s_model_get_type_for_io (agent, name, IGS_OUTPUT_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

igs_io_value_type_t igsagent_attribute_type (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_UNKNOWN_T;
    assert (name);
    assert (strlen(name));
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_value_type_t res = s_model_get_type_for_io (agent, name, IGS_ATTRIBUTE_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

size_t igsagent_input_count (igsagent_t *agent)
{
    assert (agent);
    if (!agent->uuid)
        return 0;
    if (agent->definition == NULL) {
        igsagent_warn (agent, "definition is NULL");
        return 0;
    }
    model_read_write_lock(__FUNCTION__, __LINE__);
    size_t size =  zhashx_size(agent->definition->inputs_table);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return size;
}

size_t igsagent_output_count (igsagent_t *agent)
{
    assert (agent);
    if (!agent->uuid)
        return 0;
    if (agent->definition == NULL) {
        igsagent_warn (agent, "definition is NULL");
        return 0;
    }
    model_read_write_lock(__FUNCTION__, __LINE__);
    size_t size =  zhashx_size(agent->definition->outputs_table);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return size;
}

size_t igsagent_attribute_count (igsagent_t *agent)
{
    assert (agent);
    if (!agent->uuid)
        return 0;
    if (agent->definition == NULL) {
        igsagent_warn (agent, "definition is NULL");
        return 0;
    }
    model_read_write_lock(__FUNCTION__, __LINE__);
    size_t size =  zhashx_size(agent->definition->attributes_table);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return size;
}

char **igsagent_input_list (igsagent_t *agent, size_t *nb_of_elements)
{
    assert (agent);
    if (!agent->uuid){
        *nb_of_elements = 0;
        return NULL;
    }
    model_read_write_lock(__FUNCTION__, __LINE__);
    char **res = s_model_get_io_list (agent, nb_of_elements, IGS_INPUT_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

char **igsagent_output_list (igsagent_t *agent, size_t *nb_of_elements)
{
    assert (agent);
    if (!agent->uuid){
        *nb_of_elements = 0;
        return NULL;
    }
    model_read_write_lock(__FUNCTION__, __LINE__);
    char **res = s_model_get_io_list (agent, nb_of_elements, IGS_OUTPUT_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

char **igsagent_attribute_list (igsagent_t *agent, size_t *nb_of_elements)
{
    assert (agent);
    if (!agent->uuid){
        *nb_of_elements = 0;
        return NULL;
    }
    model_read_write_lock(__FUNCTION__, __LINE__);
    char **res = s_model_get_io_list (agent, nb_of_elements, IGS_ATTRIBUTE_T);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

void igs_free_io_list (char **list, size_t nb_of_elements)
{
    // FIXME: secure this function if nb_of_elements exceeds allocated value
    assert(list);
    if (*list && nb_of_elements) {
        size_t i = 0;
        for (i = 0; i < nb_of_elements; i++) {
            if (list[i])
                free (list[i]);
        }
    }
    free (list);
}

bool igsagent_input_exists (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return false;
    assert (name);
    if (agent->definition == NULL)
        return false;
    model_read_write_lock(__FUNCTION__, __LINE__);
    bool res = s_model_check_io_existence (agent, name, agent->definition->inputs_table);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

bool igsagent_output_exists (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return false;
    assert (name);
    if (agent->definition == NULL)
        return false;
    model_read_write_lock(__FUNCTION__, __LINE__);
    bool res = s_model_check_io_existence (agent, name, agent->definition->outputs_table);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

bool igsagent_attribute_exists (igsagent_t *agent, const char *name)
{
    assert (agent);
    if (!agent->uuid)
        return false;
    assert (name);
    if (agent->definition == NULL)
        return false;
    model_read_write_lock(__FUNCTION__, __LINE__);
    bool res = s_model_check_io_existence (agent, name, agent->definition->attributes_table);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

// --------------------------------  OBSERVE  ------------------------------------//

void igsagent_observe_input (igsagent_t *agent,
                             const char *name,
                             igsagent_io_fn cb,
                             void *my_data)
{
    assert (agent);
    if (!agent->uuid)
        return;
    assert (name);
    assert (cb);
    model_read_write_lock(__FUNCTION__, __LINE__);
    s_model_observe (agent, name, IGS_INPUT_T, cb, my_data);
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

void igsagent_observe_output (igsagent_t *agent,
                              const char *name,
                              igsagent_io_fn cb,
                              void *my_data)
{
    assert (agent);
    if (!agent->uuid)
        return;
    assert (name);
    assert (cb);
    model_read_write_lock(__FUNCTION__, __LINE__);
    s_model_observe (agent, name, IGS_OUTPUT_T, cb, my_data);
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

void igsagent_observe_attribute (igsagent_t *agent,
                                 const char *name,
                                 igsagent_io_fn cb,
                                 void *my_data)
{
    assert (agent);
    if (!agent->uuid)
        return;
    assert (name);
    assert (cb);
    model_read_write_lock(__FUNCTION__, __LINE__);
    s_model_observe (agent, name, IGS_ATTRIBUTE_T, cb, my_data);
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

// --------------------------------  MUTE ------------------------------------//

void igsagent_output_mute (igsagent_t *agent, const char *name)
{
    assert(agent);
    if (!agent->uuid)
        return;
    assert(name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_t *io = model_find_io_by_name (agent, name, IGS_OUTPUT_T);
    if (io == NULL || io->type != IGS_OUTPUT_T) {
        igsagent_error (agent, "Output '%s' not found", name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return;
    }
    io->is_muted = true;
    if (core_context && core_context->node) {
        s_lock_zyre_peer (__FUNCTION__, __LINE__);
        zmsg_t *msg = zmsg_new ();
        zmsg_addstr (msg, OUTPUT_MUTED_MSG);
        zmsg_addstr (msg, name);
        zmsg_addstr (msg, agent->uuid);
        zyre_shout (core_context->node, IGS_PRIVATE_CHANNEL, &msg);
        s_unlock_zyre_peer (__FUNCTION__, __LINE__);
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

void igsagent_output_unmute (igsagent_t *agent, const char *name)
{
    assert(agent);
    if (!agent->uuid)
        return;
    assert(name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_t *io = model_find_io_by_name (agent, name, IGS_OUTPUT_T);
    if (io == NULL || io->type != IGS_OUTPUT_T) {
        igsagent_error (agent, "Output '%s' not found", name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return;
    }
    io->is_muted = false;
    if (core_context && core_context->node) {
        s_lock_zyre_peer (__FUNCTION__, __LINE__);
        zmsg_t *msg = zmsg_new ();
        zmsg_addstr (msg, OUTPUT_UNMUTED_MSG);
        zmsg_addstr (msg, name);
        zmsg_addstr (msg, agent->uuid);
        zyre_shout (core_context->node, IGS_PRIVATE_CHANNEL, &msg);
        s_unlock_zyre_peer (__FUNCTION__, __LINE__);
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

bool igsagent_output_is_muted (igsagent_t *agent, const char *name)
{
    assert(agent);
    if (!agent->uuid)
        return false;
    assert(name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_io_t *io = model_find_io_by_name (agent, name, IGS_OUTPUT_T);
    if (io == NULL || io->type != IGS_OUTPUT_T) {
        igsagent_warn (agent, "Output '%s' not found", name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return 0;
    }
    bool res = io->is_muted;
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}
