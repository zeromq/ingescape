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

int mtic_start(const char *agentName, const char *networkDevice, int zyrePort, const char *channel);
int mtic_stop();
void mtic_send_definition();
void mtic_pause();
void mtic_resume();
bool mtic_toggle_play_pause();
void mtic_set_verbose(bool is_verbose);

#endif /* MTIC_NETWORK_H */
