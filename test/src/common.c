//
//  common.c
//  testing
//
//  Created by Stephane Vales on 17/07/2020.
//  Copyright © 2020 Ingenuity i/o. All rights reserved.
//

#include "common.h"
#include <ingescape.h>

///////////////////////////////////////////////////////////////////////////////
// Global callbacks
void myIOPCallback(igs_iop_type_t iopType, const char* name, igs_iop_value_type_t valueType, void* cbValue, size_t valueSize, void* myCbData){
    IGS_UNUSED(iopType)
    IGS_UNUSED(cbValue)
    IGS_UNUSED(myCbData)
    printf("input %s changed", name);
    switch (valueType) {
    case IGS_IMPULSION_T:
        printf(" (impulsion)\n");
        break;
    case IGS_BOOL_T:
        printf(" to %d\n", igs_input_bool(name));
        break;
    case IGS_INTEGER_T:
        printf(" to %d\n", igs_input_int(name));
        break;
    case IGS_DOUBLE_T:
        printf(" to %lf\n", igs_input_double(name));
        break;
    case IGS_STRING_T:
    {
        char *stringValue = igs_input_string(name);
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

void myServiceCallback(const char *senderAgentName, const char *senderAgentUUID,
                    const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                    const char *token, void* myCbData){
    IGS_UNUSED(token)
    IGS_UNUSED(myCbData)
    printf("received %s call from %s(%s) (", serviceName, senderAgentName, senderAgentUUID);
    igs_service_arg_t *currentArg = firstArgument;
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

void myChannelsCallback(const char *event, const char *peerID, const char *name,
                        const char *address, const char *channel,
                        zhash_t *headers, zmsg_t *msg, void *myCbData){
    IGS_UNUSED(address)
    IGS_UNUSED(myCbData)
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
void agentIOPCallback(igsagent_t *agent, igs_iop_type_t iopType, const char* name, igs_iop_value_type_t valueType,
                      void* value, size_t valueSize, void* myCbData){
    IGS_UNUSED(myCbData)
    IGS_UNUSED(value)
    IGS_UNUSED(iopType)
    char *thisAgentName = igsagent_name(agent);
    printf("input %s changed on %s", name, thisAgentName);
    free(thisAgentName);
    switch (valueType) {
    case IGS_IMPULSION_T:
        printf(" (impulsion)\n");
        break;
    case IGS_BOOL_T:
        printf(" to %d\n", igsagent_input_bool(agent, name));
        break;
    case IGS_INTEGER_T:
        printf(" to %d\n", igsagent_input_int(agent, name));
        break;
    case IGS_DOUBLE_T:
        printf(" to %lf\n", igsagent_input_double(agent, name));
        break;
    case IGS_STRING_T:
    {
        char *stringValue = igsagent_input_string(agent, name);
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

void agentServiceCallback(igsagent_t *agent, const char *senderAgentName, const char *senderAgentUUID,
                          const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                          const char *token, void* myCbData){
    IGS_UNUSED(token)
    IGS_UNUSED(myCbData)
    char *thisAgentName = igsagent_name(agent);
    printf("%s received service %s from %s(%s) (", thisAgentName, serviceName, senderAgentName, senderAgentUUID);
    free(thisAgentName);
    igs_service_arg_t *currentArg = firstArgument;
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
void print_usage(const char *myAgentName) {
    printf("Usage example: %s --verbose --port %d --device device_name\n", myAgentName, port);
    printf("\nthese parameters have default value (indicated here above):\n");
    printf("--verbose : enable verbose mode in the application (default is disabled)\n");
    printf("--port port_number : port used for autodiscovery between agents (default: %d)\n", port);
    printf("--device device_name : name of the network device to be used (useful if several devices available)\n");
    printf("--name agent_name : published name for this agent (default: %s)\n", myAgentName);
    printf("--interactiveloop : enables interactive loop to pass commands in CLI (default: false)\n");
    printf("--auto : enables automatic network tests based on timers and network events\n");
    printf("--static : runs static tests only\n");
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
    printf("\t/services : runs the service tests\n");
    printf("\t/channels : runs the channels tests\n");
    printf("\t/editor agent_uuid : runs the editor (i.e. private bus API) tests on a specific agent\n");
    printf("\t/activate : activates secondAgent\n");
    printf("\t/deactivate : deactivates secondAgent\n");
    printf("\t/service_local : firstAgent calls service secondCall on secondAgent\n");
    printf("\t/broker ip_address : restart in broker mode on the given ip address\n");
    printf("\t/security ip_address : restart and enable security in broker mode (edit code to use self-discovery instead of broker)\n");
    printf("\t/quit : quits the agent\n");
    printf("\t/help : displays this message\n");
}

void publishCommand(void){
    igs_output_set_impulsion("my_impulsion");
    igs_output_set_bool("my_bool", true);
    igs_output_set_int("my_int", 2);
    igs_output_set_double("my_double", 2.0);
    igs_output_set_string("my_string", "output string test");
    igs_output_set_data("my_data", myOtherData, 64);
}

void publishCommandSparing(void){
    igs_output_set_impulsion("sparing_impulsion");
    igs_output_set_bool("sparing_bool", true);
    igs_output_set_int("sparing_int", 2);
    igs_output_set_double("sparing_double", 2.2);
    igs_output_set_string("sparing_string", "output string test");
    igs_output_set_data("sparing_data", myOtherData, 64);
}

void servicesCommand(void){
    igs_service_arg_t *args = NULL;
    igs_service_args_add_bool(&args, true);
    igs_service_args_add_int(&args, 3);
    igs_service_args_add_double(&args, 3.0);
    igs_service_args_add_string(&args, "service string test");
    igs_service_args_add_data(&args, myOtherData, 64);
    igs_service_call("partner", "sparingService", &args, "token");
}

void servicesCommandSparing(void){
    igs_service_arg_t *args = NULL;
    igs_service_args_add_bool(&args, true);
    igs_service_args_add_int(&args, 3);
    igs_service_args_add_double(&args, 3.3);
    igs_service_args_add_string(&args, "service string test");
    igs_service_args_add_data(&args, myOtherData, 64);
    igs_service_call("tester", "myService", &args, "token");
}

void channelsCommand(void){
    assert(igs_channel_shout_str("TEST_CHANNEL", "test igs_channel_shout_str") == IGS_SUCCESS);
    assert(igs_channel_shout_data("TEST_CHANNEL", myOtherData, 64) == IGS_SUCCESS);
    zmsg_t *msg = zmsg_new();
    zmsg_addstr(msg, "message content");
    assert(igs_channel_shout_zmsg("TEST_CHANNEL", &msg) == IGS_SUCCESS);

    assert(igs_channel_whisper_str("partner", "test igs_channel_whisper_str") == IGS_SUCCESS);
    assert(igs_channel_whisper_data("partner", myOtherData, 64) == IGS_SUCCESS);
    msg = zmsg_new();
    zmsg_addstr(msg, "message content");
    assert(igs_channel_whisper_zmsg("partner", &msg) == IGS_SUCCESS);
}

void channelsCommandSparing(void){
    assert(igs_channel_shout_str("TEST_CHANNEL", "test igs_channel_shout_str") == IGS_SUCCESS);
    assert(igs_channel_shout_data("TEST_CHANNEL", myOtherData, 64) == IGS_SUCCESS);
    zmsg_t *msg = zmsg_new();
    zmsg_addstr(msg, "message content");
    assert(igs_channel_shout_zmsg("TEST_CHANNEL", &msg) == IGS_SUCCESS);

    assert(igs_channel_whisper_str("tester", "test igs_channel_whisper_str") == IGS_SUCCESS);
    assert(igs_channel_whisper_data("tester", myOtherData, 64) == IGS_SUCCESS);
    msg = zmsg_new();
    zmsg_addstr(msg, "message content");
    assert(igs_channel_whisper_zmsg("tester", &msg) == IGS_SUCCESS);
}

void editorCommand(const char *agentUUID, const char *input){
    zmsg_t *msg = zmsg_new();
    zmsg_addstr(msg, "GET_CURRENT_OUTPUTS");
    zmsg_addstr(msg, agentUUID);
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "GET_CURRENT_INPUTS");
    zmsg_addstr(msg, agentUUID);
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "GET_CURRENT_PARAMETERS");
    zmsg_addstr(msg, agentUUID);
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "CLEAR_MAPPING");
    zmsg_addstr(msg, agentUUID);
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "FREEZE");
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "UNFREEZE");
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "MUTE_ALL");
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "UNMUTE_ALL");
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "MUTE");
    zmsg_addstr(msg, input);
    zmsg_addstr(msg, agentUUID);
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "UNMUTE");
    zmsg_addstr(msg, input);
    zmsg_addstr(msg, agentUUID);
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "SET_INPUT");
    zmsg_addstr(msg, input);
    zmsg_addstr(msg, "10");
    zmsg_addstr(msg, agentUUID);
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "SET_OUTPUT");
    zmsg_addstr(msg, input);
    zmsg_addstr(msg, "10");
    zmsg_addstr(msg, agentUUID);
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "SET_PARAMETER");
    zmsg_addstr(msg, input);
    zmsg_addstr(msg, "10");
    zmsg_addstr(msg, agentUUID);
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "MAP");
    zmsg_addstr(msg, input);
    zmsg_addstr(msg, "partner");
    zmsg_addstr(msg, "sparing_bool");
    zmsg_addstr(msg, agentUUID);
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "UNMAP");
    zmsg_addstr(msg, input);
    zmsg_addstr(msg, "partner");
    zmsg_addstr(msg, "sparing_bool");
    zmsg_addstr(msg, agentUUID);
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "DISABLE_LOG_STREAM");
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "ENABLE_LOG_STREAM");
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "DISABLE_LOG_FILE");
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "ENABLE_LOG_FILE");
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "SET_LOG_PATH");
    zmsg_addstr(msg, "/tmp/new_log_file.log");
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "SET_DEFINITION_PATH");
    zmsg_addstr(msg, "/tmp/new_definition_path.json");
    zmsg_addstr(msg, agentUUID);
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "SET_MAPPING_PATH");
    zmsg_addstr(msg, "/tmp/new_mapping_path.json");
    zmsg_addstr(msg, agentUUID);
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "SAVE_DEFINITION_TO_PATH");
    zmsg_addstr(msg, agentUUID);
    igs_channel_whisper_zmsg(agentUUID, &msg);

    msg = zmsg_new();
    zmsg_addstr(msg, "SAVE_MAPPING_TO_PATH");
    zmsg_addstr(msg, agentUUID);
    igs_channel_whisper_zmsg(agentUUID, &msg);
}

