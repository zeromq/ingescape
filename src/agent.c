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

igs_agent_t *igsAgent_new(const char *name){
    igs_agent_t *agent = calloc(1, sizeof(igs_agent_t));
    zuuid_t *uuid = zuuid_new();
    agent->uuid = strdup(zuuid_str(uuid));
    zuuid_destroy(&uuid);
    agent->name = strndup((name == NULL)?IGS_DEFAULT_AGENT_NAME:name, IGS_MAX_AGENT_NAME_LENGTH);
    return agent;
}
void igsAgent_destroy(igs_agent_t **agent){
    if ((*agent)->uuid != NULL)
        free((*agent)->uuid);
    if ((*agent)->name != NULL)
        free((*agent)->name);
    if ((*agent)->definitionPath != NULL)
        free((*agent)->definitionPath);
    if ((*agent)->mappingPath != NULL)
        free((*agent)->mappingPath);
    //FIXME: handle def and mapping destruction
    free(*agent);
    *agent = NULL;
}



int igsAgent_activate(void){
    return EXIT_SUCCESS;
}

int igsAgent_deactivate(void){
    return EXIT_SUCCESS;
}

void igsAgent_log(igs_logLevel_t level, const char *function, igs_agent_t *agent, const char *format, ...){
    va_list list;
    va_start(list, format);
    char content[MAX_STRING_MSG_LENGTH] = "";
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, format, list);
    va_end(list);
    admin_log(agent, level, function, "%s", content);
}
