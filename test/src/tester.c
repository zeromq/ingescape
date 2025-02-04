#include "common.h"

#include <stdio.h>
#include <getopt.h> //command line options at statrtup
#include <stdlib.h> //standard C functions such as getenv, atoi, exit, etc.
#include <string.h> //C string handling functions
#include <signal.h> //catching interruptions
#include <czmq.h>
#include <igsagent.h>
#include "ingescape_private.h"

unsigned int port = 5670;
const char *agentName = "tester";
const char *networkDevice = "en0"; //can be set to a default device name
bool rt = false;
bool verbose = false;
bool autoTests = false;
bool autoTestsHaveStarted = false;

//reference values for static tests
bool myBool = true;
int myInt = 1;
double myDouble = 1.0;
char *myString = (char *)"my string";
void *myData = NULL;
void *myOtherData = NULL;

int ingescapeSentMessage(zloop_t *loop, zsock_t *reader, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(arg)
    char *message = NULL;
    zsock_recv(reader, "s", &message);
    if (streq(message, "LOOP_STOPPED")){
        igs_info("LOOP_STOPPED received from ingescape thread");
        return -1;
    }else if (strncmp(message, "input", 5) == 0)
        igs_info("'%s' received from ingescape thread", message);
    return 0;
}

//callbacks and variables for agent events
bool tester_firstAgentEntered = false;
bool tester_firstAgentKnowsUs = false;
bool tester_firstAgentExited = false;
bool tester_secondAgentEntered = false;
bool tester_secondAgentKnowsUs = false;
bool tester_secondAgentExited = false;
void agentEvent(igs_agent_event_t event, const char *uuid, const char *name, const void *eventData, void *myCbData){
    IGS_UNUSED(eventData)
    IGS_UNUSED(myCbData)
    if (autoTests)
        return;
    printf("agentEvent: in tester - %d - %s - %s\n", event, uuid, name);
    if (streq(name, "firstAgent")){
        if (event == IGS_AGENT_ENTERED)
            tester_firstAgentEntered = true;
        if (event == IGS_AGENT_KNOWS_US)
            tester_firstAgentKnowsUs = true;
        if (event == IGS_AGENT_EXITED)
            tester_firstAgentExited = true;
    }else if(streq(name, "secondAgent")){
        if (event == IGS_AGENT_ENTERED)
            tester_secondAgentEntered = true;
        if (event == IGS_AGENT_KNOWS_US)
            tester_secondAgentKnowsUs = true;
        if (event == IGS_AGENT_EXITED)
            tester_secondAgentExited = true;
    }
}

igsagent_t *firstAgent = NULL;
igsagent_t *secondAgent = NULL;
bool first_secondAgentEntered = false;
bool first_secondAgentKnowsUs = false;
bool first_secondAgentExited = false;
bool first_testerAgentEntered = false;
bool first_testergentKnowsUs = false;
bool first_testerAgentExited = false;
bool second_firstAgentEntered = false;
bool second_firstAgentKnowsUs = false;
bool second_firstAgentExited = false;
bool second_testerAgentEntered = false;
bool second_testergentKnowsUs = false;
bool second_testerAgentExited = false;
void agentEvent2(igsagent_t *agent, igs_agent_event_t event, const char *uuid, const char *name, void *eventData, void *myCbData){
    IGS_UNUSED(eventData)
    IGS_UNUSED(myCbData)
    if (autoTests)
        return;
    printf("agentEvent2: in %s - %d - %s - %s\n", igsagent_name(agent), event, uuid, name); //intentional memory leak on agent name
    assert(agent == firstAgent || agent == secondAgent);
    if (agent == firstAgent){
        if (streq(name, "tester")){
            if (event == IGS_AGENT_ENTERED)
                first_testerAgentEntered = true;
            if (event == IGS_AGENT_KNOWS_US)
                first_testergentKnowsUs = true;
            if (event == IGS_AGENT_EXITED)
                first_testerAgentExited = true;
        }else if(streq(name, "secondAgent")){
            if (event == IGS_AGENT_ENTERED)
                first_secondAgentEntered = true;
            if (event == IGS_AGENT_KNOWS_US)
                first_secondAgentKnowsUs = true;
            if (event == IGS_AGENT_EXITED)
                first_secondAgentExited = true;
        }
    }else if (agent == secondAgent){
        if (streq(name, "tester")){
            if (event == IGS_AGENT_ENTERED)
                second_testerAgentEntered = true;
            if (event == IGS_AGENT_KNOWS_US)
                second_testergentKnowsUs = true;
            if (event == IGS_AGENT_EXITED)
                second_testerAgentExited = true;
        }else if(streq(name, "firstAgent")){
            if (event == IGS_AGENT_ENTERED)
                second_firstAgentEntered = true;
            if (event == IGS_AGENT_KNOWS_US)
                second_firstAgentKnowsUs = true;
            if (event == IGS_AGENT_EXITED)
                second_firstAgentExited = true;
        }
    }
}

//callbacks for services
void testerServiceCallback(const char *senderAgentName, const char *senderAgentUUID,
                           const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                           const char *token, void* myCbData){
    IGS_UNUSED(myCbData)
    printf("received service %s from %s(%s) (", serviceName, senderAgentName, senderAgentUUID);
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
                printf(" data - %zu bytes", currentArg->size);
                break;
            default:
                break;
        }
        currentArg = currentArg->next;
    }
    printf(" )\n");
    int64_t timestamp = igs_rt_get_current_timestamp();
    if (timestamp != INT64_MIN)
        printf("\twith timestamp %lld\n", timestamp);
    if (autoTestsHaveStarted){
        assert(token);
        assert(streq(token, "token"));
        assert(firstArgument->type == IGS_BOOL_T);
        assert(firstArgument->b);
        assert(firstArgument->next->type == IGS_INTEGER_T);
        assert(firstArgument->next->i == 3);
        assert(firstArgument->next->next->type == IGS_DOUBLE_T);
        assert(firstArgument->next->next->d - 3.3 < 0.0001);
        assert(firstArgument->next->next->next->type == IGS_STRING_T);
        assert(streq(firstArgument->next->next->next->c,"service string test"));
        assert(firstArgument->next->next->next->next->type == IGS_DATA_T);
        assert(firstArgument->next->next->next->next->size == 64);
        printf("services test is OK\n");
    }
}