void brokerCommand(char * ip_address){
    char buffer[1024] = "";
    igs_stop();
    snprintf(buffer, 1024, "tcp://%s:5670", ip_address);
    assert(igs_start_with_brokers(buffer) == IGS_FAILURE);
    snprintf(buffer, 1024, "tcp://%s:5661", ip_address);
    assert(igs_broker_add(buffer) == IGS_SUCCESS);
    assert(igs_broker_add(buffer) == IGS_FAILURE);
    //snprintf(buffer, 1024, "tcp://%s:5660", ip_address);
    //igs_broker_enable_with_endpoint(buffer);
    snprintf(buffer, 1024, "tcp://%s:5670", ip_address);
    assert(igs_start_with_brokers(buffer) == IGS_SUCCESS);
}

void brokerCommandSparing(char * ip_address){
    char buffer[1024] = "";
    igs_stop();
    //snprintf(buffer, 1024, "tcp://%s:5660", ip_address);
    //igs_broker_add(buffer);
    //snprintf(buffer, 1024, "tcp://%s:5659", ip_address);
    //igs_broker_add(buffer);
    snprintf(buffer, 1024, "tcp://%s:5661", ip_address);
    igs_broker_enable_with_endpoint(buffer);
    snprintf(buffer, 1024, "tcp://%s:5671", ip_address);
    igs_start_with_brokers(buffer);
}

