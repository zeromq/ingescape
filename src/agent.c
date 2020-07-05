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

igs_agent_t *igsAgent_new(void){
    igs_agent_t *agent = calloc(1, sizeof(igs_agent_t));
    sprintf(agent->agentName, IGS_DEFAULT_AGENT_NAME);
    return agent;
}
void igsAgent_destroy(igs_agent_t **agent){
    //TODO: clean all agent structure properly
    free(*agent);
    *agent = NULL;
}



int igsAgent_activate(void){
    
}

int igsAgent_deactivate(void){
    
}

//void igsAgent_log(igs_logLevel_t level, const char *function, igs_agent_t *agent, const char *format, ...){
//    va_list list;
//    va_start(list, format);
//    char content[MAX_STRING_MSG_LENGTH] = "";
//    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, format, list);
//    va_end(list);
//    admin_log(agent, level, function, "%s", content);
//}
