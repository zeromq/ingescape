//
//  mapping.c
//  wrapperNode
//
//  Created by Chloe Roumieu on 14/05/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include "../headers/mapping.h"

// Wrapper for : 
// PUBLIC int igs_loadMapping (const char* json_str);
napi_value node_igs_loadMapping(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * json_str = convert_napi_to_string(env, argv[0]);

    // call igs function
    int res = igs_loadMapping(json_str);
    free(json_str);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_loadMappingFromPath (const char* file_path);
napi_value node_igs_loadMappingFromPath(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * file_path = convert_napi_to_string(env, argv[0]);

    // call igs function
    int res = igs_loadMappingFromPath(file_path);
    free(file_path);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_clearMapping(void); 
napi_value node_igs_clearMapping(napi_env env, napi_callback_info info) {
    // call igs function
    int res = igs_clearMapping();

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC char* igs_getMapping(void);
napi_value node_igs_getMapping(napi_env env, napi_callback_info info) {
    // call igs function
    char * res = igs_getMapping();

    // convert result into napi_value
    napi_value res_convert;
    if (res != NULL) {
        convert_string_to_napi(env, res, &res_convert);
        free(res);
    }
    else {
        convert_null_to_napi(env, &res_convert);
    }
    return res_convert;
}

// Wrapper for : 
// PUBLIC char *igs_getMappingName(void); 
napi_value node_igs_getMappingName(napi_env env, napi_callback_info info) {
    // call igs function
    char * res = igs_getMappingName();

    // convert result into napi_value
    napi_value res_convert;
    if (res != NULL) {
        convert_string_to_napi(env, res, &res_convert);
        free(res);
    }
    else {
        convert_null_to_napi(env, &res_convert);
    }
    return res_convert;
}

// Wrapper for : 
// PUBLIC char *igs_getMappingDescription(void); 
napi_value node_igs_getMappingDescription(napi_env env, napi_callback_info info) {
    // call igs function
    char * res = igs_getMappingDescription();

    // convert result into napi_value
    napi_value res_convert;
    if (res != NULL) {
        convert_string_to_napi(env, res, &res_convert);
        free(res);
    }
    else {
        convert_null_to_napi(env, &res_convert);
    }
    return res_convert;
}

// Wrapper for : 
// PUBLIC char *igs_getMappingVersion(void); 
napi_value node_igs_getMappingVersion(napi_env env, napi_callback_info info) {
    // call igs function
    char * res = igs_getMappingVersion();

    // convert result into napi_value
    napi_value res_convert;
    if (res != NULL) {
        convert_string_to_napi(env, res, &res_convert);
        free(res);
    }
    else {
        convert_null_to_napi(env, &res_convert);
    }
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_setMappingName(const char *name);
napi_value node_igs_setMappingName(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    int res = igs_setMappingName(name);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_setMappingDescription(const char *description);
napi_value node_igs_setMappingDescription(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * desc = convert_napi_to_string(env, argv[0]);

    // call igs function
    int res = igs_setMappingDescription(desc);
    free(desc);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_setMappingVersion(const char *version);
napi_value node_igs_setMappingVersion(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * version = convert_napi_to_string(env, argv[0]);

    // call igs function
    int res = igs_setMappingVersion(version);
    free(version);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_getMappingEntriesNumber(void); 
napi_value node_igs_getMappingEntriesNumber(napi_env env, napi_callback_info info) {
    // call igs function
    int res = igs_getMappingEntriesNumber();

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC unsigned long igs_addMappingEntry(const char *fromOurInput, const char *toAgent, const char *withOutput); 
// returns a string ID because napi doesn't convert unsigned long
napi_value node_igs_addMappingEntry(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 3;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * fromOurInput = convert_napi_to_string(env, argv[0]);
    char * toAgent = convert_napi_to_string(env, argv[1]);
    char * withOutput = convert_napi_to_string(env, argv[2]);

    // call igs function
    unsigned long res = igs_addMappingEntry(fromOurInput, toAgent, withOutput);
    free(fromOurInput);
    free(toAgent);
    free(withOutput);

    // convert unsigned long as char * because napi doesn't convert unsigned long
    char buffer[sizeof(unsigned long) * sizeof(res)];
    sprintf(buffer, "%lu", res);

    // convert result into napi_value
    napi_value res_convert;
    convert_string_to_napi(env, buffer, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_removeMappingEntryWithId(unsigned long theId);
// take a string as arg because napi doesn't convert unsigned long
napi_value node_igs_removeMappingEntryWithId(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * value = convert_napi_to_string(env, argv[0]);

    //convert char * as long 
    unsigned long value_long = strtoul(value, NULL, 10);

    // call igs function
    int res = igs_removeMappingEntryWithId(value_long); 
    free(value);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_removeMappingEntryWithName(const char *fromOurInput, const char *toAgent, const char *withOutput);
napi_value node_igs_removeMappingEntryWithName(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 3;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * fromOurInput = convert_napi_to_string(env, argv[0]);
    char * toAgent = convert_napi_to_string(env, argv[1]);
    char * withOutput = convert_napi_to_string(env, argv[2]);

    // call igs function
    int res = igs_removeMappingEntryWithName(fromOurInput, toAgent, withOutput);
    free(fromOurInput);
    free(toAgent);
    free(withOutput);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Allow callback for mapping ingescape code 
napi_value init_mapping(napi_env env, napi_value exports) {
    exports = enable_callback_into_js(env, node_igs_loadMapping, "loadMapping", exports);
    exports = enable_callback_into_js(env, node_igs_loadMappingFromPath, "loadMappingFromPath", exports);
    exports = enable_callback_into_js(env, node_igs_clearMapping, "clearMapping", exports);
    exports = enable_callback_into_js(env, node_igs_getMapping, "getMapping", exports);
    exports = enable_callback_into_js(env, node_igs_getMappingName, "getMappingName", exports);
    exports = enable_callback_into_js(env, node_igs_getMappingDescription, "getMappingDescription", exports);
    exports = enable_callback_into_js(env, node_igs_getMappingVersion, "getMappingVersion", exports);
    exports = enable_callback_into_js(env, node_igs_setMappingName, "setMappingName", exports);
    exports = enable_callback_into_js(env, node_igs_setMappingDescription, "setMappingDescription", exports);
    exports = enable_callback_into_js(env, node_igs_setMappingVersion, "setMappingVersion", exports);
    exports = enable_callback_into_js(env, node_igs_getMappingEntriesNumber, "getMappingEntriesNumber", exports);
    exports = enable_callback_into_js(env, node_igs_addMappingEntry, "addMappingEntry", exports);
    exports = enable_callback_into_js(env, node_igs_removeMappingEntryWithId, "removeMappingEntryWithId", exports);
    exports = enable_callback_into_js(env, node_igs_removeMappingEntryWithName, "removeMappingEntryWithName", exports);
    return exports;
}