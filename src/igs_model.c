/*  =========================================================================
    model - read/write/observe inputs, outputs and parameters

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "ingescape_classes.h"
#include "ingescape_private.h"
#include "uthash/utlist.h"
#include <czmq.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef W_OK
#define W_OK 02
#endif

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
#define NUMBER_TO_STRING_MAX_LENGTH 255
#define BOOL_TO_STRING_MAX_LENGTH 6

uint8_t *s_model_string_to_bytes (char *string)
{
    assert (string);
    size_t slength = strlen (string);
    if ((slength % 2) != 0) // must be even
        return NULL;
    size_t dlength = slength / 2;
    uint8_t *data = (uint8_t *) zmalloc (dlength);
    size_t index = 0;
    while (index < slength) {
        char c = string[index];
        int value = 0;
        if (c >= '0' && c <= '9')
            value = (c - '0');
        else
        if (c >= 'A' && c <= 'F')
            value = (10 + (c - 'A'));
        else
        if (c >= 'a' && c <= 'f')
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

char *model_get_iop_value_as_string (igs_iop_t *iop)
{
    assert (iop);
    char *str_value = NULL;
    if (iop) {
        switch (iop->value_type) {
            case IGS_INTEGER_T:
                str_value = (char *) zmalloc (NUMBER_TO_STRING_MAX_LENGTH + 1);
                snprintf (str_value, NUMBER_TO_STRING_MAX_LENGTH, "%i",
                          iop->value.i);
                break;
            case IGS_DOUBLE_T:
                str_value = (char *) zmalloc (NUMBER_TO_STRING_MAX_LENGTH + 1);
                snprintf (str_value, NUMBER_TO_STRING_MAX_LENGTH, "%lf",
                          iop->value.d);
                break;
            case IGS_BOOL_T:
                str_value = (char *) zmalloc (BOOL_TO_STRING_MAX_LENGTH);
                if (iop->value.b == true)
                    snprintf (str_value, BOOL_TO_STRING_MAX_LENGTH, "%s",
                              "true");
                else
                    snprintf (str_value, BOOL_TO_STRING_MAX_LENGTH, "%s",
                              "false");
                break;
            case IGS_STRING_T:
                str_value = (char *) zmalloc (strlen (iop->value.s) + 1);
                snprintf (str_value, strlen (iop->value.s) + 1, "%s",
                          iop->value.s);
                break;
            case IGS_IMPULSION_T:
                break;
            case IGS_DATA_T:
                str_value = (char *) zmalloc (iop->value_size + 1);
                snprintf (str_value, iop->value_size + 1, "%s",
                          (char *) iop->value.data);
                break;
            default:
                break;
        }
    }
    return str_value;
}

#define MAX_IOP_VALUE_LOG_BUFFER_LENGTH 256

void s_model_run_observe_callbacks_for_iop (igsagent_t *agent,
                                            igs_iop_t *iop,
                                            void *value,
                                            size_t value_size)
{
    if (agent && agent->uuid) {
        igs_observe_wrapper_t *cb;
        DL_FOREACH (iop->callbacks, cb)
            cb->callback_ptr (agent, iop->type, iop->name, iop->value_type,
                              value, value_size, cb->data);
    }
}

const igs_iop_t *model_write_iop (igsagent_t *agent, const char *name,
                                  igs_iop_type_t type, igs_iop_value_type_t value_type,
                                  void *value, size_t size)
{
    assert (agent);
    assert (name);
    model_read_write_lock (__FUNCTION__, __LINE__);
    igs_iop_t *iop = model_find_iop_by_name (agent, name, type);
    if (!iop) {
        igsagent_error (agent, "%s not found for writing", name);
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return NULL;
    }
    int ret = 1;
    void *out_value = NULL;
    size_t out_size = 0;
    char buf[NUMBER_TO_STRING_MAX_LENGTH + 1] = "";
    // check that this agent has not been destroyed when we were locked
    if (!agent || !(agent->uuid)) {
        model_read_write_unlock (__FUNCTION__, __LINE__);
        return NULL;
    }
    
    //apply constraint if any
    if (iop->constraint && agent->enforce_constraints){
        if (iop->value_type == IGS_INTEGER_T){
            int converted_value = 0;
            switch (value_type) {
                case IGS_STRING_T:
                    converted_value = atoi((char *)value);
                    break;
                case IGS_DATA_T:
                    igsagent_error(agent, "constraint type error for %s (value is data and IOP is integer)", iop->name);
                    model_read_write_unlock (__FUNCTION__, __LINE__);
                    return NULL;
                case IGS_DOUBLE_T:
                    converted_value = (int)(*(double*)value);
                    break;
                default:
                    converted_value = *(int*)value;
                    break;
            }
            
            switch (iop->constraint->type) {
                case IGS_CONSTRAINT_MIN:
                    if (converted_value < iop->constraint->min_int.min){
                        igsagent_error(agent, "constraint error for %s (too low)", iop->name);
                        model_read_write_unlock (__FUNCTION__, __LINE__);
                        return NULL;
                    }
                    break;
                case IGS_CONSTRAINT_MAX:
                    if (converted_value > iop->constraint->max_int.max){
                        igsagent_error(agent, "constraint error for %s (too high)", iop->name);
                        model_read_write_unlock (__FUNCTION__, __LINE__);
                        return NULL;
                    }
                    break;
                case IGS_CONSTRAINT_RANGE:
                    if (converted_value > iop->constraint->range_int.max){
                        igsagent_error(agent, "constraint error for %s (too high)", iop->name);
                        model_read_write_unlock (__FUNCTION__, __LINE__);
                        return NULL;
                    }else if (converted_value < iop->constraint->range_int.min){
                        igsagent_error(agent, "constraint error for %s (too low)", iop->name);
                        model_read_write_unlock (__FUNCTION__, __LINE__);
                        return NULL;
                    }
                    break;
                    
                default:
                    break;
            }
        }else if(iop->value_type == IGS_DOUBLE_T){
            double converted_value = 0;
            switch (value_type) {
                case IGS_STRING_T:
                    converted_value = atof((char *)value);
                    break;
                case IGS_DATA_T:
                    igsagent_error(agent, "constraint type error for %s (value is data and IOP is double)", iop->name);
                    model_read_write_unlock (__FUNCTION__, __LINE__);
                    return NULL;
                case IGS_INTEGER_T:
                case IGS_BOOL_T:
                    converted_value = (double)(*(int*)value);
                    break;
                default:
                    converted_value = *(double*)value;
                    break;
            }
            
            switch (iop->constraint->type) {
                case IGS_CONSTRAINT_MIN:
                    if (converted_value < iop->constraint->min_double.min){
                        igsagent_error(agent, "constraint error for %s (too low)", iop->name);
                        model_read_write_unlock (__FUNCTION__, __LINE__);
                        return NULL;
                    }
                    break;
                case IGS_CONSTRAINT_MAX:
                    if (converted_value > iop->constraint->max_double.max){
                        igsagent_error(agent, "constraint error for %s (too high)", iop->name);
                        model_read_write_unlock (__FUNCTION__, __LINE__);
                        return NULL;
                    }
                    break;
                case IGS_CONSTRAINT_RANGE:
                    if (converted_value > iop->constraint->range_double.max){
                        igsagent_error(agent, "constraint error for %s (too high)", iop->name);
                        model_read_write_unlock (__FUNCTION__, __LINE__);
                        return NULL;
                    }else if (converted_value < iop->constraint->range_double.min){
                        igsagent_error(agent, "constraint error for %s (too low)", iop->name);
                        model_read_write_unlock (__FUNCTION__, __LINE__);
                        return NULL;
                    }
                    break;
                    
                default:
                    break;
            }
        }else if (iop->value_type == IGS_STRING_T){
            char *converted_value = NULL;
            switch (value_type) {
                case IGS_STRING_T:
                    converted_value = (char *)value;
                    break;
                case IGS_DATA_T:
                    igsagent_error(agent, "constraint type error for %s (value is data and IOP is string)", iop->name);
                    model_read_write_unlock (__FUNCTION__, __LINE__);
                    return NULL;
                case IGS_INTEGER_T:
                case IGS_BOOL_T:
                    snprintf (buf, NUMBER_TO_STRING_MAX_LENGTH + 1, "%d",
                              (value == NULL) ? 0 : *(int *) (value));
                    converted_value = buf;
                    break;
                case IGS_DOUBLE_T:
                    snprintf (buf, NUMBER_TO_STRING_MAX_LENGTH + 1, "%f",
                              (value == NULL) ? 0 : *(double *) (value));
                    converted_value = buf;
                    break;
                default:
                    snprintf (buf, NUMBER_TO_STRING_MAX_LENGTH + 1, "");
                    converted_value = buf;
                    break;
            }
            if (!converted_value){
                igsagent_error(agent, "constraint error for %s (value is NULL)", iop->name);
                model_read_write_unlock (__FUNCTION__, __LINE__);
                return NULL;
            }
            if (!zrex_matches(iop->constraint->regexp.rex, converted_value)){
                igsagent_error(agent, "constraint error for %s (not matching regexp)", iop->name);
                model_read_write_unlock (__FUNCTION__, __LINE__);
                return NULL;
            }
        }
    }
    
    // TODO: optimize if value is NULL
    switch (value_type) {
        case IGS_INTEGER_T: {
            switch (iop->value_type) {
                case IGS_INTEGER_T:
                    out_size = iop->value_size = sizeof (int);
                    iop->value.i = (value == NULL) ? 0 : *(int *) (value);
                    out_value = &(iop->value.i);
                    break;
                case IGS_DOUBLE_T:
                    out_size = iop->value_size = sizeof (double);
                    iop->value.d = (value == NULL) ? 0 : *(int *) (value);
                    out_value = &(iop->value.d);
                    break;
                case IGS_BOOL_T:
                    out_size = iop->value_size = sizeof (bool);
                    iop->value.b = (value == NULL)
                                     ? false
                                     : ((*(int *) (value)) ? true : false);
                    out_value = &(iop->value.b);
                    break;
                case IGS_STRING_T: {
                    if (iop->value.s)
                        free (iop->value.s);
                    if (value == NULL)
                        iop->value.s = strdup ("");
                    else {
                        snprintf (buf, NUMBER_TO_STRING_MAX_LENGTH + 1, "%d",
                                  (value == NULL) ? 0 : *(int *) (value));
                        iop->value.s = strdup (buf);
                    }
                    out_size = iop->value_size =
                      (strlen (iop->value.s) + 1) * sizeof (char);
                    out_value = iop->value.s;
                } break;
                case IGS_IMPULSION_T:
                    iop->value_size = 0;
                    break;
                case IGS_DATA_T: {
                    if (iop->value.data)
                        free (iop->value.data);
                    iop->value.data = NULL;
                    iop->value.data = (void *) zmalloc (sizeof (int));
                    memcpy (iop->value.data, value, sizeof (int));
                    out_size = iop->value_size = sizeof (int);
                    out_value = iop->value.data;
                } break;
                default:
                    igsagent_error (agent, "%s has an invalid value type %d",
                                     name, iop->value_type);
                    ret = 0;
                    break;
            }
        } break;
        case IGS_DOUBLE_T: {
            switch (iop->value_type) {
                case IGS_INTEGER_T:
                    out_size = iop->value_size = sizeof (int);
                    iop->value.i =
                      (value == NULL) ? 0 : (int) (*(double *) (value));
                    out_value = &(iop->value.i);
                    break;
                case IGS_DOUBLE_T:
                    out_size = iop->value_size = sizeof (double);
                    iop->value.d =
                      (value == NULL) ? 0 : (double) (*(double *) (value));
                    out_value = &(iop->value.d);
                    break;
                case IGS_BOOL_T:
                    out_size = iop->value_size = sizeof (bool);
                    iop->value.b =
                      (value == NULL)
                        ? false
                        : (((int) (*(double *) (value))) ? true : false);
                    out_value = &(iop->value.b);
                    break;
                case IGS_STRING_T: {
                    if (iop->value.s)
                        free (iop->value.s);
                    if (value == NULL)
                        iop->value.s = strdup ("");
                    else {
                        snprintf (buf, NUMBER_TO_STRING_MAX_LENGTH + 1, "%lf",
                                  (value == NULL) ? 0 : *(double *) (value));
                        iop->value.s = strdup (buf);
                    }
                    out_size = iop->value_size =
                      (strlen (iop->value.s) + 1) * sizeof (char);
                    out_value = iop->value.s;
                } break;
                case IGS_IMPULSION_T:
                    iop->value_size = 0;
                    break;
                case IGS_DATA_T: {
                    if (iop->value.data)
                        free (iop->value.data);
                    iop->value.data = NULL;
                    iop->value.data = (void *) zmalloc (sizeof (double));
                    memcpy (iop->value.data, value, sizeof (double));
                    out_size = iop->value_size = sizeof (double);
                    out_value = iop->value.data;
                } break;
                default:
                    igsagent_error(agent, "%s has an invalid value type %d",
                                   name, iop->value_type);
                    ret = 0;
                    break;
            }
        } break;
        case IGS_BOOL_T: {
            switch (iop->value_type) {
                case IGS_INTEGER_T:
                    out_size = iop->value_size = sizeof (int);
                    iop->value.i = (value == NULL) ? 0 : *(bool *) (value);
                    out_value = &(iop->value.i);
                    break;
                case IGS_DOUBLE_T:
                    out_size = iop->value_size = sizeof (double);
                    iop->value.d = (value == NULL) ? 0 : *(bool *) (value);
                    out_value = &(iop->value.d);
                    break;
                case IGS_BOOL_T:
                    out_size = iop->value_size = sizeof (bool);
                    iop->value.b = (value == NULL) ? false : *(bool *) value;
                    out_value = &(iop->value.b);
                    break;
                case IGS_STRING_T: {
                    if (iop->value.s)
                        free (iop->value.s);
                    if (value == NULL)
                        iop->value.s = strdup ("");
                    else {
                        snprintf (buf, NUMBER_TO_STRING_MAX_LENGTH + 1, "%d",
                                  (value == NULL) ? 0 : *(bool *) value);
                        iop->value.s = strdup (buf);
                    }
                    out_size = iop->value_size =
                      (strlen (iop->value.s) + 1) * sizeof (char);
                    out_value = iop->value.s;
                } break;
                case IGS_IMPULSION_T:
                    iop->value_size = 0;
                    break;
                case IGS_DATA_T: {
                    if (iop->value.data)
                        free (iop->value.data);
                    iop->value.data = NULL;
                    iop->value.data = (void *) zmalloc (sizeof (bool));
                    memcpy (iop->value.data, value, sizeof (bool));
                    out_size = iop->value_size = sizeof (bool);
                    out_value = iop->value.data;
                } break;
                default:
                    igsagent_error(agent, "%s has an invalid value type %d",
                                   name, iop->value_type);
                    ret = 0;
                    break;
            }
        } break;
        case IGS_STRING_T: {
            switch (iop->value_type) {
                case IGS_INTEGER_T:
                    out_size = iop->value_size = sizeof (int);
                    iop->value.i = (value == NULL) ? 0 : atoi ((char *) value);
                    out_value = &(iop->value.i);
                    break;
                case IGS_DOUBLE_T:
                    out_size = iop->value_size = sizeof (double);
                    iop->value.d = (value == NULL) ? 0 : atof ((char *) value);
                    out_value = &(iop->value.d);
                    break;
                case IGS_BOOL_T: {
                    char *v = (char *) value;
                    if (v == NULL)
                        iop->value.b = false;
                    else
                    if (streq (v, "false") || streq (v, "False")
                        || streq (v, "FALSE"))
                        iop->value.b = false;
                    else
                    if (streq (v, "true") || streq (v, "True")
                        || streq (v, "TRUE"))
                        iop->value.b = true;
                    else
                        iop->value.b = atoi (v) ? true : false;
                }
                    out_size = iop->value_size = sizeof (bool);
                    out_value = &(iop->value.b);
                    break;
                case IGS_STRING_T: {
                    if (iop->value.s)
                        free (iop->value.s);
                    if (value == NULL)
                        iop->value.s = strdup ("");
                    else
                        iop->value.s = strdup ((char *) value);
                    out_size = iop->value_size =
                      (strlen (iop->value.s) + 1) * sizeof (char);
                    out_value = iop->value.s;
                } break;
                case IGS_IMPULSION_T:
                    iop->value_size = 0;
                    break;
                case IGS_DATA_T: {
                    if (iop->value.data)
                        free (iop->value.data);
                    iop->value.data = NULL;
                    size_t s = 0;
                    if (value) {
                        uint8_t *converted = s_model_string_to_bytes (value);
                        iop->value.data = converted;
                        if (converted)
                            s = strlen (value) / 2;
                        else {
                            igs_error ("string %s is not a valid "
                                       "hexadecimal-encoded string",
                                       (char *) value);
                            model_read_write_unlock (__FUNCTION__, __LINE__);
                            return NULL;
                        }
                    }
                    out_size = iop->value_size = s;
                    out_value = iop->value.data;
                } break;
                default:
                    igsagent_error(agent, "%s has an invalid value type %d",
                                   name, iop->value_type);
                    ret = 0;
                    break;
            }
        } break;
        case IGS_IMPULSION_T: {
            switch (iop->value_type) {
                case IGS_INTEGER_T:
                    out_size = iop->value_size = sizeof (int);
                    iop->value.i = 0;
                    out_value = &(iop->value.i);
                    break;
                case IGS_DOUBLE_T:
                    out_size = iop->value_size = sizeof (double);
                    iop->value.d = 0.0;
                    out_value = &(iop->value.d);
                    break;
                case IGS_BOOL_T:
                    out_size = iop->value_size = sizeof (bool);
                    iop->value.b = false;
                    out_value = &(iop->value.b);
                    break;
                case IGS_STRING_T: {
                    if (iop->value.s)
                        free (iop->value.s);
                    iop->value.s = strdup ("");
                    out_size = iop->value_size = sizeof (char);
                    out_value = iop->value.s;
                } break;
                case IGS_IMPULSION_T:
                    iop->value_size = 0;
                    break;
                case IGS_DATA_T: {
                    if (iop->value.data)
                        free (iop->value.data);
                    iop->value.data = NULL;
                    iop->value_size = 0;
                } break;
                default:
                    igsagent_error(agent, "%s has an invalid value type %d",
                                   name, iop->value_type);
                    ret = 0;
                    break;
            }
        } break;
        case IGS_DATA_T: {
            switch (iop->value_type) {
                case IGS_INTEGER_T:
                    igsagent_warn (
                      agent, "Raw data is not allowed into integer IOP %s",
                      name);
                    ret = 0;
                    break;
                case IGS_DOUBLE_T:
                    igsagent_warn (
                      agent, "Raw data is not allowed into double IOP %s",
                      name);
                    ret = 0;
                    break;
                case IGS_BOOL_T:
                    igsagent_warn (
                      agent, "Raw data is not allowed into boolean IOP %s",
                      name);
                    ret = 0;
                    break;
                case IGS_STRING_T: {
                    igsagent_warn (
                      agent, "Raw data is not allowed into string IOP %s",
                      name);
                    ret = 0;
                } break;
                case IGS_IMPULSION_T:
                    iop->value_size = 0;
                    break;
                case IGS_DATA_T: {
                    if (iop->value.data)
                        free (iop->value.data);
                    iop->value.data = NULL;
                    iop->value.data = (void *) zmalloc (size);
                    memcpy (iop->value.data, value, size);
                    out_size = iop->value_size = size;
                    out_value = iop->value.data;
                } break;
                default:
                    igsagent_error(agent, "%s has an invalid value type %d",
                                   name, iop->value_type);
                    ret = 0;
                    break;
            }
        } break;
        default:
            break;
    }

    if (ret) {
        // compose log entry
        const char *log_iop_type = NULL;
        switch (type) {
            case IGS_INPUT_T:
                log_iop_type = "input";
                break;
            case IGS_OUTPUT_T:
                log_iop_type = "output";
                break;
            case IGS_PARAMETER_T:
                log_iop_type = "parameter";
                break;
            default:
                break;
        }
        char log_iop_value_buffer[MAX_IOP_VALUE_LOG_BUFFER_LENGTH] = "";
        char *log_iop_value = NULL;
        switch (iop->value_type) {
            case IGS_IMPULSION_T:
                log_iop_value = strdup ("impulsion (no value)");
                break;
            case IGS_BOOL_T:
                snprintf (log_iop_value_buffer, MAX_IOP_VALUE_LOG_BUFFER_LENGTH,
                          "bool %d", iop->value.b);
                log_iop_value = strdup (log_iop_value_buffer);
                break;
            case IGS_INTEGER_T:
                snprintf (log_iop_value_buffer, MAX_IOP_VALUE_LOG_BUFFER_LENGTH,
                          "int %d", iop->value.i);
                log_iop_value = strdup (log_iop_value_buffer);
                break;
            case IGS_DOUBLE_T:
                snprintf (log_iop_value_buffer, MAX_IOP_VALUE_LOG_BUFFER_LENGTH,
                          "double %f", iop->value.d);
                log_iop_value = strdup (log_iop_value_buffer);
                break;
            case IGS_STRING_T:
                log_iop_value = zmalloc ((strlen (iop->value.s) + strlen ("string ") + 1) * sizeof (char));
                sprintf (log_iop_value, "string %s", iop->value.s);
                break;
            case IGS_DATA_T: {
                if (core_context->enable_data_logging) {
                    if (iop->value_size > 0) {
                        zchunk_t *chunk = zchunk_new (iop->value.data, iop->value_size);
                        char *hex_chunk = zchunk_strhex (chunk);
                        log_iop_value = zmalloc ((strlen (hex_chunk) + strlen ("data ") + 1) * sizeof (char));
                        sprintf (log_iop_value, "data %s", hex_chunk);
                        free (hex_chunk);
                        zchunk_destroy (&chunk);
                    }
                    else {
                        log_iop_value = (void *) zmalloc ((strlen ("data 00") + 1) * sizeof (char));
                        sprintf (log_iop_value, "data 00");
                    }
                }
                else {
                    snprintf (log_iop_value_buffer,
                              MAX_IOP_VALUE_LOG_BUFFER_LENGTH,
                              "data |size: %zu bytes", iop->value_size);
                    log_iop_value = strdup (log_iop_value_buffer);
                }
            } break;
            default:
                break;
        }
        igsagent_debug (agent, "set %s %s to %s", log_iop_type, name,
                        log_iop_value);
        free (log_iop_value);
        
        model_read_write_unlock (__FUNCTION__, __LINE__);
        // handle iop callbacks
        s_model_run_observe_callbacks_for_iop (agent, iop, out_value, out_size);
    }else
        model_read_write_unlock (__FUNCTION__, __LINE__);
    return iop;
}

igs_iop_t *s_model_find_input_by_name (igsagent_t *agent, const char *name)
{
    igs_iop_t *found = NULL;
    if (name && agent->definition)
        HASH_FIND_STR (agent->definition->inputs_table, name, found);
    else {
        if (name == NULL || strlen (name) == 0)
            igsagent_error (agent, "Input name cannot be NULL or empty");
        else
            igsagent_error (agent, "Definition is NULL");
    }
    return found;
}

igs_iop_t *s_model_find_output_by_name (igsagent_t *agent, const char *name)
{
    igs_iop_t *found = NULL;
    if (name && agent->definition)
        HASH_FIND_STR (agent->definition->outputs_table, name, found);
    else {
        if (name == NULL || strlen (name) == 0)
            igsagent_error (agent, "Output name cannot be NULL or empty");
        else
            igsagent_error (agent, "Definition is NULL");
    }
    return found;
}

igs_iop_t *s_model_find_parameter_by_name (igsagent_t *agent, const char *name)
{
    igs_iop_t *found = NULL;
    if (name && agent->definition)
        HASH_FIND_STR (agent->definition->params_table, name, found);
    else {
        if (name == NULL || strlen (name) == 0)
            igsagent_error (agent, "Parameter name cannot be NULL or empty");
        else
            igsagent_error (agent, "Definition is NULL");
    }
    return found;
}

igs_iop_t *model_find_iop_by_name (igsagent_t *agent,
                                   const char *name,
                                   igs_iop_type_t type)
{
    igs_iop_t *found = NULL;
    switch (type) {
        case IGS_INPUT_T:
            return s_model_find_input_by_name (agent, name);
        case IGS_OUTPUT_T:
            return s_model_find_output_by_name (agent, name);
        case IGS_PARAMETER_T:
            return s_model_find_parameter_by_name (agent, name);
        default:
            igsagent_error (agent, "Unknown IOP type %d", type);
            break;
    }
    return found;
}

void model_clear_iop (igsagent_t *agent, const char *name, igs_iop_type_t type)
{
    assert (agent);
    assert (name);
    igs_iop_t *iop = model_find_iop_by_name (agent, name, type);
    if (!iop)
        return;
    switch (iop->value_type) {
        case IGS_IMPULSION_T:
            break;
        case IGS_DATA_T:
            if (iop->value.data) {
                free (iop->value.data);
                iop->value.data = NULL;
                iop->value_size = 0;
            }
            break;
        case IGS_STRING_T:
            if (iop->value.s) {
                free (iop->value.s);
                iop->value.s = NULL;
                iop->value_size = 0;
            }
            break;
        case IGS_DOUBLE_T:
            iop->value.d = 0;
            break;
        case IGS_INTEGER_T:
            iop->value.i = 0;
            break;
        case IGS_BOOL_T:
            iop->value.b = false;
            break;
        case IGS_UNKNOWN_T:
            igsagent_error (agent, "%s cannot be reset", name);
            break;
        default:
            break;
    }
}

////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////

void igs_log_include_data (bool enable)
{
    core_init_context ();
    core_context->enable_data_logging = enable;
}

void igs_log_include_services (bool enable)
{
    core_init_context ();
    core_context->enable_service_logging = enable;
}

// --------------------------------  READ ------------------------------------//

void *
s_model_get_value_for (igsagent_t *agent, const char *name, igs_iop_type_t type)
{
    igs_iop_t *iop = model_find_iop_by_name (agent, name, type);
    if (iop == NULL) {
        igsagent_error (agent, "%s not found", name);
        return NULL;
    }
    switch (iop->value_type) {
        case IGS_INTEGER_T:
            return &iop->value.i;
        case IGS_DOUBLE_T:
            return &iop->value.d;
        case IGS_BOOL_T:
            return &iop->value.b;
        case IGS_STRING_T:
            return iop->value.s;
        case IGS_IMPULSION_T:
            return NULL;
        case IGS_DATA_T:
            return iop->value.data;
        default:
            igsagent_error (agent, "Unknown value type for %s", name);
            break;
    }
    return NULL;
}

igs_result_t s_read_iop (igsagent_t *agent,
                         const char *name,
                         igs_iop_type_t type,
                         void **value,
                         size_t *size)
{
    igs_iop_t *iop = model_find_iop_by_name (agent, name, type);
    if (iop == NULL) {
        igsagent_error (agent, "%s not found", name);
        return IGS_FAILURE;
    }
    if (iop->value_type == IGS_IMPULSION_T
        || (iop->value_type == IGS_STRING_T && iop->value.s == NULL)
        || (iop->value_type == IGS_DATA_T && iop->value.data == NULL)) {
        *value = NULL;
        *size = 0;
    }
    else {
        *value = (void *) zmalloc (iop->value_size);
        memcpy (*value, s_model_get_value_for (agent, name, type),
                iop->value_size);
        *size = iop->value_size;
    }
    return IGS_SUCCESS;
}

igs_result_t igsagent_input_buffer (igsagent_t *agent,
                                    const char *name,
                                    void **value,
                                    size_t *size)
{
    assert (agent);
    assert (name);
    return s_read_iop (agent, name, IGS_INPUT_T, value, size);
}

igs_result_t igsagent_output_buffer (igsagent_t *agent,
                                     const char *name,
                                     void **value,
                                     size_t *size)
{
    assert (agent);
    assert (name);
    return s_read_iop (agent, name, IGS_OUTPUT_T, value, size);
}

igs_result_t igsagent_parameter_buffer (igsagent_t *agent,
                                        const char *name,
                                        void **value,
                                        size_t *size)
{
    assert (agent);
    assert (name);
    return s_read_iop (agent, name, IGS_PARAMETER_T, value, size);
}

bool s_model_read_iop_as_bool (igsagent_t *agent,
                               const char *name,
                               igs_iop_type_t type)
{
    bool res = false;
    igs_iop_t *iop = model_find_iop_by_name (agent, name, type);
    if (iop == NULL) {
        igsagent_error (agent, "%s not found", name);
        return false;
    }
    switch (iop->value_type) {
        case IGS_BOOL_T:
            res = iop->value.b;
            return res;
        case IGS_INTEGER_T:
            igsagent_warn (
              agent, "Implicit conversion from int to bool for %s", name);
            res = (iop->value.i == 0) ? false : true;
            return res;
        case IGS_DOUBLE_T:
            igsagent_warn (
              agent, "Implicit conversion from double to bool for %s", name);
            res = (iop->value.d >= 0 && iop->value.d <= 0) ? false : true;
            return res;
        case IGS_STRING_T:
            if (streq (iop->value.s, "true")) {
                igsagent_warn (
                  agent, "Implicit conversion from string to bool for %s",
                  name);
                return true;
            }
            else
            if (streq (iop->value.s, "false")) {
                igsagent_warn (
                  agent, "Implicit conversion from string to bool for %s",
                  name);
                return false;
            }
            else {
                igsagent_warn (
                  agent,
                  "Implicit conversion from double to bool for %s (string "
                  "value is %s and false was returned)",
                  name, iop->value.s);
                return false;
            }
        default:
            igsagent_error (
              agent,
              "No implicit conversion possible for %s (false was returned)",
              name);
            return false;
    }
}

bool igsagent_input_bool (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    return s_model_read_iop_as_bool (agent, name, IGS_INPUT_T);
}

int s_model_read_iop_as_int (igsagent_t *agent,
                             const char *name,
                             igs_iop_type_t type)
{
    int res = 0;
    igs_iop_t *iop = model_find_iop_by_name (agent, name, type);
    if (iop == NULL) {
        igsagent_error (agent, "%s not found", name);
        return 0;
    }
    switch (iop->value_type) {
        case IGS_BOOL_T:
            igsagent_warn (
              agent, "Implicit conversion from bool to int for %s", name);
            res = (iop->value.b) ? 1 : 0;
            return res;
        case IGS_INTEGER_T:
            res = iop->value.i;
            return res;
        case IGS_DOUBLE_T:
            igsagent_warn (
              agent, "Implicit conversion from double to int for %s", name);
            if (iop->value.d < 0)
                res = (int) (iop->value.d - 0.5);
            else
                res = (int) (iop->value.d + 0.5);
            return res;
        case IGS_STRING_T:
            igsagent_warn (agent,
                            "Implicit conversion from string %s to int for %s",
                            iop->value.s, name);
            res = atoi (iop->value.s);
            return res;
        default:
            igsagent_error (
              agent, "No implicit conversion possible for %s (0 was returned)",
              name);
            return 0;
    }
}

int igsagent_input_int (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    return s_model_read_iop_as_int (agent, name, IGS_INPUT_T);
}

double s_model_read_iop_as_double (igsagent_t *agent,
                                   const char *name,
                                   igs_iop_type_t type)
{
    double res = 0;
    igs_iop_t *iop = model_find_iop_by_name (agent, name, type);
    if (iop == NULL) {
        igsagent_error (agent, "%s not found", name);
        return 0;
    }
    switch (iop->value_type) {
        case IGS_BOOL_T:
            igsagent_warn (
              agent, "Implicit conversion from bool to double for %s", name);
            res = (iop->value.b) ? 1 : 0;
            return res;
        case IGS_INTEGER_T:
            igsagent_warn (
              agent, "Implicit conversion from int to double for %s", name);
            res = iop->value.i;
            return res;
        case IGS_DOUBLE_T:
            res = iop->value.d;
            return res;
        case IGS_STRING_T:
            igsagent_warn (
              agent, "Implicit conversion from string %s to double for %s",
              iop->value.s, name);
            res = atof (iop->value.s);
            return res;
        default:
            igsagent_error (
              agent, "No implicit conversion possible for %s (0 was returned)",
              name);
            return 0;
    }
}

double igsagent_input_double (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    return s_model_read_iop_as_double (agent, name, IGS_INPUT_T);
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

char *s_model_read_iop_as_string (igsagent_t *agent,
                                  const char *name,
                                  igs_iop_type_t type)
{
    char *res = NULL;
    igs_iop_t *iop = model_find_iop_by_name (agent, name, type);
    if (iop == NULL) {
        igsagent_error (agent, "%s not found", name);
        return NULL;
    }
    switch (iop->value_type) {
        case IGS_STRING_T:
            res = strdup (iop->value.s);
            return res;
        case IGS_BOOL_T:
            igsagent_warn (
              agent, "Implicit conversion from bool to string for %s", name);
            res = iop->value.b ? strdup ("true") : strdup ("false");
            return res;
        case IGS_INTEGER_T:
            igsagent_warn (
              agent, "Implicit conversion from int to string for %s", name);
            res = s_model_int_to_string (iop->value.i);
            return res;
        case IGS_DOUBLE_T:
            igsagent_warn (
              agent, "Implicit conversion from double to string for %s", name);
            res = s_model_double_to_string (iop->value.d);
            return res;
        default:
            igsagent_error (
              agent,
              "No implicit conversion possible for %s (NULL was returned)",
              name);
            return NULL;
    }
}

char *igsagent_input_string (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    return s_model_read_iop_as_string (agent, name, IGS_INPUT_T);
}

igs_result_t s_model_read_iop_as_data (igsagent_t *agent,
                                       const char *name,
                                       igs_iop_type_t type,
                                       void **value,
                                       size_t *size)
{
    assert (agent);
    assert (value);
    assert (size);
    igs_iop_t *iop = model_find_iop_by_name (agent, name, type);
    if (iop == NULL) {
        igsagent_error (agent, "%s not found", name);
        *value = NULL;
        *size = 0;
        return IGS_FAILURE;
    }
    if (iop->value_type == IGS_IMPULSION_T || iop->value_type == IGS_UNKNOWN_T
        || (iop->value_type == IGS_DATA_T && iop->value.data == NULL)) {
        *value = NULL;
        *size = 0;
    }else{
        *size = iop->value_size;
        *value = (void *) zmalloc (iop->value_size);
        memcpy (*value, s_model_get_value_for (agent, name, type), *size);
    }
    return IGS_SUCCESS;
}

igs_result_t igsagent_input_data (igsagent_t *agent,
                                   const char *name,
                                   void **data,
                                   size_t *size)
{
    assert (agent);
    assert (name);
    return s_model_read_iop_as_data (agent, name, IGS_INPUT_T, data, size);
}

igs_result_t
igsagent_input_zmsg (igsagent_t *agent, const char *name, zmsg_t **msg)
{
    assert (agent);
    assert (name);
    void *data = NULL;
    size_t size = 0;
    igs_result_t ret =
      s_model_read_iop_as_data (agent, name, IGS_INPUT_T, &data, &size);
    zframe_t *frame = zframe_new (data, size);
    free (data);
    *msg = zmsg_decode (frame);
    zframe_destroy (&frame);
    return ret;
}

bool igsagent_output_bool (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    return s_model_read_iop_as_bool (agent, name, IGS_OUTPUT_T);
}

int igsagent_output_int (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    return s_model_read_iop_as_int (agent, name, IGS_OUTPUT_T);
}

double igsagent_output_double (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    return s_model_read_iop_as_double (agent, name, IGS_OUTPUT_T);
}

char *igsagent_output_string (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    return s_model_read_iop_as_string (agent, name, IGS_OUTPUT_T);
}

igs_result_t igsagent_output_data (igsagent_t *agent,
                                    const char *name,
                                    void **data,
                                    size_t *size)
{
    assert (agent);
    assert (name);
    return s_model_read_iop_as_data (agent, name, IGS_OUTPUT_T, data, size);
}

bool igsagent_parameter_bool (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    return s_model_read_iop_as_bool (agent, name, IGS_PARAMETER_T);
}

int igsagent_parameter_int (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    return s_model_read_iop_as_int (agent, name, IGS_PARAMETER_T);
}

double igsagent_parameter_double (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    return s_model_read_iop_as_double (agent, name, IGS_PARAMETER_T);
}

char *igsagent_parameter_string (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    return s_model_read_iop_as_string (agent, name, IGS_PARAMETER_T);
}

igs_result_t igsagent_parameter_data (igsagent_t *agent,
                                       const char *name,
                                       void **data,
                                       size_t *size)
{
    assert (agent);
    assert (name);
    return s_model_read_iop_as_data (agent, name, IGS_PARAMETER_T, data, size);
}

// --------------------------------  WRITE
// ------------------------------------//

igs_result_t
igsagent_input_set_bool (igsagent_t *agent, const char *name, bool value)
{
    assert (agent);
    assert (name);
    const igs_iop_t *iop = model_write_iop (agent, name, IGS_INPUT_T,
                                            IGS_BOOL_T, &value, sizeof (bool));
    return (iop == NULL) ? IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t
igsagent_input_set_int (igsagent_t *agent, const char *name, int value)
{
    assert (agent);
    assert (name);
    const igs_iop_t *iop = model_write_iop (agent, name, IGS_INPUT_T, IGS_INTEGER_T, &value, sizeof (int));
    return (iop == NULL) ? IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t
igsagent_input_set_double (igsagent_t *agent, const char *name, double value)
{
    assert (agent);
    assert (name);
    const igs_iop_t *iop = model_write_iop (agent, name, IGS_INPUT_T, IGS_DOUBLE_T, &value, sizeof (double));
    return (iop == NULL) ? IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsagent_input_set_string (igsagent_t *agent,
                                         const char *name,
                                         const char *value)
{
    assert (agent);
    assert (name);
    size_t value_length = (value == NULL) ? 0 : strlen (value) + 1;
    const igs_iop_t *iop = model_write_iop (agent, name, IGS_INPUT_T, IGS_STRING_T, (char *) value, value_length);
    return (iop == NULL) ? IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsagent_input_set_impulsion (igsagent_t *agent,
                                            const char *name)
{
    assert (agent);
    assert (name);
    const igs_iop_t *iop = model_write_iop (agent, name, IGS_INPUT_T, IGS_IMPULSION_T, NULL, 0);
    return (iop == NULL) ? IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsagent_input_set_data (igsagent_t *agent,
                                       const char *name,
                                       void *value,
                                       size_t size)
{
    assert (agent);
    assert (name);
    const igs_iop_t *iop = model_write_iop (agent, name, IGS_INPUT_T, IGS_DATA_T, value, size);
    return (iop == NULL) ? IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t
igsagent_output_set_bool (igsagent_t *agent, const char *name, bool value)
{
    assert (agent);
    assert (name);
    const igs_iop_t *iop = model_write_iop (agent, name, IGS_OUTPUT_T,
                                            IGS_BOOL_T, &value, sizeof (bool));
    if (iop)
        network_publish_output (agent, iop);
    return (iop == NULL) ? IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t
igsagent_output_set_int (igsagent_t *agent, const char *name, int value)
{
    assert (agent);
    assert (name);
    const igs_iop_t *iop = model_write_iop (agent, name, IGS_OUTPUT_T,
                                            IGS_INTEGER_T, &value, sizeof (int));
    if (iop)
        network_publish_output (agent, iop);
    return (iop == NULL) ? IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t
igsagent_output_set_double (igsagent_t *agent, const char *name, double value)
{
    assert (agent);
    assert (name);
    const igs_iop_t *iop = model_write_iop (agent, name, IGS_OUTPUT_T,
                                            IGS_DOUBLE_T, &value, sizeof (double));
    if (iop)
        network_publish_output (agent, iop);
    return (iop == NULL) ? IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsagent_output_set_string (igsagent_t *agent,
                                          const char *name,
                                          const char *value)
{
    assert (agent);
    assert (name);
    size_t length = (value == NULL) ? 0 : strlen (value) + 1;
    const igs_iop_t *iop = model_write_iop (agent, name, IGS_OUTPUT_T,
                                            IGS_STRING_T, (char *) value, length);
    if (iop)
        network_publish_output (agent, iop);
    return (iop == NULL) ? IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsagent_output_set_impulsion (igsagent_t *agent,
                                             const char *name)
{
    assert (agent);
    assert (name);
    const igs_iop_t *iop = model_write_iop (agent, name, IGS_OUTPUT_T, IGS_IMPULSION_T, NULL, 0);
    if (iop)
        network_publish_output (agent, iop);
    return (iop == NULL) ? IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsagent_output_set_data (igsagent_t *agent,
                                        const char *name,
                                        void *value,
                                        size_t size)
{
    assert (agent);
    assert (name);
    const igs_iop_t *iop = model_write_iop (agent, name, IGS_OUTPUT_T, IGS_DATA_T, value, size);
    if (iop)
        network_publish_output (agent, iop);
    return (iop == NULL) ? IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t
igsagent_output_set_zmsg (igsagent_t *agent, const char *name, zmsg_t *msg)
{
    assert (agent);
    assert (name);
    assert (msg);
    zframe_t *frame = zmsg_encode (msg);
    void *value = zframe_data (frame);
    size_t size = zframe_size (frame);
    const igs_iop_t *iop = model_write_iop (agent, name, IGS_OUTPUT_T, IGS_DATA_T, value, size);
    if (iop)
        network_publish_output (agent, iop);
    zframe_destroy (&frame);
    return (iop == NULL) ? IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t
igsagent_parameter_set_bool (igsagent_t *agent, const char *name, bool value)
{
    assert (agent);
    assert (name);
    const igs_iop_t *iop = model_write_iop (agent, name, IGS_PARAMETER_T,
                                            IGS_BOOL_T, &value, sizeof (bool));
    return (iop == NULL) ? IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t
igsagent_parameter_set_int (igsagent_t *agent, const char *name, int value)
{
    assert (agent);
    assert (name);
    const igs_iop_t *iop = model_write_iop (
      agent, name, IGS_PARAMETER_T, IGS_INTEGER_T, &value, sizeof (int));
    return (iop == NULL) ? IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsagent_parameter_set_double (igsagent_t *agent,
                                             const char *name,
                                             double value)
{
    assert (agent);
    assert (name);
    const igs_iop_t *iop = model_write_iop (
      agent, name, IGS_PARAMETER_T, IGS_DOUBLE_T, &value, sizeof (double));
    return (iop == NULL) ? IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsagent_parameter_set_string (igsagent_t *agent,
                                             const char *name,
                                             const char *value)
{
    assert (agent);
    assert (name);
    size_t value_length = (value == NULL) ? 0 : strlen (value) + 1;
    const igs_iop_t *iop = model_write_iop (
      agent, name, IGS_PARAMETER_T, IGS_STRING_T, (char *) value, value_length);
    return (iop == NULL) ? IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsagent_parameter_set_data (igsagent_t *agent,
                                           const char *name,
                                           void *value,
                                           size_t size)
{
    assert (agent);
    assert (name);
    const igs_iop_t *iop =
      model_write_iop (agent, name, IGS_PARAMETER_T, IGS_DATA_T, value, size);
    return (iop == NULL) ? IGS_FAILURE : IGS_SUCCESS;
}

igs_constraint_t* s_model_parse_constraint(igs_iop_value_type_t type,
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
        //FIXME: apply verifications on values to check that min <= max
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

igs_result_t s_model_add_constraint (igsagent_t *self, igs_iop_type_t type,
                                     const char *name,
                                     const char *constraint)
{
    assert(self);
    assert(name);
    assert(constraint);
    igs_iop_t *iop = NULL;
    if (type == IGS_INPUT_T) {
        HASH_FIND_STR (self->definition->inputs_table, name, iop);
        if (!iop) {
            igsagent_error (self, "Input %s cannot be found", name);
            return IGS_FAILURE;
        }
    }
    else
    if (type == IGS_OUTPUT_T) {
        HASH_FIND_STR (self->definition->outputs_table, name, iop);
        if (!iop) {
            igsagent_error (self, "Output %s cannot be found", name);
            return IGS_FAILURE;
        }
    }
    else
    if (type == IGS_PARAMETER_T) {
        HASH_FIND_STR (self->definition->params_table, name, iop);
        if (!iop) {
            igsagent_error (self, "Parameter %s cannot be found", name);
            return IGS_FAILURE;
        }
    }
    else {
        igsagent_error (self, "Unknown IOP type %d", type);
        return IGS_FAILURE;
    }
    if (iop->constraint){
        igsagent_warn (self, "%s already has a constraint that will be removed", name);
        definition_free_constraint(&iop->constraint);
    }
    char *error = NULL;
    iop->constraint = s_model_parse_constraint(iop->value_type, constraint, &error);
    if (!iop->constraint){
        if (error){
            igsagent_error (self, "%s", error);
            free(error);
        }
        return IGS_FAILURE;
    }
    return IGS_SUCCESS;
}

void s_model_set_description(igsagent_t *self, igs_iop_type_t type,
                             const char *name,
                             const char *description)
{
    assert(self);
    assert(name);
    assert(description);
    igs_iop_t *iop = NULL;
    if (type == IGS_INPUT_T) {
        HASH_FIND_STR (self->definition->inputs_table, name, iop);
        if (!iop) {
            igsagent_error (self, "Input %s cannot be found", name);
            return;
        }
    }
    else
    if (type == IGS_OUTPUT_T) {
        HASH_FIND_STR (self->definition->outputs_table, name, iop);
        if (!iop) {
            igsagent_error (self, "Output %s cannot be found", name);
            return;
        }
    }
    else
    if (type == IGS_PARAMETER_T) {
        HASH_FIND_STR (self->definition->params_table, name, iop);
        if (!iop) {
            igsagent_error (self, "Parameter %s cannot be found", name);
            return;
        }
    }
    else {
        igsagent_error (self, "Unknown IOP type %d", type);
        return;
    }
    if (iop->description)
        free(iop->description);
    iop->description = s_strndup(description, IGS_MAX_LOG_LENGTH);
}

void igsagent_constraints_enforce(igsagent_t *self, bool enforce)
{
    self->enforce_constraints = enforce;
}

igs_result_t igsagent_input_add_constraint (igsagent_t *self, const char *name,
                                            const char *constraint)
{
    return s_model_add_constraint(self, IGS_INPUT_T, name, constraint);
}

igs_result_t igsagent_output_add_constraint (igsagent_t *self, const char *name,
                                             const char *constraint)
{
    return s_model_add_constraint(self, IGS_OUTPUT_T, name, constraint);
}

igs_result_t igsagent_parameter_add_constraint (igsagent_t *self, const char *name,
                                                const char *constraint)
{
    return s_model_add_constraint(self, IGS_PARAMETER_T, name, constraint);
}

void igsagent_input_set_description(igsagent_t *self, const char *name, const char *description)
{
    s_model_set_description(self, IGS_INPUT_T, name, description);
}

void igsagent_output_set_description(igsagent_t *self, const char *name, const char *description)
{
    s_model_set_description(self, IGS_OUTPUT_T, name, description);
}

void igsagent_parameter_set_description(igsagent_t *self, const char *name, const char *description)
{
    s_model_set_description(self, IGS_PARAMETER_T, name, description);
}

void igsagent_clear_input (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    model_clear_iop (agent, name, IGS_INPUT_T);
}

void igsagent_clear_output (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    model_clear_iop (agent, name, IGS_OUTPUT_T);
}

void igsagent_clear_parameter (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    model_clear_iop (agent, name, IGS_PARAMETER_T);
}

// --------------------------------  INTROSPECTION
// ------------------------------------//

igs_iop_value_type_t s_model_get_type_for_iop (igsagent_t *agent,
                                               const char *name,
                                               igs_iop_type_t type)
{
    assert (agent && agent->definition);
    assert (name && strlen (name) > 0);
    igs_iop_t *iop = NULL;
    if (type == IGS_INPUT_T) {
        HASH_FIND_STR (agent->definition->inputs_table, name, iop);
        if (iop == NULL) {
            igsagent_error (agent, "Input %s cannot be found", name);
            return 0;
        }
    }
    else
    if (type == IGS_OUTPUT_T) {
        HASH_FIND_STR (agent->definition->outputs_table, name, iop);
        if (iop == NULL) {
            igsagent_error (agent, "Output %s cannot be found", name);
            return 0;
        }
    }
    else
    if (type == IGS_PARAMETER_T) {
        HASH_FIND_STR (agent->definition->params_table, name, iop);
        if (iop == NULL) {
            igsagent_error (agent, "Parameter %s cannot be found", name);
            return 0;
        }
    }
    else {
        igsagent_error (agent, "Unknown IOP type %d", type);
        return 0;
    }
    return iop->value_type;
}

igs_iop_value_type_t igsagent_input_type (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    if ((name == NULL) || (strlen (name) == 0)) {
        igsagent_error (agent, "Input name cannot be NULL or empty");
        return IGS_UNKNOWN_T;
    }
    return s_model_get_type_for_iop (agent, name, IGS_INPUT_T);
}

igs_iop_value_type_t igsagent_output_type (igsagent_t *agent,
                                            const char *name)
{
    assert (agent);
    assert (name);
    if ((name == NULL) || (strlen (name) == 0)) {
        igsagent_error (agent, "Output name cannot be NULL or empty");
        return IGS_UNKNOWN_T;
    }
    return s_model_get_type_for_iop (agent, name, IGS_OUTPUT_T);
}

igs_iop_value_type_t igsagent_parameter_type (igsagent_t *agent,
                                               const char *name)
{
    assert (agent);
    assert (name);
    if ((name == NULL) || (strlen (name) == 0)) {
        igsagent_error (agent, "Parameter name cannot be NULL or empty");
        return IGS_UNKNOWN_T;
    }
    return s_model_get_type_for_iop (agent, name, IGS_PARAMETER_T);
}

size_t igsagent_input_count (igsagent_t *agent)
{
    assert (agent);
    if (agent->definition == NULL) {
        igsagent_warn (agent, "definition is NULL");
        return 0;
    }
    return HASH_COUNT (agent->definition->inputs_table);
}

size_t igsagent_output_count (igsagent_t *agent)
{
    assert (agent);
    if (agent->definition == NULL) {
        igsagent_warn (agent, "definition is NULL");
        return 0;
    }
    return HASH_COUNT (agent->definition->outputs_table);
}

size_t igsagent_parameter_count (igsagent_t *agent)
{
    assert (agent);
    if (agent->definition == NULL) {
        igsagent_warn (agent, "definition is NULL");
        return 0;
    }
    return HASH_COUNT (agent->definition->params_table);
}

char **s_model_get_iop_list (igsagent_t *agent,
                             size_t *nb_of_elements,
                             igs_iop_type_t type)
{
    if (agent->definition == NULL) {
        igsagent_warn (agent, "Definition is NULL");
        *nb_of_elements = 0;
        return NULL;
    }
    igs_iop_t *hash = NULL;
    switch (type) {
        case IGS_INPUT_T:
            hash = agent->definition->inputs_table;
            break;
        case IGS_OUTPUT_T:
            hash = agent->definition->outputs_table;
            break;
        case IGS_PARAMETER_T:
            hash = agent->definition->params_table;
            break;
        default:
            break;
    }
    size_t N = (*nb_of_elements) = HASH_COUNT (hash);
    if (N < 1)
        return NULL;

    char **list = (char **) malloc (N * sizeof (char *));
    igs_iop_t *current_iop;
    int index = 0;
    for (current_iop = hash; current_iop;
         current_iop = current_iop->hh.next) {
        list[index] = strdup (current_iop->name);
        index++;
    }
    return list;
}

char **igsagent_input_list (igsagent_t *agent, size_t *nb_of_elements)
{
    assert (agent);
    return s_model_get_iop_list (agent, nb_of_elements, IGS_INPUT_T);
}

char **igsagent_output_list (igsagent_t *agent, size_t *nb_of_elements)
{
    assert (agent);
    return s_model_get_iop_list (agent, nb_of_elements, IGS_OUTPUT_T);
}

char **igsagent_parameter_list (igsagent_t *agent, size_t *nb_of_elements)
{
    assert (agent);
    return s_model_get_iop_list (agent, nb_of_elements, IGS_PARAMETER_T);
}

void igs_free_iop_list (char **list, size_t nb_of_elements)
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

bool s_model_check_iop_existence (igsagent_t *agent,
                                  const char *name,
                                  igs_iop_t *hash)
{
    igs_iop_t *iop = NULL;
    if (agent->definition == NULL) {
        igsagent_error (agent, "Definition is NULL");
        return false;
    }
    HASH_FIND_STR (hash, name, iop);
    return (iop);
}

bool igsagent_input_exists (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    if (agent->definition == NULL)
        return false;
    return s_model_check_iop_existence (agent, name,
                                        agent->definition->inputs_table);
}

bool igsagent_output_exists (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    if (agent->definition == NULL)
        return false;
    return s_model_check_iop_existence (agent, name,
                                        agent->definition->outputs_table);
}

bool igsagent_parameter_exists (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name);
    if (agent->definition == NULL)
        return false;
    return s_model_check_iop_existence (agent, name,
                                        agent->definition->params_table);
}

// --------------------------------  OBSERVE
// ------------------------------------//

static void s_model_observe (igsagent_t *agent,
                             const char *name,
                             igs_iop_type_t value_type,
                             igsagent_iop_fn cb,
                             void *my_data)
{
    igs_iop_t *iop = model_find_iop_by_name (agent, name, value_type);
    // Check if the input has been returned.
    if (iop == NULL) {
        const char *t = NULL;
        switch (value_type) {
            case IGS_INPUT_T:
                t = "input";
                break;
            case IGS_OUTPUT_T:
                t = "output";
                break;
            case IGS_PARAMETER_T:
                t = "parameter";
                break;
            default:
                break;
        }
        igsagent_error (agent, "Cannot find %s %s", t, name);
        return;
    }
    igs_observe_wrapper_t *new_callback =
      (igs_observe_wrapper_t *) zmalloc (sizeof (igs_observe_wrapper_t));
    new_callback->callback_ptr = cb;
    new_callback->data = my_data;
    DL_APPEND (iop->callbacks, new_callback);
    // igsagent_debug(agent, "observe iop with name %s and type %d\n", name,
    // value_type);
}

void igsagent_observe_input (igsagent_t *agent,
                              const char *name,
                              igsagent_iop_fn cb,
                              void *my_data)
{
    assert (agent);
    assert (name);
    assert (cb);
    s_model_observe (agent, name, IGS_INPUT_T, cb, my_data);
}

void igsagent_observe_output (igsagent_t *agent,
                               const char *name,
                               igsagent_iop_fn cb,
                               void *my_data)
{
    assert (agent);
    assert (name);
    assert (cb);
    s_model_observe (agent, name, IGS_OUTPUT_T, cb, my_data);
}

void igsagent_observe_parameter (igsagent_t *agent,
                                  const char *name,
                                  igsagent_iop_fn cb,
                                  void *my_data)
{
    assert (agent);
    assert (name);
    assert (cb);
    s_model_observe (agent, name, IGS_PARAMETER_T, cb, my_data);
}

// --------------------------------  MUTE ------------------------------------//

void igsagent_output_mute (igsagent_t *agent, const char *name)
{
    igs_iop_t *iop = model_find_iop_by_name (agent, name, IGS_OUTPUT_T);
    if (iop == NULL || iop->type != IGS_OUTPUT_T) {
        igsagent_error (agent, "Output '%s' not found", name);
        return;
    }
    iop->is_muted = true;
    if (core_context && core_context->node) {
        s_lock_zyre_peer (__FUNCTION__, __LINE__);
        zmsg_t *msg = zmsg_new ();
        zmsg_addstr (msg, OUTPUT_MUTED_MSG);
        zmsg_addstr (msg, name);
        zmsg_addstr (msg, agent->uuid);
        zyre_shout (core_context->node, IGS_PRIVATE_CHANNEL, &msg);
        s_unlock_zyre_peer (__FUNCTION__, __LINE__);
    }
}

void igsagent_output_unmute (igsagent_t *agent, const char *name)
{
    igs_iop_t *iop = model_find_iop_by_name (agent, name, IGS_OUTPUT_T);
    if (iop == NULL || iop->type != IGS_OUTPUT_T) {
        igsagent_error (agent, "Output '%s' not found", name);
        return;
    }
    iop->is_muted = false;
    if (core_context && core_context->node) {
        s_lock_zyre_peer (__FUNCTION__, __LINE__);
        zmsg_t *msg = zmsg_new ();
        zmsg_addstr (msg, OUTPUT_UNMUTED_MSG);
        zmsg_addstr (msg, name);
        zmsg_addstr (msg, agent->uuid);
        zyre_shout (core_context->node, IGS_PRIVATE_CHANNEL, &msg);
        s_unlock_zyre_peer (__FUNCTION__, __LINE__);
    }
}

bool igsagent_output_is_muted (igsagent_t *agent, const char *name)
{
    igs_iop_t *iop = model_find_iop_by_name (agent, name, IGS_OUTPUT_T);
    if (iop == NULL || iop->type != IGS_OUTPUT_T) {
        igsagent_warn (agent, "Output '%s' not found", name);
        return 0;
    }
    return iop->is_muted;
}
