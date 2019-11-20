//
//  agent.c
//  ingescape
//
//  Created by Stephane Vales on 19/11/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "ingescape_private.h"
#include "ingescape_agent.h"

igsAgent_t *igsAgent_new(void){
    igsAgent_t *agent = calloc(1, sizeof(igsAgent_t));
    agent->network_discoveryInterval = 1000;
    agent->network_agentTimeout = 30000;
    agent->network_hwmValue = 1000;
    return agent;
}
void igsAgent_destroy(igsAgent_t **agent){
    igsAgent_stop(*agent);
    free(*agent);
    *agent = NULL;
}
