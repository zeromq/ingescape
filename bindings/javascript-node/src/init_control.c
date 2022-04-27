/*  =========================================================================
    init_control - init and control an agent on the network

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "../include/private.h"

threadsafe_context_t *observe_forced_stop_contexts = NULL;
threadsafe_context_t *observe_mute_contexts = NULL;
threadsafe_context_t *observe_freeze_contexts = NULL;
threadsafe_context_t *observe_agent_events_contexts = NULL;

napi_value node_igs_start_with_device(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *device = convert_napi_to_string(env, argv[0]);
    int port;
    convert_napi_to_int(env, argv[1], &port);
    int success = igs_start_with_device(device, port);
    free(device);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_start_with_ip(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *ip_address = convert_napi_to_string(env, argv[0]);
    int port;
    convert_napi_to_int(env, argv[1], &port);
    int success = igs_start_with_ip(ip_address, port);
    free(ip_address);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_stop(napi_env env, napi_callback_info info) {
    igs_stop();
    return NULL;
}

napi_value node_igs_is_started(napi_env env, napi_callback_info info) {
    bool is_started = igs_is_started();
    napi_value is_started_js;
    convert_bool_to_napi(env, is_started, &is_started_js);
    return is_started_js;
}

static void cb_external_stop_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    napi_ref my_data_ref = (napi_ref) data;
    napi_value argv[1];

    if (my_data_ref == NULL)
        convert_null_to_napi(env, &argv[0]);
    else {
        status = napi_get_reference_value(env, my_data_ref, &argv[0]);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to get reference value.");
    }

    // Since a function call must have a receiver, we use undefined
    napi_value undefined;
    status = napi_get_undefined(env, &undefined);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Impossible to get undefined.");

    // Callback into javascript
    status = napi_call_function(env, undefined, js_callback, 1, argv, NULL);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Unable to call javascript function.");
}

void forced_stop_fn (void *my_data) {
    threadsafe_context_t *threadsafe_context = (threadsafe_context_t *) my_data;
    napi_call_threadsafe_function(threadsafe_context->threadsafe_func, threadsafe_context->my_data_ref, napi_tsfn_nonblocking);
}

napi_value node_igs_observe_forced_stop(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);

    // Initiate struct
    threadsafe_context_t *threadsafe_context = calloc(1, sizeof(threadsafe_context_t));
    DL_APPEND(observe_forced_stop_contexts, threadsafe_context);

    // Create threadsafe function
    napi_status status;
    napi_value async_name;
    status = napi_create_string_utf8(env, "ingescape/forcedStopCallback", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Invalid name for async_name napi_value.");
    status = napi_create_threadsafe_function(env, argv[0], NULL, async_name, 0, 1, NULL, NULL, NULL,
    cb_external_stop_into_js, &(threadsafe_context->threadsafe_func));
    if (status != napi_ok)
        trigger_exception(env, NULL, "Impossible to create threadsafe function.");

    // create reference for callback arguments if not null
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

    igs_observe_forced_stop(forced_stop_fn, threadsafe_context);
    return NULL;
}

napi_value node_igs_agent_set_name(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    igs_agent_set_name(name);
    free(name);
    return NULL;
}

napi_value node_igs_agent_name(napi_env env, napi_callback_info info) {
    char *name = igs_agent_name();
    napi_value name_js;
    convert_string_to_napi(env, name, &name_js);
    free(name);
    return name_js;
}

napi_value node_igs_agent_uuid(napi_env env, napi_callback_info info) {
    char *uuid = (char*) igs_agent_uuid();
    napi_value uuid_js;
    convert_string_to_napi(env, uuid, &uuid_js);
    free(uuid);
    return uuid_js;
}

napi_value node_igs_agent_set_state(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *state = convert_napi_to_string(env, argv[0]);
    igs_agent_set_state(state);
    free(state);
    return NULL;
}

napi_value node_igs_agent_state(napi_env env, napi_callback_info info) {
    char *state = igs_agent_state();
    napi_value state_js;
    convert_string_to_napi(env, state, &state_js);
    free(state);
    return state_js;
}

napi_value node_igs_agent_mute(napi_env env, napi_callback_info info) {
    igs_agent_mute();
    return NULL;
}

napi_value node_igs_agent_unmute(napi_env env, napi_callback_info info) {
    igs_agent_unmute();
    return NULL;
}

napi_value node_igs_agent_is_muted(napi_env env, napi_callback_info info) {
    bool is_muted = igs_agent_is_muted();
    napi_value is_muted_js;
    convert_bool_to_napi(env, is_muted, &is_muted_js);
    return is_muted_js;
}

static void cb_mute_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    mute_callback_args_t *callback_arg = (mute_callback_args_t *) data;
    napi_value argv[2];
    convert_bool_to_napi(env, callback_arg->agent_is_muted, &argv[0]);
    if (callback_arg->my_data_ref == NULL)
        convert_null_to_napi(env, &argv[1]);
    else {
        status = napi_get_reference_value(env, callback_arg->my_data_ref, &argv[1]);
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
    status = napi_call_function(env, undefined, js_callback, 2, argv, NULL);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Unable to call javascript function.");
}

void mute_fn(bool agent_is_muted, void *my_data) {
    threadsafe_context_t *threadsafe_context = (threadsafe_context_t *) my_data;
    mute_callback_args_t *callback_arg =  calloc(1, sizeof(mute_callback_args_t));
    callback_arg->agent_is_muted = agent_is_muted;
    callback_arg->my_data_ref = threadsafe_context->my_data_ref;
    napi_call_threadsafe_function(threadsafe_context->threadsafe_func, callback_arg, napi_tsfn_nonblocking);
}

napi_value node_igs_observe_mute(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);

    // Initiate threadsafe context
    threadsafe_context_t *threadsafe_context = calloc(1, sizeof(threadsafe_context_t));
    DL_APPEND(observe_mute_contexts, threadsafe_context);

    // Create threadsafe function
    napi_status status;
    napi_value async_name;
    status = napi_create_string_utf8(env, "ingescape/muteCallback", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Invalid name for async_name napi_value.");
    status = napi_create_threadsafe_function(env, argv[0], NULL, async_name, 0, 1, NULL, NULL, NULL,
    cb_mute_into_js, &(threadsafe_context->threadsafe_func));
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

    igs_observe_mute(mute_fn, threadsafe_context);
    return NULL;
}

napi_value node_igs_freeze(napi_env env, napi_callback_info info) {
    igs_freeze();
    return NULL;
}

napi_value node_igs_is_frozen(napi_env env, napi_callback_info info) {
    bool is_frozen = igs_is_frozen();
    napi_value is_frozen_js;
    convert_bool_to_napi(env, is_frozen, &is_frozen_js);
    return is_frozen_js;
}

napi_value node_igs_unfreeze(napi_env env, napi_callback_info info) {
    igs_unfreeze();
    return NULL;
}

static void cb_freeze_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    freeze_callback_args_t *callback_arg = (freeze_callback_args_t *) data;
    napi_value argv[2];
    convert_bool_to_napi(env, callback_arg->is_paused, &argv[0]);
    if (callback_arg->my_data_ref == NULL)
        convert_null_to_napi(env, &argv[1]);
    else {
        status = napi_get_reference_value(env, callback_arg->my_data_ref, &argv[1]);
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
    status = napi_call_function(env, undefined, js_callback, 2, argv, NULL);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Unable to call javascript function.");
}

void freeze_fn(bool is_paused, void *my_data) {
    threadsafe_context_t *threadsafe_context = (threadsafe_context_t *) my_data;
    freeze_callback_args_t * callback_arg =  calloc(1, sizeof(freeze_callback_args_t));
    callback_arg->is_paused = is_paused;
    callback_arg->my_data_ref = threadsafe_context->my_data_ref;
    napi_call_threadsafe_function(threadsafe_context->threadsafe_func, callback_arg, napi_tsfn_nonblocking);
}

napi_value node_igs_observe_freeze(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);

    // Initiate struct
    threadsafe_context_t *threadsafe_context = calloc(1, sizeof(threadsafe_context_t));
    DL_APPEND(observe_freeze_contexts, threadsafe_context);

    // Create threadsafe function
    napi_status status;
    napi_value async_name;
    status = napi_create_string_utf8(env, "ingescape/freezeCallback", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Invalid name for async_name napi_value.");
    status = napi_create_threadsafe_function(env, argv[0], NULL, async_name, 0, 1, NULL, NULL, NULL,
    cb_freeze_into_js, &(threadsafe_context->threadsafe_func));
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

    igs_observe_freeze(freeze_fn, threadsafe_context);
    return NULL;
}

static void cb_agent_event_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    agent_events_callback_args_t *callback_arg = (agent_events_callback_args_t *) data;
    napi_value argv[5];
    convert_int_to_napi(env, callback_arg->event, &argv[0]);
    convert_string_to_napi(env, callback_arg->uuid, &argv[1]);
    convert_string_to_napi(env, callback_arg->name, &argv[2]);
    if ((callback_arg->event == IGS_AGENT_WON_ELECTION) || (callback_arg->event == IGS_AGENT_LOST_ELECTION))
        convert_string_to_napi(env, (char*)callback_arg->event_data, &argv[3]);
    else
        convert_null_to_napi(env, &argv[3]);
    if (callback_arg->my_data_ref == NULL)
        convert_null_to_napi(env, &argv[4]);
    else {
        status = napi_get_reference_value(env, callback_arg->my_data_ref, &argv[4]);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to get reference value.");
    }
    free(callback_arg->uuid);
    free(callback_arg->name);
    free(callback_arg);

    // Since a function call must have a receiver, we use undefined
    napi_value undefined;
    status = napi_get_undefined(env, &undefined);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Impossible to get undefined.");

    // Callback into javascript
    status = napi_call_function(env, undefined, js_callback, 5, argv, NULL);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Unable to call javascript function.");
}

void agent_events_fn (igs_agent_event_t event, const char *uuid, const char *name, void* event_data, void *my_data) {
    threadsafe_context_t *threadsafe_context = (threadsafe_context_t *) my_data;
    agent_events_callback_args_t *callback_arg = calloc(1, sizeof(agent_events_callback_args_t));
    callback_arg->event = event;
    callback_arg->uuid = strdup(uuid);
    callback_arg->name = strdup(name);
    if ((event == IGS_AGENT_WON_ELECTION) || (event == IGS_AGENT_LOST_ELECTION))
        callback_arg->event_data = strdup((char*)event_data);
    else
        callback_arg->event_data = NULL;
    callback_arg->my_data_ref = threadsafe_context->my_data_ref;
    napi_call_threadsafe_function(threadsafe_context->threadsafe_func, callback_arg, napi_tsfn_nonblocking);
}

napi_value node_igs_observe_agent_events(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);

    // Initiate threadsafe context
    threadsafe_context_t * threadsafe_context = calloc(1, sizeof(threadsafe_context_t));
    DL_APPEND(observe_agent_events_contexts, threadsafe_context);

    napi_status status;
    napi_value async_name;
    status = napi_create_string_utf8(env, "ingescape/agentEventCallback", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Invalid name for async_name napi_value.");
    status = napi_create_threadsafe_function(env, argv[0], NULL, async_name, 0, 1, NULL, NULL, NULL,
    cb_agent_event_into_js, &(threadsafe_context->threadsafe_func));
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

    igs_observe_agent_events(agent_events_fn, threadsafe_context);
    return NULL;
}

napi_value init_init_control(napi_env env, napi_value exports) {
    exports = enable_callback_into_js(env, node_igs_start_with_device, "startWithDevice", exports);
    exports = enable_callback_into_js(env, node_igs_start_with_ip, "startWithIp", exports);
    exports = enable_callback_into_js(env, node_igs_stop, "stop", exports);
    exports = enable_callback_into_js(env, node_igs_is_started, "isStarted", exports);
    exports = enable_callback_into_js(env, node_igs_observe_forced_stop, "observeForcedStop", exports);
    exports = enable_callback_into_js(env, node_igs_agent_set_name, "agentSetName", exports);
    exports = enable_callback_into_js(env, node_igs_agent_name, "agentName", exports);
    exports = enable_callback_into_js(env, node_igs_agent_uuid, "agentUuid", exports);
    exports = enable_callback_into_js(env, node_igs_agent_set_state, "agentSetState", exports);
    exports = enable_callback_into_js(env, node_igs_agent_state, "agentState", exports);
    exports = enable_callback_into_js(env, node_igs_agent_mute, "agentMute", exports);
    exports = enable_callback_into_js(env, node_igs_agent_unmute, "agentUnmute", exports);
    exports = enable_callback_into_js(env, node_igs_agent_is_muted, "agentIsMuted", exports);
    exports = enable_callback_into_js(env, node_igs_observe_mute, "observeMute", exports);
    exports = enable_callback_into_js(env, node_igs_freeze, "freeze", exports);
    exports = enable_callback_into_js(env, node_igs_is_frozen, "isFrozen", exports);
    exports = enable_callback_into_js(env, node_igs_unfreeze, "unfreeze", exports);
    exports = enable_callback_into_js(env, node_igs_observe_freeze, "observeFreeze", exports);
    exports = enable_callback_into_js(env, node_igs_observe_agent_events, "observeAgentEvents", exports);
    return exports;
}