//callbacks for channels
size_t msgCountForAutoTests = 0;
void testerChannelCallback(const char *event, const char *peerID, const char *name,
                            const char *address, const char *channel,
                            zhash_t *headers, zmsg_t *msg, void *myCbData){
    IGS_UNUSED(address)
    IGS_UNUSED(myCbData)
    if (autoTests && autoTestsHaveStarted){
        assert(streq(name, "partner"));
        if (streq(event, "SHOUT")){
            assert(streq(channel, "TEST_CHANNEL"));
            if (msgCountForAutoTests == 0){
                char *s = zmsg_popstr(msg);
                assert(streq(s, "test igs_channel_shout_str"));
                free(s);
            }
            if (msgCountForAutoTests == 1){
                size_t size = zmsg_size(msg);
                assert(size == 1);
                zframe_t *f = zmsg_first(msg);
                size = zframe_size(f);
                assert(size == 64);
            }
            if (msgCountForAutoTests == 2){
                char *s = zmsg_popstr(msg);
                assert(streq(s, "message content"));
                free(s);
            }
        }else if (streq(event, "WHISPER")){
            zframe_t *frame = zmsg_first(msg);
            char *s = zframe_strdup(frame);
            if (streq(s, "SERVICE") || streq(s, "SPLITTER_WORK")){
                //we are catching the service or splitter test : dismiss
                msgCountForAutoTests--; //compensating
            }else{
                if (msgCountForAutoTests == 3){
                    assert(streq(s, "test igs_channel_whisper_str"));
                }
                if (msgCountForAutoTests == 4){
                    size_t size = zmsg_size(msg);
                    assert(size == 2);
                    zframe_t *f = zmsg_first(msg);
                    size = zframe_size(f);
                    assert(size == 64);
                }
                if (msgCountForAutoTests == 5){
                    assert(streq(s, "message content"));
                    printf("channels test is OK\n");
                }
            }
            if (s)
                free(s);
        }
        msgCountForAutoTests++;
        return;
    }else if (autoTests){
        if (streq(event, "WHISPER")){
            char *s = zmsg_popstr(msg);
            if(s && streq(name, "partner") && streq(s, "starting autotests"))
                autoTestsHaveStarted = true;
            if (s)
                free(s);
        }
        return;
    }


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

//callbacks for ios
void testerIOCallback(igs_io_type_t ioType, const char* name, igs_io_value_type_t valueType, void* value, size_t valueSize, void* myCbData){
    IGS_UNUSED(myCbData)
    IGS_UNUSED(ioType)
    IGS_UNUSED(value)

    zsock_t *pipe = igs_pipe_inside_ingescape();
    if (pipe)
        zstr_sendf(pipe, "input %s was written", name);

    if (autoTestsHaveStarted){
        switch (valueType) {
            case IGS_BOOL_T:
                assert(igs_input_bool(name));
                assert(valueSize == sizeof(bool));
                printf("bool publish test is OK\n");
                break;
            case IGS_INTEGER_T:
                assert(igs_input_int(name) == 2);
                assert(valueSize == sizeof(int));
                printf("int publish test is OK\n");
                break;
            case IGS_DOUBLE_T:
                assert(igs_input_double(name) - 2.2 < 0.000001);
                assert(valueSize == sizeof(double));
                printf("double publish test is OK\n");
                break;
            case IGS_STRING_T:
            {
                char *stringValue = igs_input_string(name);
                assert(streq(stringValue, "output string test"));
                assert(valueSize == strlen(stringValue) + 1);
                free(stringValue);
                printf("string publish test is OK\n");
                break;
            }
            case IGS_DATA_T:
            {
                zframe_t *f = zframe_new(value, valueSize);
                char *f_string = zframe_strdup(f);
                assert(streq(f_string, "data test"));
                free(f_string);
                zframe_destroy(&f);
                printf("data publish test is OK\n");
                break;
            }
            default:
                break;
        }
        return;
    }

    if (autoTests)
        return;

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
    int64_t timestamp = igs_rt_get_current_timestamp();
    if (timestamp != INT64_MIN)
        printf("\twith timestamp %lld\n", timestamp);
}


// static tests function
void run_static_tests (int argc, const char * argv[]){
    igs_log_set_syslog(false);
    //agent name and uuid
    char *name = igs_agent_name();
    assert(streq(name, "no_name"));
    assert(igs_definition_class() == NULL);
    assert(igs_definition_package() == NULL);
    free(name);
    igs_agent_set_name("simple Demo Agent");
    assert(streq(igs_definition_class(),"simple Demo Agent")); //intentional memory leak here
    name = igs_agent_name();
    assert(streq(name, "simple Demo Agent"));
    free(name);
    name = NULL;
    igs_agent_set_name(agentName);
    assert(streq(igs_definition_class(),agentName)); //intentional memory leak here
    assert(igs_agent_uuid()); //intentional memory leak here

    //package and class
    igs_definition_set_class("my class");
    assert(streq(igs_definition_class(),"my class")); //intentional memory leak here
    igs_definition_set_package("my::pac kage");
    assert(streq(igs_definition_package(),"my::pac kage")); //intentional memory leak here

    //constraints
    igs_input_create("constraint_impulsion", IGS_IMPULSION_T, 0, 0);
    igs_input_create("constraint_int", IGS_INTEGER_T, 0, 0);
    igs_input_create("constraint_bool", IGS_BOOL_T, 0, 0);
    igs_input_create("constraint_double", IGS_DOUBLE_T, 0, 0);
    igs_input_create("constraint_string", IGS_STRING_T, 0, 0);
    igs_input_create("constraint_data", IGS_DATA_T, 0, 0);

    assert(igs_input_add_constraint("constraint_int", "min 10.12") == IGS_SUCCESS); //will set 10 as min constraint
    assert(igs_input_add_constraint("constraint_int", "max 10.12") == IGS_SUCCESS); //will set 10 as max constraint
    assert(igs_input_add_constraint("constraint_int", "[-.1, +10.13]") == IGS_SUCCESS);
    assert(igs_input_add_constraint("constraint_int", "[-.1  ,  +10.13]") == IGS_SUCCESS);
    assert(igs_input_add_constraint("constraint_int", "[-.1,+10.13]") == IGS_SUCCESS);
    assert(igs_input_add_constraint("constraint_int", "[1,-10.13]") == IGS_FAILURE);

    assert(igs_input_add_constraint("constraint_double", "min 10.12") == IGS_SUCCESS);
    assert(igs_input_add_constraint("constraint_double", "max 10.12") == IGS_SUCCESS);
    assert(igs_input_add_constraint("constraint_double", "[-.1, +10.13]") == IGS_SUCCESS);
    assert(igs_input_add_constraint("constraint_double", "[12.12,12.12]") == IGS_SUCCESS);

    assert(igs_input_add_constraint("constraint_bool", "min 10.12") == IGS_FAILURE);
    assert(igs_input_add_constraint("constraint_bool", "max 10.12") == IGS_FAILURE);
    assert(igs_input_add_constraint("constraint_bool", "[1,10.13]") == IGS_FAILURE);

    assert(igs_input_add_constraint("constraint_impulsion", "min 10.12") == IGS_FAILURE);
    assert(igs_input_add_constraint("constraint_impulsion", "max 10.12") == IGS_FAILURE);
    assert(igs_input_add_constraint("constraint_impulsion", "[1,10.13]") == IGS_FAILURE);

    assert(igs_input_add_constraint("constraint_string", "min 10.12") == IGS_FAILURE);
    assert(igs_input_add_constraint("constraint_string", "max 10.12") == IGS_FAILURE);
    assert(igs_input_add_constraint("constraint_string", "[1,10.13]") == IGS_FAILURE);

    assert(igs_input_add_constraint("constraint_data", "min 10.12") == IGS_FAILURE);
    assert(igs_input_add_constraint("constraint_data", "max 10.12") == IGS_FAILURE);
    assert(igs_input_add_constraint("constraint_data", "[1,-10.13]") == IGS_FAILURE);

    assert(igs_input_add_constraint("constraint_string", "~ [^ +") == IGS_FAILURE); //bad format for regex
    assert(igs_input_add_constraint("constraint_string", "~ (\\d+)") == IGS_SUCCESS);
    assert(igs_input_add_constraint("constraint_string", "~ (\\d+)") == IGS_SUCCESS);
    assert(igs_input_add_constraint("constraint_impulsion", "~ (\\d+)") == IGS_FAILURE);
    assert(igs_input_add_constraint("constraint_int", "~ (\\d+)") == IGS_FAILURE);
    assert(igs_input_add_constraint("constraint_double", "~ (\\d+)") == IGS_FAILURE);
    assert(igs_input_add_constraint("constraint_bool", "~ (\\d+)") == IGS_FAILURE);
    assert(igs_input_add_constraint("constraint_data", "~ (\\d+)") == IGS_FAILURE);

    igs_input_remove("constraint_impulsion");
    igs_input_remove("constraint_int");
    igs_input_remove("constraint_bool");
    igs_input_remove("constraint_double");
    igs_input_remove("constraint_string");
    igs_input_remove("constraint_data");


    //agent family
    char *family = igs_agent_family();
    assert(family == NULL);
    igs_agent_set_family("family_test");
    family = igs_agent_family();
    assert(streq(family, "family_test"));
    free(family);
    family = NULL;

    //logs
    assert(!igs_log_console());
    assert(!igs_log_console_color());
    assert(!igs_log_stream());
    assert(!igs_log_file());
    char *logPath = igs_log_file_path();
    assert(!logPath);
    igs_log_set_console(true);
    assert(igs_log_console());
    igs_log_set_stream(true);
    assert(igs_log_stream());
    igs_log_set_file_path("/tmp/log.txt");
    logPath = igs_log_file_path();
    assert(logPath && streq(logPath, "/tmp/log.txt"));
    free(logPath);
    logPath = NULL;
    igs_log_set_file(true, NULL);
    logPath = igs_log_file_path();
    assert(!logPath);
    assert(igs_log_file());
    igs_log_set_console_level(IGS_LOG_TRACE);
    assert (igs_log_console_level() == IGS_LOG_TRACE);
    igs_log_set_file_level(IGS_LOG_TRACE);
    igs_log_set_file_max_line_length(1024);
    igs_trace("trace example %d", 1);
    igs_debug("debug  example %d", 2);
    igs_info("info example %d", 3);
    igs_warn("warn example %d", 4);
    igs_error("error example %d", 5);
    igs_fatal("fatal example %d", 6);
    igs_info("multi-line log \n second line");
    logPath = igs_log_file_path();
    assert(strlen(logPath) > 0);
    free(logPath);

    //try to write uninitialized definition and mapping (generates errors)
    igs_definition_save();
    igs_mapping_save();

    //utilities
    assert (igs_version() > 0);
    assert (igs_protocol() >= 2);
    int nb_devices = 0;
    char **devicesList = igs_net_devices_list(&nb_devices);
    for (int i = 0; i < nb_devices; i++){
        igs_info("device %d - %s", i, devicesList[i]);
    }
    igs_free_net_devices_list(devicesList, nb_devices);
    devicesList = igs_net_addresses_list(&nb_devices);
    for (int i = 0; i < nb_devices; i++){
        igs_info("ip %d - %s", i, devicesList[i]);
    }
    igs_free_net_addresses_list(devicesList, nb_devices);
    assert(igs_command_line() == NULL);
    igs_set_command_line("my command line");
    char *commandLine = igs_command_line();
    assert(streq("my command line", commandLine));
    free(commandLine);
    igs_set_command_line_from_args(argc, argv);
    commandLine = igs_command_line();
    igs_info("command line: %s", commandLine);
    free(commandLine);
    assert(!igs_mapping_outputs_request());
    igs_mapping_set_outputs_request(true);
    assert(igs_mapping_outputs_request());

    //general control functions
    assert(igs_pipe_to_ingescape() == NULL);
    assert(!igs_is_started());
    assert(igs_agent_state() == NULL);
    igs_agent_set_state("");
    char *state = igs_agent_state();
    assert(streq(state, ""));
    free(state);
    igs_agent_set_state("my state");
    state = igs_agent_state();
    assert(streq(state, "my state"));
    free(state);
    assert(!igs_agent_is_muted());
    igs_agent_mute();
    assert(igs_agent_is_muted());
    igs_agent_unmute();
    assert(!igs_agent_is_muted());
    assert(!igs_is_frozen());
    igs_freeze();
    assert(igs_is_frozen());
    igs_unfreeze();
    assert(!igs_is_frozen());

    //ios with NULL definition
    assert(igs_input_count() == 0);
    assert(igs_output_count() == 0);
    assert(igs_attribute_count() == 0);
    assert(!igs_input_exists("toto"));
    assert(!igs_output_exists("toto"));
    assert(!igs_attribute_exists("toto"));
    char **listOfStrings = NULL;
    size_t nbElements = 0;
    listOfStrings = igs_input_list(&nbElements);
    assert(listOfStrings == NULL && nbElements == 0);
    listOfStrings = igs_output_list(&nbElements);
    assert(listOfStrings == NULL && nbElements == 0);
    listOfStrings = igs_attribute_list(&nbElements);
    assert(listOfStrings == NULL && nbElements == 0);
    assert(!igs_output_is_muted("toto"));
    igs_output_mute("toto");
    igs_output_unmute("toto");
    assert(!igs_input_bool("toto"));
    assert(!igs_input_int("toto"));
    assert(igs_input_double("toto") < 0.000001);
    assert(!igs_input_string("toto"));
    void *data = NULL;
    size_t dataSize = 0;
    assert(igs_input_data("toto", &data, &dataSize) == IGS_FAILURE);
    igs_clear_input("toto");
    assert(!igs_output_bool("toto"));
    assert(!igs_output_int("toto"));
    assert(igs_output_double("toto") < 0.000001);
    assert(!igs_output_string("toto"));
    assert(igs_output_data("toto", &data, &dataSize) == IGS_FAILURE);
    assert(!igs_attribute_bool("toto"));
    assert(!igs_attribute_int("toto"));
    assert(igs_attribute_double("toto") < 0.000001);
    assert(!igs_attribute_string("toto"));
    assert(igs_attribute_data("toto", &data, &dataSize) == IGS_FAILURE);

    //definition - part 1
    assert(igs_definition_load_str("invalid json") == IGS_FAILURE);
    assert(igs_definition_load_file("/does not exist") == IGS_FAILURE);
    assert(igs_definition_json()); //intentional memory leak here
    assert(streq(igs_agent_name(), "tester")); //intentional memory leak here
    assert(igs_definition_description() == NULL);
    assert(igs_definition_version() == NULL);
    igs_definition_set_description("");
    igs_definition_set_version("");
    //TODO: test loading valid definitions from string and file
    igs_definition_set_description("my description");
    char *defDesc = igs_definition_description();
    assert(streq(defDesc, "my description"));
    free(defDesc);
    igs_definition_set_version("version");
    char *defVer = igs_definition_version();
    assert(streq(defVer, "version"));
    free(defVer);
    assert(igs_input_create("toto", IGS_BOOL_T, NULL, 0) == IGS_SUCCESS);
    assert(igs_output_create("toto", IGS_BOOL_T, NULL, 0) == IGS_SUCCESS);
    assert(igs_attribute_create("toto", IGS_BOOL_T, NULL, 0) == IGS_SUCCESS);
    assert(igs_input_create("toto", IGS_BOOL_T, NULL, 0) == IGS_FAILURE);
    assert(igs_output_create("toto", IGS_BOOL_T, NULL, 0) == IGS_FAILURE);
    assert(igs_attribute_create("toto", IGS_BOOL_T, NULL, 0) == IGS_FAILURE);
    igs_output_mute("toto");
    assert(igs_output_is_muted("toto"));
    igs_output_unmute("toto");
    assert(!igs_output_is_muted("toto"));
    assert(igs_input_remove("toto") == IGS_SUCCESS);
    assert(igs_output_remove("toto") == IGS_SUCCESS);
    assert(igs_attribute_remove("toto") == IGS_SUCCESS);
    assert(igs_input_remove("toto") == IGS_FAILURE);
    assert(igs_output_remove("toto") == IGS_FAILURE);
    assert(igs_attribute_remove("toto") == IGS_FAILURE);

    //inputs
    assert(igs_input_create("my impulsion", IGS_IMPULSION_T, NULL, 0) == IGS_SUCCESS);
    assert(igs_input_create("my impulsion", IGS_IMPULSION_T, NULL, 0) == IGS_FAILURE);
    assert(igs_input_create("my bool", IGS_BOOL_T, &myBool, sizeof(bool)) == IGS_SUCCESS);
    assert(igs_input_create("my bool", IGS_BOOL_T, &myBool, sizeof(bool)) == IGS_FAILURE);
    assert(igs_input_create("my int", IGS_INTEGER_T, &myInt, sizeof(int)) == IGS_SUCCESS);
    assert(igs_input_create("my int", IGS_INTEGER_T, &myInt, sizeof(int)) == IGS_FAILURE);
    assert(igs_input_create("my double", IGS_DOUBLE_T, &myDouble, sizeof(double)) == IGS_SUCCESS);
    assert(igs_input_create("my double", IGS_DOUBLE_T, &myDouble, sizeof(double)) == IGS_FAILURE);
    assert(igs_input_create("my string", IGS_STRING_T, myString, strlen(myString) + 1) == IGS_SUCCESS);
    assert(igs_input_create("my string", IGS_STRING_T, myString, strlen(myString) + 1) == IGS_FAILURE);
    assert(igs_input_create("my data", IGS_DATA_T, myData, 32) == IGS_SUCCESS);
    assert(igs_input_create("my data", IGS_DATA_T, myData, 32) == IGS_FAILURE);
    listOfStrings = NULL;
    listOfStrings = igs_input_list(&nbElements);
    assert(listOfStrings && nbElements == 6);
    igs_free_io_list(listOfStrings, nbElements);
    listOfStrings = NULL;
    assert(igs_input_count() == 6);
    assert(igs_input_type("my impulsion") == IGS_IMPULSION_T);
    assert(igs_input_exists("my impulsion"));
    assert(igs_input_type("my bool") == IGS_BOOL_T);
    assert(igs_input_exists("my bool"));
    assert(igs_input_type("my int") == IGS_INTEGER_T);
    assert(igs_input_exists("my int"));
    assert(igs_input_type("my double") == IGS_DOUBLE_T);
    assert(igs_input_exists("my double"));
    assert(igs_input_type("my string") == IGS_STRING_T);
    assert(igs_input_exists("my string"));
    assert(igs_input_type("my data") == IGS_DATA_T);
    assert(igs_input_exists("my data"));
    assert(igs_input_bool("my bool"));
    assert(igs_input_int("my int") == 1);
    assert(igs_input_double("my double") - 1.0 < 0.000001);
    char *string = igs_input_string("my string");
    assert(streq(string, "my string"));
    free(string);
    data = NULL;
    dataSize = 0;
    assert(igs_input_data("my data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 32 && memcmp(data, myData, dataSize) == 0);
    free(data);
    data = &dataSize;
    dataSize = 1;
    igs_input_data("my impulsion", &data, &dataSize);
    assert(data == NULL && dataSize == 0);
    assert(igs_input_set_bool("", false) == IGS_FAILURE);
    assert(igs_input_set_bool("my bool", false) == IGS_SUCCESS);
    igs_input_data("my bool", &data, &dataSize);//intentional memory leak here
    assert(data && *(bool *)data == false && dataSize == sizeof(bool));
    assert(!igs_input_bool("my bool"));
    assert(igs_input_set_int("", 2) == IGS_FAILURE);
    assert(igs_input_set_int("my int", 2) == IGS_SUCCESS);
    igs_input_data("my int", &data, &dataSize);//intentional memory leak here
    assert(data && *(int *)data == 2 && dataSize == sizeof(int));
    assert(igs_input_int("my int") == 2);
    assert(igs_input_set_double("", 2) == IGS_FAILURE);
    assert(igs_input_set_double("my double", 2) == IGS_SUCCESS);
    igs_input_data("my double", &data, &dataSize);//intentional memory leak here
    assert(data && *(double *)data - 2  < 0.000001 && dataSize == sizeof(double));
    assert(igs_input_double("my double") - 2 < 0.000001);
    assert(igs_input_set_string("", "new string") == IGS_FAILURE);
    assert(igs_input_set_string("my string", "new string") == IGS_SUCCESS);
    igs_input_data("my string", &data, &dataSize);//intentional memory leak here
    assert(data && streq((char *)data, "new string") && dataSize == strlen("new string") + 1);
    string = igs_input_string("my string");
    assert(streq(string, "new string"));
    free(string);
    string = NULL;
    assert(igs_input_set_data("", myOtherData, 64) == IGS_FAILURE);
    assert(igs_input_set_data("my data", myOtherData, 64) == IGS_SUCCESS);
    data = NULL;
    dataSize = 0;
    assert(igs_input_data("my data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 64 && memcmp(data, myOtherData, dataSize) == 0);
    free(data);
    data = NULL;
    dataSize = 0;
    igs_clear_input("my data");
    data = NULL;
    dataSize = 0;
    assert(igs_input_data("my data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 0 && data == NULL);

    //outputs
    assert(igs_output_create("my impulsion", IGS_IMPULSION_T, NULL, 0) == IGS_SUCCESS);
    assert(igs_output_create("my impulsion", IGS_IMPULSION_T, NULL, 0) == IGS_FAILURE);
    assert(igs_output_create("my bool", IGS_BOOL_T, &myBool, sizeof(bool)) == IGS_SUCCESS);
    assert(igs_output_create("my bool", IGS_BOOL_T, &myBool, sizeof(bool)) == IGS_FAILURE);
    assert(igs_output_create("my int", IGS_INTEGER_T, &myInt, sizeof(int)) == IGS_SUCCESS);
    assert(igs_output_create("my int", IGS_INTEGER_T, &myInt, sizeof(int)) == IGS_FAILURE);
    assert(igs_output_create("my double", IGS_DOUBLE_T, &myDouble, sizeof(double)) == IGS_SUCCESS);
    assert(igs_output_create("my double", IGS_DOUBLE_T, &myDouble, sizeof(double)) == IGS_FAILURE);
    assert(igs_output_create("my string", IGS_STRING_T, myString, strlen(myString) + 1) == IGS_SUCCESS);
    assert(igs_output_create("my string", IGS_STRING_T, myString, strlen(myString) + 1) == IGS_FAILURE);
    assert(igs_output_create("my data", IGS_DATA_T, myData, 32) == IGS_SUCCESS);
    assert(igs_output_create("my data", IGS_DATA_T, myData, 32) == IGS_FAILURE);
    listOfStrings = NULL;
    listOfStrings = igs_output_list(&nbElements);
    assert(listOfStrings && nbElements == 6);
    igs_free_io_list(listOfStrings, nbElements);
    listOfStrings = NULL;
    assert(igs_output_count() == 6);
    assert(igs_output_type("my impulsion") == IGS_IMPULSION_T);
    assert(igs_output_exists("my impulsion"));
    assert(igs_output_type("my bool") == IGS_BOOL_T);
    assert(igs_output_exists("my bool"));
    assert(igs_output_type("my int") == IGS_INTEGER_T);
    assert(igs_output_exists("my int"));
    assert(igs_output_type("my double") == IGS_DOUBLE_T);
    assert(igs_output_exists("my double"));
    assert(igs_output_type("my string") == IGS_STRING_T);
    assert(igs_output_exists("my string"));
    assert(igs_output_type("my data") == IGS_DATA_T);
    assert(igs_output_exists("my data"));
    assert(igs_output_bool("my bool"));
    assert(igs_output_int("my int") == 1);
    assert(igs_output_double("my double") - 1.0 < 0.000001);
    string = igs_output_string("my string");
    assert(streq(string, "my string"));
    free(string);
    string = NULL;
    data = NULL;
    dataSize = 0;
    assert(igs_output_data("my data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 32 && memcmp(data, myData, dataSize) == 0);
    free(data);
    data = NULL;
    dataSize = 0;
    assert(igs_output_set_bool("", false) == IGS_FAILURE);
    assert(igs_output_set_bool("my bool", false) == IGS_SUCCESS);
    assert(!igs_output_bool("my bool"));
    assert(igs_output_set_int("", 2) == IGS_FAILURE);
    assert(igs_output_set_int("my int", 2) == IGS_SUCCESS);
    assert(igs_output_int("my int") == 2);
    assert(igs_output_set_double("", 2) == IGS_FAILURE);
    assert(igs_output_set_double("my double", 2) == IGS_SUCCESS);
    assert(igs_output_double("my double") - 2 < 0.000001);
    assert(igs_output_set_string("", "new string") == IGS_FAILURE);
    assert(igs_output_set_string("my string", "new string") == IGS_SUCCESS);
    string = igs_output_string("my string");
    assert(streq(string, "new string"));
    free(string);
    assert(igs_output_set_data("", myOtherData, 64) == IGS_FAILURE);
    assert(igs_output_set_data("my data", myOtherData, 64) == IGS_SUCCESS);
    data = NULL;
    dataSize = 0;
    assert(igs_output_data("my data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 64 && memcmp(data, myOtherData, dataSize) == 0);
    free(data);
    data = NULL;
    dataSize = 0;
    igs_clear_output("my data");
    data = NULL;
    dataSize = 0;
    assert(igs_output_data("my data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 0 && data == NULL);


    //parameters
    assert(igs_attribute_create("my impulsion", IGS_IMPULSION_T, NULL, 0) == IGS_SUCCESS);
    assert(igs_attribute_create("my impulsion", IGS_IMPULSION_T, NULL, 0) == IGS_FAILURE);
    assert(igs_attribute_create("my bool", IGS_BOOL_T, &myBool, sizeof(bool)) == IGS_SUCCESS);
    assert(igs_attribute_create("my bool", IGS_BOOL_T, &myBool, sizeof(bool)) == IGS_FAILURE);
    assert(igs_attribute_create("my int", IGS_INTEGER_T, &myInt, sizeof(int)) == IGS_SUCCESS);
    assert(igs_attribute_create("my int", IGS_INTEGER_T, &myInt, sizeof(int)) == IGS_FAILURE);
    assert(igs_attribute_create("my double", IGS_DOUBLE_T, &myDouble, sizeof(double)) == IGS_SUCCESS);
    assert(igs_attribute_create("my double", IGS_DOUBLE_T, &myDouble, sizeof(double)) == IGS_FAILURE);
    assert(igs_attribute_create("my string", IGS_STRING_T, myString, strlen(myString) + 1) == IGS_SUCCESS);
    assert(igs_attribute_create("my string", IGS_STRING_T, myString, strlen(myString) + 1) == IGS_FAILURE);
    assert(igs_attribute_create("my data", IGS_DATA_T, myData, 32) == IGS_SUCCESS);
    assert(igs_attribute_create("my data", IGS_DATA_T, myData, 32) == IGS_FAILURE);
    listOfStrings = NULL;
    listOfStrings = igs_attribute_list(&nbElements);
    assert(listOfStrings && nbElements == 6);
    igs_free_io_list(listOfStrings, nbElements);
    listOfStrings = NULL;
    assert(igs_attribute_count() == 6);
    assert(igs_attribute_type("my impulsion") == IGS_IMPULSION_T);
    assert(igs_attribute_exists("my impulsion"));
    assert(igs_attribute_type("my bool") == IGS_BOOL_T);
    assert(igs_attribute_exists("my bool"));
    assert(igs_attribute_type("my int") == IGS_INTEGER_T);
    assert(igs_attribute_exists("my int"));
    assert(igs_attribute_type("my double") == IGS_DOUBLE_T);
    assert(igs_attribute_exists("my double"));
    assert(igs_attribute_type("my string") == IGS_STRING_T);
    assert(igs_attribute_exists("my string"));
    assert(igs_attribute_type("my data") == IGS_DATA_T);
    assert(igs_attribute_exists("my data"));
    assert(igs_attribute_bool("my bool"));
    assert(igs_attribute_int("my int") == 1);
    assert(igs_attribute_double("my double") - 1.0 < 0.000001);
    string = igs_attribute_string("my string");
    assert(streq(string, "my string"));
    free(string);
    string = NULL;
    data = NULL;
    dataSize = 0;
    assert(igs_attribute_data("my data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 32 && memcmp(data, myData, dataSize) == 0);
    free(data);
    data = NULL;
    dataSize = 0;
    assert(igs_attribute_set_bool("", false) == IGS_FAILURE);
    assert(igs_attribute_set_bool("my bool", false) == IGS_SUCCESS);
    assert(!igs_attribute_bool("my bool"));
    assert(igs_attribute_set_int("", 2) == IGS_FAILURE);
    assert(igs_attribute_set_int("my int", 2) == IGS_SUCCESS);
    assert(igs_attribute_int("my int") == 2);
    assert(igs_attribute_set_double("", 2) == IGS_FAILURE);
    assert(igs_attribute_set_double("my double", 2) == IGS_SUCCESS);
    assert(igs_attribute_double("my double") - 2 < 0.000001);
    assert(igs_attribute_set_string("", "new string") == IGS_FAILURE);
    assert(igs_attribute_set_string("my string", "new string") == IGS_SUCCESS);
    string = igs_attribute_string("my string");
    assert(streq(string, "new string"));
    free(string);
    assert(igs_attribute_set_data("", myOtherData, 64) == IGS_FAILURE);
    assert(igs_attribute_set_data("my data", myOtherData, 64) == IGS_SUCCESS);
    data = NULL;
    dataSize = 0;
    assert(igs_attribute_data("my data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 64 && memcmp(data, myOtherData, dataSize) == 0);
    free(data);
    data = NULL;
    dataSize = 0;
    igs_clear_parameter("my data");
    data = NULL;
    dataSize = 0;
    assert(igs_attribute_data("my data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 0 && data == NULL);

    //definition - part 2
    //TODO: compare exported def, saved file and reference file
    //io description
    assert (igs_input_description("my impulsion") == NULL);
    assert (igs_input_set_description("my impulsion", "my io description here") == IGS_SUCCESS);
    char * inputDescription = igs_input_description("my impulsion");
    assert (streq(inputDescription, "my io description here"));
    free(inputDescription);
    
    assert (igs_output_description("my impulsion") == NULL);
    assert (igs_output_set_description("my impulsion", "my io description here") == IGS_SUCCESS);
    char * outputDescription = igs_output_description("my impulsion");
    assert (streq(outputDescription, "my io description here"));
    free(outputDescription);
    
    assert (igs_attribute_description("my impulsion") == NULL);
    assert (igs_attribute_set_description("my impulsion", "my io description here") == IGS_SUCCESS);
    char * attributeDescription = igs_attribute_description("my impulsion");
    assert (streq(attributeDescription, "my io description here"));
    free(attributeDescription);
    
    assert (igs_input_set_detailed_type("my impulsion", "protobuf", "some prototbuf \"here\"") == IGS_SUCCESS);
    assert (igs_output_set_detailed_type("my impulsion", "protobuf", "some prototbuf \"here\"") == IGS_SUCCESS);
    assert (igs_attribute_set_detailed_type("my impulsion", "protobuf", "some prototbuf \"here\"") == IGS_SUCCESS);
    char *exportedDef = igs_definition_json();
    assert(exportedDef);
    igs_definition_set_path("/tmp/simple Demo Agent.json");
    igs_definition_save();
    igs_clear_definition();
    igs_clear_definition();
    name = igs_agent_name();
    assert(streq(name, "tester"));
    free(name);
    name = NULL;
    defDesc = igs_definition_description();
    assert(defDesc == NULL);
    defVer = igs_definition_version();
    assert(defVer == NULL);
    listOfStrings = igs_input_list(&nbElements);
    assert(listOfStrings == NULL && nbElements == 0);
    listOfStrings = igs_output_list(&nbElements);
    assert(listOfStrings == NULL && nbElements == 0);
    listOfStrings = igs_attribute_list(&nbElements);
    assert(listOfStrings == NULL && nbElements == 0);
    listOfStrings = igs_service_list(&nbElements);
    assert(listOfStrings == NULL && nbElements == 0);
    //////////////////////////////////
    igs_definition_load_str(exportedDef);
    listOfStrings = NULL;
    listOfStrings = igs_input_list(&nbElements);
    assert(listOfStrings && nbElements == 6);
    igs_free_io_list(listOfStrings, nbElements);
    listOfStrings = NULL;
    assert(igs_input_count() == 6);
    assert(igs_input_type("my impulsion") == IGS_IMPULSION_T);
    assert(igs_input_exists("my impulsion"));
    assert(igs_input_type("my bool") == IGS_BOOL_T);
    assert(igs_input_exists("my bool"));
    assert(igs_input_type("my int") == IGS_INTEGER_T);
    assert(igs_input_exists("my int"));
    assert(igs_input_type("my double") == IGS_DOUBLE_T);
    assert(igs_input_exists("my double"));
    assert(igs_input_type("my string") == IGS_STRING_T);
    assert(igs_input_exists("my string"));
    assert(igs_input_type("my data") == IGS_DATA_T);
    assert(igs_input_exists("my data"));
    listOfStrings = igs_output_list(&nbElements);
    assert(listOfStrings && nbElements == 6);
    igs_free_io_list(listOfStrings, nbElements);
    listOfStrings = NULL;
    assert(igs_output_count() == 6);
    assert(igs_output_type("my impulsion") == IGS_IMPULSION_T);
    assert(igs_output_exists("my impulsion"));
    assert(igs_output_type("my bool") == IGS_BOOL_T);
    assert(igs_output_exists("my bool"));
    assert(igs_output_type("my int") == IGS_INTEGER_T);
    assert(igs_output_exists("my int"));
    assert(igs_output_type("my double") == IGS_DOUBLE_T);
    assert(igs_output_exists("my double"));
    assert(igs_output_type("my string") == IGS_STRING_T);
    assert(igs_output_exists("my string"));
    assert(igs_output_type("my data") == IGS_DATA_T);
    assert(igs_output_exists("my data"));
    assert(!igs_output_bool("my bool"));
    assert(igs_output_int("my int") == 0);
    assert(igs_output_double("my double") == 0.0);
    assert(!igs_output_string("my string"));
    data = NULL;
    dataSize = 0;
    assert(igs_output_data("my data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 0 && data == NULL);
    listOfStrings = igs_attribute_list(&nbElements);
    assert(listOfStrings && nbElements == 6);
    igs_free_io_list(listOfStrings, nbElements);
    listOfStrings = NULL;
    assert(igs_attribute_count() == 6);
    assert(igs_attribute_type("my impulsion") == IGS_IMPULSION_T);
    assert(igs_attribute_exists("my impulsion"));
    assert(igs_attribute_type("my bool") == IGS_BOOL_T);
    assert(igs_attribute_exists("my bool"));
    assert(igs_attribute_type("my int") == IGS_INTEGER_T);
    assert(igs_attribute_exists("my int"));
    assert(igs_attribute_type("my double") == IGS_DOUBLE_T);
    assert(igs_attribute_exists("my double"));
    assert(igs_attribute_type("my string") == IGS_STRING_T);
    assert(igs_attribute_exists("my string"));
    assert(igs_attribute_type("my data") == IGS_DATA_T);
    assert(igs_attribute_exists("my data"));
    assert(!igs_attribute_bool("my bool"));
    assert(igs_attribute_int("my int") == 0);
    assert(igs_attribute_double("my double") == 0.0);
    assert(!igs_attribute_string("my string"));
    data = NULL;
    dataSize = 0;
    assert(igs_attribute_data("my data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 0 && data == NULL);
    igs_clear_definition();
    free(exportedDef);
    exportedDef = NULL;
    //////////////////////////////////
    igs_definition_load_file("/tmp/simple Demo Agent.json");
    listOfStrings = NULL;
    listOfStrings = igs_input_list(&nbElements);
    assert(listOfStrings && nbElements == 6);
    igs_free_io_list(listOfStrings, nbElements);
    listOfStrings = NULL;
    assert(igs_input_count() == 6);
    assert(igs_input_type("my impulsion") == IGS_IMPULSION_T);
    assert(igs_input_exists("my impulsion"));
    assert(igs_input_type("my bool") == IGS_BOOL_T);
    assert(igs_input_exists("my bool"));
    assert(igs_input_type("my int") == IGS_INTEGER_T);
    assert(igs_input_exists("my int"));
    assert(igs_input_type("my double") == IGS_DOUBLE_T);
    assert(igs_input_exists("my double"));
    assert(igs_input_type("my string") == IGS_STRING_T);
    assert(igs_input_exists("my string"));
    assert(igs_input_type("my data") == IGS_DATA_T);
    assert(igs_input_exists("my data"));
    data = NULL;
    dataSize = 0;
    listOfStrings = NULL;
    listOfStrings = igs_output_list(&nbElements);
    assert(listOfStrings && nbElements == 6);
    igs_free_io_list(listOfStrings, nbElements);
    listOfStrings = NULL;
    assert(igs_output_count() == 6);
    assert(igs_output_type("my impulsion") == IGS_IMPULSION_T);
    assert(igs_output_exists("my impulsion"));
    assert(igs_output_type("my bool") == IGS_BOOL_T);
    assert(igs_output_exists("my bool"));
    assert(igs_output_type("my int") == IGS_INTEGER_T);
    assert(igs_output_exists("my int"));
    assert(igs_output_type("my double") == IGS_DOUBLE_T);
    assert(igs_output_exists("my double"));
    assert(igs_output_type("my string") == IGS_STRING_T);
    assert(igs_output_exists("my string"));
    assert(igs_output_type("my data") == IGS_DATA_T);
    assert(igs_output_exists("my data"));
    assert(!igs_output_bool("my bool"));
    assert(igs_output_int("my int") == 0);
    assert(igs_output_double("my double") == 0.0);
    assert(!igs_output_string("my string"));
    data = NULL;
    dataSize = 0;
    assert(igs_output_data("my data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 0 && data == NULL);
    free(data);
    data = NULL;
    dataSize = 0;
    listOfStrings = NULL;
    listOfStrings = igs_attribute_list(&nbElements);
    assert(listOfStrings && nbElements == 6);
    igs_free_io_list(listOfStrings, nbElements);
    listOfStrings = NULL;
    assert(igs_attribute_count() == 6);
    assert(igs_attribute_type("my impulsion") == IGS_IMPULSION_T);
    assert(igs_attribute_exists("my impulsion"));
    assert(igs_attribute_type("my bool") == IGS_BOOL_T);
    assert(igs_attribute_exists("my bool"));
    assert(igs_attribute_type("my int") == IGS_INTEGER_T);
    assert(igs_attribute_exists("my int"));
    assert(igs_attribute_type("my double") == IGS_DOUBLE_T);
    assert(igs_attribute_exists("my double"));
    assert(igs_attribute_type("my string") == IGS_STRING_T);
    assert(igs_attribute_exists("my string"));
    assert(igs_attribute_type("my data") == IGS_DATA_T);
    assert(igs_attribute_exists("my data"));
    assert(!igs_attribute_bool("my bool"));
    assert(igs_attribute_int("my int") == 0);
    assert(igs_attribute_double("my double") == 0.0);
    assert(!igs_attribute_string("my string"));
    data = NULL;
    dataSize = 0;
    assert(igs_attribute_data("my data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 0 && data == NULL);
    free(data);
    igs_clear_definition();


    //mapping & split
    assert(igs_mapping_load_str("invalid json") == IGS_FAILURE);
    assert(igs_mapping_load_file("/does not exist") == IGS_FAILURE);
    assert(igs_mapping_json()); //intentional memory leak here
    assert(igs_mapping_count() == 0);

    assert(igs_mapping_add("toto", "other_agent", "tata") != 0);
    uint64_t mapId = igs_mapping_add("toto", "other_agent", "tata");
    assert(igs_mapping_add("toto", "other_agent", "tata") == mapId);
    assert(mapId > 0);
    assert(igs_mapping_remove_with_id(12345) == IGS_FAILURE);
    assert(igs_mapping_remove_with_id(mapId) == IGS_SUCCESS);
    assert(igs_mapping_add("toto", "other_agent", "tata") > 0);
    assert(igs_mapping_count() == 1);
    assert(igs_mapping_remove_with_name("toto", "other_agent", "tata") == IGS_SUCCESS);
    assert(igs_mapping_remove_with_name("toto", "other_agent", "tata") == IGS_FAILURE);
    assert(igs_mapping_count() == 0);
    assert(igs_mapping_add("toto", "other_agent", "tata") > 0);
    assert(igs_mapping_count() == 1);
    igs_clear_mappings_with_agent("other_agent");
    assert(igs_mapping_count() == 0);
    assert(igs_mapping_remove_with_name("toto", "other_agent", "tata") == IGS_FAILURE);
    assert(igs_mapping_add("toto", "other_agent", "tata") > 0);
    assert(igs_mapping_count() == 1);
    igs_clear_mappings_for_input("toto");
    assert(igs_mapping_count() == 0);
    assert(igs_mapping_add("toto", "other_agent", "tata") > 0);
    assert(igs_mapping_count() == 1);
    igs_clear_mappings_for_input("tata");
    assert(igs_mapping_count() == 1);
    igs_clear_mappings_for_input("toto");
    assert(igs_mapping_count() == 0);


    assert(igs_split_count() == 0);
    assert(igs_split_add("toto", "other_agent", "tata") != 0);
    uint64_t splitId = igs_split_add("toto", "other_agent", "tata");
    assert(splitId > 0);
    assert(igs_split_count() == 1);
    assert(igs_split_remove_with_id(12345) == IGS_FAILURE);
    assert(igs_split_remove_with_id(splitId) == IGS_SUCCESS);
    assert(igs_split_count() == 0);
    assert(igs_split_add("toto", "other_agent", "tata") != 0);
    assert(igs_split_count() == 1);
    assert(igs_split_remove_with_name("toto", "other_agent", "tata") == IGS_SUCCESS);
    assert(igs_split_count() == 0);
    assert(igs_split_remove_with_name("toto", "other_agent", "tata") == IGS_FAILURE);

    igs_clear_mappings();
    igs_mapping_add("toto", "other_agent", "tata");
    igs_split_add("toto", "other_agent", "tata");
    char *exportedMapping = igs_mapping_json();
    assert(exportedMapping);
    igs_mapping_set_path("/tmp/simple Demo Agent mapping.json");
    igs_mapping_save();
    igs_clear_mappings();
    igs_mapping_load_str(exportedMapping);
    assert(igs_mapping_remove_with_name("toto", "other_agent", "tata") == IGS_SUCCESS);
    assert(igs_split_remove_with_name("toto", "other_agent", "tata") == IGS_SUCCESS);
    free(exportedMapping);
    igs_clear_mappings();
    igs_mapping_load_file("/tmp/simple Demo Agent mapping.json");
    assert(igs_mapping_remove_with_name("toto", "other_agent", "tata") == IGS_SUCCESS);
    assert(igs_split_remove_with_name("toto", "other_agent", "tata") == IGS_SUCCESS);
    igs_clear_mappings();

    //services
    igs_service_arg_t *list = NULL;
    igs_service_args_add_bool(&list, myBool);
    igs_service_args_add_int(&list, myInt);
    igs_service_args_add_double(&list, myDouble);
    igs_service_args_add_string(&list, myString);
    igs_service_args_add_data(&list, myData, 32);
    assert(list->name == NULL);
    assert(list->type == IGS_BOOL_T);
    assert(list->size == sizeof(bool));
    assert(list->b == myBool);
    assert(list->next->name == NULL);
    assert(list->next->type == IGS_INTEGER_T);
    assert(list->next->size == sizeof(int));
    assert(list->next->i == myInt);
    assert(list->next->next->name == NULL);
    assert(list->next->next->type == IGS_DOUBLE_T);
    assert(list->next->next->size == sizeof(double));
    assert(list->next->next->d - myDouble < 0.000001);
    assert(list->next->next->next->name == NULL);
    assert(list->next->next->next->type == IGS_STRING_T);
    assert(list->next->next->next->size == strlen(myString) + 1);
    assert(streq(list->next->next->next->c, myString));
    assert(list->next->next->next->next->name == NULL);
    assert(list->next->next->next->next->type == IGS_DATA_T);
    assert(list->next->next->next->next->size == 32);
    assert(memcmp(list->next->next->next->next->data, myData, 32) == 0);
    igs_service_arg_t *listBis = igs_service_args_clone(list);
    igs_service_args_destroy(&list);
    assert(listBis->name == NULL);
    assert(listBis->type == IGS_BOOL_T);
    assert(listBis->size == sizeof(bool));
    assert(listBis->b == myInt);
    assert(listBis->next->name == NULL);
    assert(listBis->next->type == IGS_INTEGER_T);
    assert(listBis->next->size == sizeof(int));
    assert(listBis->next->i == myBool);
    assert(listBis->next->next->name == NULL);
    assert(listBis->next->next->type == IGS_DOUBLE_T);
    assert(listBis->next->next->size == sizeof(double));
    assert(listBis->next->next->d - myDouble < 0.000001);
    assert(listBis->next->next->next->name == NULL);
    assert(listBis->next->next->next->type == IGS_STRING_T);
    assert(listBis->next->next->next->size == strlen(myString) + 1);
    assert(streq(listBis->next->next->next->c, myString));
    assert(listBis->next->next->next->next->name == NULL);
    assert(listBis->next->next->next->next->type == IGS_DATA_T);
    assert(listBis->next->next->next->next->size == 32);
    assert(memcmp(listBis->next->next->next->next->data, myData, 32) == 0);
    igs_service_args_destroy(&listBis);
    igs_service_args_destroy(&listBis);

    assert(igs_service_count() == 0);
    assert(!igs_service_exists("toto"));
    listOfStrings = igs_service_list(&nbElements);
    assert(listOfStrings == NULL && nbElements == 0);
    assert(igs_service_args_first("toto") == NULL);
    assert(igs_service_args_count("toto") == 0);
    assert(igs_service_arg_exists("toto", "toto") == 0);
    assert(igs_service_init("myService", testerServiceCallback, NULL) == IGS_SUCCESS);
    assert(igs_service_remove("myService") == IGS_SUCCESS);
    assert(igs_service_remove("myService") == IGS_FAILURE);
    assert(igs_service_arg_add("myService", "myArg", IGS_BOOL_T) == IGS_FAILURE);
    assert(igs_service_arg_remove("myService", "myArg") == IGS_FAILURE);

    assert(igs_service_init("myService", testerServiceCallback, NULL) == IGS_SUCCESS);
    assert(igs_service_arg_add("myService", "myBool", IGS_BOOL_T) == IGS_SUCCESS);
    assert(igs_service_arg_add("myService", "myInt", IGS_INTEGER_T) == IGS_SUCCESS);
    assert(igs_service_arg_add("myService", "myDouble", IGS_DOUBLE_T) == IGS_SUCCESS);
    assert(igs_service_arg_add("myService", "myString", IGS_STRING_T) == IGS_SUCCESS);
    assert(igs_service_arg_add("myService", "myData", IGS_DATA_T) == IGS_SUCCESS);
    assert(igs_service_count() == 1);
    assert(igs_service_exists("myService"));
    listOfStrings = igs_service_list(&nbElements);
    assert(nbElements == 1 && streq(listOfStrings[0], "myService"));
    
    assert(igs_service_description("myService") == NULL);
    assert(igs_service_set_description("unknow", "myService description") == IGS_FAILURE);
    assert(igs_service_set_description("myService", "myService description") == IGS_SUCCESS);
    char * servicedescription = igs_service_description("myService");
    assert(streq(servicedescription, "myService description"));
    free(servicedescription);
    
    assert(igs_service_arg_description("myService", "myBool") == NULL);
    assert(igs_service_arg_set_description("unknow", "myBool", "myBool description") == IGS_FAILURE);
    assert(igs_service_arg_set_description("myService", "unknow", "myBool description") == IGS_FAILURE);
    assert(igs_service_arg_set_description("myService", "myBool", "myBool description") == IGS_SUCCESS);
    char * serviceArgDescription = igs_service_arg_description("myService", "myBool");
    assert(streq(serviceArgDescription, "myBool description"));
    free(serviceArgDescription);
    assert(igs_service_arg_description("unknow", "myBool") == NULL);
    assert(igs_service_arg_description("myService", "unknow") == NULL);
    
    char * currentDefinition = igs_definition_json();
    igs_clear_definition();
    igs_definition_load_str(currentDefinition);
    char * newServiceDescription = igs_service_description("myService");
    assert(streq(newServiceDescription, "myService description"));
    free(newServiceDescription);
    char * newServiceArgDescription = igs_service_arg_description("myService", "myBool");
    assert(streq(newServiceArgDescription, "myBool description"));
    free(newServiceArgDescription);
    
    igs_free_services_list(listOfStrings, nbElements);
    listOfStrings = NULL;
    assert(listOfStrings == NULL);
    assert(igs_service_remove("myService") == IGS_SUCCESS);

    assert(igs_service_init("myService", testerServiceCallback, NULL) == IGS_SUCCESS);
    assert(igs_service_set_description("myService", "myService description") == IGS_SUCCESS);
    assert(igs_service_arg_add("myService", "myBool", IGS_BOOL_T) == IGS_SUCCESS);
    assert(igs_service_arg_set_description("myService", "myBool", "myBool description") == IGS_SUCCESS);
    assert(igs_service_arg_add("myService", "myInt", IGS_INTEGER_T) == IGS_SUCCESS);
    assert(igs_service_arg_add("myService", "myDouble", IGS_DOUBLE_T) == IGS_SUCCESS);
    assert(igs_service_arg_add("myService", "myString", IGS_STRING_T) == IGS_SUCCESS);
    assert(igs_service_arg_add("myService", "myData", IGS_DATA_T) == IGS_SUCCESS);
    assert(igs_service_args_count("myService") == 5);
    assert(igs_service_arg_exists("myService", "myBool"));
    assert(igs_service_arg_exists("myService", "myInt"));
    assert(igs_service_arg_exists("myService", "myDouble"));
    assert(igs_service_arg_exists("myService", "myString"));
    assert(igs_service_arg_exists("myService", "myData"));
    list = igs_service_args_first("myService");
    assert(list);
    assert(streq(list->name, "myBool"));
    assert(list->type == IGS_BOOL_T);
    assert(list->size == sizeof(bool));
    assert(list->b == false);
    assert(streq(list->next->name, "myInt"));
    assert(list->next->type == IGS_INTEGER_T);
    assert(list->next->size == sizeof(int));
    assert(list->next->i == 0);
    assert(streq(list->next->next->name, "myDouble"));
    assert(list->next->next->type == IGS_DOUBLE_T);
    assert(list->next->next->size == sizeof(double));
    assert(list->next->next->d < 0.000001);
    assert(streq(list->next->next->next->name, "myString"));
    assert(list->next->next->next->type == IGS_STRING_T);
    assert(list->next->next->next->size == 0);
    assert(list->next->next->next->c == NULL);
    assert(streq(list->next->next->next->next->name, "myData"));
    assert(list->next->next->next->next->type == IGS_DATA_T);
    assert(list->next->next->next->next->size == 0);
    assert(list->next->next->next->next->data == NULL);
    igs_definition_save();
    assert(igs_service_remove("myService") == IGS_SUCCESS);
    igs_clear_definition();
    igs_definition_load_file("/tmp/simple Demo Agent.json");
    assert(igs_service_args_count("myService") == 5);
    assert(igs_service_arg_exists("myService", "myBool"));
    assert(igs_service_arg_exists("myService", "myInt"));
    assert(igs_service_arg_exists("myService", "myDouble"));
    assert(igs_service_arg_exists("myService", "myString"));
    assert(igs_service_arg_exists("myService", "myData"));
    list = igs_service_args_first("myService");
    assert(list);
    assert(streq(list->name, "myBool"));
    assert(list->type == IGS_BOOL_T);
    assert(list->size == 0);
    assert(list->b == false);
    assert(streq(list->next->name, "myInt"));
    assert(list->next->type == IGS_INTEGER_T);
    assert(list->next->size == 0);
    assert(list->next->i == 0);
    assert(streq(list->next->next->name, "myDouble"));
    assert(list->next->next->type == IGS_DOUBLE_T);
    assert(list->next->next->size == 0);
    assert(list->next->next->d  < 0.000001);
    assert(streq(list->next->next->next->name, "myString"));
    assert(list->next->next->next->type == IGS_STRING_T);
    assert(list->next->next->next->size == 0);
    assert(list->next->next->next->c == NULL);
    assert(streq(list->next->next->next->next->name, "myData"));
    assert(list->next->next->next->next->type == IGS_DATA_T);
    assert(list->next->next->next->next->size == 0);
    assert(list->next->next->next->next->data == NULL);

    //service with reply
    assert(igs_service_init("myServiceWithReplies", testerServiceCallback, NULL) == IGS_SUCCESS);
    assert(!igs_service_has_replies("myServiceWithReplies"));
    assert(!igs_service_has_reply("myServiceWithReplies", "toto"));
    size_t replies_nb = 0;
    char** names = igs_service_reply_names("myServiceWithReplies", &replies_nb);
    assert(names == NULL && replies_nb == 0);
    assert(igs_service_reply_args_first("myServiceWithReplies", "toto") == NULL);
    assert(igs_service_reply_args_count("myServiceWithReplies", "toto") == 0);
    assert(igs_service_reply_add("myServiceWithReplies", "myReply") == IGS_SUCCESS);
    assert(igs_service_reply_add("myServiceWithReplies", "myReply2") == IGS_SUCCESS);
    assert(igs_service_has_replies("myServiceWithReplies"));
    assert(igs_service_has_reply("myServiceWithReplies", "myReply"));
    assert(igs_service_has_reply("myServiceWithReplies", "myReply2"));
    assert(!igs_service_has_reply("myServiceWithReplies", "myReply3"));
    assert(igs_service_has_replies("myServiceWithReplies"));
    names = igs_service_reply_names("myServiceWithReplies", &replies_nb);
    assert(names && replies_nb == 2 && streq("myReply", names[0]) && streq("myReply2", names[1]));
    igs_free_services_list(names, replies_nb);
    
    assert(igs_service_reply_description("myServiceWithReplies", "myReply") == NULL);
    assert(igs_service_reply_set_description("unknow", "myReply", "myServiceReply description") == IGS_FAILURE);
    assert(igs_service_reply_set_description("myServiceWithReplies", "unknow", "myServiceReply description") == IGS_FAILURE);
    assert(igs_service_reply_set_description("myServiceWithReplies", "myReply", "myServiceReply description") == IGS_SUCCESS);
    char * replyDescription = igs_service_reply_description("myServiceWithReplies", "myReply");
    assert(streq(replyDescription, "myServiceReply description"));
    free(replyDescription);
    
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply", "myBool", IGS_BOOL_T) == IGS_SUCCESS);
    assert(igs_service_reply_arg_description("myServiceWithReplies", "myReply", "myBool") == NULL);
    assert(igs_service_reply_arg_set_description("unknow", "myReply", "myBool", "myArgBool description") == IGS_FAILURE);
    assert(igs_service_reply_arg_set_description("myServiceWithReplies", "unknow", "myBool", "myArgBool description") == IGS_FAILURE);
    assert(igs_service_reply_arg_set_description("myServiceWithReplies", "myReply", "unknow", "myArgBool description") == IGS_FAILURE);
    assert(igs_service_reply_arg_set_description("myServiceWithReplies", "myReply", "myBool", "myArgBool description") == IGS_SUCCESS);
    char * replyArgDescription = igs_service_reply_arg_description("myServiceWithReplies", "myReply", "myBool");
    assert(streq(replyArgDescription, "myArgBool description"));
    free(replyArgDescription);
    
    currentDefinition = igs_definition_json();
    igs_clear_definition();
    igs_definition_load_str(currentDefinition);
    char * newReplyDescription = igs_service_reply_description("myServiceWithReplies", "myReply");
    assert(streq(newReplyDescription, "myServiceReply description"));
    free(newReplyDescription);
    char * newReplyArgDescription = igs_service_reply_arg_description("myServiceWithReplies", "myReply", "myBool");
    assert(streq(newReplyArgDescription, "myArgBool description"));
    free(newReplyArgDescription);
    
    assert(igs_service_reply_arg_set_description("myServiceWithReplies", "myReply", "myBool", "") == IGS_SUCCESS);
    igs_service_reply_arg_remove("myServiceWithReplies", "myReply", "myBool");
    
    assert(igs_service_reply_args_first("myServiceWithReplies", "myReply") == NULL);
    assert(igs_service_reply_args_count("myServiceWithReplies", "myReply") == 0);
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply", "myBool", IGS_BOOL_T) == IGS_SUCCESS);
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply", "myInt", IGS_INTEGER_T) == IGS_SUCCESS);
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply", "myDouble", IGS_DOUBLE_T) == IGS_SUCCESS);
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply", "myString", IGS_STRING_T) == IGS_SUCCESS);
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply", "myData", IGS_DATA_T) == IGS_SUCCESS);
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply2", "myBool2", IGS_BOOL_T) == IGS_SUCCESS);
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply2", "myInt2", IGS_INTEGER_T) == IGS_SUCCESS);
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply2", "myDouble2", IGS_DOUBLE_T) == IGS_SUCCESS);
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply2", "myString2", IGS_STRING_T) == IGS_SUCCESS);
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply2", "myData2", IGS_DATA_T) == IGS_SUCCESS);
    assert(igs_service_reply_args_first("myServiceWithReplies", "myReply"));
    assert(igs_service_reply_args_count("myServiceWithReplies", "myReply") == 5);
    assert(igs_service_reply_args_first("myServiceWithReplies", "myReply2"));
    assert(igs_service_reply_args_count("myServiceWithReplies", "myReply2") == 5);
    assert(igs_service_reply_arg_exists("myServiceWithReplies", "myReply", "myBool"));
    assert(igs_service_reply_arg_exists("myServiceWithReplies", "myReply", "myInt"));
    assert(igs_service_reply_arg_exists("myServiceWithReplies", "myReply", "myDouble"));
    assert(igs_service_reply_arg_exists("myServiceWithReplies", "myReply", "myString"));
    assert(igs_service_reply_arg_exists("myServiceWithReplies", "myReply", "myData"));
    assert(igs_service_reply_arg_exists("myServiceWithReplies", "myReply2", "myBool2"));
    assert(igs_service_reply_arg_exists("myServiceWithReplies", "myReply2", "myInt2"));
    assert(igs_service_reply_arg_exists("myServiceWithReplies", "myReply2", "myDouble2"));
    assert(igs_service_reply_arg_exists("myServiceWithReplies", "myReply2", "myString2"));
    assert(igs_service_reply_arg_exists("myServiceWithReplies", "myReply2", "myData2"));
    assert(igs_service_reply_arg_remove("myServiceWithReplies", "myReply", "myBool") == IGS_SUCCESS);
    assert(igs_service_reply_arg_remove("myServiceWithReplies", "myReply", "myInt") == IGS_SUCCESS);
    assert(igs_service_reply_arg_remove("myServiceWithReplies", "myReply", "myDouble") == IGS_SUCCESS);
    assert(igs_service_reply_arg_remove("myServiceWithReplies", "myReply", "myString") == IGS_SUCCESS);
    assert(igs_service_reply_arg_remove("myServiceWithReplies", "myReply", "myData") == IGS_SUCCESS);
    assert(igs_service_reply_arg_remove("myServiceWithReplies", "myReply", "myBool") == IGS_FAILURE);
    assert(igs_service_reply_arg_remove("myServiceWithReplies", "myReply", "myInt") == IGS_FAILURE);
    assert(igs_service_reply_arg_remove("myServiceWithReplies", "myReply", "myDouble") == IGS_FAILURE);
    assert(igs_service_reply_arg_remove("myServiceWithReplies", "myReply", "myString") == IGS_FAILURE);
    assert(igs_service_reply_arg_remove("myServiceWithReplies", "myReply", "myData") == IGS_FAILURE);
    assert(!igs_service_reply_arg_exists("myServiceWithReplies", "myReply", "myBool"));
    assert(!igs_service_reply_arg_exists("myServiceWithReplies", "myReply", "myInt"));
    assert(!igs_service_reply_arg_exists("myServiceWithReplies", "myReply", "myDouble"));
    assert(!igs_service_reply_arg_exists("myServiceWithReplies", "myReply", "myString"));
    assert(!igs_service_reply_arg_exists("myServiceWithReplies", "myReply", "myData"));
    assert(igs_service_reply_args_count("myServiceWithReplies", "myReply") == 0);
    assert(igs_service_reply_args_first("myServiceWithReplies", "myReply") == NULL);
    assert(igs_service_reply_remove("myServiceWithReplies", "myReply") == IGS_SUCCESS);
    assert(igs_service_reply_remove("myServiceWithReplies", "myReply") == IGS_FAILURE);
    assert(!igs_service_has_reply("myServiceWithReplies", "myReply"));
    names = igs_service_reply_names("myServiceWithReplies", &replies_nb);
    assert(replies_nb == 1 && streq("myReply2", names[0]));
    igs_free_services_list(names, replies_nb);
    assert(igs_service_reply_arg_remove("myServiceWithReplies", "myReply2", "myBool2") == IGS_SUCCESS);
    assert(igs_service_reply_arg_remove("myServiceWithReplies", "myReply2", "myInt2") == IGS_SUCCESS);
    assert(igs_service_reply_arg_remove("myServiceWithReplies", "myReply2", "myDouble2") == IGS_SUCCESS);
    assert(igs_service_reply_arg_remove("myServiceWithReplies", "myReply2", "myString2") == IGS_SUCCESS);
    assert(igs_service_reply_arg_remove("myServiceWithReplies", "myReply2", "myData2") == IGS_SUCCESS);
    assert(igs_service_reply_arg_remove("myServiceWithReplies", "myReply2", "myBool2") == IGS_FAILURE);
    assert(igs_service_reply_arg_remove("myServiceWithReplies", "myReply2", "myInt2") == IGS_FAILURE);
    assert(igs_service_reply_arg_remove("myServiceWithReplies", "myReply2", "myDouble2") == IGS_FAILURE);
    assert(igs_service_reply_arg_remove("myServiceWithReplies", "myReply2", "myString2") == IGS_FAILURE);
    assert(igs_service_reply_arg_remove("myServiceWithReplies", "myReply2", "myData2") == IGS_FAILURE);
    assert(!igs_service_reply_arg_exists("myServiceWithReplies", "myReply2", "myBool2"));
    assert(!igs_service_reply_arg_exists("myServiceWithReplies", "myReply2", "myInt2"));
    assert(!igs_service_reply_arg_exists("myServiceWithReplies", "myReply2", "myDouble2"));
    assert(!igs_service_reply_arg_exists("myServiceWithReplies", "myReply2", "myString2"));
    assert(!igs_service_reply_arg_exists("myServiceWithReplies", "myReply2", "myData2"));
    assert(igs_service_reply_args_count("myServiceWithReplies", "myReply2") == 0);
    assert(igs_service_reply_args_first("myServiceWithReplies", "myReply2") == NULL);
    assert(igs_service_has_reply("myServiceWithReplies", "myReply2"));
    assert(igs_service_reply_remove("myServiceWithReplies", "myReply2") == IGS_SUCCESS);
    assert(igs_service_reply_remove("myServiceWithReplies", "myReply2") == IGS_FAILURE);
    assert(!igs_service_has_reply("myServiceWithReplies", "myReply2"));
    names = igs_service_reply_names("myServiceWithReplies", &replies_nb);
    assert(names == NULL && replies_nb == 0);
    assert(!igs_service_has_replies("myServiceWithReplies"));
    assert(igs_service_reply_add("myServiceWithReplies", "myReply") == IGS_SUCCESS);
    assert(igs_service_reply_add("myServiceWithReplies", "myReply2") == IGS_SUCCESS);
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply", "myBool", IGS_BOOL_T) == IGS_SUCCESS);
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply", "myInt", IGS_INTEGER_T) == IGS_SUCCESS);
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply", "myDouble", IGS_DOUBLE_T) == IGS_SUCCESS);
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply", "myString", IGS_STRING_T) == IGS_SUCCESS);
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply", "myData", IGS_DATA_T) == IGS_SUCCESS);
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply2", "myBool2", IGS_BOOL_T) == IGS_SUCCESS);
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply2", "myInt2", IGS_INTEGER_T) == IGS_SUCCESS);
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply2", "myDouble2", IGS_DOUBLE_T) == IGS_SUCCESS);
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply2", "myString2", IGS_STRING_T) == IGS_SUCCESS);
    assert(igs_service_reply_arg_add("myServiceWithReplies", "myReply2", "myData2", IGS_DATA_T) == IGS_SUCCESS);
    igs_definition_set_class("my_class");
    igs_definition_set_package("my::package");
    igs_definition_save();
    assert(igs_service_remove("myServiceWithReplies") == IGS_SUCCESS);
    igs_clear_definition();
    igs_definition_load_file("/tmp/simple Demo Agent.json");
    assert(streq(igs_definition_class(),"my_class")); //intentional memory leak here
    assert(streq(igs_definition_package(),"my::package")); //intentional memory leak here
    assert(igs_service_has_reply("myServiceWithReplies", "myReply"));
    assert(igs_service_has_reply("myServiceWithReplies", "myReply2"));
    assert(igs_service_reply_args_first("myServiceWithReplies", "myReply"));
    assert(igs_service_reply_args_count("myServiceWithReplies", "myReply") == 5);
    assert(igs_service_reply_args_first("myServiceWithReplies", "myReply2"));
    assert(igs_service_reply_args_count("myServiceWithReplies", "myReply2") == 5);
    assert(igs_service_reply_arg_exists("myServiceWithReplies", "myReply", "myBool"));
    assert(igs_service_reply_arg_exists("myServiceWithReplies", "myReply", "myInt"));
    assert(igs_service_reply_arg_exists("myServiceWithReplies", "myReply", "myDouble"));
    assert(igs_service_reply_arg_exists("myServiceWithReplies", "myReply", "myString"));
    assert(igs_service_reply_arg_exists("myServiceWithReplies", "myReply", "myData"));
    assert(igs_service_reply_arg_exists("myServiceWithReplies", "myReply2", "myBool2"));
    assert(igs_service_reply_arg_exists("myServiceWithReplies", "myReply2", "myInt2"));
    assert(igs_service_reply_arg_exists("myServiceWithReplies", "myReply2", "myDouble2"));
    assert(igs_service_reply_arg_exists("myServiceWithReplies", "myReply2", "myString2"));
    assert(igs_service_reply_arg_exists("myServiceWithReplies", "myReply2", "myData2"));

    //channel
    assert(igs_peer_add_header("publisher", "toto") == IGS_FAILURE);
    assert(igs_peer_add_header("logger", "toto") == IGS_FAILURE);
    assert(igs_peer_add_header("pid", "toto") == IGS_FAILURE);
    assert(igs_peer_add_header("commandline", "toto") == IGS_FAILURE);
    assert(igs_peer_add_header("hostname", "toto") == IGS_FAILURE);
    assert(igs_peer_remove_header("bogus key") == IGS_FAILURE);
    assert(igs_peer_remove_header("publisher") == IGS_FAILURE);
    assert(igs_peer_remove_header("logger") == IGS_FAILURE);
    assert(igs_peer_remove_header("pid") == IGS_FAILURE);
    assert(igs_peer_remove_header("commandline") == IGS_FAILURE);
    assert(igs_peer_remove_header("hostname") == IGS_FAILURE);
    assert(igs_peer_add_header("new key", "toto") == IGS_SUCCESS);
    assert(igs_peer_remove_header("new key") == IGS_SUCCESS);
    assert(igs_peer_remove_header("new key") == IGS_FAILURE);
    assert(igs_channel_join("toto") == IGS_FAILURE);
    igs_channel_leave("toto");
    assert(igs_peer_add_header("new key", "toto") == IGS_SUCCESS);


    //prepare agent for dynamic tests by adding proper complete definitions
    igs_agent_set_name(agentName);
    igs_log_set_console(true);
    igs_observe_channels(testerChannelCallback, NULL);

    igs_definition_set_description("One example for each type of IO and call");
    igs_definition_set_version("1.0");
    igs_input_create("my impulsion", IGS_IMPULSION_T, NULL, 0);
    igs_input_create("my bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igs_input_create("my int", IGS_INTEGER_T, &myInt, sizeof(int));
    igs_input_create("my double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igs_input_create("my string", IGS_STRING_T, myString, strlen(myString) + 1);
    igs_input_create("my data", IGS_DATA_T, myData, 32);
    igs_input_create("my impulsion_split", IGS_IMPULSION_T, NULL, 0);
    igs_input_create("my bool_split", IGS_BOOL_T, &myBool, sizeof(bool));
    igs_input_create("my int_split", IGS_INTEGER_T, &myInt, sizeof(int));
    igs_input_create("my double_split", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igs_input_create("my string_split", IGS_STRING_T, myString, strlen(myString) + 1);
    igs_input_create("my data_split", IGS_DATA_T, myData, 32);
    igs_output_create("my impulsion", IGS_IMPULSION_T, NULL, 0);
    igs_output_create("my bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igs_output_create("my int", IGS_INTEGER_T, &myInt, sizeof(int));
    igs_output_create("my double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igs_output_create("my string", IGS_STRING_T, myString, strlen(myString) + 1);
    igs_output_create("my data", IGS_DATA_T, myData, 32);
    igs_attribute_create("my impulsion", IGS_IMPULSION_T, NULL, 0);
    igs_attribute_create("my bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igs_attribute_create("my int", IGS_INTEGER_T, &myInt, sizeof(int));
    igs_attribute_create("my double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igs_attribute_create("my string", IGS_STRING_T, myString, strlen(myString) + 1);
    igs_attribute_create("my data", IGS_DATA_T, myData, 32);
    igs_service_init("myService", testerServiceCallback, NULL);
    igs_service_arg_add("myService", "myBool", IGS_BOOL_T);
    igs_service_arg_add("myService", "myInt", IGS_INTEGER_T);
    igs_service_arg_add("myService", "myDouble", IGS_DOUBLE_T);
    igs_service_arg_add("myService", "myString", IGS_STRING_T);
    igs_service_arg_add("myService", "myData", IGS_DATA_T);

    igs_observe_input("my impulsion", testerIOCallback, NULL);
    igs_observe_input("my bool", testerIOCallback, NULL);
    igs_observe_input("my int", testerIOCallback, NULL);
    igs_observe_input("my double", testerIOCallback, NULL);
    igs_observe_input("my string", testerIOCallback, NULL);
    igs_observe_input("my data", testerIOCallback, NULL);
    igs_observe_input("my impulsion_split", testerIOCallback, NULL);
    igs_observe_input("my bool_split", testerIOCallback, NULL);
    igs_observe_input("my int_split", testerIOCallback, NULL);
    igs_observe_input("my double_split", testerIOCallback, NULL);
    igs_observe_input("my string_split", testerIOCallback, NULL);
    igs_observe_input("my data_split", testerIOCallback, NULL);

    igs_mapping_add("my impulsion", "partner", "sparing_impulsion");
    igs_mapping_add("my bool", "partner", "sparing_bool");
    igs_mapping_add("my int", "partner", "sparing_int");
    igs_mapping_add("my double", "partner", "sparing_double");
    igs_mapping_add("my string", "partner", "sparing_string");
    igs_mapping_add("my data", "partner", "sparing_data");

    igs_split_add("my impulsion_split", "partner", "sparing_impulsion");
    igs_split_add("my bool_split", "partner", "sparing_bool");
    igs_split_add("my int_split", "partner", "sparing_int");
    igs_split_add("my double_split", "partner", "sparing_double");
    igs_split_add("my string_split", "partner", "sparing_string");
    igs_split_add("my data_split", "partner", "sparing_data");

    //io description
    igs_input_set_description("my impulsion", "my io description here");
    igs_input_set_description("my impulsion", "my io description here");
    igs_output_set_description("my impulsion", "my io description here");
    igs_output_set_description("my impulsion", "my io description here");
    igs_attribute_set_description("my impulsion", "my attribute description here");
    igs_attribute_set_description("my impulsion", "my attribute description here");

    //IO writing and types conversions
    igs_input_set_impulsion("my impulsion");
    igs_input_set_impulsion("my bool");
    assert(!igs_input_bool("my bool"));
    igs_input_set_impulsion("my int");
    assert(igs_input_int("my int") == 0);
    igs_input_set_impulsion("my double");
    assert(igs_input_double("my double")  < 0.000001);
    igs_input_set_impulsion("my string");
    char *readResult = igs_input_string("my string");
    assert(strlen(readResult) == 0);
    free(readResult);
    igs_input_set_impulsion("my data");
    igs_input_data("my data", &data, &dataSize);
    assert(data == NULL && dataSize == 0);

    igs_input_set_bool("my impulsion", true);
    igs_input_set_bool("my bool", true);
    assert(igs_input_bool("my bool"));
    igs_input_set_bool("my int", true);
    assert(igs_input_int("my int") == 1);
    igs_input_set_bool("my double", true);
    assert(igs_input_double("my double") - 1.0 < 0.000001);
    igs_input_set_bool("my string", true);
    readResult = igs_input_string("my string");
    assert(streq(readResult, "1"));
    free(readResult);
    igs_input_set_bool("my data", true);
    igs_input_data("my data", &data, &dataSize);
    assert(*(bool*)data && dataSize == sizeof(bool));

    igs_input_set_int("my impulsion", 3);
    igs_input_set_int("my bool", 3);
    assert(igs_input_bool("my bool"));
    igs_input_set_int("my int", 3);
    assert(igs_input_int("my int") == 3);
    igs_input_set_int("my double", 3);
    assert(igs_input_double("my double") - 3.0 < 0.000001);
    igs_input_set_int("my string", 3);
    readResult = igs_input_string("my string");
    assert(streq(readResult, "3"));
    free(readResult);
    igs_input_set_int("my data", 3);
    igs_input_data("my data", &data, &dataSize);
    assert(*(int*)data == 3 && dataSize == sizeof(int));

    igs_input_set_double("my impulsion", 3.3);
    igs_input_set_double("my bool", 3.3);
    assert(igs_input_bool("my bool"));
    igs_input_set_double("my int", 3.3);
    assert(igs_input_int("my int") == 3);
    igs_input_set_double("my double", 3.3);
    assert(igs_input_double("my double") - 3.3 < 0.000001);
    igs_input_set_double("my string", 3.3);
    readResult = igs_input_string("my string");
    assert(streq(readResult, "3.300000"));
    free(readResult);
    igs_input_set_double("my data", 3.3);
    igs_input_data("my data", &data, &dataSize);
    assert(*(double*)data - 3.3 < 0.000001 && dataSize == sizeof(double));

    igs_input_set_string("my impulsion", "true");
    igs_input_set_string("my bool", "true");
    assert(igs_input_bool("my bool"));
    igs_input_set_string("my int", "3.3");
    assert(igs_input_int("my int") == 3);
    igs_input_set_string("my double", "3.3");
    assert(igs_input_double("my double") - 3.3 < 0.000001);
    igs_input_set_string("my string", "3.3");
    readResult = igs_input_string("my string");
    assert(streq(readResult, "3.3"));
    free(readResult);
    assert(igs_input_set_string("my data", "toto") == IGS_FAILURE);
    assert(igs_input_set_string("my data", "0123456789abcdef") == IGS_SUCCESS);
    igs_input_data("my data", &data, &dataSize);
    assert(dataSize == 8);

    data = (void*)"my data";
    dataSize = strlen("my data") + 1;
    igs_input_set_data("my impulsion", data, dataSize);
    igs_input_set_data("my bool", data, dataSize);
    igs_input_set_data("my int", data, dataSize);
    igs_input_set_data("my double", data, dataSize);
    igs_input_set_data("my string", data, dataSize);
    igs_input_set_data("my data", data, dataSize);
    igs_input_data("my data", &data, &dataSize);
    assert(streq("my data", (char*)data) && dataSize == strlen("my data") + 1);

    igs_input_set_bool("my bool", true);
    assert(igs_input_bool("my bool"));
    assert(igs_input_int("my bool") == 1);
    assert(igs_input_double("my bool") - 1 < 0.000001);
    assert(streq(igs_input_string("my bool"), "true")); //intentional memory leak here
    igs_input_set_bool("my bool", false);
    assert(!igs_input_bool("my bool"));
    assert(igs_input_int("my bool") == 0);
    assert(igs_input_double("my bool") < 0.000001);
    assert(streq(igs_input_string("my bool"), "false")); //intentional memory leak here
    assert(igs_input_data("my bool", &data, &dataSize) == IGS_SUCCESS && *(bool*)data == false && dataSize == sizeof(bool));

    igs_input_set_int("my int", 3);
    assert(igs_input_bool("my int"));
    assert(igs_input_int("my int") == 3);
    assert(igs_input_double("my int") - 3 < 0.000001);
    assert(streq(igs_input_string("my int"), "3")); //intentional memory leak here
    igs_input_set_int("my int", 0);
    assert(!igs_input_bool("my int"));
    assert(igs_input_int("my int") == 0);
    assert(igs_input_double("my int") < 0.000001);
    assert(streq(igs_input_string("my int"), "0")); //intentional memory leak here
    assert(igs_input_data("my int", &data, &dataSize) == IGS_SUCCESS && *(int*)data == 0 && dataSize == sizeof(int));

    igs_input_set_double("my double", 3.3);
    assert(igs_input_bool("my double"));
    assert(igs_input_int("my double") == 3);
    assert(igs_input_double("my double") - 3.3 < 0.000001);
    assert(streq(igs_input_string("my double"), "3.300000")); //intentional memory leak here
    igs_input_set_double("my double", 0.0);
    assert(!igs_input_bool("my double"));
    assert(igs_input_int("my double") == 0);
    assert(igs_input_double("my double") < 0.000001);
    assert(streq(igs_input_string("my double"), "0.000000")); //intentional memory leak here
    assert(igs_input_data("my double", &data, &dataSize) == IGS_SUCCESS && *(double *)data < 0.000001 && dataSize == sizeof(double));

    igs_input_set_string("my string", "true");
    assert(igs_input_bool("my string"));
    igs_input_set_string("my string", "false");
    assert(!igs_input_bool("my string"));
    igs_input_set_string("my string", "10.1");
    assert(igs_input_int("my string") == 10);
    assert(igs_input_double("my string") - 10.1 < 0.000001);
    assert(streq(igs_input_string("my string"), "10.1")); //intentional memory leak here
    assert(igs_input_data("my string", &data, &dataSize) == IGS_SUCCESS && streq((char *)data, "10.1") && dataSize == 5);

    data = (void*)"my data";
    dataSize = strlen("my data") + 1;
    igs_input_set_data("my data", NULL, 0);
    assert(!igs_input_bool("my data"));
    igs_input_set_data("my data", data, dataSize);
    assert(!igs_input_bool("my data"));
    assert(igs_input_int("my data") == 0);
    assert(igs_input_double("my data") < 0.000001);
    assert(igs_input_string("my data") == NULL);
    assert(igs_input_data("my data", &data, &dataSize) == IGS_SUCCESS);
    assert(streq((char*)data, "my data") && strlen((char*)data) == dataSize - 1);


    //add multiple agents to be enabled and disabled on demand
    //first additional agent is activated immediately
    firstAgent = igsagent_new("firstAgent_bogus", true);
    assert(streq("firstAgent_bogus", igsagent_name(firstAgent))); //intentional memory leak here
    igsagent_set_name(firstAgent, "firstAgent");
    assert(streq("firstAgent", igsagent_name(firstAgent))); //intentional memory leak here
    igsagent_definition_set_description(firstAgent, "First virtual agent");
    igsagent_definition_set_version(firstAgent, "1.0");
    igsagent_input_create(firstAgent, "first_impulsion", IGS_IMPULSION_T, NULL, 0);
    igsagent_input_create(firstAgent, "first_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igsagent_input_create(firstAgent, "first_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igsagent_input_create(firstAgent, "first_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igsagent_input_create(firstAgent, "first_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igsagent_input_create(firstAgent, "first_data", IGS_DATA_T, myData, 32);
    igsagent_input_create(firstAgent, "first_impulsion_split", IGS_IMPULSION_T, NULL, 0);
    igsagent_input_create(firstAgent, "first_bool_split", IGS_BOOL_T, &myBool, sizeof(bool));
    igsagent_input_create(firstAgent, "first_int_split", IGS_INTEGER_T, &myInt, sizeof(int));
    igsagent_input_create(firstAgent, "first_double_split", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igsagent_input_create(firstAgent, "first_string_split", IGS_STRING_T, myString, strlen(myString) + 1);
    igsagent_input_create(firstAgent, "first_data_split", IGS_DATA_T, myData, 32);
    igsagent_output_create(firstAgent, "first_impulsion", IGS_IMPULSION_T, NULL, 0);
    igsagent_output_create(firstAgent, "first_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igsagent_output_create(firstAgent, "first_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igsagent_output_create(firstAgent, "first_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igsagent_output_create(firstAgent, "first_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igsagent_output_create(firstAgent, "first_data", IGS_DATA_T, myData, 32);
    igsagent_attribute_create(firstAgent, "first_impulsion", IGS_IMPULSION_T, NULL, 0);
    igsagent_attribute_create(firstAgent, "first_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igsagent_attribute_create(firstAgent, "first_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igsagent_attribute_create(firstAgent, "first_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igsagent_attribute_create(firstAgent, "first_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igsagent_attribute_create(firstAgent, "first_data", IGS_DATA_T, myData, 32);

    igsagent_service_init(firstAgent, "firstService", agentServiceCallback, NULL);
    igsagent_service_arg_add(firstAgent, "firstService", "firstBool", IGS_BOOL_T);
    igsagent_service_arg_add(firstAgent, "firstService", "firstInt", IGS_INTEGER_T);
    igsagent_service_arg_add(firstAgent, "firstService", "firstDouble", IGS_DOUBLE_T);
    igsagent_service_arg_add(firstAgent, "firstService", "firstString", IGS_STRING_T);
    igsagent_service_arg_add(firstAgent, "firstService", "firstData", IGS_DATA_T);

    igsagent_observe_input(firstAgent, "first_impulsion", agentIOCallback, NULL);
    igsagent_observe_input(firstAgent, "first_bool", agentIOCallback, NULL);
    igsagent_observe_input(firstAgent, "first_int", agentIOCallback, NULL);
    igsagent_observe_input(firstAgent, "first_double", agentIOCallback, NULL);
    igsagent_observe_input(firstAgent, "first_string", agentIOCallback, NULL);
    igsagent_observe_input(firstAgent, "first_data", agentIOCallback, NULL);

    igsagent_observe_input(firstAgent, "first_impulsion_split", agentIOCallback, NULL);
    igsagent_observe_input(firstAgent, "first_bool_split", agentIOCallback, NULL);
    igsagent_observe_input(firstAgent, "first_int_split", agentIOCallback, NULL);
    igsagent_observe_input(firstAgent, "first_double_split", agentIOCallback, NULL);
    igsagent_observe_input(firstAgent, "first_string_split", agentIOCallback, NULL);
    igsagent_observe_input(firstAgent, "first_data_split", agentIOCallback, NULL);

    igsagent_mapping_add(firstAgent, "first_impulsion", "partner", "sparing_impulsion");
    igsagent_mapping_add(firstAgent, "first_bool", "partner", "sparing_bool");
    igsagent_mapping_add(firstAgent, "first_int", "partner", "sparing_int");
    igsagent_mapping_add(firstAgent, "first_double", "partner", "sparing_double");
    igsagent_mapping_add(firstAgent, "first_string", "partner", "sparing_string");
    igsagent_mapping_add(firstAgent, "first_data", "partner", "sparing_data");

    igsagent_split_add(firstAgent, "first_impulsion_split", "partner", "sparing_impulsion");
    igsagent_split_add(firstAgent, "first_bool_split", "partner", "sparing_bool");
    igsagent_split_add(firstAgent, "first_int_split", "partner", "sparing_int");
    igsagent_split_add(firstAgent, "first_double_split", "partner", "sparing_double");
    igsagent_split_add(firstAgent, "first_string_split", "partner", "sparing_string");
    igsagent_split_add(firstAgent, "first_data_split", "partner", "sparing_data");

    //second additional agent is NOT activated immediately
    secondAgent = igsagent_new("secondAgent", false);
    igsagent_definition_set_description(secondAgent, "Second virtual agent");
    igsagent_definition_set_version(secondAgent, "1.0");
    igsagent_input_create(secondAgent, "second_impulsion", IGS_IMPULSION_T, NULL, 0);
    igsagent_input_create(secondAgent, "second_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igsagent_input_create(secondAgent, "second_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igsagent_input_create(secondAgent, "second_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igsagent_input_create(secondAgent, "second_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igsagent_input_create(secondAgent, "second_data", IGS_DATA_T, myData, 32);
    igsagent_input_create(secondAgent, "second_impulsion_split", IGS_IMPULSION_T, NULL, 0);
    igsagent_input_create(secondAgent, "second_bool_split", IGS_BOOL_T, &myBool, sizeof(bool));
    igsagent_input_create(secondAgent, "second_int_split", IGS_INTEGER_T, &myInt, sizeof(int));
    igsagent_input_create(secondAgent, "second_double_split", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igsagent_input_create(secondAgent, "second_string_split", IGS_STRING_T, myString, strlen(myString) + 1);
    igsagent_input_create(secondAgent, "second_data_split", IGS_DATA_T, myData, 32);
    igsagent_output_create(secondAgent, "second_impulsion", IGS_IMPULSION_T, NULL, 0);
    igsagent_output_create(secondAgent, "second_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igsagent_output_create(secondAgent, "second_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igsagent_output_create(secondAgent, "second_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igsagent_output_create(secondAgent, "second_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igsagent_output_create(secondAgent, "second_data", IGS_DATA_T, myData, 32);
    igsagent_attribute_create(secondAgent, "second_impulsion", IGS_IMPULSION_T, NULL, 0);
    igsagent_attribute_create(secondAgent, "second_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igsagent_attribute_create(secondAgent, "second_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igsagent_attribute_create(secondAgent, "second_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igsagent_attribute_create(secondAgent, "second_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igsagent_attribute_create(secondAgent, "second_data", IGS_DATA_T, myData, 32);
    igsagent_service_init(secondAgent, "secondService", agentServiceCallback, NULL);
    igsagent_service_arg_add(secondAgent, "secondService", "secondBool", IGS_BOOL_T);
    igsagent_service_arg_add(secondAgent, "secondService", "secondInt", IGS_INTEGER_T);
    igsagent_service_arg_add(secondAgent, "secondService", "secondDouble", IGS_DOUBLE_T);
    igsagent_service_arg_add(secondAgent, "secondService", "secondString", IGS_STRING_T);
    igsagent_service_arg_add(secondAgent, "secondService", "secondData", IGS_DATA_T);

    igsagent_observe_input(secondAgent, "second_impulsion", agentIOCallback, NULL);
    igsagent_observe_input(secondAgent, "second_bool", agentIOCallback, NULL);
    igsagent_observe_input(secondAgent, "second_int", agentIOCallback, NULL);
    igsagent_observe_input(secondAgent, "second_double", agentIOCallback, NULL);
    igsagent_observe_input(secondAgent, "second_string", agentIOCallback, NULL);
    igsagent_observe_input(secondAgent, "second_data", agentIOCallback, NULL);

    igsagent_observe_input(secondAgent, "second_impulsion_split", agentIOCallback, NULL);
    igsagent_observe_input(secondAgent, "second_bool_split", agentIOCallback, NULL);
    igsagent_observe_input(secondAgent, "second_int_split", agentIOCallback, NULL);
    igsagent_observe_input(secondAgent, "second_double_split", agentIOCallback, NULL);
    igsagent_observe_input(secondAgent, "second_string_split", agentIOCallback, NULL);
    igsagent_observe_input(secondAgent, "second_data_split", agentIOCallback, NULL);

    igsagent_mapping_add(secondAgent, "second_impulsion", "partner", "sparing_impulsion");
    igsagent_mapping_add(secondAgent, "second_bool", "partner", "sparing_bool");
    igsagent_mapping_add(secondAgent, "second_int", "partner", "sparing_int");
    igsagent_mapping_add(secondAgent, "second_double", "partner", "sparing_double");
    igsagent_mapping_add(secondAgent, "second_string", "partner", "sparing_string");
    igsagent_mapping_add(secondAgent, "second_data", "partner", "sparing_data");

    igsagent_mapping_add(secondAgent, "second_impulsion", "tester", "my impulsion");
    igsagent_mapping_add(secondAgent, "second_bool", "tester", "my bool");
    igsagent_mapping_add(secondAgent, "second_int", "tester", "my int");
    igsagent_mapping_add(secondAgent, "second_double", "tester", "my double");
    igsagent_mapping_add(secondAgent, "second_string", "tester", "my string");
    igsagent_mapping_add(secondAgent, "second_data", "tester", "my data");

    igsagent_mapping_add(secondAgent, "second_impulsion", "firstAgent", "first_impulsion");
    igsagent_mapping_add(secondAgent, "second_bool", "firstAgent", "first_bool");
    igsagent_mapping_add(secondAgent, "second_int", "firstAgent", "first_int");
    igsagent_mapping_add(secondAgent, "second_double", "firstAgent", "first_double");
    igsagent_mapping_add(secondAgent, "second_string", "firstAgent", "first_string");
    igsagent_mapping_add(secondAgent, "second_data", "firstAgent", "first_data");

    igsagent_split_add(secondAgent, "second_impulsion_split", "partner", "sparing_impulsion");
    igsagent_split_add(secondAgent, "second_bool_split", "partner", "sparing_bool");
    igsagent_split_add(secondAgent, "second_int_split", "partner", "sparing_int");
    igsagent_split_add(secondAgent, "second_double_split", "partner", "sparing_double");
    igsagent_split_add(secondAgent, "second_string_split", "partner", "sparing_string");
    igsagent_split_add(secondAgent, "second_data_split", "partner", "sparing_data");

    igsagent_split_add(secondAgent, "second_impulsion_split", "tester", "my impulsion");
    igsagent_split_add(secondAgent, "second_bool_split", "tester", "my bool");
    igsagent_split_add(secondAgent, "second_int_split", "tester", "my int");
    igsagent_split_add(secondAgent, "second_double_split", "tester", "my double");
    igsagent_split_add(secondAgent, "second_string_split", "tester", "my string");
    igsagent_split_add(secondAgent, "second_data_split", "tester", "my data");

    igsagent_split_add(secondAgent, "second_impulsion_split", "firstAgent", "first_impulsion");
    igsagent_split_add(secondAgent, "second_bool_split", "firstAgent", "first_bool");
    igsagent_split_add(secondAgent, "second_int_split", "firstAgent", "first_int");
    igsagent_split_add(secondAgent, "second_double_split", "firstAgent", "first_double");
    igsagent_split_add(secondAgent, "second_string_split", "firstAgent", "first_string");
    igsagent_split_add(secondAgent, "second_data_split", "firstAgent", "first_data");

    //test mapping in same process between second_agent and first_agent
    //NB: these tests have been obsolete since the delegation of internal
    //publication handling to the ingescape zloop. Internal mappings require
    //a running ingescape loop in order to work properly.
//    igsagent_activate(secondAgent);
//    igsagent_output_set_bool(firstAgent, "first_bool", true);
//    assert(igsagent_input_bool(secondAgent, "second_bool"));
//    igsagent_output_set_bool(firstAgent, "first_bool", false);
//    assert(!igsagent_input_bool(secondAgent, "second_bool"));
//    igsagent_output_set_int(firstAgent, "first_int", 5);
//    assert(igsagent_input_int(secondAgent, "second_int") == 5);
//    igsagent_output_set_double(firstAgent, "first_double", 5.5);
//    assert(igsagent_input_double(secondAgent, "second_double") - 5.5 < 0.000001);
//    igsagent_output_set_string(firstAgent, "first_string", "test string mapping");
//    assert(streq(igsagent_input_string(secondAgent, "second_string"), "test string mapping")); //intentional memory leak here
//    data = (void*)"my data";
//    dataSize = strlen("my data") + 1;
//    igsagent_output_set_data(firstAgent, "first_data", data, dataSize);
//    assert(igsagent_input_data(secondAgent, "second_data", &data, &dataSize) == IGS_SUCCESS);
//    assert(streq((char*)data, "my data") && strlen((char*)data) == dataSize - 1);

    //test service in the same process
    list = NULL;
    igs_service_args_add_bool(&list, true);
    igs_service_args_add_int(&list, 13);
    igs_service_args_add_double(&list, 13.3);
    igs_service_args_add_string(&list, "my string arg");
    igs_service_args_add_data(&list, data, dataSize);
    igsagent_service_call(firstAgent, "secondAgent", "secondService", &list, "token");

    //test agent events in same process
    igsagent_deactivate(secondAgent);
    igsagent_deactivate(firstAgent);
    igs_observe_agent_events(agentEvent, NULL);
    igsagent_observe_agent_events(firstAgent, agentEvent2, NULL);
    igsagent_observe_agent_events(secondAgent, agentEvent2, NULL);
    igsagent_activate(firstAgent);
    assert(tester_firstAgentEntered);
    assert(tester_firstAgentKnowsUs);
    assert(first_testerAgentEntered);
    assert(first_testergentKnowsUs);
    igsagent_activate(secondAgent);
    assert(second_firstAgentEntered);
    assert(second_firstAgentKnowsUs);
    assert(second_testerAgentEntered);
    assert(second_testergentKnowsUs);
    assert(tester_secondAgentEntered);
    assert(tester_secondAgentKnowsUs);
    assert(first_secondAgentEntered);
    assert(first_secondAgentKnowsUs);
    igsagent_deactivate(firstAgent);
    assert(tester_firstAgentExited);
    assert(second_firstAgentExited);
    igsagent_deactivate(secondAgent);
    assert(tester_secondAgentExited);

    //elections
    assert(igs_election_leave("my election") == IGS_SUCCESS);
    assert(igs_election_join("my election") == IGS_SUCCESS);
    assert(igs_election_join("my election") == IGS_FAILURE);
    assert(igs_election_join("INGESCAPE_PRIVATE") == IGS_FAILURE);
    assert(igs_election_leave("my election") == IGS_SUCCESS);
    assert(igs_election_leave("my election") == IGS_SUCCESS);
    assert(igs_election_leave("my other election") == IGS_SUCCESS);
    assert(igs_election_join("my other election") == IGS_SUCCESS);
    assert(igs_election_join("my other election") == IGS_FAILURE);
    assert(igs_election_leave("my other election") == IGS_SUCCESS);
    assert(igs_election_leave("my other election") == IGS_SUCCESS);

    igs_agent_set_family("family_test");

    //replay
    //    igs_replay_terminate();
    //    igs_replay_init("tester_replay.log",
    //                   0, NULL, false, 0, NULL);
    //    igs_replay_init("tester_replay.log",
    //                   10, "11:26:12", false, 0, "firstAgent");
    //    igs_replay_init("tester_replay.log",
    //                   10, "11:26:12", false, IGS_REPLAY_INPUT, "firstAgent");
    //    igs_replay_init("tester_replay.log",
    //                   10, "11:26:12", false, IGS_REPLAY_INPUT+IGS_REPLAY_OUTPUT, "firstAgent");
    //    igs_replay_init("tester_replay.log",
    //                   10, "11:26:12", false, IGS_REPLAY_INPUT+IGS_REPLAY_OUTPUT+IGS_REPLAY_ATTRIBUTE, "firstAgent");
    //    igs_replay_init("tester_replay.log",
    //                   10, "11:26:12", false, IGS_REPLAY_INPUT+IGS_REPLAY_OUTPUT+IGS_REPLAY_ATTRIBUTE+IGS_REPLAY_EXECUTE_SERVICE+IGS_REPLAY_CALL_SERVICE, "firstAgent");
    //    igs_replay_terminate();
    //    igs_replay_init("tester_replay.log",
    //                   100, NULL, false, 0, NULL);
    ////    igs_replay_terminate();
    //    igs_replay_init("tester_replay.log",
    //                   100, NULL, true, 0, NULL);
    //    igs_replay_start();
    //    igs_replay_terminate();
}

int rt_timer (zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(timer_id)
    IGS_UNUSED(arg)
    igs_output_set_impulsion("my impulsion");
    igs_output_set_bool("my bool", myBool = !myBool);
    igs_output_set_int("my int", myInt++);
    igs_output_set_double("my double", myDouble++);

    if (!(myInt % 3))
        igs_rt_set_time(zclock_mono()*1000);
    return 0;
}

void set_timeCB(igs_io_type_t io_type,
                const char *name,
                igs_io_value_type_t value_type,
                void *value,
                size_t value_size,
                void *my_data){
    IGS_UNUSED(io_type)
    IGS_UNUSED(name)
    IGS_UNUSED(value_type)
    IGS_UNUSED(value_size)
    IGS_UNUSED(my_data)
    /*
     The usual timestamp values in milliseconds exceed the size of a signed 64 bits integer.
     Use a data input with proper casting to feed an agent with a realistic timestamp, which
     is ALWAYS expected in microseconds.
     */
    igs_rt_set_time(*(int*)value * 1000);
}


///////////////////////////////////////////////////////////////////////////////
// MAIN & OPTIONS & COMMAND INTERPRETER
//
int main(int argc, const char * argv[]) {
    
    const char *t0 = "Ma classe étrange <(🦄)>";
    char *t1 = strdup("Ma classe étrange <(🦄)>");
    char *t2 = strdup("Ma classe étrange <(🦄)>\n");
    char *t3 = strdup("\nMa classe étrange <(🦄)>");
    char *t4 = strdup("M\ta\v \rc\nl\fasse étrange <(🦄)>");
    char *t5 = strdup("Ma classe\t\v\r\n\f étrange <(🦄)>");
    char *t6 = strdup("");
    char *t7 = strdup("\n");
    char *t8 = strdup("\t\v\r\n\f");
    char *t9 = strdup("Ma classe étrange <(🦄)>with noise at the end");
    assert(model_check_string(t1, strlen(t0)));
    assert(!model_check_string(t4, strlen(t4)));
    assert(!model_check_string(t4, strlen(t0)));
    assert(!model_clean_string(t1, INT64_MAX) && streq(t1, t0));
    assert(model_clean_string(t2, INT64_MAX) && streq(t2, t0));
    assert(model_clean_string(t3, INT64_MAX) && streq(t3, t0));
    assert(model_clean_string(t4, INT64_MAX) && streq(t4, t0));
    assert(model_clean_string(t5, INT64_MAX) && streq(t5, t0));
    assert(!model_clean_string(t6, INT64_MAX) && streq(t6, ""));
    assert(model_clean_string(t7, INT64_MAX) && streq(t7, ""));
    assert(model_clean_string(t8, INT64_MAX) && streq(t8, ""));
    assert(model_clean_string(t9, strlen(t0)) && streq(t9, t0));
    
    myData = calloc(32, sizeof(char));
    myOtherData = calloc(64, sizeof(char));

    //manage options
    int opt = 0;
    bool interactiveloop = false;
    bool staticTests = false;

    static struct option long_options[] = {
        {"verbose",     no_argument, 0,  'v' },
        {"interactiveloop",     no_argument, 0,  'i' },
        {"definition",  required_argument, 0,  'f' },
        {"mapping",  required_argument, 0,  'm' },
        {"device",      required_argument, 0,  'd' },
        {"port",        required_argument, 0,  'p' },
        {"name",        required_argument, 0,  'n' },
        {"auto",        no_argument, 0,  'a' },
        {"static",        no_argument, 0,  's' },
        {"rt",        no_argument, 0,  'r' },
        {"help",        no_argument, 0,  'h' },
        {"broker",  required_argument, 0,  'b' },
        {0, 0, 0, 0}
    };

    char * broker = NULL;
    int long_index = 0;
    while ((opt = getopt_long(argc, (char *const *)argv, "p", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'v':
                verbose = true;
                break;
            case 'i':
                interactiveloop = true;
                break;
            case 'p':
                port = (unsigned int)atoi(optarg);
                break;
            case 'b':
                broker = strdup(optarg);
                break;
            case 'd':
                networkDevice = optarg;
                break;
            case 'n':
                agentName = optarg;
                break;
            case 'a':
                autoTests = true;
                break;
            case 's':
                staticTests = true;
                break;
            case 'r':
                rt = true;
                break;
            case 'h':
                print_usage(agentName);
                exit(0);
            default:
                print_usage(agentName);
                exit(1);
        }
    }
    igs_clear_context();
    //NB: on macos, because syslog is broken, logs can be checked using this command:
    //log stream --info --debug --predicate 'sender == "ingescape"' --style syslog
    igs_log_include_data(true);
    igs_log_include_services(true);
    igs_log_set_syslog(false);

    if (staticTests){
        autoTests = false;
        run_static_tests(argc, argv);
        //we terminate now after passing the static tests
        igsagent_destroy(&secondAgent);
        igsagent_destroy(&firstAgent);
        igs_fatal("static tests have terminated with SUCCESS");
        exit(EXIT_SUCCESS);
    }

    igs_agent_set_name(agentName);
    igs_log_set_console(true);

    igs_observe_channels(testerChannelCallback, NULL);

    igs_definition_set_description("One example for each type of IO and call");
    igs_definition_set_version("1.0");
    igs_input_create("my impulsion", IGS_IMPULSION_T, NULL, 0);
    igs_input_create("my bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igs_input_create("my int", IGS_INTEGER_T, &myInt, sizeof(int));
    igs_input_create("my double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igs_input_create("my string", IGS_STRING_T, myString, strlen(myString) + 1);
    igs_input_create("my data", IGS_DATA_T, myData, 32);
    igs_service_init("myService", testerServiceCallback, NULL);
    igs_service_arg_add("myService", "myBool", IGS_BOOL_T);
    igs_service_arg_add("myService", "myInt", IGS_INTEGER_T);
    igs_service_arg_add("myService", "myDouble", IGS_DOUBLE_T);
    igs_service_arg_add("myService", "myString", IGS_STRING_T);
    igs_service_arg_add("myService", "myData", IGS_DATA_T);

    igs_observe_input("my impulsion", testerIOCallback, NULL);
    igs_observe_input("my bool", testerIOCallback, NULL);
    igs_observe_input("my int", testerIOCallback, NULL);
    igs_observe_input("my double", testerIOCallback, NULL);
    igs_observe_input("my string", testerIOCallback, NULL);
    igs_observe_input("my data", testerIOCallback, NULL);

    igs_mapping_add("my impulsion", "partner", "sparing_impulsion");
    igs_mapping_add("my bool", "partner", "sparing_bool");
    igs_mapping_add("my int", "partner", "sparing_int");
    igs_mapping_add("my double", "partner", "sparing_double");
    igs_mapping_add("my string", "partner", "sparing_string");
    igs_mapping_add("my data", "partner", "sparing_data");

    //RT tests
    if (rt){
        igs_input_create("set_time", IGS_INTEGER_T, NULL, 0);
        igs_observe_input("set_time", set_timeCB, NULL);
        igs_output_create("my impulsion", IGS_IMPULSION_T, NULL, 0);
        igs_output_create("my bool", IGS_BOOL_T, &myBool, sizeof(bool));
        igs_output_create("my int", IGS_INTEGER_T, &myInt, sizeof(int));
        igs_output_create("my double", IGS_DOUBLE_T, &myDouble, sizeof(double));
        igs_output_create("my string", IGS_STRING_T, myString, strlen(myString) + 1);
        igs_output_create("my data", IGS_DATA_T, myData, 32);
        igs_rt_set_synchronous_mode(true);
    }

    if (verbose)
        igs_log_set_console_level(IGS_LOG_TRACE);
    else
        igs_log_set_console_level(IGS_LOG_FATAL);

    if (autoTests){
        igs_start_with_device(networkDevice, port);
        igs_channel_join("TEST_CHANNEL");
        zloop_t *loop = zloop_new();
        zsock_t *pipe = igs_pipe_to_ingescape();
        zloop_reader(loop, pipe, ingescapeSentMessage, NULL);
        igs_fatal("ready to start autotests");
        //we run a loop dedicated to automatic tests
        zloop_start(loop);
        zloop_destroy(&loop);
        igs_stop();
        igs_clear_context();
        exit(EXIT_SUCCESS);
    }else{
        //we run normally
        if (networkDevice == NULL){
            //we have no device to start with: try to find one
            int nbD = 0;
            int nbA = 0;
            char **devices = igs_net_devices_list(&nbD);
            char **addresses = igs_net_addresses_list(&nbA);
            assert(nbD == nbA);
            if (nbD == 1){
                //we have exactly one compliant network device available: we use it
                networkDevice = strdup(devices[0]);
                igs_info("using %s as default network device (this is the only one available)", networkDevice);
            }else if (nbD == 2 && (strcmp(addresses[0], "127.0.0.1") == 0 || strcmp(addresses[1], "127.0.0.1") == 0)){
                //we have two devices, one of which is the loopback
                //pick the device that is NOT the loopback
                if (strcmp(addresses[0], "127.0.0.1") == 0){
                    networkDevice = strdup(devices[1]);
                }else{
                    networkDevice = strdup(devices[0]);
                }
                igs_info("using %s as default network device (this is the only one available that is not the loopback)", networkDevice);
            }else{
                if (nbD == 0){
                    igs_error("No network device found: aborting.");
                }else{
                    igs_error("No network device passed as command line parameter and several are available.");
                    printf("Please use one of these network devices:\n");
                    for (int i = 0; i < nbD; i++){
                        printf("\t%s\n", devices[i]);
                    }
                    printf("\n");
                    print_usage(agentName);
                }
                exit(EXIT_FAILURE);
            }
            igs_free_net_devices_list(devices, nbD);
            igs_free_net_addresses_list(addresses, nbD);
        }
    }
    if (broker) {
        char buffer[1024] = "";
        snprintf(buffer, 1024, "tcp://%s:5661", broker);
        assert(igs_broker_add(buffer) == IGS_SUCCESS);
        snprintf(buffer, 1024, "tcp://%s:5670", broker);
        assert(igs_start_with_brokers(buffer) == IGS_SUCCESS);
    } else {
        //start/stop stress tests
        igs_start_with_device(networkDevice, port);
//    igs_start_with_device(networkDevice, port);
//    igs_stop();
//    igs_stop();
//    igs_stop();

        igs_start_with_device(networkDevice, port);
    }

    //mainloop management (two modes)
    if (!interactiveloop) {
        //Run the main loop (non-interactive mode):
        //we rely on CZMQ which is an ingeScape dependency and is thus
        //always here.
        zloop_t *loop = zloop_new();
        zsock_t *pipe = igs_pipe_to_ingescape();
        zloop_reader(loop, pipe, ingescapeSentMessage, NULL);
        if (rt)
            zloop_timer(loop, 500, 0, rt_timer, NULL);
        zloop_start(loop);
        zloop_destroy(&loop);
    }else{
        char message[BUFFER_SIZE];
        char command[BUFFER_SIZE];
        char param1[BUFFER_SIZE];
        char param2[BUFFER_SIZE];
        int usedChar = 0;
        print_cli_usage();
        while (igs_is_started()){
            if (!fgets(message, 1024, stdin))
                break;
            if ((message[0] == '/') && (strlen(message) > 2)) {
                int matches = sscanf(message + 1, "%s %s%n%s", command, param1, &usedChar, param2);
                if (matches > 2) {
                    // copy the remaining of the message in param 2
                    strncpy(param2, message + usedChar + 2, BUFFER_SIZE);
                    // remove '\n' at the end
                    param2[strnlen(param2, BUFFER_SIZE) - 1] = '\0';
                }
                // Process command
                if (matches == -1) {
                    //printf("Error: could not interpret message %s\n", message + 1);
                }else if (matches == 1) {
                    if (streq(command, "quit")){
                        break;
                    }else if(streq(command, "help")){
                        print_cli_usage();
                    }else if(streq(command, "publish")){
                        publishCommand();
                    }else if(streq(command, "services")){
                        servicesCommand();
                    }else if(streq(command, "channels")){
                        channelsCommand();
                    }else if(streq(command, "activate")){
                        igsagent_activate(secondAgent);
                    }else if(streq(command, "deactivate")){
                        igsagent_deactivate(secondAgent);
                    }else if(streq(command, "service_local")){
                        igs_service_arg_t *args = NULL;
                        igs_service_args_add_bool(&args, true);
                        igs_service_args_add_int(&args, 3);
                        igs_service_args_add_double(&args, 3.0);
                        igs_service_args_add_string(&args, "service string test");
                        igs_service_args_add_data(&args, myOtherData, 64);
                        igsagent_service_call(firstAgent, "secondAgent", "secondService", &args, "token");
                    }else {
                        printf("unhandled command: %s\n", command);
                    }
                }else if (matches == 2) {
                    if (streq(command, "editor")){
                        editorCommand(param1, "sparing_int");
                    }else if(streq(command, "security")){
                        securityCommand(param1);

                    }else if(streq(command, "broker")){
                        brokerCommand(param1);

                    }else{
                        printf("Received command: %s + %s\n", command, param1);
                    }
                }else if (matches == 3) {
                    printf("Received command: %s + %s + %s\n", command, param1, param2);
                }else{
                    printf("Error: message returned %d matches (%s)\n", matches, message);
                }
            }
        }
    }

    if (secondAgent)
        igsagent_destroy(&secondAgent);
    if (firstAgent)
        igsagent_destroy(&firstAgent);
    igs_stop();

    return EXIT_SUCCESS;
}
