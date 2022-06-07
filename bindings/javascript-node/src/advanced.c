/*  =========================================================================
    advanced - brokers, security, elections, advanced network config, 
               performance check, agent family, network monitoring, 
               logs replay and clean global context

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "../include/private.h"

threadsafe_context_t *observe_monitor_contexts = NULL;

// Brokers
napi_value node_igs_broker_add(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *broker_endpoint = convert_napi_to_string(env, argv[0]);
    int success = igs_broker_add(broker_endpoint);
    free(broker_endpoint);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_clear_brokers(napi_env env, napi_callback_info info) {
    igs_clear_brokers();
    return NULL;
}

napi_value node_igs_broker_enable_with_endpoint(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *broker_endpoint = convert_napi_to_string(env, argv[0]);
    igs_broker_enable_with_endpoint(broker_endpoint);
    free(broker_endpoint);
    return NULL;
}

napi_value node_igs_broker_set_advertized_endpoint(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    napi_status status;
    napi_valuetype arg_type;
    char *advertized_endpoint = NULL;
    
    status = napi_typeof(env, argv[0], &arg_type);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get napi value type.");
    if ((arg_type != napi_null) && (arg_type != napi_undefined))
        advertized_endpoint = convert_napi_to_string(env, argv[0]);

    igs_broker_set_advertized_endpoint(advertized_endpoint);
    if (advertized_endpoint != NULL)
        free(advertized_endpoint);
    return NULL;
}

napi_value node_igs_start_with_brokers(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *agent_endpoint = convert_napi_to_string(env, argv[0]);
    int success = igs_start_with_brokers(agent_endpoint);
    free(agent_endpoint);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}


// Security
napi_value node_igs_enable_security(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    napi_status status;
    napi_valuetype arg_type;
    char *private_certificate_file = NULL;
    char *public_certificates_directory = NULL;

    status = napi_typeof(env, argv[0], &arg_type);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get napi value type.");
    if ((arg_type != napi_null) && (arg_type != napi_undefined))
        private_certificate_file = convert_napi_to_string(env, argv[0]);

    status = napi_typeof(env, argv[1], &arg_type);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get napi value type.");
    if ((arg_type != napi_null) && (arg_type != napi_undefined))
        public_certificates_directory = convert_napi_to_string(env, argv[1]);

    int success = igs_enable_security(private_certificate_file, public_certificates_directory);
    if (private_certificate_file != NULL)
        free(private_certificate_file);
    if (public_certificates_directory != NULL)
        free(public_certificates_directory);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_disable_security(napi_env env, napi_callback_info info) {
    igs_disable_security();
    return NULL;
}

napi_value node_igs_broker_add_secure(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *broker_endpoint = convert_napi_to_string(env, argv[0]);
    char *public_certificates_directory = convert_napi_to_string(env, argv[1]);
    int success = igs_broker_add_secure(broker_endpoint, public_certificates_directory);
    free(broker_endpoint);
    free(public_certificates_directory);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}


// Elections between agents
napi_value node_igs_election_join(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *election_name = convert_napi_to_string(env, argv[0]);
    int success = igs_election_join(election_name);
    free(election_name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_election_leave(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *election_name = convert_napi_to_string(env, argv[0]);
    int success = igs_election_leave(election_name);
    free(election_name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}


// Network configuration
napi_value node_igs_net_set_publishing_port(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    int publishing_port;
    convert_napi_to_int(env, argv[0], &publishing_port);
    igs_net_set_publishing_port(publishing_port);
    return NULL;
}

napi_value node_igs_net_set_log_stream_port(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    int log_stream_port;
    convert_napi_to_int(env, argv[0], &log_stream_port);
    igs_net_set_log_stream_port(log_stream_port);
    return NULL;
}

napi_value node_igs_net_set_discovery_interval(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    int discovery_interval;
    convert_napi_to_int(env, argv[0], &discovery_interval);
    igs_net_set_discovery_interval(discovery_interval);
    return NULL;
}

napi_value node_igs_net_set_timeout(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    int timeout;
    convert_napi_to_int(env, argv[0], &timeout);
    igs_net_set_timeout(timeout);
    return NULL;
}

napi_value node_igs_net_raise_sockets_limit(napi_env env, napi_callback_info info) {
    igs_net_raise_sockets_limit();
    return NULL;
}

napi_value node_igs_net_set_high_water_marks(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    int hwm_value;
    convert_napi_to_int(env, argv[0], &hwm_value);
    igs_net_set_high_water_marks(hwm_value);
    return NULL;
}


// Performance check
napi_value node_igs_net_performance_check(napi_env env, napi_callback_info info) {
    napi_value argv[3];
    get_function_arguments(env, info, 3, argv);
    char *peer_id = convert_napi_to_string(env, argv[0]);
    int msg_size, msg_nb;
    convert_napi_to_int(env, argv[1], &msg_size);
    convert_napi_to_int(env, argv[2], &msg_nb);
    igs_net_performance_check(peer_id, msg_size, msg_nb);
    free(peer_id);
    return NULL;
}


// Agent family
napi_value node_igs_agent_set_family(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *family = convert_napi_to_string(env, argv[0]);
    igs_agent_set_family(family);
    free(family);
    return NULL;
}

napi_value node_igs_agent_family(napi_env env, napi_callback_info info) {
    char *family = igs_agent_family();
    napi_value family_js;
    convert_string_to_napi(env, family, &family_js);
    free(family);
    return family_js;
}


// Network monitoring
napi_value node_igs_monitor_start(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    int period;
    convert_napi_to_int(env, argv[0], &period);
    igs_monitor_start(period);
    return NULL;
}

napi_value node_igs_monitor_start_with_network(napi_env env, napi_callback_info info) {
    napi_value argv[3];
    get_function_arguments(env, info, 3, argv);
    int period, port;
    char *network_device = convert_napi_to_string(env, argv[1]);
    convert_napi_to_int(env, argv[0], &period);
    convert_napi_to_int(env, argv[2], &port);
    igs_monitor_start_with_network(period, network_device, port);
    free(network_device);
    return NULL;
}

napi_value node_igs_monitor_stop(napi_env env, napi_callback_info info) {
    igs_monitor_stop();
    return NULL;
}

napi_value node_igs_monitor_is_running(napi_env env, napi_callback_info info) {
    bool monitor_is_running = igs_monitor_is_running();
    napi_value monitor_is_running_js;
    convert_bool_to_napi(env, monitor_is_running, &monitor_is_running_js);
    return monitor_is_running_js; 
}

napi_value node_igs_monitor_set_start_stop(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    bool flag;
    convert_napi_to_bool(env, argv[0], &flag);
    igs_monitor_set_start_stop(flag);
    return NULL;
}

static void cb_monitor_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    monitor_callback_args_t * callback_arg = (monitor_callback_args_t *) data;
    napi_value argv[4];
    convert_int_to_napi(env, callback_arg->event, &argv[0]);
    convert_string_to_napi(env, callback_arg->device, &argv[1]);
    convert_string_to_napi(env, callback_arg->ip_address, &argv[2]);
    if (callback_arg->my_data_ref == NULL)
        convert_null_to_napi(env, &argv[3]);
    else {
        status = napi_get_reference_value(env, callback_arg->my_data_ref, &argv[3]);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to get reference value.");
    }

    free(callback_arg->device);
    free(callback_arg->ip_address);
    free(callback_arg);
 
    // Since a function call must have a receiver, we use undefined
    napi_value undefined;
    status = napi_get_undefined(env, &undefined);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Impossible to get undefined.");

    // Callback into javascript
    status = napi_call_function(env, undefined, js_callback, 4, argv, NULL);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Unable to call javascript function.");
}

void monitor_fn (igs_monitor_event_t event, const char *device, const char *ip_address, void *my_data) {
    threadsafe_context_t *threadsafe_context = (threadsafe_context_t *) my_data;
    monitor_callback_args_t *callback_arg = calloc(1, sizeof(monitor_callback_args_t)); 
    callback_arg->event = event;
    callback_arg->device = strdup(device);
    callback_arg->ip_address = strdup(ip_address);
    callback_arg->my_data_ref = threadsafe_context->my_data_ref;
    napi_call_threadsafe_function(threadsafe_context->threadsafe_func, callback_arg, napi_tsfn_nonblocking);
}   

napi_value node_igs_observe_monitor(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);

    // Initiate threadsafe context
    threadsafe_context_t *threadsafe_context = calloc(1, sizeof(threadsafe_context_t));
    DL_APPEND(observe_monitor_contexts, threadsafe_context);

    napi_status status;
    napi_value async_name;
    status = napi_create_string_utf8(env, "ingescape/monitorCallback", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Invalid name for async_name napi_value.");
    status = napi_create_threadsafe_function(env, argv[0], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cb_monitor_into_js, &(threadsafe_context->threadsafe_func));
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

    igs_observe_monitor(monitor_fn, threadsafe_context);
    return NULL;
}

// Logs replay
napi_value node_igs_replay_init(napi_env env, napi_callback_info info) {
    napi_value argv[6];
    get_function_arguments(env, info, 6, argv);
    
    int speed, replay_mode;
    bool wait_for_start;
    char *log_file_path = convert_napi_to_string(env, argv[0]),
         *start_time = convert_napi_to_string(env, argv[2]),
         *agent = convert_napi_to_string(env, argv[5]);
    convert_napi_to_int(env, argv[1], &speed);
    convert_napi_to_bool(env, argv[3], &wait_for_start);
    convert_napi_to_int(env, argv[4], &replay_mode);
    igs_replay_init(log_file_path, speed, start_time, wait_for_start, replay_mode, agent);
    free(log_file_path);
    free(start_time);
    free(agent);
    return NULL;
}

napi_value node_igs_replay_start(napi_env env, napi_callback_info info) {
    igs_replay_start();
    return NULL;
}

napi_value node_igs_replay_pause(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    bool pause;
    convert_napi_to_bool(env, argv[0], &pause);
    igs_replay_pause(pause);
    return NULL;
}

napi_value node_igs_replay_terminate(napi_env env, napi_callback_info info) {
    igs_replay_terminate();
    return NULL;
}

napi_value node_igs_clear_context(napi_env env, napi_callback_info info) {
    igs_clear_context();
    threadsafe_context_hash_t *threadsafe_context_hash, *threadsafe_context_hash_tmp;
    HASH_ITER (hh, observed_input_contexts, threadsafe_context_hash, threadsafe_context_hash_tmp) {
        HASH_DEL (observed_input_contexts, threadsafe_context_hash);
        free_threadsafe_context_hash(env, &threadsafe_context_hash);
    }
    HASH_ITER (hh, observed_output_contexts, threadsafe_context_hash, threadsafe_context_hash_tmp) {
        HASH_DEL (observed_output_contexts, threadsafe_context_hash);
        free_threadsafe_context_hash(env, &threadsafe_context_hash);
    }
    HASH_ITER (hh, observed_parameter_contexts, threadsafe_context_hash, threadsafe_context_hash_tmp) {
        HASH_DEL (observed_parameter_contexts, threadsafe_context_hash);
        free_threadsafe_context_hash(env, &threadsafe_context_hash);
    }
    HASH_ITER (hh, service_contexts, threadsafe_context_hash, threadsafe_context_hash_tmp) {
        HASH_DEL (service_contexts, threadsafe_context_hash);
        free_threadsafe_context_hash(env, &threadsafe_context_hash);
    }

    threadsafe_context_t *threadsafe_context, *threadsafe_context_tmp;
    LL_FOREACH_SAFE (observe_mute_contexts, threadsafe_context, threadsafe_context_tmp) {
        LL_DELETE (observe_mute_contexts, threadsafe_context);
        free_threadsafe_context(env, &threadsafe_context);
    }
    LL_FOREACH_SAFE (observe_agent_events_contexts, threadsafe_context, threadsafe_context_tmp) {
        LL_DELETE (observe_agent_events_contexts, threadsafe_context);
        free_threadsafe_context(env, &threadsafe_context);
    }
    LL_FOREACH_SAFE (observe_freeze_contexts, threadsafe_context, threadsafe_context_tmp) {
        LL_DELETE (observe_freeze_contexts, threadsafe_context);
        free_threadsafe_context(env, &threadsafe_context);
    }
    LL_FOREACH_SAFE (observe_forced_stop_contexts, threadsafe_context, threadsafe_context_tmp) {
        LL_DELETE (observe_forced_stop_contexts, threadsafe_context);
        free_threadsafe_context(env, &threadsafe_context);
    }
    LL_FOREACH_SAFE (observe_monitor_contexts, threadsafe_context, threadsafe_context_tmp) {
        LL_DELETE (observe_monitor_contexts, threadsafe_context);
        free_threadsafe_context(env, &threadsafe_context);
    }
    LL_FOREACH_SAFE (observe_monitor_contexts, threadsafe_context, threadsafe_context_tmp) {
        LL_DELETE (timer_contexts, threadsafe_context);
        free_threadsafe_context(env, &threadsafe_context);
    }
    return NULL;
}

napi_value init_advanced(napi_env env, napi_value exports) {
    // Brokers
    exports = enable_callback_into_js(env, node_igs_broker_add, "brokerAdd", exports);
    exports = enable_callback_into_js(env, node_igs_clear_brokers, "clearBrokers", exports);
    exports = enable_callback_into_js(env, node_igs_broker_enable_with_endpoint, "brokerEnableWithEndpoint", exports);
    exports = enable_callback_into_js(env, node_igs_broker_set_advertized_endpoint, "brokerSetAdvertizedEndpoint", exports);
    exports = enable_callback_into_js(env, node_igs_start_with_brokers, "startWithBrokers", exports);
    // Security
    exports = enable_callback_into_js(env, node_igs_enable_security, "enableSecurity", exports);
    exports = enable_callback_into_js(env, node_igs_disable_security, "disableSecurity", exports);
    exports = enable_callback_into_js(env, node_igs_broker_add_secure, "brokerAddSecure", exports);
    // Elections between agents
    exports = enable_callback_into_js(env, node_igs_election_join, "electionJoin", exports);
    exports = enable_callback_into_js(env, node_igs_election_leave, "electionLeave", exports);
    // Network configuration
    exports = enable_callback_into_js(env, node_igs_net_set_publishing_port, "netSetPublishingPort", exports);
    exports = enable_callback_into_js(env, node_igs_net_set_log_stream_port, "netSetLogStreamPort", exports);
    exports = enable_callback_into_js(env, node_igs_net_set_discovery_interval, "netSetDiscoveryInterval", exports);
    exports = enable_callback_into_js(env, node_igs_net_set_timeout, "netSetTimeout", exports);
    exports = enable_callback_into_js(env, node_igs_net_raise_sockets_limit, "netRaiseSocketsLimit", exports);
    exports = enable_callback_into_js(env, node_igs_net_set_high_water_marks, "netSetHighWaterMarks", exports);
    // Performance check
    exports = enable_callback_into_js(env, node_igs_net_performance_check, "netPerformanceCheck", exports);
    // Agent family
    exports = enable_callback_into_js(env, node_igs_agent_set_family, "agentSetFamily", exports);
    exports = enable_callback_into_js(env, node_igs_agent_family, "agentFamily", exports);
    // Network monitoring
    exports = enable_callback_into_js(env, node_igs_monitor_start, "monitorStart", exports);
    exports = enable_callback_into_js(env, node_igs_monitor_start_with_network, "monitorStartWithNetwork", exports);
    exports = enable_callback_into_js(env, node_igs_monitor_stop, "monitorStop", exports);
    exports = enable_callback_into_js(env, node_igs_monitor_is_running, "monitorIsRunning", exports);
    exports = enable_callback_into_js(env, node_igs_monitor_set_start_stop, "monitorSetStartStop", exports);
    exports = enable_callback_into_js(env, node_igs_observe_monitor, "observeMonitor", exports);
    // Logs replay
    exports = enable_callback_into_js(env, node_igs_replay_init, "replayInit", exports);
    exports = enable_callback_into_js(env, node_igs_replay_start, "replayStart", exports);
    exports = enable_callback_into_js(env, node_igs_replay_pause, "replayPause", exports);
    exports = enable_callback_into_js(env, node_igs_replay_terminate, "replayTerminate", exports);
    // Clean context
    exports = enable_callback_into_js(env, node_igs_clear_context, "clearContext", exports);
    return exports;
}
