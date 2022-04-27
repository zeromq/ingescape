/*  =========================================================================
    definition - create/edit/delete definitions

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "../include/private.h"

napi_value node_igs_definition_load_str(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *json_str = convert_napi_to_string(env, argv[0]);
    int success = igs_definition_load_str(json_str);
    if (success == IGS_SUCCESS) {
        // Free old associated IOPC callback data
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
    }
    free(json_str);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_definition_load_file(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *file_path = convert_napi_to_string(env, argv[0]);
    int success = igs_definition_load_file(file_path);
    if (success == IGS_SUCCESS) {
        // Free old associated IOPC callback data
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
    }
    free(file_path);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_clear_definition(napi_env env, napi_callback_info info) {
    igs_clear_definition();
    // Free associated IOPC callback data 
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
    return NULL;
}

napi_value node_igs_definition_json(napi_env env, napi_callback_info info) {
    char *json_definition = igs_definition_json();
    napi_value json_definition_js;
    convert_string_to_napi(env, json_definition, &json_definition_js);
    free(json_definition);
    return json_definition_js;
}

napi_value node_igs_definition_description(napi_env env, napi_callback_info info) {
    char *description = igs_definition_description();
    napi_value description_js;
    convert_string_to_napi(env, description, &description_js);
    free(description);
    return description_js;
}

napi_value node_igs_definition_version(napi_env env, napi_callback_info info) {
    char *version = igs_definition_version();
    napi_value version_js;
    convert_string_to_napi(env, version, &version_js);
    free(version);
    return version_js;
}

napi_value node_igs_definition_set_description(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *description = convert_napi_to_string(env, argv[0]);
    igs_definition_set_description(description);
    free(description);
    return NULL;
}

napi_value node_igs_definition_set_version(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *version = convert_napi_to_string(env, argv[0]);
    igs_definition_set_version(version);
    free(version); 
    return NULL;
}

napi_value node_igs_input_create(napi_env env, napi_callback_info info) {
    napi_value argv[3];
    get_function_arguments(env, info, 3, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    int type, success;
    convert_napi_to_int(env, argv[1], &type);
    size_t size;
    void *c_value = convert_value_with_good_type(env, argv[2], type, &size);
    success = igs_input_create(name, type, c_value, size);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_output_create(napi_env env, napi_callback_info info) {
    napi_value argv[3];
    get_function_arguments(env, info, 3, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    int type, success;
    convert_napi_to_int(env, argv[1], &type);
    size_t size;
    void *c_value = convert_value_with_good_type(env, argv[2], type, &size);
    success = igs_output_create(name, type, c_value, size);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_parameter_create(napi_env env, napi_callback_info info) {
    napi_value argv[3];
    get_function_arguments(env, info, 3, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    int type = 0, success;
    convert_napi_to_int(env, argv[1], &type);
    size_t size;
    void *c_value = convert_value_with_good_type(env, argv[2], type, &size);
    success = igs_parameter_create(name, type, c_value, size);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_input_remove(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    int success = igs_input_remove(name);
    if (success == IGS_SUCCESS) {
        // Free associated callback data
        threadsafe_context_hash_t *threadsafe_context_hash = NULL;
        HASH_FIND_STR (observed_input_contexts, name, threadsafe_context_hash);
        if (threadsafe_context_hash != NULL) {
            HASH_DEL (observed_input_contexts, threadsafe_context_hash);
            free_threadsafe_context_hash(env, &threadsafe_context_hash);
        }
    }
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_output_remove(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    int success = igs_output_remove(name);
    if (success == IGS_SUCCESS) {
        // Free associated callback data
        threadsafe_context_hash_t *threadsafe_context_hash = NULL;
        HASH_FIND_STR (observed_output_contexts, name, threadsafe_context_hash);
        if (threadsafe_context_hash != NULL) {
            HASH_DEL (observed_output_contexts, threadsafe_context_hash);
            free_threadsafe_context_hash(env, &threadsafe_context_hash);
        }
    }
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_parameter_remove(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    int success = igs_parameter_remove(name);
    if (success == IGS_SUCCESS) {
        // Free associated callback data
        threadsafe_context_hash_t *threadsafe_context_hash = NULL;
        HASH_FIND_STR (observed_parameter_contexts, name, threadsafe_context_hash);
        if (threadsafe_context_hash != NULL) {
            HASH_DEL (observed_parameter_contexts, threadsafe_context_hash);
            free_threadsafe_context_hash(env, &threadsafe_context_hash);
        }
    }
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}


napi_value init_definition(napi_env env, napi_value exports) {
    exports = enable_callback_into_js(env, node_igs_definition_load_str, "definitionLoadStr", exports);
    exports = enable_callback_into_js(env, node_igs_definition_load_file, "definitionLoadFile", exports);
    exports = enable_callback_into_js(env, node_igs_clear_definition, "clearDefinition", exports);
    exports = enable_callback_into_js(env, node_igs_definition_json, "definitionJson", exports);
    exports = enable_callback_into_js(env, node_igs_definition_description, "definitionDescription", exports);
    exports = enable_callback_into_js(env, node_igs_definition_version, "definitionVersion", exports);
    exports = enable_callback_into_js(env, node_igs_definition_set_description, "definitionSetDescription", exports);
    exports = enable_callback_into_js(env, node_igs_definition_set_version, "definitionSetVersion", exports);
    exports = enable_callback_into_js(env, node_igs_input_create, "inputCreate", exports);
    exports = enable_callback_into_js(env, node_igs_output_create, "outputCreate", exports);
    exports = enable_callback_into_js(env, node_igs_parameter_create, "parameterCreate", exports);
    exports = enable_callback_into_js(env, node_igs_input_remove, "inputRemove", exports);
    exports = enable_callback_into_js(env, node_igs_output_remove, "outputRemove", exports);
    exports = enable_callback_into_js(env, node_igs_parameter_remove, "parameterRemove", exports);
    return exports;
}
