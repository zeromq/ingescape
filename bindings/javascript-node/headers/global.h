//
//  global.h
//  wrapperNode
//
//  Created by Chloe Roumieu on 28/03/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include <node_api.h>
#include <ingescape/ingescape.h> 
#include <ingescape/ingescape_advanced.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef IOPTYPEIGS
#define IOPTYPEIGS
typedef enum {
    IGS_NUMBER_JS = 1,  ///< number value type
    IGS_STRING_JS,       ///< string value type
    IGS_BOOL_JS,         ///< bool value type
    IGS_IMPULSION_JS,    ///< impulsion value type
    IGS_DATA_JS,         ///< data value type
} iopType_js;
#endif

// convert N-API to C
char * convert_napi_to_string(napi_env env, napi_value value); //must be free by caller
int convert_napi_to_bool(napi_env env, napi_value value, bool * converted_value);
int convert_napi_to_int(napi_env env, napi_value value, int * value_converted);
int convert_napi_to_double(napi_env env, napi_value value, double * value_converted);
int convert_napi_to_data(napi_env env, napi_value value, void ** value_converted, size_t* size_value_converted);

// convert C to N-API
int convert_int_to_napi(napi_env env, int value, napi_value* value_converted);
int convert_bool_to_napi(napi_env env, bool value, napi_value* value_converted);
int convert_string_to_napi(napi_env env, const char * value, napi_value* value_converted);
int convert_double_to_napi(napi_env env, double value, napi_value* value_converted);
int convert_null_to_napi(napi_env env, napi_value* value_converted);
int convert_data_to_napi(napi_env env, void * value, size_t size, napi_value* value_converted);
int convert_string_list_to_napi_array(napi_env env, char ** list, size_t length, napi_value* value_converted);
// javascript callbacks 
int get_function_arguments(napi_env env, napi_callback_info info, size_t argc, napi_value * argv);
napi_value enable_callback_into_js(napi_env env, napi_callback cb, const char * js_name, napi_value exports);

//ioptype_t conversions
iopType_t get_iop_type_t_from_iop_type_js(iopType_js type_js);
iopType_js get_iop_type_js_from_iop_type_t(iopType_t type);

// utils Call
void getArrayJSFromCallArgumentList(napi_env env, igs_callArgument_t *firstArgument, napi_value *arrayJS);
void getCallArgumentListFromArrayJS(napi_env env, napi_value array, igs_callArgument_t **firstArgument);