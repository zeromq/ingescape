//
//  main.c
//  wrapperNode
//
//  Created by Chloe Roumieu on 27/03/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include "./headers/admin_config_utils.h"
#include "./headers/callback_igs.h"
#include "./headers/definition.h"
#include "./headers/init_control.h"
#include "./headers/iop.h"
#include "./headers/mapping.h"
#include "./headers/licenses.h"


// Allow calling into the add-on native code from JavaScript
napi_value Init(napi_env env, napi_value exports) {
	exports = init_admin_config_utils(env, exports);
	exports = init_callback_igs(env, exports);
	exports = init_initialization(env, exports);
	exports = init_definition(env, exports);
	exports = init_iop(env, exports);
	exports = init_mapping(env, exports);
	exports = init_licenses(env, exports);

	return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init);