void securityCommand(char * ip_address){
    char buffer[1024] = "";
    igs_stop();

    assert(igs_enable_security(NULL, NULL) == IGS_SUCCESS);
    assert(igs_enable_security("toto", NULL) == IGS_FAILURE);
    assert(igs_enable_security("/usr/local/share/ingescape/certificates/tester.cert_secret", "toto") == IGS_FAILURE);
    assert(igs_enable_security("/usr/local/share/ingescape/certificates/tester.cert_secret", "/usr/local/share/ingescape/certificates") == IGS_SUCCESS);
    
    snprintf(buffer, 1024, "tcp://%s:5661", ip_address);
    assert(igs_broker_add(buffer) == IGS_FAILURE);
    snprintf(buffer, 1024, "tcp://%s:5661", ip_address);
    assert(igs_broker_add_secure(buffer, "toto") == IGS_FAILURE);
    snprintf(buffer, 1024, "tcp://%s:5661", ip_address);
    assert(igs_broker_add_secure(buffer, "/usr/local/share/ingescape/certificates/partner.cert") == IGS_SUCCESS);
    snprintf(buffer, 1024, "tcp://%s:5660", ip_address);
    //igs_broker_enable_with_endpoint(buffer); //uncomment to test two brokers on the same platform
    
    snprintf(buffer, 1024, "tcp://%s:5670", ip_address);
    assert(igs_start_with_brokers(buffer) == IGS_SUCCESS); //comment to use UDP instead of broker
    //assert(igs_start_with_device(networkDevice, port) == IGS_SUCCESS); //uncomment to use UDP instead of broker
}

void securityCommandSparing(char * ip_address){
    char buffer[1024] = "";
    igs_stop();

    //igs_enable_security(NULL, NULL);
    igs_enable_security("/usr/local/share/ingescape/certificates/partner.cert_secret", "/usr/local/share/ingescape/certificates");
    
    //snprintf(buffer, 1024, "tcp://%s:5660", ip_address);
    //igs_broker_add_secure(buffer, "/usr/local/share/ingescape/certificates/tester.cert");
    snprintf(buffer, 1024, "tcp://%s:5661", ip_address);
    igs_broker_enable_with_endpoint(buffer);
    
    snprintf(buffer, 1024, "tcp://%s:5671", ip_address);
    igs_start_with_brokers(buffer); //comment to use UDP instead of broker
    //igs_start_with_device(networkDevice, port); //uncomment to use UDP instead of broker
}
