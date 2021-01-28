//
//  common.c
//  testing
//
//  Created by Stephane Vales on 17/07/2020.
//  Copyright © 2020 Ingenuity i/o. All rights reserved.
//

#include "common.h"
#include <ingescape/ingescape_advanced.h>

///////////////////////////////////////////////////////////////////////////////
// Global callbacks
void myIOPCallback(iop_t iopType, const char* name, iopType_t valueType, void* value, size_t valueSize, void* myData){
    printf("input %s changed", name);
    switch (valueType) {
    case IGS_IMPULSION_T:
        printf(" (impulsion)\n");
        break;
    case IGS_BOOL_T:
        printf(" to %d\n", igs_readInputAsBool(name));
        break;
    case IGS_INTEGER_T:
        printf(" to %d\n", igs_readInputAsInt(name));
        break;
    case IGS_DOUBLE_T:
        printf(" to %lf\n", igs_readInputAsDouble(name));
        break;
    case IGS_STRING_T:
    {
        char *stringValue = igs_readInputAsString(name);
        printf(" to %s\n", (char *)stringValue);
        free(stringValue);
        break;
    }
    case IGS_DATA_T:
        //NB: for IGS_DATA_T, value and valueSize are already provided
        printf(" with size %zu\n", valueSize);
        break;
    default:
        break;
    }
}

void myCallCallback(const char *senderAgentName, const char *senderAgentUUID,
                    const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
                    const char *token, void* myData){
    printf("received call %s from %s(%s) (", callName, senderAgentName, senderAgentUUID);
    igs_callArgument_t *currentArg = firstArgument;
    for (size_t i = 0; i < nbArgs; i++){
        switch (currentArg->type) {
            case IGS_BOOL_T:
                printf(" %d", currentArg->b);
                break;
            case IGS_INTEGER_T:
                printf(" %d", currentArg->i);
                break;
            case IGS_DOUBLE_T:
                printf(" %f", currentArg->d);
                break;
            case IGS_STRING_T:
                printf(" '%s'", currentArg->c);
                break;
            case IGS_DATA_T:
                printf(" data(%zu bytes)", currentArg->size);
                break;
            default:
                break;
        }
        currentArg = currentArg->next;
    }
    printf(" )\n");
}

