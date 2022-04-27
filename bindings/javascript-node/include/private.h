/*  =========================================================================
    private - private header defining private classes and functions

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include <node_api.h>
#include <ingescape/ingescape.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "./uthash/utlist.h"
#include "./uthash/uthash.h"


// Trigger 'uncaughtException' in JavaScript
void trigger_exception(napi_env env, const char *code, const char *message);

// Convert N-API to C
char * convert_napi_to_string(napi_env env, napi_value value); //must be free by caller
int convert_napi_to_bool(napi_env env, napi_value value, bool *converted_value);
int convert_napi_to_int(napi_env env, napi_value value, int *converted_value);
int convert_napi_to_uint64(napi_env env, napi_value value, uint64_t *convertedValue);
int convert_napi_to_double(napi_env env, napi_value value, double *converted_value);
int convert_napi_to_data(napi_env env, napi_value value, void **converted_value, size_t *size_converted_value);

// Convert C to N-API
int convert_int_to_napi(napi_env env, int value, napi_value *converted_value);
int convert_uint64_to_napi(napi_env env, uint64_t value, napi_value *converted_value);
int convert_bool_to_napi(napi_env env, bool value, napi_value *converted_value);
int convert_string_to_napi(napi_env env, const char * value, napi_value *converted_value);
int convert_double_to_napi(napi_env env, double value, napi_value *converted_value);
int convert_null_to_napi(napi_env env, napi_value *converted_value);
int convert_data_to_napi(napi_env env, void *value, size_t size, napi_value *converted_value);
int convert_string_list_to_napi_array(napi_env env, char **list, size_t length, napi_value *converted_value);

// IGS utils
int convert_value_IOP_into_napi(napi_env env, igs_iop_value_type_t type, void *value, size_t size, napi_value *value_napi);
void * convert_value_with_good_type(napi_env env, napi_value value, igs_iop_value_type_t type, size_t *size_convert);
void service_args_c_from_js(napi_env env, napi_value array, igs_service_arg_t **first_argument);
void service_args_js_from_c(napi_env env, igs_service_arg_t *first_argument, napi_value *arrayJS);

// N-API utils
int get_function_arguments(napi_env env, napi_callback_info info, size_t argc, napi_value *argv);
napi_value enable_callback_into_js(napi_env env, napi_callback cb, const char * js_name, napi_value exports);

// Store data associated to ingescape javascript callbacks 
#ifndef CALLBACK_IGS
#define CALLBACK_IGS
typedef struct threadsafe_context {
    napi_threadsafe_function threadsafe_func;
    napi_ref my_data_ref;
    napi_ref this_ref; // only for igsagent
    int cnt;
    bool delete_after_use;
    struct threadsafe_context *prev, *next;
} threadsafe_context_t;

typedef struct threadsafe_context_hash {
    char *key;
    threadsafe_context_t *list;
    UT_hash_handle hh;
} threadsafe_context_hash_t;

void free_threadsafe_context (napi_env env, threadsafe_context_t **threadsafe_context);
void free_threadsafe_context_hash (napi_env env, threadsafe_context_hash_t **threadsafe_context_hash);

threadsafe_context_hash_t *observed_input_contexts;
threadsafe_context_hash_t *observed_output_contexts;
threadsafe_context_hash_t *observed_parameter_contexts;
threadsafe_context_hash_t *service_contexts;
threadsafe_context_t *observe_mute_contexts;
threadsafe_context_t *observe_agent_events_contexts;
threadsafe_context_t *observe_freeze_contexts;
threadsafe_context_t *observe_forced_stop_contexts;
threadsafe_context_t *observe_monitor_contexts;
threadsafe_context_t *timer_contexts;

typedef struct mute_callback_args {
	bool agent_is_muted;
    napi_ref my_data_ref;
} mute_callback_args_t;

typedef struct igsagent_mute_callback_args {
	bool agent_is_muted;
    napi_ref my_data_ref;
    napi_ref this_ref;
} igsagent_mute_callback_args_t;

typedef struct freeze_callback_args {
	bool is_paused;
    napi_ref my_data_ref;
} freeze_callback_args_t;

typedef struct igsagent_iop_callback_args {
	igs_iop_type_t iop_type;
	char* name;
	igs_iop_value_type_t value_type;
	void* value;
	size_t value_size;
    napi_ref my_data_ref;
    napi_ref this_ref;
} igsagent_iop_callback_args_t;

typedef struct iop_callback_args {
	igs_iop_type_t iop_type;
	char* name;
	igs_iop_value_type_t value_type;
	void* value;
	size_t value_size;
    napi_ref my_data_ref;
} iop_callback_args_t;

typedef struct service_callback_args {
    char *sender_agent_name;
    char *sender_agent_uuid;
    char *service_name;
    igs_service_arg_t *first_argument;
    size_t args_nb;
    char *token;
    napi_ref my_data_ref;
} service_callback_args_t;

typedef struct igsagent_service_callback_args {
    char *sender_agent_name;
    char *sender_agent_uuid;
    char *service_name;
    igs_service_arg_t *first_argument;
    size_t args_nb;
    char *token;
    napi_ref my_data_ref;
    napi_ref this_ref;
} igsagent_service_callback_args_t;

typedef struct monitor_callback_args {
    igs_monitor_event_t event;
    char *device;
    char *ip_address;
    napi_ref my_data_ref;
} monitor_callback_args_t;

typedef struct agent_events_callback_args {
    igs_agent_event_t event;
    char *uuid;
    char *name;
    void *event_data;
    napi_ref my_data_ref;
} agent_events_callback_args_t;

typedef struct igsagent_agent_events_callback_args {
    igs_agent_event_t event;
    char *uuid;
    char *name;
    void *event_data;
    napi_ref my_data_ref;
    napi_ref this_ref;
} igsagent_agent_events_callback_args_t;

typedef struct timer_callback_args {
    int timer_id;
    napi_ref my_data_ref;
} timer_callback_args_t;

typedef struct igsagent_observe_callback {
    bool is_activated;
    napi_ref my_data_ref;
    napi_ref this_ref;
    threadsafe_context_t* threadsafe_context;
} igsagent_observe_callback_t;
#endif

// Allow calling into the add-on native code from javascript
napi_value init_agent(napi_env env, napi_value exports);
napi_value init_admin_config_utils(napi_env env, napi_value exports);
napi_value init_advanced(napi_env env, napi_value exports);
napi_value init_definition(napi_env env, napi_value exports);
napi_value init_enum(napi_env env, napi_value exports);
napi_value init_init_control(napi_env env, napi_value exports);
napi_value init_iop(napi_env env, napi_value exports);
napi_value init_mapping(napi_env env, napi_value exports);
napi_value init_service(napi_env env, napi_value exports);
