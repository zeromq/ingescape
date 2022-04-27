/*  =========================================================================
    agent - igsagent class

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "../include/private.h"
#include <ingescape/igsagent.h>

threadsafe_context_hash_t *igsagent_observed_input_contexts = NULL;
threadsafe_context_hash_t *igsagent_observed_output_contexts = NULL;
threadsafe_context_hash_t *igsagent_observed_parameter_contexts = NULL;
threadsafe_context_hash_t *igsagent_service_contexts = NULL;
threadsafe_context_t *igsagent_observe_contexts = NULL;
threadsafe_context_t *igsagent_observe_mute_contexts = NULL;
threadsafe_context_t *igsagent_observe_agent_events_contexts = NULL;

napi_value get_function_this_argument(napi_env env, napi_callback_info info) {
    napi_status status;
    napi_value this;
    status = napi_get_cb_info(env, info, NULL, NULL, &this, NULL);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get 'this' argument .");
    return this;
}

igsagent_t * unwrap_native_agent(napi_env env, napi_value this_arg) {
    napi_status status;
    void *agent = NULL;
    status = napi_unwrap(env, this_arg, &agent);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to unwrap igsagent_t from 'this' argument.");
    return (igsagent_t *) agent;
}

igsagent_t * get_igsagent_from_this_ref(napi_env env, napi_ref this_ref) {
    napi_value this;
    napi_status status = napi_get_reference_value(env, this_ref, &this);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Unable to get reference value");
    igsagent_t *concerned_agent = unwrap_native_agent(env, this);
    return concerned_agent;
}

napi_value destroy_agent_context(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);
    igsagent_t *agent = unwrap_native_agent(env, this);
    // Deactivate our agent now so that igsagent_observe callbacks will be executed
    if (igsagent_is_activated(agent))
        igsagent_deactivate(agent);

    threadsafe_context_hash_t *threadsafe_context_hash, *threadsafe_context_hash_tmp;
    threadsafe_context_t *threadsafe_context, *threadsafe_context_tmp;
    igsagent_t *concerned_agent = NULL;
    HASH_ITER (hh, igsagent_observed_input_contexts, threadsafe_context_hash, threadsafe_context_hash_tmp) {
        LL_FOREACH_SAFE (threadsafe_context_hash->list, threadsafe_context, threadsafe_context_tmp) {
            concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
            if (concerned_agent == agent) {
                LL_DELETE (threadsafe_context_hash->list, threadsafe_context);
                free_threadsafe_context(env, &threadsafe_context);
            }
        }
        if (threadsafe_context_hash->list == NULL) {
            HASH_DEL (igsagent_observed_input_contexts, threadsafe_context_hash);
            free_threadsafe_context_hash(env, &threadsafe_context_hash);
        }
    }
    HASH_ITER (hh, igsagent_observed_output_contexts, threadsafe_context_hash, threadsafe_context_hash_tmp) {
        LL_FOREACH_SAFE (threadsafe_context_hash->list, threadsafe_context, threadsafe_context_tmp) {
            concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
            if (concerned_agent == agent) {
                LL_DELETE (threadsafe_context_hash->list, threadsafe_context);
                free_threadsafe_context(env, &threadsafe_context);
            }
        }
        if (threadsafe_context_hash->list == NULL) {
            HASH_DEL (igsagent_observed_output_contexts, threadsafe_context_hash);
            free_threadsafe_context_hash(env, &threadsafe_context_hash);
        }
    }
    HASH_ITER (hh, igsagent_observed_parameter_contexts, threadsafe_context_hash, threadsafe_context_hash_tmp) {
        LL_FOREACH_SAFE (threadsafe_context_hash->list, threadsafe_context, threadsafe_context_tmp) {
            concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
            if (concerned_agent == agent) {
                LL_DELETE (threadsafe_context_hash->list, threadsafe_context);
                free_threadsafe_context(env, &threadsafe_context);
            }
        }
        if (threadsafe_context_hash->list == NULL) {
            HASH_DEL (igsagent_observed_parameter_contexts, threadsafe_context_hash);
            free_threadsafe_context_hash(env, &threadsafe_context_hash);
        }
    }
    HASH_ITER (hh, igsagent_service_contexts, threadsafe_context_hash, threadsafe_context_hash_tmp) {
        LL_FOREACH_SAFE (threadsafe_context_hash->list, threadsafe_context, threadsafe_context_tmp) {
            concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
            if (concerned_agent == agent) {
                LL_DELETE (threadsafe_context_hash->list, threadsafe_context);
                free_threadsafe_context(env, &threadsafe_context);
            }
        }
        if (threadsafe_context_hash->list == NULL) {
            HASH_DEL (igsagent_service_contexts, threadsafe_context_hash);
            free_threadsafe_context_hash(env, &threadsafe_context_hash);
        }
    }

    LL_FOREACH_SAFE (igsagent_observe_contexts, threadsafe_context, threadsafe_context_tmp) {
        concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
        if (concerned_agent == agent) {
            if (threadsafe_context->cnt == 0) {
                LL_DELETE (igsagent_observe_contexts, threadsafe_context);
                free_threadsafe_context(env, &threadsafe_context);
            }
            else
                threadsafe_context->delete_after_use = true;
        }
    }

    LL_FOREACH_SAFE (igsagent_observe_mute_contexts, threadsafe_context, threadsafe_context_tmp) {
        concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
        if (concerned_agent == agent) {
            LL_DELETE (igsagent_observe_mute_contexts, threadsafe_context);
            free_threadsafe_context(env, &threadsafe_context);
        }
    }
    LL_FOREACH_SAFE (igsagent_observe_agent_events_contexts, threadsafe_context, threadsafe_context_tmp) {
        concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
        if (concerned_agent == agent) {
            LL_DELETE (igsagent_observe_agent_events_contexts, threadsafe_context);
            free_threadsafe_context(env, &threadsafe_context);
        }
    }
    return NULL;
}

void node_igsagent_destroy(napi_env env, void *finalize_data, void *finalize_hint) {
    igsagent_t *agent = (igsagent_t *) finalize_hint;
    igsagent_destroy(&agent);
}

napi_value node_igsagent_new(napi_env env, napi_callback_info info) {
    napi_status status;
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    bool activate_immediately;
    char *name = convert_napi_to_string(env, argv[0]);
    convert_napi_to_bool(env, argv[1], &activate_immediately);
    igsagent_t *agent = igsagent_new(name, activate_immediately);

    // Wrap agent created to retrieve native agent pointer later
    status = napi_wrap(env, this, agent, node_igsagent_destroy, agent, NULL);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to wrap igsagent_t in 'this' argument.");
    return this;
}

napi_value node_igsagent_activate(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);;
    igsagent_activate(unwrap_native_agent(env, this));
    return NULL;
}

napi_value node_igsagent_deactivate(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);;
    igsagent_deactivate(unwrap_native_agent(env, this));
    return NULL;
}

napi_value node_igsagent_is_activated(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);;
    bool is_activated = igsagent_is_activated(unwrap_native_agent(env, this));
    napi_value is_activated_js;
    convert_bool_to_napi(env, is_activated, &is_activated_js);
    return is_activated_js;
}

static void cb_igsagent_observe_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    igsagent_observe_callback_t * callback_arg = (igsagent_observe_callback_t *) data;
    napi_value argv[3];
    status = napi_get_reference_value(env, callback_arg->this_ref, &argv[0]);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get reference value.");
    convert_bool_to_napi(env, callback_arg->is_activated, &argv[1]);
    if (callback_arg->my_data_ref == NULL)
        convert_null_to_napi(env, &argv[2]);
    else {
        status = napi_get_reference_value(env, callback_arg->my_data_ref, &argv[2]);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to get reference value.");
    }

    // Since a function call must have a receiver, we use undefined
    napi_value undefined;
    status = napi_get_undefined(env, &undefined);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Impossible to get undefined.");

    // Callback into javascript
    status = napi_call_function(env, undefined, js_callback, 3, argv, NULL);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Unable to call javascript function.");

    callback_arg->threadsafe_context->cnt --;
    if (callback_arg->threadsafe_context->delete_after_use && (callback_arg->threadsafe_context->cnt == 0)) {
        LL_DELETE (igsagent_observe_contexts, callback_arg->threadsafe_context);
        free_threadsafe_context(env, &(callback_arg->threadsafe_context));
    }

    free(callback_arg);
}

void igsagent_observe_callback(igsagent_t *agent, bool is_activated, void *my_data) {
    threadsafe_context_t *threadsafe_context = (threadsafe_context_t *) my_data;
    igsagent_observe_callback_t* callback_arg = calloc(1, sizeof(igsagent_observe_callback_t));
    callback_arg->is_activated = is_activated;
    callback_arg->my_data_ref = threadsafe_context->my_data_ref;
    callback_arg->this_ref = threadsafe_context->this_ref;
    callback_arg->threadsafe_context = threadsafe_context;
    threadsafe_context->cnt ++;
    napi_call_threadsafe_function(threadsafe_context->threadsafe_func, callback_arg, napi_tsfn_nonblocking);
}

napi_value node_igsagent_observe(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);

    // Initiate threadsafe contexte
    threadsafe_context_t *threadsafe_context = calloc(1, sizeof(threadsafe_context_t));
    DL_APPEND(igsagent_observe_contexts, threadsafe_context);

    napi_status status;
    napi_value async_name;
    status = napi_create_string_utf8(env, "ingescape/igsagent_observe", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Invalid name for async_name napi_value.");
    status = napi_create_threadsafe_function(env, argv[0], NULL, async_name, 0, 1, NULL, NULL, NULL,
    cb_igsagent_observe_into_js, &(threadsafe_context->threadsafe_func));
    if (status != napi_ok)
        trigger_exception(env, NULL, "Impossible to create threadsafe function.");

    // Create reference for callback arguments if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[1], &value_type);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    if ((value_type == napi_null) || (value_type == napi_undefined))
        threadsafe_context->my_data_ref = NULL;
    else {
        status = napi_create_reference(env, argv[1], 1, &(threadsafe_context->my_data_ref));
        if (status != napi_ok)
            trigger_exception(env, NULL, "2nd argument must be a javascript Object or an Array or null or undefined.");
    }
    status = napi_create_reference(env, this, 1, &(threadsafe_context->this_ref));
    if (status != napi_ok)
        trigger_exception(env, NULL, "Failed to create reference of 'this' argument");

    igsagent_observe(unwrap_native_agent(env, this), igsagent_observe_callback, threadsafe_context);
    return NULL;
}

napi_value node_igsagent_trace(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *log = convert_napi_to_string(env, argv[0]);
    igsagent_trace(unwrap_native_agent(env, this), "%s", log);
    free(log);
    return NULL;
}

napi_value node_igsagent_debug(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *log = convert_napi_to_string(env, argv[0]);
    igsagent_debug(unwrap_native_agent(env, this), "%s", log);
    free(log);
    return NULL;
}

napi_value node_igsagent_info(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *log = convert_napi_to_string(env, argv[0]);
    igsagent_info(unwrap_native_agent(env, this), "%s", log);
    free(log);
    return NULL;
}

napi_value node_igsagent_warn(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *log = convert_napi_to_string(env, argv[0]);
    igsagent_warn(unwrap_native_agent(env, this), "%s", log);
    free(log);
    return NULL;
}

napi_value node_igsagent_error(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *log = convert_napi_to_string(env, argv[0]);
    igsagent_error(unwrap_native_agent(env, this), "%s", log);
    free(log);
    return NULL;
}

napi_value node_igsagent_fatal(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *log = convert_napi_to_string(env, argv[0]);
    igsagent_fatal(unwrap_native_agent(env, this), "%s", log);
    free(log);
    return NULL;
}

napi_value node_igsagent_name(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);
    char *name = igsagent_name(unwrap_native_agent(env, this));
    napi_value name_js;
    convert_string_to_napi(env, name, &name_js);
    free(name);
    return name_js;
}

napi_value node_igsagent_set_name(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    igsagent_set_name(unwrap_native_agent(env, this), name);
    free(name);
    return NULL;
}

napi_value node_igsagent_family(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);;
    char *family = igsagent_family(unwrap_native_agent(env, this));
    napi_value family_js;
    convert_string_to_napi(env, family, &family_js);
    free(family);
    return family_js;
}

napi_value node_igsagent_set_family(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *family = convert_napi_to_string(env, argv[0]);
    igsagent_set_family(unwrap_native_agent(env, this), family);
    free(family);
    return NULL;
}

napi_value node_igsagent_uuid(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);;
    char *uuid = igsagent_uuid(unwrap_native_agent(env, this));
    napi_value uuid_js;
    convert_string_to_napi(env, uuid, &uuid_js);
    free(uuid);
    return uuid_js;
}

napi_value node_igsagent_state(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);;
    char *state = igsagent_state(unwrap_native_agent(env, this));
    napi_value state_js;
    convert_string_to_napi(env, state, &state_js);
    free(state);
    return state_js;
}

napi_value node_igsagent_set_state(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *state = convert_napi_to_string(env, argv[0]);
    igsagent_set_state(unwrap_native_agent(env, this), state);
    free(state);
    return NULL;
}

napi_value node_igsagent_mute(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);;
    igsagent_mute(unwrap_native_agent(env, this));
    return NULL;
}

napi_value node_igsagent_unmute(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);;
    igsagent_unmute(unwrap_native_agent(env, this));
    return NULL;
}

napi_value node_igsagent_is_muted(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);;
    bool is_muted = igsagent_is_muted(unwrap_native_agent(env, this));
    napi_value is_muted_js;
    convert_bool_to_napi(env, is_muted, &is_muted_js);
    return is_muted_js;
}

static void cb_igsagent_mute_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    igsagent_mute_callback_args_t * callback_arg = (igsagent_mute_callback_args_t *) data;
    napi_value argv[3];
    status = napi_get_reference_value(env, callback_arg->this_ref, &argv[0]);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get reference value.");
    convert_bool_to_napi(env, callback_arg->agent_is_muted, &argv[1]);
    if (callback_arg->my_data_ref == NULL)
        convert_null_to_napi(env, &argv[2]);
    else {
        status = napi_get_reference_value(env, callback_arg->my_data_ref, &argv[2]);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to get reference value.");
    }

    free(callback_arg);

    // Since a function call must have a receiver, we use undefined
    napi_value undefined;
    status = napi_get_undefined(env, &undefined);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Impossible to get undefined.");

    // Callback into javascript
    status = napi_call_function(env, undefined, js_callback, 3, argv, NULL);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Unable to call javascript function.");
}

void igsagent_mute_callback(igsagent_t *agent, bool mute, void *my_data) {
    threadsafe_context_t *threadsafe_context = (threadsafe_context_t *) my_data;
    igsagent_mute_callback_args_t* callback_arg = calloc(1, sizeof(igsagent_mute_callback_args_t));
    callback_arg->agent_is_muted = mute;
    callback_arg->my_data_ref = threadsafe_context->my_data_ref;
    callback_arg->this_ref = threadsafe_context->this_ref;
    napi_call_threadsafe_function(threadsafe_context->threadsafe_func, callback_arg, napi_tsfn_nonblocking);
}

napi_value node_igsagent_observe_mute(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);

    // Initiate threadsafe context
    threadsafe_context_t *threadsafe_context = calloc(1, sizeof(threadsafe_context_t));
    DL_APPEND(igsagent_observe_mute_contexts, threadsafe_context);

    // Create threadsafe function
    napi_status status;
    napi_value async_name;
    status = napi_create_string_utf8(env, "ingescape/igsagent_muteCallback", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Invalid name for async_name napi_value.");
    status = napi_create_threadsafe_function(env, argv[0], NULL, async_name, 0, 1, NULL, NULL, NULL,
    cb_igsagent_mute_into_js, &(threadsafe_context->threadsafe_func));
    if (status != napi_ok)
        trigger_exception(env, NULL, "Impossible to create threadsafe function.");

    // Create reference for callback arguments if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[1], &value_type);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    if ((value_type == napi_null) || (value_type == napi_undefined))
        threadsafe_context->my_data_ref = NULL;
    else {
        status = napi_create_reference(env, argv[1], 1, &(threadsafe_context->my_data_ref));
        if (status != napi_ok)
            trigger_exception(env, NULL, "2nd argument must be a javascript Object or an Array or null or undefined.");
    }
    status = napi_create_reference(env, this, 1, &(threadsafe_context->this_ref));
    if (status != napi_ok)
        trigger_exception(env, NULL, "Failed to create reference of 'this' argument");

    igsagent_observe_mute(unwrap_native_agent(env, this), igsagent_mute_callback, threadsafe_context);
    return NULL;
}

static void cb_igsagent_agent_events_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    igsagent_agent_events_callback_args_t * callback_arg = (igsagent_agent_events_callback_args_t *) data;
    napi_value argv[6];
    status = napi_get_reference_value(env, callback_arg->this_ref, &argv[0]);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get reference value.");
    convert_int_to_napi(env, callback_arg->event, &argv[1]);
    convert_string_to_napi(env, callback_arg->uuid, &argv[2]);
    convert_string_to_napi(env, callback_arg->name, &argv[3]);
    if ((callback_arg->event == IGS_AGENT_WON_ELECTION) || (callback_arg->event == IGS_AGENT_LOST_ELECTION))
        convert_string_to_napi(env, (char*)callback_arg->event_data, &argv[4]);
    else
        convert_null_to_napi(env, &argv[4]);
    if (callback_arg->my_data_ref == NULL)
        convert_null_to_napi(env, &argv[5]);
    else {
        status = napi_get_reference_value(env, callback_arg->my_data_ref, &argv[5]);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to get reference value.");
    }

    free(callback_arg);

    // Since a function call must have a receiver, we use undefined
    napi_value undefined;
    status = napi_get_undefined(env, &undefined);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Impossible to get undefined.");

    // Callback into javascript
    status = napi_call_function(env, undefined, js_callback, 6, argv, NULL);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Unable to call javascript function.");
}

void igsagent_agent_events_callback(igsagent_t *agent, igs_agent_event_t event, const char *uuid, const char *name,
                                    void *event_data, void *my_data) {
    threadsafe_context_t *threadsafe_context = (threadsafe_context_t *) my_data;
    igsagent_agent_events_callback_args_t* callback_arg = calloc(1, sizeof(igsagent_agent_events_callback_args_t));
    callback_arg->event = event;
    callback_arg->uuid = strdup(uuid);
    callback_arg->name = strdup(name);
    if ((event == IGS_AGENT_WON_ELECTION) || (event == IGS_AGENT_LOST_ELECTION))
        callback_arg->event_data = strdup((char*)event_data);
    else
        callback_arg->event_data = NULL;
    callback_arg->my_data_ref = threadsafe_context->my_data_ref;
    callback_arg->this_ref = threadsafe_context->this_ref;
    napi_call_threadsafe_function(threadsafe_context->threadsafe_func, callback_arg, napi_tsfn_nonblocking);
}

napi_value node_igsagent_observe_agent_events(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);

    // Initiate threadsafe context
    threadsafe_context_t * threadsafe_context = calloc(1, sizeof(threadsafe_context_t));
    DL_APPEND(igsagent_observe_agent_events_contexts, threadsafe_context);

    napi_status status;
    napi_value async_name;
    status = napi_create_string_utf8(env, "ingescape/igsagent_agentEventCallback", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Invalid name for async_name napi_value.");
    status = napi_create_threadsafe_function(env, argv[0], NULL, async_name, 0, 1, NULL, NULL, NULL,
    cb_igsagent_agent_events_into_js, &(threadsafe_context->threadsafe_func));
    if (status != napi_ok)
        trigger_exception(env, NULL, "Impossible to create threadsafe function.");

    // Create reference for callback arguments if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[1], &value_type);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    if ((value_type == napi_null) || (value_type == napi_undefined))
        threadsafe_context->my_data_ref = NULL;
    else {
        status = napi_create_reference(env, argv[1], 1, &(threadsafe_context->my_data_ref));
        if (status != napi_ok)
            trigger_exception(env, NULL, "2nd argument must be a javascript Object or an Array or null or undefined.");
    }
    status = napi_create_reference(env, this, 1, &(threadsafe_context->this_ref));
    if (status != napi_ok)
        trigger_exception(env, NULL, "Failed to create reference of 'this' argument");

    igsagent_observe_agent_events(unwrap_native_agent(env, this), igsagent_agent_events_callback, threadsafe_context);
    return NULL;
}

napi_value node_igsagent_definition_load_str(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *definition = convert_napi_to_string(env, argv[0]);
    igsagent_t* associated_agent = unwrap_native_agent(env, this);
    int success = igsagent_definition_load_str(associated_agent, definition);
    if (success == IGS_SUCCESS) {
        threadsafe_context_hash_t *threadsafe_context_hash, *threadsafe_context_hash_tmp;
        threadsafe_context_t *threadsafe_context, *threadsafe_context_tmp;
        igsagent_t *concerned_agent = NULL;
        HASH_ITER (hh, igsagent_observed_input_contexts, threadsafe_context_hash, threadsafe_context_hash_tmp) {
            LL_FOREACH_SAFE (threadsafe_context_hash->list, threadsafe_context, threadsafe_context_tmp) {
                concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
                if (concerned_agent == associated_agent) {
                    LL_DELETE (threadsafe_context_hash->list, threadsafe_context);
                    free_threadsafe_context(env, &threadsafe_context);
                }
            }
            if (threadsafe_context_hash->list == NULL) {
                HASH_DEL (igsagent_observed_input_contexts, threadsafe_context_hash);
                free_threadsafe_context_hash(env, &threadsafe_context_hash);
            }
        }
        HASH_ITER (hh, igsagent_observed_output_contexts, threadsafe_context_hash, threadsafe_context_hash_tmp) {
            LL_FOREACH_SAFE (threadsafe_context_hash->list, threadsafe_context, threadsafe_context_tmp) {
                concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
                if (concerned_agent == associated_agent) {
                    LL_DELETE (threadsafe_context_hash->list, threadsafe_context);
                    free_threadsafe_context(env, &threadsafe_context);
                }
            }
            if (threadsafe_context_hash->list == NULL) {
                HASH_DEL (igsagent_observed_output_contexts, threadsafe_context_hash);
                free_threadsafe_context_hash(env, &threadsafe_context_hash);
            }
        }
        HASH_ITER (hh, igsagent_observed_parameter_contexts, threadsafe_context_hash, threadsafe_context_hash_tmp) {
            LL_FOREACH_SAFE (threadsafe_context_hash->list, threadsafe_context, threadsafe_context_tmp) {
                concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
                if (concerned_agent == associated_agent) {
                    LL_DELETE (threadsafe_context_hash->list, threadsafe_context);
                    free_threadsafe_context(env, &threadsafe_context);
                }
            }
            if (threadsafe_context_hash->list == NULL) {
                HASH_DEL (igsagent_observed_parameter_contexts, threadsafe_context_hash);
                free_threadsafe_context_hash(env, &threadsafe_context_hash);
            }
        }
        HASH_ITER (hh, igsagent_service_contexts, threadsafe_context_hash, threadsafe_context_hash_tmp) {
            LL_FOREACH_SAFE (threadsafe_context_hash->list, threadsafe_context, threadsafe_context_tmp) {
                concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
                if (concerned_agent == associated_agent) {
                    LL_DELETE (threadsafe_context_hash->list, threadsafe_context);
                    free_threadsafe_context(env, &threadsafe_context);
                }
            }
            if (threadsafe_context_hash->list == NULL) {
                HASH_DEL (igsagent_service_contexts, threadsafe_context_hash);
                free_threadsafe_context_hash(env, &threadsafe_context_hash);
            }
        }
    }
    free(definition);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_definition_load_file(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *file_path = convert_napi_to_string(env, argv[0]);
    igsagent_t* associated_agent = unwrap_native_agent(env, this);
    int success = igsagent_definition_load_file(associated_agent, file_path);
    if (success == IGS_SUCCESS) {
        threadsafe_context_hash_t *threadsafe_context_hash, *threadsafe_context_hash_tmp;
        threadsafe_context_t *threadsafe_context, *threadsafe_context_tmp;
        igsagent_t *concerned_agent = NULL;
        HASH_ITER (hh, igsagent_observed_input_contexts, threadsafe_context_hash, threadsafe_context_hash_tmp) {
            LL_FOREACH_SAFE (threadsafe_context_hash->list, threadsafe_context, threadsafe_context_tmp) {
                concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
                if (concerned_agent == associated_agent) {
                    LL_DELETE (threadsafe_context_hash->list, threadsafe_context);
                    free_threadsafe_context(env, &threadsafe_context);
                }
            }
            if (threadsafe_context_hash->list == NULL) {
                HASH_DEL (igsagent_observed_input_contexts, threadsafe_context_hash);
                free_threadsafe_context_hash(env, &threadsafe_context_hash);
            }
        }
        HASH_ITER (hh, igsagent_observed_output_contexts, threadsafe_context_hash, threadsafe_context_hash_tmp) {
            LL_FOREACH_SAFE (threadsafe_context_hash->list, threadsafe_context, threadsafe_context_tmp) {
                concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
                if (concerned_agent == associated_agent) {
                    LL_DELETE (threadsafe_context_hash->list, threadsafe_context);
                    free_threadsafe_context(env, &threadsafe_context);
                }
            }
            if (threadsafe_context_hash->list == NULL) {
                HASH_DEL (igsagent_observed_output_contexts, threadsafe_context_hash);
                free_threadsafe_context_hash(env, &threadsafe_context_hash);
            }
        }
        HASH_ITER (hh, igsagent_observed_parameter_contexts, threadsafe_context_hash, threadsafe_context_hash_tmp) {
            LL_FOREACH_SAFE (threadsafe_context_hash->list, threadsafe_context, threadsafe_context_tmp) {
                concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
                if (concerned_agent == associated_agent) {
                    LL_DELETE (threadsafe_context_hash->list, threadsafe_context);
                    free_threadsafe_context(env, &threadsafe_context);
                }
            }
            if (threadsafe_context_hash->list == NULL) {
                HASH_DEL (igsagent_observed_parameter_contexts, threadsafe_context_hash);
                free_threadsafe_context_hash(env, &threadsafe_context_hash);
            }
        }
        HASH_ITER (hh, igsagent_service_contexts, threadsafe_context_hash, threadsafe_context_hash_tmp) {
            LL_FOREACH_SAFE (threadsafe_context_hash->list, threadsafe_context, threadsafe_context_tmp) {
                concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
                if (concerned_agent == associated_agent) {
                    LL_DELETE (threadsafe_context_hash->list, threadsafe_context);
                    free_threadsafe_context(env, &threadsafe_context);
                }
            }
            if (threadsafe_context_hash->list == NULL) {
                HASH_DEL (igsagent_service_contexts, threadsafe_context_hash);
                free_threadsafe_context_hash(env, &threadsafe_context_hash);
            }
        }
    }
    free(file_path);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_clear_definition(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);
    igsagent_t *associated_agent = unwrap_native_agent(env, this);
    igsagent_clear_definition(associated_agent);
    // Free associated IOPC callback data
    threadsafe_context_hash_t *threadsafe_context_hash, *threadsafe_context_hash_tmp;
    threadsafe_context_t *threadsafe_context, *threadsafe_context_tmp;
    igsagent_t *concerned_agent = NULL;
    HASH_ITER (hh, igsagent_observed_input_contexts, threadsafe_context_hash, threadsafe_context_hash_tmp) {
        LL_FOREACH_SAFE (threadsafe_context_hash->list, threadsafe_context, threadsafe_context_tmp) {
            concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
            if (concerned_agent == associated_agent) {
                LL_DELETE (threadsafe_context_hash->list, threadsafe_context);
                free_threadsafe_context(env, &threadsafe_context);
            }
        }
        if (threadsafe_context_hash->list == NULL) {
            HASH_DEL (igsagent_observed_input_contexts, threadsafe_context_hash);
            free_threadsafe_context_hash(env, &threadsafe_context_hash);
        }
    }
    HASH_ITER (hh, igsagent_observed_output_contexts, threadsafe_context_hash, threadsafe_context_hash_tmp) {
        LL_FOREACH_SAFE (threadsafe_context_hash->list, threadsafe_context, threadsafe_context_tmp) {
            concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
            if (concerned_agent == associated_agent) {
                LL_DELETE (threadsafe_context_hash->list, threadsafe_context);
                free_threadsafe_context(env, &threadsafe_context);
            }
        }
        if (threadsafe_context_hash->list == NULL) {
            HASH_DEL (igsagent_observed_output_contexts, threadsafe_context_hash);
            free_threadsafe_context_hash(env, &threadsafe_context_hash);
        }
    }
    HASH_ITER (hh, igsagent_observed_parameter_contexts, threadsafe_context_hash, threadsafe_context_hash_tmp) {
        LL_FOREACH_SAFE (threadsafe_context_hash->list, threadsafe_context, threadsafe_context_tmp) {
            concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
            if (concerned_agent == associated_agent) {
                LL_DELETE (threadsafe_context_hash->list, threadsafe_context);
                free_threadsafe_context(env, &threadsafe_context);
            }
        }
        if (threadsafe_context_hash->list == NULL) {
            HASH_DEL (igsagent_observed_parameter_contexts, threadsafe_context_hash);
            free_threadsafe_context_hash(env, &threadsafe_context_hash);
        }
    }
    HASH_ITER (hh, igsagent_service_contexts, threadsafe_context_hash, threadsafe_context_hash_tmp) {
        LL_FOREACH_SAFE (threadsafe_context_hash->list, threadsafe_context, threadsafe_context_tmp) {
            concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
            if (concerned_agent == associated_agent) {
                LL_DELETE (threadsafe_context_hash->list, threadsafe_context);
                free_threadsafe_context(env, &threadsafe_context);
            }
        }
        if (threadsafe_context_hash->list == NULL) {
            HASH_DEL (igsagent_service_contexts, threadsafe_context_hash);
            free_threadsafe_context_hash(env, &threadsafe_context_hash);
        }
    }
    return NULL;
}

napi_value node_igsagent_definition_json(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);;
    char *json = igsagent_definition_json(unwrap_native_agent(env, this));
    napi_value json_js;
    convert_string_to_napi(env, json, &json_js);
    free(json);
    return json_js;
}

napi_value node_igsagent_definition_description(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);;
    char *description = igsagent_definition_description(unwrap_native_agent(env, this));
    napi_value description_js;
    convert_string_to_napi(env, description, &description_js);
    free(description);
    return description_js;
}

napi_value node_igsagent_definition_version(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);;
    char *version = igsagent_definition_version(unwrap_native_agent(env, this));
    napi_value version_js;
    convert_string_to_napi(env, version, &version_js);
    free(version);
    return version_js;
}

napi_value node_igsagent_definition_set_description(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *description = convert_napi_to_string(env, argv[0]);
    igsagent_definition_set_description(unwrap_native_agent(env, this), description);
    free(description);
    return NULL;
}

napi_value node_igsagent_definition_set_version(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *version = convert_napi_to_string(env, argv[0]);
    igsagent_definition_set_version(unwrap_native_agent(env, this), version);
    free(version);
    return NULL;
}

napi_value node_igsagent_input_create(napi_env env, napi_callback_info info) {
    napi_value argv[3], this;
    get_function_arguments(env, info, 3, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    int type, success;
    convert_napi_to_int(env, argv[1], &type);
    size_t size;
    void *c_value = convert_value_with_good_type(env, argv[2], type, &size);
    success = igsagent_input_create(unwrap_native_agent(env, this), name, type, c_value, size);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_output_create(napi_env env, napi_callback_info info) {
    napi_value argv[3], this;
    get_function_arguments(env, info, 3, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    int type, success;
    convert_napi_to_int(env, argv[1], &type);
    size_t size;
    void *c_value = convert_value_with_good_type(env, argv[2], type, &size);
    success = igsagent_output_create(unwrap_native_agent(env, this), name, type, c_value, size);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_parameter_create(napi_env env, napi_callback_info info) {
    napi_value argv[3], this;
    get_function_arguments(env, info, 3, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    int type = 0, success;
    convert_napi_to_int(env, argv[1], &type);
    size_t size;
    void *c_value = convert_value_with_good_type(env, argv[2], type, &size);
    success = igsagent_parameter_create(unwrap_native_agent(env, this), name, type, c_value, size);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_input_remove(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    igsagent_t *associated_agent = unwrap_native_agent(env, this);
    int success = igsagent_input_remove(associated_agent, name);
    if (success == IGS_SUCCESS) {
        // Free associated callback data
        threadsafe_context_hash_t *threadsafe_context_hash = NULL;
        threadsafe_context_t *threadsafe_context, *threadsafe_context_tmp;
        igsagent_t *concerned_agent = NULL;
        HASH_FIND_STR (igsagent_observed_input_contexts, name, threadsafe_context_hash);
        if (threadsafe_context_hash != NULL) {
            LL_FOREACH_SAFE (threadsafe_context_hash->list, threadsafe_context, threadsafe_context_tmp)  {
                concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
                if (concerned_agent == associated_agent) {
                    LL_DELETE (threadsafe_context_hash->list, threadsafe_context);
                    free_threadsafe_context(env, &threadsafe_context);
                }
            }
            if (threadsafe_context_hash->list == NULL) {
                HASH_DEL (igsagent_observed_input_contexts, threadsafe_context_hash);
                free_threadsafe_context_hash(env, &threadsafe_context_hash);
            }
        }
    }
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_output_remove(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    igsagent_t *associated_agent = unwrap_native_agent(env, this);
    int success = igsagent_output_remove(associated_agent, name);
    if (success == IGS_SUCCESS) {
        // Free associated callback data
        threadsafe_context_hash_t *threadsafe_context_hash = NULL;
        threadsafe_context_t *threadsafe_context, *threadsafe_context_tmp;
        igsagent_t *concerned_agent = NULL;
        HASH_FIND_STR (igsagent_observed_output_contexts, name, threadsafe_context_hash);
        if (threadsafe_context_hash != NULL) {
            LL_FOREACH_SAFE (threadsafe_context_hash->list, threadsafe_context, threadsafe_context_tmp) {
                concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
                if (concerned_agent == associated_agent) {
                    LL_DELETE (threadsafe_context_hash->list, threadsafe_context);
                    free_threadsafe_context(env, &threadsafe_context);
                }
            }
            if (threadsafe_context_hash->list == NULL) {
                HASH_DEL (igsagent_observed_output_contexts, threadsafe_context_hash);
                free_threadsafe_context_hash(env, &threadsafe_context_hash);
            }
        }
    }
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_parameter_remove(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    igsagent_t *associated_agent = unwrap_native_agent(env, this);
    int success = igsagent_parameter_remove(associated_agent, name);
    if (success == IGS_SUCCESS) {
        // Free associated callback data
        threadsafe_context_hash_t *threadsafe_context_hash = NULL;
        threadsafe_context_t *threadsafe_context, *threadsafe_context_tmp;
        igsagent_t *concerned_agent = NULL;
        HASH_FIND_STR (igsagent_observed_parameter_contexts, name, threadsafe_context_hash);
        if (threadsafe_context_hash != NULL) {
            LL_FOREACH_SAFE (threadsafe_context_hash->list, threadsafe_context, threadsafe_context_tmp) {
                concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
                if (concerned_agent == associated_agent) {
                    LL_DELETE (threadsafe_context_hash->list, threadsafe_context);
                    free_threadsafe_context(env, &threadsafe_context);
                }
            }
            if (threadsafe_context_hash->list == NULL) {
                HASH_DEL (igsagent_observed_parameter_contexts, threadsafe_context_hash);
                free_threadsafe_context_hash(env, &threadsafe_context_hash);
            }
        }
    }
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_input_type(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    int type = igsagent_input_type(unwrap_native_agent(env, this), name);
    free(name);
    napi_value type_js;
    convert_int_to_napi(env, type, &type_js);
    return type_js;
}

napi_value node_igsagent_output_type(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    int type = igsagent_output_type(unwrap_native_agent(env, this), name);
    free(name);
    napi_value type_js;
    convert_int_to_napi(env, type, &type_js);
    return type_js;
}

napi_value node_igsagent_parameter_type(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    int type = igsagent_parameter_type(unwrap_native_agent(env, this), name);
    free(name);
    napi_value type_js;
    convert_int_to_napi(env, type, &type_js);
    return type_js;
}

napi_value node_igsagent_input_count(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);
    int count = igsagent_input_count(unwrap_native_agent(env, this));
    napi_value count_js;
    convert_int_to_napi(env, count, &count_js);
    return count_js;
}

napi_value node_igsagent_output_count(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);
    int count = igsagent_output_count(unwrap_native_agent(env, this));
    napi_value count_js;
    convert_int_to_napi(env, count, &count_js);
    return count_js;
}

napi_value node_igsagent_parameter_count(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);
    int count = igsagent_parameter_count(unwrap_native_agent(env, this));
    napi_value count_js;
    convert_int_to_napi(env, count, &count_js);
    return count_js;
}

napi_value node_igsagent_input_list(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);
    size_t inputs_nb = 0;
    char **inputs = igsagent_input_list(unwrap_native_agent(env, this), &inputs_nb);
    napi_value inputs_js;
    convert_string_list_to_napi_array(env, inputs, inputs_nb, &inputs_js);
    if (inputs != NULL)
        igs_free_iop_list(inputs, inputs_nb);
    return inputs_js;
}

napi_value node_igsagent_output_list(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);
    size_t outputs_nb = 0;
    char **outputs = igsagent_output_list(unwrap_native_agent(env, this), &outputs_nb);
    napi_value outputs_js;
    convert_string_list_to_napi_array(env, outputs, outputs_nb, &outputs_js);
    if (outputs != NULL)
        igs_free_iop_list(outputs, outputs_nb);
    return outputs_js;
}

napi_value node_igsagent_parameter_list(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);
    size_t parameters_nb = 0;
    char **parameters = igsagent_parameter_list(unwrap_native_agent(env, this), &parameters_nb);
    napi_value parameters_js;
    convert_string_list_to_napi_array(env, parameters, parameters_nb, &parameters_js);
    if (parameters != NULL)
        igs_free_iop_list(parameters, parameters_nb);
    return parameters_js;
}

napi_value node_igsagent_input_exists(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    bool exists = igsagent_input_exists(unwrap_native_agent(env, this), name);
    free(name);
    napi_value exists_js;
    convert_bool_to_napi(env, exists, &exists_js);
    return exists_js;
}

napi_value node_igsagent_output_exists(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    bool exists = igsagent_output_exists(unwrap_native_agent(env, this), name);
    free(name);
    napi_value exists_js;
    convert_bool_to_napi(env, exists, &exists_js);
    return exists_js;
}

napi_value node_igsagent_parameter_exists(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    bool exists = igsagent_parameter_exists(unwrap_native_agent(env, this), name);
    free(name);
    napi_value exists_js;
    convert_bool_to_napi(env, exists, &exists_js);
    return exists_js;
}

napi_value node_igsagent_input_bool(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    bool value = igsagent_input_bool(unwrap_native_agent(env, this), name);
    free(name);
    napi_value value_js;
    convert_bool_to_napi(env, value, &value_js);
    return value_js;
}

napi_value node_igsagent_input_int(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    int value = igsagent_input_int(unwrap_native_agent(env, this), name);
    free(name);
    napi_value value_js;
    convert_int_to_napi(env, value, &value_js);
    return value_js;
}

napi_value node_igsagent_input_double(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    double value = igsagent_input_double(unwrap_native_agent(env, this), name);
    free(name);
    napi_value value_js;
    convert_double_to_napi(env, value, &value_js);
    return value_js;
}

napi_value node_igsagent_input_string(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    char *value = igsagent_input_string(unwrap_native_agent(env, this), name);
    free(name);
    napi_value value_js;
    convert_string_to_napi(env, value, &value_js);
    if (value)
        free(value);
    return value_js;
}

napi_value node_igsagent_input_data(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    void *data = NULL;
    size_t size;
    igsagent_input_data(unwrap_native_agent(env, this), name, &data, &size);
    free(name);
    napi_value data_js;
    convert_data_to_napi(env, data, size, &data_js);
    if (data != NULL)
        free(data);
    return data_js;
}

napi_value node_igsagent_output_bool(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    bool value = igsagent_output_bool(unwrap_native_agent(env, this), name);
    free(name);
    napi_value value_js;
    convert_bool_to_napi(env, value, &value_js);
    return value_js;
}

napi_value node_igsagent_output_int(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    int value = igsagent_output_int(unwrap_native_agent(env, this), name);
    free(name);
    napi_value value_js;
    convert_int_to_napi(env, value, &value_js);
    return value_js;
}

napi_value node_igsagent_output_double(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    double value = igsagent_output_double(unwrap_native_agent(env, this), name);
    free(name);
    napi_value value_js;
    convert_double_to_napi(env, value, &value_js);
    return value_js;
}

napi_value node_igsagent_output_string(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    char *value = igsagent_output_string(unwrap_native_agent(env, this), name);
    free(name);
    napi_value value_js;
    convert_string_to_napi(env, value, &value_js);
    if (value)
        free(value);
    return value_js;
}

napi_value node_igsagent_output_data(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    void *data = NULL;
    size_t size;
    igsagent_output_data(unwrap_native_agent(env, this), name, &data, &size);
    free(name);
    napi_value data_js;
    convert_data_to_napi(env, data, size, &data_js);
    if (data != NULL)
        free(data);
    return data_js;
}

napi_value node_igsagent_parameter_bool(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    bool value = igsagent_parameter_bool(unwrap_native_agent(env, this), name);
    free(name);
    napi_value value_js;
    convert_bool_to_napi(env, value, &value_js);
    return value_js;
}

napi_value node_igsagent_parameter_int(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    int value = igsagent_parameter_int(unwrap_native_agent(env, this), name);
    free(name);
    napi_value value_js;
    convert_int_to_napi(env, value, &value_js);
    return value_js;
}

napi_value node_igsagent_parameter_double(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    double value = igsagent_parameter_double(unwrap_native_agent(env, this), name);
    free(name);
    napi_value value_js;
    convert_double_to_napi(env, value, &value_js);
    return value_js;
}

napi_value node_igsagent_parameter_string(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    char *value = igsagent_parameter_string(unwrap_native_agent(env, this), name);
    free(name);
    napi_value value_js;
    convert_string_to_napi(env, value, &value_js);
    if (value)
        free(value);
    return value_js;
}

napi_value node_igsagent_parameter_data(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    void *data = NULL;
    size_t size;
    igsagent_parameter_data(unwrap_native_agent(env, this), name, &data, &size);
    free(name);
    napi_value data_js;
    convert_data_to_napi(env, data, size, &data_js);
    if (data != NULL)
        free(data);
    return data_js;
}

napi_value node_igsagent_input_set_bool(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    bool value;
    convert_napi_to_bool(env, argv[1], &value);
    int success = igsagent_input_set_bool(unwrap_native_agent(env, this), name, value);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_input_set_int(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    int value;
    convert_napi_to_int(env, argv[1], &value);
    int success = igsagent_input_set_int(unwrap_native_agent(env, this), name, value);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_input_set_double(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    double value;
    convert_napi_to_double(env, argv[1], &value);
    int success = igsagent_input_set_double(unwrap_native_agent(env, this), name, value);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_input_set_string(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    char *value = convert_napi_to_string(env, argv[1]);
    int success = igsagent_input_set_string(unwrap_native_agent(env, this), name, value);
    free(name);
    free(value);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_input_set_impulsion(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    int success = igsagent_input_set_impulsion(unwrap_native_agent(env, this), name);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_input_set_data(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    size_t length_value;
    void *data;
    convert_napi_to_data(env, argv[1], &data, &length_value);
    int success = igsagent_input_set_data(unwrap_native_agent(env, this), name, data, length_value);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_output_set_bool(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    bool value;
    convert_napi_to_bool(env, argv[1], &value);
    int success = igsagent_output_set_bool(unwrap_native_agent(env, this), name, value);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_output_set_int(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    int value;
    convert_napi_to_int(env, argv[1], &value);
    int success = igsagent_output_set_int(unwrap_native_agent(env, this), name, value);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_output_set_double(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    double value;
    convert_napi_to_double(env, argv[1], &value);
    int success = igsagent_output_set_double(unwrap_native_agent(env, this), name, value);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_output_set_string(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    char *value = convert_napi_to_string(env, argv[1]);
    int success = igsagent_output_set_string(unwrap_native_agent(env, this), name, value);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    free(name);
    free(value);
    return success_js;
}

napi_value node_igsagent_output_set_impulsion(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    int success = igsagent_output_set_impulsion(unwrap_native_agent(env, this), name);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_output_set_data(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    void *data;
    size_t size;
    convert_napi_to_data(env, argv[1], &data, &size);
    int success = igsagent_output_set_data(unwrap_native_agent(env, this), name, data, size);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_parameter_set_bool(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    bool value;
    convert_napi_to_bool(env, argv[1], &value);
    int success = igsagent_parameter_set_bool(unwrap_native_agent(env, this), name, value);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_parameter_set_int(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    int value;
    convert_napi_to_int(env, argv[1], &value);
    int success = igsagent_parameter_set_int(unwrap_native_agent(env, this), name, value);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_parameter_set_double(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    double value;
    convert_napi_to_double(env, argv[1], &value);
    int success = igsagent_parameter_set_double(unwrap_native_agent(env, this), name, value);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_parameter_set_string(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    char *value = convert_napi_to_string(env, argv[1]);
    int success = igsagent_parameter_set_string(unwrap_native_agent(env, this), name, value);
    free(name);
    free(value);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_parameter_set_data(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    void *data;
    size_t size;
    convert_napi_to_data(env, argv[1], &data, &size);
    int success = igsagent_parameter_set_data(unwrap_native_agent(env, this), name, data, size);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_constraints_enforce(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    bool enforce;
    convert_napi_to_bool(env, argv[0], &enforce);
    igsagent_constraints_enforce(unwrap_native_agent(env, this), enforce);
    return NULL;
}

napi_value node_igsagent_input_add_constraint(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    char *constraint = convert_napi_to_string(env, argv[1]);
    int success = igsagent_input_add_constraint(unwrap_native_agent(env, this), name, constraint);
    free(name);
    free(constraint);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_output_add_constraint(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    char *constraint = convert_napi_to_string(env, argv[1]);
    int success = igsagent_output_add_constraint(unwrap_native_agent(env, this), name, constraint);
    free(name);
    free(constraint);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_parameter_add_constraint(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    char *constraint = convert_napi_to_string(env, argv[1]);
    int success = igsagent_parameter_add_constraint(unwrap_native_agent(env, this), name, constraint);
    free(name);
    free(constraint);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_input_set_description(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    char *description = convert_napi_to_string(env, argv[1]);
    igsagent_input_set_description(unwrap_native_agent(env, this), name, description);
    free(name);
    free(description);
    return NULL;
}

napi_value node_igsagent_output_set_description(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    char *description = convert_napi_to_string(env, argv[1]);
    igsagent_output_set_description(unwrap_native_agent(env, this), name, description);
    free(name);
    free(description);
    return NULL;
}

napi_value node_igsagent_parameter_set_description(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    char *description = convert_napi_to_string(env, argv[1]);
    igsagent_parameter_set_description(unwrap_native_agent(env, this), name, description);
    free(name);
    free(description);
    return NULL;
}

napi_value node_igsagent_clear_input(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    igsagent_clear_input(unwrap_native_agent(env, this), name);
    free(name);
    return NULL;
}

napi_value node_igsagent_clear_output(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    igsagent_clear_output(unwrap_native_agent(env, this), name);
    free(name);
    return NULL;
}

napi_value node_igsagent_clear_parameter(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    igsagent_clear_parameter(unwrap_native_agent(env, this), name);
    free(name);
    return NULL;
}

static void cb_igsagent_iop_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    igsagent_iop_callback_args_t * callback_arg = (igsagent_iop_callback_args_t *) data;
    napi_value argv[6];
    status = napi_get_reference_value(env, callback_arg->this_ref, &argv[0]);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get reference value.");
    convert_int_to_napi(env, callback_arg->iop_type, &argv[1]);
    convert_string_to_napi(env, callback_arg->name, &argv[2]);
    convert_int_to_napi(env, callback_arg->value_type, &argv[3]);
    convert_value_IOP_into_napi(env, callback_arg->value_type, callback_arg->value, callback_arg->value_size, &argv[4]);
    if (callback_arg->my_data_ref == NULL)
        convert_null_to_napi(env, &argv[5]);
    else {
        status = napi_get_reference_value(env, callback_arg->my_data_ref, &argv[5]);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to get reference value.");
    }
    free(callback_arg->name);
    free(callback_arg->value);
    free(callback_arg);

    // Since a function call must have a receiver, we use undefined
    napi_value undefined;
    status = napi_get_undefined(env, &undefined);
    if (status != napi_ok) {
        trigger_exception(env, NULL, "Impossible to get undefined.");
    }

    // Callback into javascript
    status = napi_call_function(env, undefined, js_callback, 6, argv, NULL);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Unable to call javascript function.");
}

void igsagent_iop_callback(igsagent_t *agent, igs_iop_type_t iop_type, const char *name,
                           igs_iop_value_type_t value_type, void *value, size_t value_size,
                           void *my_data) {
    threadsafe_context_t *threadsafe_context = (threadsafe_context_t *) my_data;
    igsagent_iop_callback_args_t * callback_arg =  calloc(1, sizeof(igsagent_iop_callback_args_t));
    callback_arg->iop_type = iop_type;
    callback_arg->name = strdup(name);
    callback_arg->value_type = value_type;
    callback_arg->value = calloc(1, value_size);
    memcpy(callback_arg->value, value, value_size);
    callback_arg->value_size = value_size;
    callback_arg->my_data_ref = threadsafe_context->my_data_ref;
    callback_arg->this_ref = threadsafe_context->this_ref;
    napi_call_threadsafe_function(threadsafe_context->threadsafe_func, callback_arg, napi_tsfn_nonblocking);
}

napi_value node_igsagent_observe_input(napi_env env, napi_callback_info info) {
    napi_value argv[3], this;
    get_function_arguments(env, info, 3, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);

    // Initiate threadsafe context
    threadsafe_context_t *threadsafe_context = calloc(1, sizeof(threadsafe_context_t));
    threadsafe_context_hash_t *threadsafe_hash = NULL;
    HASH_FIND_STR (threadsafe_hash, name, threadsafe_hash);
    if (threadsafe_hash == NULL) {
        threadsafe_hash = (threadsafe_context_hash_t *) zmalloc (sizeof (threadsafe_context_hash_t));
        threadsafe_hash->key = strdup (name);
        threadsafe_hash->list = NULL;
        HASH_ADD_STR (igsagent_observed_input_contexts, key, threadsafe_hash);
    }
    LL_APPEND (threadsafe_hash->list, threadsafe_context);

    napi_status status;
    napi_value async_name;
    status = napi_create_string_utf8(env, "ingescape/igsagent_inputCallback", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Invalid name for async_name napi_value.");
    status = napi_create_threadsafe_function(env, argv[1], NULL, async_name, 0, 1, NULL, NULL, NULL,
    cb_igsagent_iop_into_js, &(threadsafe_context->threadsafe_func));
    if (status != napi_ok)
        trigger_exception(env, NULL, "Impossible to create threadsafe function.");

    // Create reference for callback arguments if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[2], &value_type);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    if ((value_type == napi_null) || (value_type == napi_undefined))
        threadsafe_context->my_data_ref = NULL;
    else {
        status = napi_create_reference(env, argv[2], 1, &(threadsafe_context->my_data_ref));
        if (status != napi_ok) {
            trigger_exception(env, NULL, "2nd argument must be a javascript Object or an Array or null or undefined.");
        }
    }
    status = napi_create_reference(env, this, 1, &(threadsafe_context->this_ref));
    if (status != napi_ok)
        trigger_exception(env, NULL, "Failed to create reference of 'this' argument");

    igsagent_observe_input(unwrap_native_agent(env, this), name, igsagent_iop_callback, threadsafe_context);
    free(name);
    return NULL;
}

napi_value node_igsagent_observe_output(napi_env env, napi_callback_info info) {
    napi_value argv[3], this;
    get_function_arguments(env, info, 3, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);

    // Initiate threadsafe context
    threadsafe_context_t *threadsafe_context = calloc(1, sizeof(threadsafe_context_t));
    threadsafe_context_hash_t *threadsafe_hash = NULL;
    HASH_FIND_STR (threadsafe_hash, name, threadsafe_hash);
    if (threadsafe_hash == NULL) {
        threadsafe_hash = (threadsafe_context_hash_t *) zmalloc (sizeof (threadsafe_context_hash_t));
        threadsafe_hash->key = strdup (name);
        threadsafe_hash->list = NULL;
        HASH_ADD_STR (igsagent_observed_output_contexts, key, threadsafe_hash);
    }
    LL_APPEND (threadsafe_hash->list, threadsafe_context);

    napi_status status;
    napi_value async_name;
    status = napi_create_string_utf8(env, "ingescape/igsagent_outputCallback", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Invalid name for async_name napi_value.");
    status = napi_create_threadsafe_function(env, argv[1], NULL, async_name, 0, 1, NULL, NULL, NULL,
    cb_igsagent_iop_into_js, &(threadsafe_context->threadsafe_func));
    if (status != napi_ok)
        trigger_exception(env, NULL, "Impossible to create threadsafe function.");

    // Create reference for callback arguments if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[2], &value_type);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    if ((value_type == napi_null) || (value_type == napi_undefined))
        threadsafe_context->my_data_ref = NULL;
    else {
        status = napi_create_reference(env, argv[2], 1, &(threadsafe_context->my_data_ref));
        if (status != napi_ok) {
            trigger_exception(env, NULL, "2nd argument must be a javascript Object or an Array or null or undefined.");
        }
    }
    status = napi_create_reference(env, this, 1, &(threadsafe_context->this_ref));
    if (status != napi_ok)
        trigger_exception(env, NULL, "Failed to create reference of 'this' argument");

    igsagent_observe_output(unwrap_native_agent(env, this), name, igsagent_iop_callback, threadsafe_context);
    free(name);
    return NULL;
}

napi_value node_igsagent_observe_parameter(napi_env env, napi_callback_info info) {
    napi_value argv[3], this;
    get_function_arguments(env, info, 3, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);

    // Initiate threadsafe context
    threadsafe_context_t *threadsafe_context = calloc(1, sizeof(threadsafe_context_t));
    threadsafe_context_hash_t *threadsafe_hash = NULL;
    HASH_FIND_STR (threadsafe_hash, name, threadsafe_hash);
    if (threadsafe_hash == NULL) {
        threadsafe_hash = (threadsafe_context_hash_t *) zmalloc (sizeof (threadsafe_context_hash_t));
        threadsafe_hash->key = strdup (name);
        threadsafe_hash->list = NULL;
        HASH_ADD_STR (igsagent_observed_parameter_contexts, key, threadsafe_hash);
    }
    LL_APPEND (threadsafe_hash->list, threadsafe_context);

    napi_status status;
    napi_value async_name;
    status = napi_create_string_utf8(env, "ingescape/igsagent_parameterCallback", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Invalid name for async_name napi_value.");
    status = napi_create_threadsafe_function(env, argv[1], NULL, async_name, 0, 1, NULL, NULL, NULL,
    cb_igsagent_iop_into_js, &(threadsafe_context->threadsafe_func));
    if (status != napi_ok)
        trigger_exception(env, NULL, "Impossible to create threadsafe function.");

    // Create reference for callback arguments if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[2], &value_type);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    if ((value_type == napi_null) || (value_type == napi_undefined))
        threadsafe_context->my_data_ref = NULL;
    else {
        status = napi_create_reference(env, argv[2], 1, &(threadsafe_context->my_data_ref));
        if (status != napi_ok) {
            trigger_exception(env, NULL, "2nd argument must be a javascript Object or an Array or null or undefined.");
        }
    }
    status = napi_create_reference(env, this, 1, &(threadsafe_context->this_ref));
    if (status != napi_ok)
        trigger_exception(env, NULL, "Failed to create reference of 'this' argument");

    igsagent_observe_parameter(unwrap_native_agent(env, this), name, igsagent_iop_callback, threadsafe_context);
    free(name);
    return NULL;
}

napi_value node_igsagent_output_mute(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    igsagent_output_mute(unwrap_native_agent(env, this), name);
    free(name);
    return NULL;
}

napi_value node_igsagent_output_unmute(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    igsagent_output_unmute(unwrap_native_agent(env, this), name);
    free(name);
    return NULL;
}

napi_value node_igsagent_output_is_muted(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    bool is_muted = igsagent_output_is_muted(unwrap_native_agent(env, this), name);
    free(name);
    napi_value is_muted_js;
    convert_bool_to_napi(env, is_muted, &is_muted_js);
    return is_muted_js;
}

napi_value node_igsagent_mapping_load_str(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *json_str = convert_napi_to_string(env, argv[0]);
    int success = igsagent_mapping_load_str(unwrap_native_agent(env, this), json_str);
    free(json_str);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_mapping_load_file(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *file_path = convert_napi_to_string(env, argv[0]);
    int success = igsagent_mapping_load_file(unwrap_native_agent(env, this), file_path);
    free(file_path);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_mapping_json(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);
    char *json = igsagent_mapping_json(unwrap_native_agent(env, this));
    napi_value json_js;
    convert_string_to_napi(env, json, &json_js);
    free(json);
    return json_js;
}

napi_value node_igsagent_mapping_count(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);
    int count = igsagent_mapping_count(unwrap_native_agent(env, this));
    napi_value count_js;
    convert_int_to_napi(env, count, &count_js);
    return count_js;
}

napi_value node_igsagent_clear_mappings(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);
    igsagent_clear_mappings(unwrap_native_agent(env, this));
    return NULL;
}

napi_value node_igsagent_clear_mappings_with_agent(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *agent_name = convert_napi_to_string(env, argv[0]);
    igsagent_clear_mappings_with_agent(unwrap_native_agent(env, this), agent_name);
    free(agent_name);
    return NULL;
}

napi_value node_igsagent_mapping_add(napi_env env, napi_callback_info info) {
    napi_value argv[3], this;
    get_function_arguments(env, info, 3, argv);
    this = get_function_this_argument(env, info);
    char *from_our_input = convert_napi_to_string(env, argv[0]);
    char *to_agent = convert_napi_to_string(env, argv[1]);
    char *with_output = convert_napi_to_string(env, argv[2]);
    uint64_t id = igsagent_mapping_add(unwrap_native_agent(env, this), from_our_input, to_agent, with_output);
    free(from_our_input);
    free(to_agent);
    free(with_output);

    napi_value id_js;
    convert_uint64_to_napi(env, id, &id_js);
    return id_js;
}

napi_value node_igsagent_mapping_remove_with_id(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    uint64_t id;
    convert_napi_to_uint64(env, argv[0], &id);
    int success = igsagent_mapping_remove_with_id(unwrap_native_agent(env, this), id);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_mapping_remove_with_name(napi_env env, napi_callback_info info) {
    napi_value argv[3], this;
    get_function_arguments(env, info, 3, argv);
    this = get_function_this_argument(env, info);
    char *from_our_input = convert_napi_to_string(env, argv[0]);
    char *to_agent = convert_napi_to_string(env, argv[1]);
    char *with_output = convert_napi_to_string(env, argv[2]);
    int success = igsagent_mapping_remove_with_name(unwrap_native_agent(env, this), from_our_input, to_agent, with_output);
    free(from_our_input);
    free(to_agent);
    free(with_output);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_split_count(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);
    int count = igsagent_split_count(unwrap_native_agent(env, this));
    napi_value count_js;
    convert_int_to_napi(env, count, &count_js);
    return count_js;
}

napi_value node_igsagent_split_add(napi_env env, napi_callback_info info) {
    napi_value argv[3], this;
    get_function_arguments(env, info, 3, argv);
    this = get_function_this_argument(env, info);
    char *from_our_input = convert_napi_to_string(env, argv[0]);
    char *to_agent = convert_napi_to_string(env, argv[1]);
    char *with_output = convert_napi_to_string(env, argv[2]);
    uint64_t id = igsagent_split_add(unwrap_native_agent(env, this), from_our_input, to_agent, with_output);
    free(from_our_input);
    free(to_agent);
    free(with_output);
    napi_value id_js;
    convert_uint64_to_napi(env, id, &id_js);
    return id_js;
}

napi_value node_igsagent_split_remove_with_id(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    uint64_t id;
    convert_napi_to_uint64(env, argv[0], &id);
    int success = igsagent_split_remove_with_id(unwrap_native_agent(env, this), id);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_split_remove_with_name(napi_env env, napi_callback_info info) {
    napi_value argv[3], this;
    get_function_arguments(env, info, 3, argv);
    this = get_function_this_argument(env, info);
    char *from_our_input = convert_napi_to_string(env, argv[0]);
    char *to_agent = convert_napi_to_string(env, argv[1]);
    char *with_output = convert_napi_to_string(env, argv[2]);
    int success = igsagent_split_remove_with_name(unwrap_native_agent(env, this), from_our_input, to_agent, with_output);
    free(from_our_input);
    free(to_agent);
    free(with_output);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_mapping_outputs_request(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);
    bool outputs_request = igsagent_mapping_outputs_request(unwrap_native_agent(env, this));
    napi_value outputs_request_js;
    convert_bool_to_napi(env, outputs_request, &outputs_request_js);
    return outputs_request_js;
}

napi_value node_igsagent_mapping_set_outputs_request(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    bool outputs_request;
    convert_napi_to_bool(env, argv[0], &outputs_request);
    igsagent_mapping_set_outputs_request(unwrap_native_agent(env, this), outputs_request);
    return NULL;
}

napi_value node_igsagent_service_call(napi_env env, napi_callback_info info) {
    napi_value argv[4], this;
    get_function_arguments(env, info, 4, argv);
    this = get_function_this_argument(env, info);
    char *agent_name_or_uuid = convert_napi_to_string(env, argv[0]);
    char *service_name = convert_napi_to_string(env, argv[1]);
    char *token = convert_napi_to_string(env, argv[3]);
    napi_status status;
    napi_valuetype value_type;
    status = napi_typeof(env, argv[2], &value_type);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get napi value type.");

    int res;
    igsagent_t *agent = unwrap_native_agent(env, this);
    if ((value_type != napi_null) && (value_type != napi_undefined)) {
        bool is_array;
        status = napi_is_array(env, argv[2], &is_array);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Invalid array was passed as argument.");
        if (!is_array)
            trigger_exception(env, NULL, "N-API: Argument must be an array or null or undefined");

        uint32_t length;
        status = napi_get_array_length(env, argv[2], &length);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to get array length.");

        if (length == 0)
            res = igsagent_service_call(agent, agent_name_or_uuid, service_name, NULL, token);
        else {
            igs_service_arg_t * list = NULL;
            service_args_c_from_js(env, argv[2], &list);
            res = igsagent_service_call(agent, agent_name_or_uuid, service_name, &list, token);
        }
    }
    else
        res = igsagent_service_call(agent, agent_name_or_uuid, service_name, NULL, token);
    free(agent_name_or_uuid);
    free(service_name);
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

static void cb_igsagent_service_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    igsagent_service_callback_args_t * callback_arg = (igsagent_service_callback_args_t *) data;
    napi_value argv[7];
    status = napi_get_reference_value(env, callback_arg->this_ref, &argv[0]);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get reference value.");
    convert_string_to_napi(env, callback_arg->sender_agent_name, &argv[1]);
    convert_string_to_napi(env, callback_arg->sender_agent_uuid, &argv[2]);
    convert_string_to_napi(env, callback_arg->service_name, &argv[3]);
    service_args_js_from_c(env, callback_arg->first_argument, &argv[4]); //convert chained list to arraybuffer
    convert_string_to_napi(env, callback_arg->token, &argv[5]);
    if (callback_arg->my_data_ref == NULL)
        convert_null_to_napi(env, &argv[6]);
    else {
        status = napi_get_reference_value(env, callback_arg->my_data_ref, &argv[6]);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to get reference value.");
    }

    free(callback_arg->sender_agent_name);
    free(callback_arg->sender_agent_uuid);
    free(callback_arg->service_name);
    igs_service_args_destroy(&(callback_arg->first_argument));
    free(callback_arg->token);
    free(callback_arg);

    // Since a function call must have a receiver, we use undefined
    napi_value undefined;
    status = napi_get_undefined(env, &undefined);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Impossible to get undefined.");

    // Callback into javascript
    status = napi_call_function(env, undefined, js_callback, 7, argv, NULL);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Unable to call javascript function.");
}

void igsagent_service_callback(igsagent_t *agent, const char *sender_agent_name, const char *sender_agent_uuid,
                               const char *service_name, igs_service_arg_t *first_argument, size_t args_nbr,
                               const char *token, void *my_data) {
    threadsafe_context_t *threadsafe_context = (threadsafe_context_t *) my_data;
    igsagent_service_callback_args_t* callback_arg = calloc(1, sizeof(igsagent_service_callback_args_t));
    callback_arg->sender_agent_name = strdup(sender_agent_name);
    callback_arg->sender_agent_uuid = strdup(sender_agent_uuid);
    callback_arg->service_name = strdup(service_name);
    if (first_argument != NULL)
        callback_arg->first_argument = igs_service_args_clone(first_argument); //clone it to keep it
    else
        callback_arg->first_argument = NULL;
    callback_arg->args_nb = args_nbr;
    if (token != NULL)
        callback_arg->token = strdup(token);
    else
        callback_arg->token = NULL;
    callback_arg->my_data_ref = threadsafe_context->my_data_ref;
    callback_arg->this_ref = threadsafe_context->this_ref;
    napi_call_threadsafe_function(threadsafe_context->threadsafe_func, callback_arg, napi_tsfn_nonblocking);
}

napi_value node_igsagent_service_init(napi_env env, napi_callback_info info) {
    napi_value argv[3], this;
    get_function_arguments(env, info, 3, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);

    // Initiate threadsafe contexte
    threadsafe_context_t *threadsafe_context = calloc(1, sizeof(threadsafe_context_t));
    threadsafe_context_hash_t *threadsafe_hash = NULL;
    HASH_FIND_STR (threadsafe_hash, name, threadsafe_hash);
    if (threadsafe_hash == NULL) {
        threadsafe_hash = (threadsafe_context_hash_t *) zmalloc (sizeof (threadsafe_context_hash_t));
        threadsafe_hash->key = strdup (name);
        threadsafe_hash->list = NULL;
        HASH_ADD_STR (igsagent_service_contexts, key, threadsafe_hash);
    }
    LL_APPEND (threadsafe_hash->list, threadsafe_context);

    napi_status status;
    napi_value async_name;
    status = napi_create_string_utf8(env, "ingescape/igsagent_serviceCallback", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Invalid name for async_name napi_value.");
    status = napi_create_threadsafe_function(env, argv[1], NULL, async_name, 0, 1, NULL, NULL, NULL,
    cb_igsagent_service_into_js, &(threadsafe_context->threadsafe_func));
    if (status != napi_ok)
        trigger_exception(env, NULL, "Impossible to create threadsafe function.");

    // Create reference for callback arguments if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[2], &value_type);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    if ((value_type == napi_null) || (value_type == napi_undefined))
        threadsafe_context->my_data_ref = NULL;
    else {
        status = napi_create_reference(env, argv[2], 1, &(threadsafe_context->my_data_ref));
        if (status != napi_ok)
            trigger_exception(env, NULL, "3rd argument must be a javascript Object or an Array or null or undefined.");
    }
    status = napi_create_reference(env, this, 1, &(threadsafe_context->this_ref));
    if (status != napi_ok)
        trigger_exception(env, NULL, "Failed to create reference of 'this' argument");

    int res = igsagent_service_init(unwrap_native_agent(env, this), name, igsagent_service_callback, threadsafe_context);
    free(name);
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

napi_value node_igsagent_service_remove(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    igsagent_t *associated_agent = unwrap_native_agent(env, this);
    int success = igsagent_service_remove(associated_agent, name);
    if (success == IGS_SUCCESS) {
        // Free associated callback data
        threadsafe_context_hash_t *threadsafe_context_hash = NULL;
        threadsafe_context_t *threadsafe_context, *threadsafe_context_tmp;
        igsagent_t *concerned_agent = NULL;
        HASH_FIND_STR (igsagent_service_contexts, name, threadsafe_context_hash);
        if (threadsafe_context_hash != NULL) {
            LL_FOREACH_SAFE (threadsafe_context_hash->list, threadsafe_context, threadsafe_context_tmp) {
                concerned_agent = get_igsagent_from_this_ref(env, threadsafe_context->this_ref);
                if (concerned_agent == associated_agent) {
                    LL_DELETE (threadsafe_context_hash->list, threadsafe_context);
                    free_threadsafe_context(env, &threadsafe_context);
                }
            }
            if (threadsafe_context_hash->list == NULL) {
                HASH_DEL (igsagent_service_contexts, threadsafe_context_hash);
                free_threadsafe_context_hash(env, &threadsafe_context_hash);
            }
        }
    }
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_service_arg_add(napi_env env, napi_callback_info info) {
    napi_value argv[3], this;
    get_function_arguments(env, info, 3, argv);
    this = get_function_this_argument(env, info);
    char *service_name = convert_napi_to_string(env, argv[0]);
    char *arg_name = convert_napi_to_string(env, argv[1]);
	int type;
    convert_napi_to_int(env, argv[2], &type);
	int res = igsagent_service_arg_add(unwrap_native_agent(env, this), service_name, arg_name, type);
    free(service_name);
    free(arg_name);
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

napi_value node_igsagent_service_arg_remove(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char * service_name = convert_napi_to_string(env, argv[0]);
    char * arg_name = convert_napi_to_string(env, argv[1]);
	int res = igsagent_service_arg_remove(unwrap_native_agent(env, this), service_name, arg_name);
    free(service_name);
    free(arg_name);
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

napi_value node_igsagent_service_count(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);
    size_t res = igsagent_service_count(unwrap_native_agent(env, this));
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

napi_value node_igsagent_service_exists(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    bool res = igsagent_service_exists(unwrap_native_agent(env, this), name);
    free(name);
    napi_value res_convert;
    convert_bool_to_napi(env, res, &res_convert);
    return res_convert;
}

napi_value node_igsagent_service_list(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);
    size_t elements_nb = 0;
    char **services_list = igsagent_service_list(unwrap_native_agent(env, this), &elements_nb);
    napi_value napi_services;
    convert_string_list_to_napi_array(env, services_list, elements_nb, &napi_services);
    if (services_list != NULL)
        igs_free_services_list(services_list, elements_nb);
    return napi_services;
}

napi_value node_igsagent_service_args_list(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    igs_service_arg_t * head = igsagent_service_args_first(unwrap_native_agent(env, this), name);
    free(name);
    napi_status status;
    napi_value arrayJS;
    status = napi_create_array(env, &arrayJS);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to create array.");

    igs_service_arg_t * elt, * tmp;
    int i = 0;
    napi_value arg_name, arg_type, service_argument;
    LL_FOREACH_SAFE(head, elt, tmp) {
        status = napi_create_object(env, &service_argument);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to create object.");

        // Add argument's name & argument's type
        convert_string_to_napi(env, elt->name, &arg_name);
        status = napi_set_named_property(env, service_argument, "name", arg_name);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to set name of service argument.");

        convert_int_to_napi(env, elt->type, &arg_type);
        status = napi_set_named_property(env, service_argument, "type", arg_type);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to set type of service argument.");

        status = napi_set_element(env, arrayJS, i, service_argument);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to set service argument in array.");
        i++;
    }
    return arrayJS;
}

napi_value node_igsagent_service_args_count(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *name = convert_napi_to_string(env, argv[0]);
    size_t nb_arg = igsagent_service_args_count(unwrap_native_agent(env, this), name);
    free(name);
    napi_value res_convert;
    convert_int_to_napi(env, nb_arg, &res_convert);
    return res_convert;
}

napi_value node_igsagent_service_arg_exists(napi_env env, napi_callback_info info) {
    napi_value argv[2], this;
    get_function_arguments(env, info, 2, argv);
    this = get_function_this_argument(env, info);
    char *service_name = convert_napi_to_string(env, argv[0]);
    char *arg_name = convert_napi_to_string(env, argv[1]);
    bool exist = igsagent_service_arg_exists(unwrap_native_agent(env, this), service_name, arg_name);
    free(service_name);
    free(arg_name);
    napi_value res_convert;
    convert_bool_to_napi(env, exist, &res_convert);
    return res_convert;
}

napi_value node_igsagent_election_join(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *election_name = convert_napi_to_string(env, argv[0]);
    int success = igsagent_election_join(unwrap_native_agent(env, this), election_name);
    free(election_name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_election_leave(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *election_name = convert_napi_to_string(env, argv[0]);
    int success = igsagent_election_leave(unwrap_native_agent(env, this), election_name);
    free(election_name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igsagent_definition_set_path(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *definition_path = convert_napi_to_string(env, argv[0]);
    igsagent_definition_set_path(unwrap_native_agent(env, this), definition_path);
    free(definition_path);
    return NULL;
}

napi_value node_igsagent_definition_save(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);
    igsagent_definition_save(unwrap_native_agent(env, this));
    return NULL;
}

napi_value node_igsagent_mapping_set_path(napi_env env, napi_callback_info info) {
    napi_value argv[1], this;
    get_function_arguments(env, info, 1, argv);
    this = get_function_this_argument(env, info);
    char *mapping_path = convert_napi_to_string(env, argv[0]);
    igsagent_mapping_set_path(unwrap_native_agent(env, this), mapping_path);
    free(mapping_path);
    return NULL;
}

napi_value node_igsagent_mapping_save(napi_env env, napi_callback_info info) {
    napi_value this = get_function_this_argument(env, info);
    igsagent_mapping_save(unwrap_native_agent(env, this));
    return NULL;
}


napi_value init_agent(napi_env env, napi_value exports) {
    napi_value cons;
    napi_status status;
    napi_property_descriptor properties[] = {
        { "destroy", NULL, destroy_agent_context, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "activate", NULL, node_igsagent_activate, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "deactivate", NULL, node_igsagent_deactivate, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "isActivated", NULL, node_igsagent_is_activated, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "observe", NULL, node_igsagent_observe, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "trace", NULL, node_igsagent_trace, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "debug", NULL, node_igsagent_debug, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "info", NULL, node_igsagent_info, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "warn", NULL, node_igsagent_warn, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "error", NULL, node_igsagent_error, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "fatal", NULL, node_igsagent_fatal, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "name", NULL, node_igsagent_name, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "setName", NULL, node_igsagent_set_name, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "family", NULL, node_igsagent_family, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "setFamily", NULL, node_igsagent_set_family, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "uuid", NULL, node_igsagent_uuid, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "state", NULL, node_igsagent_state, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "setState", NULL, node_igsagent_set_state, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "mute", NULL, node_igsagent_mute, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "unmute", NULL, node_igsagent_unmute, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "isMuted", NULL, node_igsagent_is_muted, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "observeMute", NULL, node_igsagent_observe_mute, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "observeAgentEvents", NULL, node_igsagent_observe_agent_events, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "definitionLoadStr", NULL, node_igsagent_definition_load_str, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "definitionLoadFile", NULL, node_igsagent_definition_load_file, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "clearDefinition", NULL, node_igsagent_clear_definition, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "definitionJson", NULL, node_igsagent_definition_json, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "definitionDescription", NULL, node_igsagent_definition_description, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "definitionVersion", NULL, node_igsagent_definition_version, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "definitionSetDescription", NULL, node_igsagent_definition_set_description, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "definitionSetVersion", NULL, node_igsagent_definition_set_version, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "inputCreate", NULL, node_igsagent_input_create, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "outputCreate", NULL, node_igsagent_output_create, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "parameterCreate", NULL, node_igsagent_parameter_create, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "inputRemove", NULL, node_igsagent_input_remove, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "outputRemove", NULL, node_igsagent_output_remove, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "parameterRemove", NULL, node_igsagent_parameter_remove, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "inputType", NULL, node_igsagent_input_type, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "outputType", NULL, node_igsagent_output_type, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "parameterType", NULL, node_igsagent_parameter_type, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "inputCount", NULL, node_igsagent_input_count, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "outputCount", NULL, node_igsagent_output_count, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "parameterCount", NULL, node_igsagent_parameter_count, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "inputList", NULL, node_igsagent_input_list, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "outputList", NULL, node_igsagent_output_list, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "parameterList", NULL, node_igsagent_parameter_list, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "inputExists", NULL, node_igsagent_input_exists, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "outputExists", NULL, node_igsagent_output_exists, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "parameterExists", NULL, node_igsagent_parameter_exists, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "inputBool", NULL, node_igsagent_input_bool, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "inputInt", NULL, node_igsagent_input_int, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "inputDouble", NULL, node_igsagent_input_double, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "inputString", NULL, node_igsagent_input_string, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "inputData", NULL, node_igsagent_input_data, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "outputBool", NULL, node_igsagent_output_bool, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "outputInt", NULL, node_igsagent_output_int, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "outputDouble", NULL, node_igsagent_output_double, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "outputString", NULL, node_igsagent_output_string, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "outputData", NULL, node_igsagent_output_data, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "parameterBool", NULL, node_igsagent_parameter_bool, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "parameterInt", NULL, node_igsagent_parameter_int, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "parameterDouble", NULL, node_igsagent_parameter_double, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "parameterString", NULL, node_igsagent_parameter_string, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "parameterData", NULL, node_igsagent_parameter_data, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "inputSetBool", NULL, node_igsagent_input_set_bool, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "inputSetInt", NULL, node_igsagent_input_set_int, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "inputSetDouble", NULL, node_igsagent_input_set_double, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "inputSetString", NULL, node_igsagent_input_set_string, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "inputSetImpulsion", NULL, node_igsagent_input_set_impulsion, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "inputSetData", NULL, node_igsagent_input_set_data, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "outputSetBool", NULL, node_igsagent_output_set_bool, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "outputSetInt", NULL, node_igsagent_output_set_int, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "outputSetDouble", NULL, node_igsagent_output_set_double, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "outputSetString", NULL, node_igsagent_output_set_string, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "outputSetImpulsion", NULL, node_igsagent_output_set_impulsion, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "outputSetData", NULL, node_igsagent_output_set_data, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "parameterSetBool", NULL, node_igsagent_parameter_set_bool, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "parameterSetInt", NULL, node_igsagent_parameter_set_int, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "parameterSetDouble", NULL, node_igsagent_parameter_set_double, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "parameterSetString", NULL, node_igsagent_parameter_set_string, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "parameterSetData", NULL, node_igsagent_parameter_set_data, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "constraintsEnforce", NULL, node_igsagent_constraints_enforce, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "inputAddConstraint", NULL, node_igsagent_input_add_constraint, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "outputAddConstraint", NULL, node_igsagent_output_add_constraint, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "parameterAddConstraint", NULL, node_igsagent_parameter_add_constraint, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "inputSetDescription", NULL, node_igsagent_input_set_description, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "outputSetDescription", NULL, node_igsagent_output_set_description, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "parameterSetDescription", NULL, node_igsagent_parameter_set_description, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "clearInput", NULL, node_igsagent_clear_input, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "clearOutput", NULL, node_igsagent_clear_output, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "clearParameter", NULL, node_igsagent_clear_parameter, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "observeInput", NULL, node_igsagent_observe_input, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "observeOutput", NULL, node_igsagent_observe_output, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "observeParameter", NULL, node_igsagent_observe_parameter, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "outputMute", NULL, node_igsagent_output_mute, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "outputUnmute", NULL, node_igsagent_output_unmute, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "outputIsMuted", NULL, node_igsagent_output_is_muted, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "mappingLoadStr", NULL, node_igsagent_mapping_load_str, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "mappingLoadFile", NULL, node_igsagent_mapping_load_file, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "mappingJson", NULL, node_igsagent_mapping_json, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "mappingCount", NULL, node_igsagent_mapping_count, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "clearMappings", NULL, node_igsagent_clear_mappings, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "clearMappingsWithAgent", NULL, node_igsagent_clear_mappings_with_agent, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "mappingAdd", NULL, node_igsagent_mapping_add, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "mappingRemoveWithId", NULL, node_igsagent_mapping_remove_with_id, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "mappingRemoveWithName", NULL, node_igsagent_mapping_remove_with_name, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "splitCount", NULL, node_igsagent_split_count, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "splitAdd", NULL, node_igsagent_split_add, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "splitRemoveWithId", NULL, node_igsagent_split_remove_with_id, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "splitRemoveWithName", NULL, node_igsagent_split_remove_with_name, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "mappingOutputsRequest", NULL, node_igsagent_mapping_outputs_request, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "mappingSetOutputsRequest", NULL, node_igsagent_mapping_set_outputs_request, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "serviceCall", NULL, node_igsagent_service_call, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "serviceInit", NULL, node_igsagent_service_init, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "serviceRemove", NULL, node_igsagent_service_remove, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "serviceArgAdd", NULL, node_igsagent_service_arg_add, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "serviceArgRemove", NULL, node_igsagent_service_arg_remove, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "serviceCount", NULL, node_igsagent_service_count, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "serviceExists", NULL, node_igsagent_service_exists, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "serviceList", NULL, node_igsagent_service_list, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "serviceArgsList", NULL, node_igsagent_service_args_list, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "serviceArgsCount", NULL, node_igsagent_service_args_count, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "serviceArgExists", NULL, node_igsagent_service_arg_exists, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "electionJoin", NULL, node_igsagent_election_join, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "electionLeave", NULL, node_igsagent_election_leave, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},

        { "definitionSetPath", NULL, node_igsagent_definition_set_path, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "definitionSave", NULL, node_igsagent_definition_save, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "mappingSetPath", NULL, node_igsagent_mapping_set_path, NULL, NULL, NULL, napi_writable | napi_configurable, NULL},
        { "mappingSave", NULL, node_igsagent_mapping_save, NULL, NULL, NULL, napi_writable | napi_configurable, NULL}
    };
    status = napi_define_class(env, "Agent", NAPI_AUTO_LENGTH, node_igsagent_new, NULL, 121, properties, &cons);
    if (status != napi_ok) {
        trigger_exception(env, NULL, "Unable to wrap Agent class.");
        return NULL;
    }
    status = napi_set_named_property(env, exports, "Agent", cons);
    if (status != napi_ok) {
        trigger_exception(env, NULL, "Unable to populate exports.");
        return NULL;
    }
    return exports;
}
