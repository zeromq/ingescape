//
//  network.h
//
//  Created by Stephane Vales on 10/06/2016.
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

#ifndef NETWORK_H
#define NETWORK_H

#include "definition.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <Ws2tcpip.h>
    #include <iphlpapi.h>
#else
#endif

int publishOutput(const char* output_name);


/*
 * Function: masticStart
 * ----------------------------
 *   Start the network service
 *
 *   usage : masticStart(agentName, networkDevice, zyrePort,channel)
 *         example : masticStart("MASTIC_TEST", "en15", 5670, "MASTIC")
 *
 *   agentName          : agent name
 *   networkDevice      : device of connection
 *   zyrePort           : zyre port used
 *   channel            : name of the channel used to communication on zyre
 *
 *   returns : the state of the connection
 */
MASTICAPI_COMMON_DLLSPEC int masticStart(const char *agentName, const char *networkDevice, int zyrePort, const char *channel);

/*
 * Function: masticStop
 * ----------------------------
 *   Stop the network service
 *
 *   usage : masticStop()
 *
 *   returns : the state of the disconnection
 */
MASTICAPI_COMMON_DLLSPEC int masticStop();

/*
 * Function: subscribeTo
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
int subscribeTo(const char *agentName, const char *filterDescription);

/*
 * Function: unsubscribeTo
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
int unsubscribeTo(const char *agentName, const char *outputName);

/*
 * Function: mtic_sendDefinition
 * ----------------------------
 *   Send my definition through the agent publisher
 *   The agent definition must be defined : mtic_definition_loaded
 *
 *   usage : mtic_sendDefinition()
 *
 */
MASTICAPI_COMMON_DLLSPEC void mtic_sendDefinition();

/*
 * Function: publishOutput
 * ----------------------------
 *   Publishing output to the bus
 *
 *   usage : publishOutput(output_name)
 *
 */
int publishOutput(const char* output_name);

/*
 * Function: checkAndSubscribeTo
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
int checkAndSubscribeTo(const char* agentName);

/*
 * Function: masticPause
 * ----------------------------
 *   Pause the agent (all subscribtion and publish)
 *
 *   usage : masticPause()
 *
 *
 */
MASTICAPI_COMMON_DLLSPEC void masticPause();

/*
 * Function: masticResume
 * ----------------------------
 *   Resume the agent (all subscribtion and publish)
 *
 *   usage : masticResume()
 *
 *
 */
MASTICAPI_COMMON_DLLSPEC void masticResume();

/*
 * Function: masticTogglePlayPause
 * ----------------------------
 *   Play the agent if it is pauses, resume it otherwise.
 *
 *   usage : masticTogglePlayPause()
 *
 *   return : is_paused state
 *
 */
MASTICAPI_COMMON_DLLSPEC bool masticTogglePlayPause();

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

#endif /* network_h */
