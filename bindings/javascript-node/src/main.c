/*  =========================================================================
    main

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "../include/private.h"

// Allow calling into the add-on native code from JavaScript
napi_value Init(napi_env env, napi_value exports) {
	exports = init_admin_config_utils(env, exports);
	exports = init_advanced(env, exports);
	exports = init_agent(env, exports);
	exports = init_init_control(env, exports);
	exports = init_definition(env, exports);
	exports = init_iop(env, exports);
	exports = init_mapping(env, exports);
	exports = init_service(env, exports);
	exports = init_enum(env, exports);
	return exports;

	// FIXME: methods not wrapped :
	// zsock_t * igs_pipe_to_ingescape(void) not wrapped 

	// igs_result_t igs_output_set_zmsg(const char *name, zmsg_t *msg);
	// igs_result_t igs_input_zmsg(const char *name, zmsg_t **msg);

	// void igs_observe_channels(igs_channels_fn cb, void *my_data);
	// igs_result_t igs_channel_join(const char *channel);
	// void igs_channel_leave(const char *channel);

	// igs_result_t igs_channel_shout_str(const char *channel, const char *msg, ...);
	// igs_result_t igs_channel_shout_data(const char *channel, void *data, size_t size);
	// igs_result_t igs_channel_shout_zmsg(const char *channel, zmsg_t **msg_p);

	// igs_result_t igs_channel_whisper_str(const char *agent_name_or_agent_id_or_peer_id, const char *msg, ...);
	// igs_result_t igs_channel_whisper_data(const char *agent_name_or_agent_id_or_peer_id, void *data, size_t size);
	// igs_result_t igs_channel_whisper_zmsg(const char *agent_name_or_agent_id_or_peer_id, zmsg_t **msg_p); 

	// igs_result_t igs_peer_add_header(const char *key, const char *value);
	// igs_result_t igs_peer_remove_header(const char *key);

	// zactor_t * igs_zmq_authenticator(void);

	// igs_result_t igsagent_input_zmsg (igsagent_t *self, const char *name, zmsg_t **msg);
	// igs_result_t igsagent_output_set_zmsg (igsagent_t *self, const char *name, zmsg_t *value);
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init);
