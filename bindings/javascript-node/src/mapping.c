/*  =========================================================================
    mapping - create/edit/delete mappings

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "../include/private.h"

napi_value node_igs_mapping_load_str(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *json_str = convert_napi_to_string(env, argv[0]);
    int success = igs_mapping_load_str(json_str);
    free(json_str);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_mapping_load_file(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *file_path = convert_napi_to_string(env, argv[0]);
    int success = igs_mapping_load_file(file_path);
    free(file_path);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_clear_mappings(napi_env env, napi_callback_info info) {
    igs_clear_mappings();
    return NULL;
}

napi_value node_igs_clear_mappings_with_agent(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *agent_name = convert_napi_to_string(env, argv[0]);
    igs_clear_mappings_with_agent(agent_name);
    free(agent_name);
    return NULL;
}

napi_value node_igs_mapping_json(napi_env env, napi_callback_info info) {
    char *json = igs_mapping_json();
    napi_value json_js;
    convert_string_to_napi(env, json, &json_js);
    free(json);
    return json_js;
}

napi_value node_igs_mapping_count(napi_env env, napi_callback_info info) {
    int count = igs_mapping_count();
    napi_value count_js;
    convert_int_to_napi(env, count, &count_js);
    return count_js;
}

napi_value node_igs_mapping_add(napi_env env, napi_callback_info info) {
    napi_value argv[3];
    get_function_arguments(env, info, 3, argv);
    char *from_our_input = convert_napi_to_string(env, argv[0]);
    char *to_agent = convert_napi_to_string(env, argv[1]);
    char *with_output = convert_napi_to_string(env, argv[2]);
    uint64_t id = igs_mapping_add(from_our_input, to_agent, with_output);
    free(from_our_input);
    free(to_agent);
    free(with_output);

    napi_value id_js;
    convert_uint64_to_napi(env, id, &id_js);
    return id_js;
}

napi_value node_igs_mapping_remove_with_id(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    uint64_t id;
    convert_napi_to_uint64(env, argv[0], &id);
    int success = igs_mapping_remove_with_id(id); 
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_mapping_remove_with_name(napi_env env, napi_callback_info info) {
    napi_value argv[3];
    get_function_arguments(env, info, 3, argv);
    char *from_our_input = convert_napi_to_string(env, argv[0]);
    char *to_agent = convert_napi_to_string(env, argv[1]);
    char *with_output = convert_napi_to_string(env, argv[2]);
    int success = igs_mapping_remove_with_name(from_our_input, to_agent, with_output);
    free(from_our_input);
    free(to_agent);
    free(with_output);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_split_count(napi_env env, napi_callback_info info) {
    int count = igs_split_count();
    napi_value count_js;
    convert_int_to_napi(env, count, &count_js);
    return count_js;
}

napi_value node_igs_split_add(napi_env env, napi_callback_info info) {
    napi_value argv[3];
    get_function_arguments(env, info, 3, argv);
    char *from_our_input = convert_napi_to_string(env, argv[0]);
    char *to_agent = convert_napi_to_string(env, argv[1]);
    char *with_output = convert_napi_to_string(env, argv[2]);
    uint64_t id = igs_split_add(from_our_input, to_agent, with_output);
    free(from_our_input);
    free(to_agent);
    free(with_output);
    napi_value id_js;
    convert_uint64_to_napi(env, id, &id_js);
    return id_js;
}

napi_value node_igs_split_remove_with_id(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    uint64_t id;
    convert_napi_to_uint64(env, argv[0], &id);
    int success = igs_split_remove_with_id(id); 
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_split_remove_with_name(napi_env env, napi_callback_info info) {
    napi_value argv[3];
    get_function_arguments(env, info, 3, argv);
    char *from_our_input = convert_napi_to_string(env, argv[0]);
    char *to_agent = convert_napi_to_string(env, argv[1]);
    char *with_output = convert_napi_to_string(env, argv[2]);
    int success = igs_split_remove_with_name(from_our_input, to_agent, with_output);
    free(from_our_input);
    free(to_agent);
    free(with_output);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value init_mapping(napi_env env, napi_value exports) {
    exports = enable_callback_into_js(env, node_igs_mapping_load_str, "mappingLoadStr", exports);
    exports = enable_callback_into_js(env, node_igs_mapping_load_file, "mappingLoadFile", exports);
    exports = enable_callback_into_js(env, node_igs_clear_mappings, "clearMappings", exports);
    exports = enable_callback_into_js(env, node_igs_clear_mappings_with_agent, "clearMappingsWithAgent", exports);    
    exports = enable_callback_into_js(env, node_igs_mapping_json, "mappingJson", exports);
    exports = enable_callback_into_js(env, node_igs_mapping_count, "mappingCount", exports);
    exports = enable_callback_into_js(env, node_igs_mapping_add, "mappingAdd", exports);
    exports = enable_callback_into_js(env, node_igs_mapping_remove_with_id, "mappingRemoveWithId", exports);
    exports = enable_callback_into_js(env, node_igs_mapping_remove_with_name, "mappingRemoveWithName", exports);
    exports = enable_callback_into_js(env, node_igs_split_count, "splitCount", exports);
    exports = enable_callback_into_js(env, node_igs_split_add, "splitAdd", exports);
    exports = enable_callback_into_js(env, node_igs_split_remove_with_id, "splitRemoveWithId", exports);
    exports = enable_callback_into_js(env, node_igs_split_remove_with_name, "splitRemoveWithName", exports);
    return exports;
}
