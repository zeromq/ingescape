//
//  performance.c
//  ingescape
//
//  Created by Stephane Vales on 15/10/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <czmq.h>
#include "ingescape_advanced.h"
#include "ingescape_private.h"

size_t performanceMsgCounter = 0;
size_t performanceMsgCountTarget = 0;
size_t performanceMsgSize = 0;
int64_t performanceStart = 0;
int64_t performanceStop = 0;

void igs_performanceCheck(const char *peerId, size_t msgSize, size_t nbOfMsg){
    if (agentElements == NULL || agentElements->node == NULL){
        igs_error("agent must be started to execute performance tests");
        return;
    }
    if (performanceMsgCountTarget != 0){
        igs_error("check already in progress");
        return;
    }
    if (nbOfMsg == 0){
        igs_error("nbOfMsg must be greater than zero");
        return;
    }
    
    performanceMsgCountTarget = nbOfMsg;
    performanceMsgCounter = 1;
    performanceMsgSize = msgSize;
    
    zmsg_t *msg = zmsg_new();
    zmsg_addstr(msg, "PING");
    zmsg_addmem(msg, &performanceMsgCounter, sizeof(size_t));
    void *mem = malloc(msgSize);
    zmsg_addmem(msg, mem, msgSize);
    zyre_t *node = agentElements->node;
    
    performanceStart = zclock_usecs();
    bus_zyreLock();
    zyre_whisper(node, peerId, &msg);
    bus_zyreUnlock();
    free(mem);
}
