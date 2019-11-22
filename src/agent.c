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
    agent->allowIpc = true;
    agent->allowInproc = true;
    return agent;
}
void igsAgent_destroy(igsAgent_t **agent){
    igsAgent_stop(*agent);
    free(*agent);
    *agent = NULL;
}

void igsAgent_trace2(const char *function, igsAgent_t *agent, const char *format, ...){
    va_list list;
    va_start(list, format);
    char content[MAX_STRING_MSG_LENGTH] = "";
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, format, list);
    va_end(list);
    igs_log(agent->agentName, IGS_LOG_TRACE, function, "%s", content);
}
void igsAgent_debug2(const char *function, igsAgent_t *agent, const char *format, ...){
    va_list list;
    va_start(list, format);
    char content[MAX_STRING_MSG_LENGTH] = "";
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, format, list);
    va_end(list);
    igs_log(agent->agentName, IGS_LOG_DEBUG, function, "%s", content);
}
void igsAgent_info2(const char *function, igsAgent_t *agent, const char *format, ...){
    va_list list;
    va_start(list, format);
    char content[MAX_STRING_MSG_LENGTH] = "";
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, format, list);
    va_end(list);
    igs_log(agent->agentName, IGS_LOG_INFO, function, "%s", content);
}
void igsAgent_warn2(const char *function, igsAgent_t *agent, const char *format, ...){
    va_list list;
    va_start(list, format);
    char content[MAX_STRING_MSG_LENGTH] = "";
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, format, list);
    va_end(list);
    igs_log(agent->agentName, IGS_LOG_WARN, function, "%s", content);
}
void igsAgent_error2(const char *function, igsAgent_t *agent, const char *format, ...){
    va_list list;
    va_start(list, format);
    char content[MAX_STRING_MSG_LENGTH] = "";
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, format, list);
    va_end(list);
    igs_log(agent->agentName, IGS_LOG_ERROR, function, "%s", content);
}
void igsAgent_fatal2(const char *function, igsAgent_t *agent, const char *format, ...){
    va_list list;
    va_start(list, format);
    char content[MAX_STRING_MSG_LENGTH] = "";
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, format, list);
    va_end(list);
    igs_log(agent->agentName, IGS_LOG_FATAL, function, "%s", content);
}
