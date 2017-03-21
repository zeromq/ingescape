//
//  mtic_network.h
//
//  Created by Stephane Vales on 10/06/2016.
//  Modified by Vincent Deliencourt
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

#ifndef MTIC_NETWORK_H
#define MTIC_NETWORK_H

#include "definition.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <Ws2tcpip.h>
    #include <iphlpapi.h>
#else
#endif

int publish_output(const char* output_name);


/*
 * Function: mtic_start
 * ----------------------------
 *   Start the network service
 *
 *   usage : mtic_masticStart(agentName, networkDevice, zyrePort,channel)
 *         example : mtic_masticStart("MASTIC_TEST", "en15", 5670, "MASTIC")
 *
 *   agentName          : agent name
 *   networkDevice      : device of connection
 *   zyrePort           : zyre port used
 *   channel            : name of the channel used to communication on zyre
 *
 *   returns : the state of the connection
 */
MASTICAPI_COMMON_DLLSPEC int mtic_start(const char *agentName, const char *networkDevice, int zyrePort, const char *channel);

/*
 * Function: mtic_stop
 * ----------------------------
 *   Stop the network service
 *
 *   usage : mtic_masticStop()
 *
 *   returns : the state of the disconnection
 */
MASTICAPI_COMMON_DLLSPEC int mtic_stop();

/*
 * Function: subscribe_to
 * ----------------------------
 *   Add a subscription to an agent
 *
 *   usage : subscribeTo(const char *agentName, const char *filterDescription)
 *
 *   agentName          : agent name
 *   filterDescription  : filter description (ex: "moduletest.output1")
 *
 *   returns : the state of the subscribtion
 */
int subscribe_to(const char *agentName, const char *filterDescription);

/*
 * Function: unsubscribe_to
 * ----------------------------
 *   Remove a subscription to an agent
 *
 *   usage : unsubscribeTo(const char *agentName, const char *filterDescription)
 *
 *   agentName          : agent name
 *   filterDescription  : filter description (ex: "moduletest.output1")
 *
 *   returns : the state of the subscribtion removal
 */
int unsubscribe_to(const char *agentName, const char *outputName);

/*
 * Function: mtic_send_definition
 * ----------------------------
 *   Send my definition through the agent publisher
 *   The agent definition must be defined : mtic_definition_loaded
 *
 *   usage : mtic_sendDefinition()
 *
 */
MASTICAPI_COMMON_DLLSPEC void mtic_send_definition();

/*
 * Function: publish_output
 * ----------------------------
 *   Publishing output to the bus
 *
 *   usage : publishOutput(output_name)
 *
 */
int publish_output(const char* output_name);

/*
 * Function: check_and_subscribe_to
 * ----------------------------
 *   Check mappings made on agent name
 *   Connect to these outputs if compatibility is ok
 *
 *   usage : checkAndSubscribeTo(char* agentName)
 *
 *   agentName          : agent name
 *
 *   returns : -1 if an error occured, 1 otherwise.
 *
 */
int check_and_subscribe_to(const char* agentName);

/*
 * Function: mtic_pause
 * ----------------------------
 *   Pause the agent (all subscribtion and publish)
 *
 *   usage : mtic_masticPause()
 *
 *
 */
MASTICAPI_COMMON_DLLSPEC void mtic_pause();

/*
 * Function: mtic_resume
 * ----------------------------
 *   Resume the agent (all subscribtion and publish)
 *
 *   usage : mtic_masticResume()
 *
 *
 */
MASTICAPI_COMMON_DLLSPEC void mtic_resume();

/*
 * Function: mtic_toggle_play_pause
 * ----------------------------
 *   Play the agent if it is pauses, resume it otherwise.
 *
 *   usage : mtic_masticTogglePlayPause()
 *
 *   return : is_paused state
 *
 */
MASTICAPI_COMMON_DLLSPEC bool mtic_toggle_play_pause();

/*
 * Function: mtic_set_verbose
 * ----------------------------
 *   Set verbose mode
 *
 *   usage : mtic_set_verbose(is_verbose)
 *
 */
MASTICAPI_COMMON_DLLSPEC void mtic_set_verbose(bool is_verbose);

/*
 * Function: debug
 * ----------------------------
 *   trace debug messages only on verbose mode
 *   Print file name and line number in debug mode
 *
 *   usage : mtic_debug(format,msg)
 *
 */
MASTICAPI_COMMON_DLLSPEC void mtic_debug(const char *fmt, ...);

#endif /* MTIC_NETWORK_H */