void myBusCallback(const char *event, const char *peerID, const char *name,
                   const char *address, const char *channel,
                   zhash_t *headers, zmsg_t *msg, void *myData){
    if (streq(event, "ENTER")){
        printf("-> %s (%s)\n", name, peerID);
        zlist_t *keys = zhash_keys(headers);
        char *key = zlist_first(keys);
        while (key != NULL) {
            printf("%s--->%s\n", key, (char *)zhash_lookup(headers, key));
            key = zlist_next(keys);
        }
    } else if (streq(event, "EXIT")){
        printf("<- %s (%s)\n", name, peerID);
    } else if (streq(event, "JOIN")){
        printf("+%s %s (%s)\n", channel, name, peerID);
    } else if (streq(event, "LEAVE")){
        printf("-%s %s (%s)\n", channel, name, peerID);
    } else if (streq(event, "SHOUT")){
        char *message = zmsg_popstr(msg);
        printf("#%s:%s(%s) - %s |", channel, name, peerID, message);
        free(message);
        while ((message = zmsg_popstr(msg))){
            printf("%s |", message);
            free(message);
        }
        printf("\n");
        
    } else if (streq(event, "WHISPER")){
        char *message = zmsg_popstr(msg);
        printf("#%s(%s) - %s |", name, peerID, message);
        free(message);
        while ((message = zmsg_popstr(msg))){
            printf("%s |", message);
            free(message);
        }
        printf("\n");
    } else if (streq(event, "SILENT")){
        printf("[SILENT] %s (%s)\n", name, peerID);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Agent callbacks
void agentIOPCallback(igs_agent_t *agent, iop_t iopType, const char* name, iopType_t valueType,
                      void* value, size_t valueSize, void* myData){
    char *agentName = igsAgent_getAgentName(agent);
    printf("input %s changed on %s", name, agentName);
    free(agentName);
    switch (valueType) {
    case IGS_IMPULSION_T:
        printf(" (impulsion)\n");
        break;
    case IGS_BOOL_T:
        printf(" to %d\n", igsAgent_readInputAsBool(agent, name));
        break;
    case IGS_INTEGER_T:
        printf(" to %d\n", igsAgent_readInputAsInt(agent, name));
        break;
    case IGS_DOUBLE_T:
        printf(" to %lf\n", igsAgent_readInputAsDouble(agent, name));
        break;
    case IGS_STRING_T:
    {
        char *stringValue = igsAgent_readInputAsString(agent, name);
        printf(" to %s\n", (char *)stringValue);
        free(stringValue);
        break;
    }
    case IGS_DATA_T:
        //NB: for IGS_DATA_T, value and valueSize are already provided
        printf(" with size %zu\n", valueSize);
        break;
    default:
        break;
    }
}

void agentCallCallback(igs_agent_t *agent, const char *senderAgentName, const char *senderAgentUUID,
                       const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
                       const char *token, void* myData){
    char *agentName = igsAgent_getAgentName(agent);
    printf("%s received call %s from %s(%s) (", agentName, callName, senderAgentName, senderAgentUUID);
    free(agentName);
    igs_callArgument_t *currentArg = firstArgument;
    for (size_t i = 0; i < nbArgs; i++){
        switch (currentArg->type) {
            case IGS_BOOL_T:
                printf(" %d", currentArg->b);
                break;
            case IGS_INTEGER_T:
                printf(" %d", currentArg->i);
                break;
            case IGS_DOUBLE_T:
                printf(" %f", currentArg->d);
                break;
            case IGS_STRING_T:
                printf(" '%s'", currentArg->c);
                break;
            case IGS_DATA_T:
                printf(" data(%zu bytes)", currentArg->size);
                break;
            default:
                break;
        }
        currentArg = currentArg->next;
    }
    printf(" )\n");
}



///////////////////////////////////////////////////////////////////////////////
// COMMAND LINE AND INTERPRETER OPTIONS
//
void print_usage(const char *agentName) {
    printf("Usage example: %s --verbose --port %d --device device_name\n", agentName, port);
    printf("\nthese parameters have default value (indicated here above):\n");
    printf("--verbose : enable verbose mode in the application (default is disabled)\n");
    printf("--port port_number : port used for autodiscovery between agents (default: %d)\n", port);
    printf("--device device_name : name of the network device to be used (useful if several devices available)\n");
    printf("--name agent_name : published name for this agent (default: %s)\n", agentName);
    printf("--interactiveloop : enables interactive loop to pass commands in CLI (default: false)\n");
    printf("--auto : enables automatic network tests based on timers and network events\n");
}

//helper to convert paths starting with ~ to absolute paths
void makeFilePath(char *from, char *to, size_t size_of_to) {
    if (from[0] == '~') {
        from++;
#ifdef _WIN32
        char *home = getenv("USERPROFILE");
#else
        char *home = getenv("HOME");
#endif
        if (home == NULL) {
            igs_error("could not find path for home directory");
        }
        else {
            strncpy(to, home, size_of_to);
            strncat(to, from, size_of_to);
        }
    }
    else {
        strncpy(to, from, size_of_to);
    }
}

void print_cli_usage() {
    printf("Available commands in the terminal:\n");
    printf("\t/publish : runs the iop publication tests\n");
    printf("\t/call : runs the call tests\n");
    printf("\t/bus : runs the bus tests\n");
    printf("\t/editor agent_uuid : runs the editor (i.e. private bus API) tests on a specific agent\n");
    printf("\t/activate : activates secondAgent\n");
    printf("\t/deactivate : deactivates secondAgent\n");
    printf("\t/call_local : firstAgent calls secondCall on secondAgent\n");
    printf("\t/gossip : restart in gossip mode\n");
    printf("\t/security : restart and enable security in gossip mode (edit code to use self-discovery instead of gossip)\n");
    printf("\t/quit : quits the agent\n");
    printf("\t/help : displays this message\n");
}

void publishCommand(void){
    igs_writeOutputAsImpulsion("my_impulsion");
    igs_writeOutputAsBool("my_bool", true);
    igs_writeOutputAsInt("my_int", 2);
    igs_writeOutputAsDouble("my_double", 2.0);
    igs_writeOutputAsString("my_string", "output string test");
    igs_writeOutputAsData("my_data", myOtherData, 64);
}

void publishCommandSparing(void){
    igs_writeOutputAsImpulsion("sparing_impulsion");
    igs_writeOutputAsBool("sparing_bool", true);
    igs_writeOutputAsInt("sparing_int", 2);
    igs_writeOutputAsDouble("sparing_double", 2.2);
    igs_writeOutputAsString("sparing_string", "output string test");
    igs_writeOutputAsData("sparing_data", myOtherData, 64);
}

void callCommand(void){
    igs_callArgument_t *args = NULL;
    igs_addBoolToArgumentsList(&args, true);
    igs_addIntToArgumentsList(&args, 3);
    igs_addDoubleToArgumentsList(&args, 3.0);
    igs_addStringToArgumentsList(&args, "call string test");
    igs_addDataToArgumentsList(&args, myOtherData, 64);
    igs_sendCall("partner", "sparingCall", &args, "token");
}

void callCommandSparing(void){
    igs_callArgument_t *args = NULL;
    igs_addBoolToArgumentsList(&args, true);
    igs_addIntToArgumentsList(&args, 3);
    igs_addDoubleToArgumentsList(&args, 3.3);
    igs_addStringToArgumentsList(&args, "call string test");
    igs_addDataToArgumentsList(&args, myOtherData, 64);
    igs_sendCall("tester", "myCall", &args, "token");
}

void busCommand(void){
    assert(igs_busSendStringToChannel("TEST_CHANNEL", "test igs_busSendStringToChannel") == IGS_SUCCESS);
    assert(igs_busSendDataToChannel("TEST_CHANNEL", myOtherData, 64) == IGS_SUCCESS);
    zmsg_t *msg = zmsg_new();
    zmsg_addstr(msg, "message content");
    assert(igs_busSendZMQMsgToChannel("TEST_CHANNEL", &msg) == IGS_SUCCESS);
    
    assert(igs_busSendStringToAgent("partner", "test igs_busSendStringToAgent") == IGS_SUCCESS);
    assert(igs_busSendDataToAgent("partner", myOtherData, 64) == IGS_SUCCESS);
    msg = zmsg_new();
    zmsg_addstr(msg, "message content");
    assert(igs_busSendZMQMsgToAgent("partner", &msg) == IGS_SUCCESS);
}

void busCommandSparing(void){
    assert(igs_busSendStringToChannel("TEST_CHANNEL", "test igs_busSendStringToChannel") == IGS_SUCCESS);
    assert(igs_busSendDataToChannel("TEST_CHANNEL", myOtherData, 64) == IGS_SUCCESS);
    zmsg_t *msg = zmsg_new();
    zmsg_addstr(msg, "message content");
    assert(igs_busSendZMQMsgToChannel("TEST_CHANNEL", &msg) == IGS_SUCCESS);
    
    assert(igs_busSendStringToAgent("tester", "test igs_busSendStringToAgent") == IGS_SUCCESS);
    assert(igs_busSendDataToAgent("tester", myOtherData, 64) == IGS_SUCCESS);
    msg = zmsg_new();
    zmsg_addstr(msg, "message content");
    assert(igs_busSendZMQMsgToAgent("tester", &msg) == IGS_SUCCESS);
}

void editorCommand(const char *agentUUID, const char *input){
    zmsg_t *msg = zmsg_new();
    zmsg_addstr(msg, "GET_CURRENT_OUTPUTS");
    zmsg_addstr(msg, agentUUID);
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "GET_CURRENT_INPUTS");
    zmsg_addstr(msg, agentUUID);
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "GET_CURRENT_PARAMETERS");
    zmsg_addstr(msg, agentUUID);
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "GET_LICENSE_INFO");
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "CLEAR_MAPPING");
    zmsg_addstr(msg, agentUUID);
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "FREEZE");
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "UNFREEZE");
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "MUTE_ALL");
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "UNMUTE_ALL");
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "MUTE");
    zmsg_addstr(msg, input);
    zmsg_addstr(msg, agentUUID);
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "UNMUTE");
    zmsg_addstr(msg, input);
    zmsg_addstr(msg, agentUUID);
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "SET_INPUT");
    zmsg_addstr(msg, input);
    zmsg_addstr(msg, "10");
    zmsg_addstr(msg, agentUUID);
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "SET_OUTPUT");
    zmsg_addstr(msg, input);
    zmsg_addstr(msg, "10");
    zmsg_addstr(msg, agentUUID);
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "SET_PARAMETER");
    zmsg_addstr(msg, input);
    zmsg_addstr(msg, "10");
    zmsg_addstr(msg, agentUUID);
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "MAP");
    zmsg_addstr(msg, input);
    zmsg_addstr(msg, "partner");
    zmsg_addstr(msg, "sparing_bool");
    zmsg_addstr(msg, agentUUID);
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "UNMAP");
    zmsg_addstr(msg, input);
    zmsg_addstr(msg, "partner");
    zmsg_addstr(msg, "sparing_bool");
    zmsg_addstr(msg, agentUUID);
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "DISABLE_LOG_STREAM");
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "ENABLE_LOG_STREAM");
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "DISABLE_LOG_FILE");
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "ENABLE_LOG_FILE");
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "SET_LOG_PATH");
    zmsg_addstr(msg, "/tmp/new_log_file.log");
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "SET_DEFINITION_PATH");
    zmsg_addstr(msg, "/tmp/new_definition_path.json");
    zmsg_addstr(msg, agentUUID);
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "SET_MAPPING_PATH");
    zmsg_addstr(msg, "/tmp/new_mapping_path.json");
    zmsg_addstr(msg, agentUUID);
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "SAVE_DEFINITION_TO_PATH");
    zmsg_addstr(msg, agentUUID);
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
    
    msg = zmsg_new();
    zmsg_addstr(msg, "SAVE_MAPPING_TO_PATH");
    zmsg_addstr(msg, agentUUID);
    igs_busSendZMQMsgToAgent(agentUUID, &msg);
}

void gossipCommand(void){
    igs_stop();
    assert(igs_startWithBrokers("tcp://10.0.0.8:5670") == IGS_FAILURE);
    assert(igs_brokerAdd("tcp://10.0.0.8:5661") == IGS_SUCCESS);
    assert(igs_brokerAdd("tcp://10.0.0.8:5661") == IGS_FAILURE);
    igs_enableAsBroker("tcp://10.0.0.8:5660");
    assert(igs_startWithBrokers("tcp://10.0.0.8:5670") == IGS_SUCCESS);
}

void gossipCommandSparing(void){
    igs_stop();
    igs_brokerAdd("tcp://10.0.0.8:5660");
    igs_brokerAdd("tcp://10.0.0.8:5659");
    igs_enableAsBroker("tcp://10.0.0.8:5661");
    igs_startWithBrokers("tcp://10.0.0.8:5671");
}

void securityCommand(void){ //for ingescape tester
    igs_stop();
    
    assert(igs_enableSecurity(NULL, NULL) == IGS_SUCCESS);
    assert(igs_enableSecurity("toto", NULL) == IGS_FAILURE);
    assert(igs_enableSecurity("certificates/tester.cert_secret", "toto") == IGS_FAILURE);
    assert(igs_enableSecurity("certificates/tester.cert_secret", "certificates") == IGS_SUCCESS);
    
    assert(igs_brokerAdd("tcp://10.0.0.8:5661") == IGS_FAILURE);
    assert(igs_brokerAddSecure("tcp://10.0.0.8:5661", "toto") == IGS_FAILURE);
    assert(igs_brokerAddSecure("tcp://10.0.0.8:5661", "certificates/partner.cert") == IGS_SUCCESS);
    //igs_enableAsBroker("tcp://10.0.0.8:5660"); //uncomment to test two brokers on the same platform
    
    assert(igs_startWithBrokers("tcp://10.0.0.8:5670") == IGS_SUCCESS);
    //assert(igs_startWithDevice(networkDevice, port) == IGS_SUCCESS); //uncomment to use UDP instead of gossip
}

void securityCommandSparing(void){
    igs_stop();
    
    //igs_enableSecurity(NULL, NULL);
    igs_enableSecurity("certificates/partner.cert_secret", "certificates");
    
    igs_brokerAddSecure("tcp://10.0.0.8:5660", "certificates/tester.cert");
    igs_enableAsBroker("tcp://10.0.0.8:5661");
    
    igs_startWithBrokers("tcp://10.0.0.8:5671");
    //igs_startWithDevice(networkDevice, port); //uncomment to use UDP instead of gossip
}
