
#include <stdio.h>
#include <getopt.h> //command line options at statrtup
#include <stdlib.h> //standard C functions such as getenv, atoi, exit, etc.
#include <string.h> //C string handling functions
#include <signal.h> //catching interruptions
#include <czmq.h>
#include <ingescape/ingescape_agent.h>
#include "common.h"

unsigned int port = 5669;
char *agentName = "tester";
char *networkDevice = "en0"; //can be set to a default device name
bool verbose = false;
bool autoTests = false;
bool autoTestsHaveStarted = false;

//reference values for static tests
bool myBool = true;
int myInt = 1;
double myDouble = 1.0;
char *myString = "my string";
void *myData = NULL;
void *myOtherData = NULL;

int ingescapeSentMessage(zloop_t *loop, zsock_t *reader, void *arg){
    char *message = NULL;
    zsock_recv(reader, "s", &message);
    if (streq(message, "LOOP_STOPPED")){
        igs_info("LOOP_STOPPED received in main app");
        return -1;
    }else{
        return 0;
    }
}


//callbacks and variables for agent events
bool tester_firstAgentEntered = false;
bool tester_firstAgentKnowsUs = false;
bool tester_firstAgentExited = false;
bool tester_secondAgentEntered = false;
bool tester_secondAgentKnowsUs = false;
bool tester_secondAgentExited = false;
void agentEvent(igs_agent_event_t event, const char *uuid, const char *name, void *eventData, void *myData){
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

igs_agent_t *firstAgent = NULL;
igs_agent_t *secondAgent = NULL;
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
void agentEvent2(igs_agent_t *agent, igs_agent_event_t event, const char *uuid, const char *name, void *eventData, void *myData){
    printf("agentEvent2: in %s - %d - %s - %s\n", igsAgent_getAgentName(agent), event, uuid, name); //memory leak on agent name
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

//callbacks for calls
void testerCallCallback(const char *senderAgentName, const char *senderAgentUUID,
                         const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
                         const char *token, void* myData){
    if (autoTestsHaveStarted){
        assert(token);
        assert(streq(token, "token"));
        assert(firstArgument->type == IGS_BOOL_T);
        assert(firstArgument->b);
        assert(firstArgument->next->type == IGS_INTEGER_T);
        assert(firstArgument->next->i == 3);
        assert(firstArgument->next->next->type == IGS_DOUBLE_T);
        assert(firstArgument->next->next->d == 3.3);
        assert(firstArgument->next->next->next->type == IGS_STRING_T);
        assert(streq(firstArgument->next->next->next->c,"call string test"));
        assert(firstArgument->next->next->next->next->type == IGS_DATA_T);
        assert(firstArgument->next->next->next->next->size == 64);
        return;
    }
    
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

//callbacks for bus
size_t msgCountForAutoTests = 0;
void testerBusCallback(const char *event, const char *peerID, const char *name,
                        const char *address, const char *channel,
                        zhash_t *headers, zmsg_t *msg, void *myData){
    if (autoTests && autoTestsHaveStarted){
        assert(streq(name, "partner"));
        if (msg){
            printf("new message:\n");
            zmsg_print(msg);
        }
        if (streq(event, "SHOUT")){
            assert(streq(channel, "TEST_CHANNEL"));
            if (msgCountForAutoTests == 0){
                char *s = zmsg_popstr(msg);
                assert(streq(s, "test igs_busSendStringToChannel"));
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
            zframe_t *f = zmsg_first(msg);
            char *s = zframe_strdup(f);
            if (streq(s, "CALL")){
                //we are catching the call test : dismiss
                msgCountForAutoTests--; //compensating
            }else{
                if (msgCountForAutoTests == 3){
                    assert(streq(s, "test igs_busSendStringToAgent"));
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
            if(streq(name, "partner") && streq(s, "starting autotests")){
                autoTestsHaveStarted = true;
            }
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

//callbacks for iops
void testerIOPCallback(iop_t iopType, const char* name, iopType_t valueType, void* value, size_t valueSize, void* myData){
    if (autoTestsHaveStarted){
        switch (valueType) {
            case IGS_BOOL_T:
                assert(igs_readInputAsBool(name));
                assert(valueSize == sizeof(bool));
                break;
            case IGS_INTEGER_T:
                assert(igs_readInputAsInt(name) == 2);
                assert(valueSize == sizeof(int));
                break;
            case IGS_DOUBLE_T:
                assert(igs_readInputAsDouble(name) == 2.2);
                assert(valueSize == sizeof(double));
                break;
            case IGS_STRING_T:
            {
                char *stringValue = igs_readInputAsString(name);
                assert(streq(stringValue, "output string test"));
                assert(valueSize == strlen(stringValue) + 1);
                free(stringValue);
                break;
            }
            case IGS_DATA_T:
                assert(valueSize == 64);
                break;
            default:
                break;
        }
        return;
    }
    
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

///////////////////////////////////////////////////////////////////////////////
// MAIN & OPTIONS & COMMAND INTERPRETER
//
int main(int argc, const char * argv[]) {
    myData = malloc(32);
    myOtherData = malloc(64);
    
    //manage options
    int opt = 0;
    bool interactiveloop = false;
    
    static struct option long_options[] = {
        {"verbose",     no_argument, 0,  'v' },
        {"license",      required_argument, 0,  'l' },
        {"interactiveloop",     no_argument, 0,  'i' },
        {"definition",  required_argument, 0,  'f' },
        {"mapping",  required_argument, 0,  'm' },
        {"device",      required_argument, 0,  'd' },
        {"port",        required_argument, 0,  'p' },
        {"name",        required_argument, 0,  'n' },
        {"auto",        no_argument, 0,  'a' },
        {"help",        no_argument, 0,  'h' },
        {0, 0, 0, 0}
    };
    
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
            case 'd':
                networkDevice = optarg;
                break;
            case 'n':
                agentName = optarg;
                break;
            case 'a':
                autoTests = true;
                break;
            case 'h':
                print_usage(agentName);
                exit(0);
            default:
                print_usage(agentName);
                exit(1);
        }
    }
    igs_cleanContext();
    igs_enableDataLogging(true);
    igs_enableCallLogging(true);
    
    //agent name and state
    char *name = igs_getAgentName();
    assert(streq(name, "no_name"));
    free(name);
    igs_setAgentName("simple Demo Agent");
    name = igs_getAgentName();
    assert(streq(name, "simple_Demo_Agent"));
    free(name);
    name = NULL;
    igs_setAgentName(agentName); //to set proper log file name
    
    //logs
    assert(!igs_isVerbose());
    assert(!igs_getLogStream());
    assert(!igs_getLogInFile());
    char *logPath = igs_getLogPath();
    assert(strlen(logPath) == 0);
    free(logPath);
    igs_setVerbose(true);
    assert(igs_isVerbose());
    igs_setLogStream(true);
    assert(igs_getLogStream());
    igs_setLogInFile(true);
    assert(igs_getLogInFile());
    igs_setLogLevel(IGS_LOG_TRACE);
    assert (igs_getLogLevel() == IGS_LOG_TRACE);
    igs_trace("trace example %d", 1);
    igs_debug("debug  example %d", 2);
    igs_info("info example %d", 3);
    igs_warn("warn example %d", 4);
    igs_error("error example %d", 5);
    igs_fatal("fatal example %d", 6);
    igs_license("license example %d", 7);
    igs_info("multi-line log \n second line");
    logPath = igs_getLogPath();
    assert(strlen(logPath) > 0);
    free(logPath);
    
    //write uninitialized definition and mapping
    igs_writeDefinitionToPath();
    igs_writeMappingToPath();
    
    //licenses
    assert (igs_getLicensePath() == NULL);
    igs_setLicensePath(NULL);
    assert (igs_getLicensePath() == NULL);
    igs_setLicensePath("/does not exist");
    assert (igs_getLicensePath() == NULL);
    igs_setLicensePath("/tmp");
    char *newLicPath = igs_getLicensePath();
    assert(streq(newLicPath, "/tmp"));
    free(newLicPath);
    
    //utilities
    assert (igs_version() > 0);
    assert (igs_protocol() >= 2);
    int nb_devices = 0;
    char **devicesList = NULL;
    igs_getNetdevicesList(&devicesList, &nb_devices);
    for (int i = 0; i < nb_devices; i++){
        igs_info("device %d - %s", i, devicesList[i]);
    }
    igs_freeNetdevicesList(devicesList, nb_devices);
    igs_getNetaddressesList(&devicesList, &nb_devices);
    for (int i = 0; i < nb_devices; i++){
        igs_info("ip %d - %s", i, devicesList[i]);
    }
    igs_freeNetaddressesList(devicesList, nb_devices);
    assert(igs_getCommandLine() == NULL);
    igs_setCommandLine("my command line");
    char *commandLine = igs_getCommandLine();
    assert(streq("my command line", commandLine));
    free(commandLine);
    igs_setCommandLineFromArgs(argc, argv);
    commandLine = igs_getCommandLine();
    igs_info("command line: %s", commandLine);
    free(commandLine);
    assert(!igs_getRequestOutputsFromMappedAgents());
    igs_setRequestOutputsFromMappedAgents(true);
    assert(igs_getRequestOutputsFromMappedAgents());
    
    //general control functions
    assert(igs_getPipeToIngescape() == NULL);
    assert(!igs_isStarted());
    assert(igs_getAgentState() == NULL);
    igs_setAgentState("");
    char *state = igs_getAgentState();
    assert(streq(state, ""));
    free(state);
    igs_setAgentState("my state");
    state = igs_getAgentState();
    assert(streq(state, "my state"));
    free(state);
    assert(!igs_isMuted());
    igs_mute();
    assert(igs_isMuted());
    igs_unmute();
    assert(!igs_isMuted());
    assert(!igs_isFrozen());
    igs_freeze();
    assert(igs_isFrozen());
    igs_unfreeze();
    assert(!igs_isFrozen());
    
    //iops with NULL definition
    assert(igs_getInputsNumber() == 0);
    assert(igs_getOutputsNumber() == 0);
    assert(igs_getOutputsNumber() == 0);
    assert(!igs_checkInputExistence("toto"));
    assert(!igs_checkOutputExistence("toto"));
    assert(!igs_checkParameterExistence("toto"));
    char **elements = NULL;
    size_t nbElements = 0;
    elements = igs_getInputsList(&nbElements);
    assert(elements == NULL && nbElements == 0);
    elements = igs_getOutputsList(&nbElements);
    assert(elements == NULL && nbElements == 0);
    elements = igs_getOutputsList(&nbElements);
    assert(elements == NULL && nbElements == 0);
    assert(!igs_isOutputMuted(NULL));
    assert(!igs_isOutputMuted("toto"));
    igs_muteOutput("toto");
    igs_unmuteOutput("toto");
    assert(!igs_readInputAsBool("toto"));
    assert(!igs_readInputAsInt("toto"));
    assert(!igs_readInputAsDouble("toto"));
    assert(!igs_readInputAsString("toto"));
    void *data = NULL;
    size_t dataSize = 0;
    assert(igs_readInputAsData("toto", &data, &dataSize) == IGS_FAILURE);
    igs_clearDataForInput("toto");
    assert(!igs_readOutputAsBool("toto"));
    assert(!igs_readOutputAsInt("toto"));
    assert(!igs_readOutputAsDouble("toto"));
    assert(!igs_readOutputAsString("toto"));
    assert(igs_readOutputAsData("toto", &data, &dataSize) == IGS_FAILURE);
    assert(!igs_readParameterAsBool("toto"));
    assert(!igs_readParameterAsInt("toto"));
    assert(!igs_readParameterAsDouble("toto"));
    assert(!igs_readParameterAsString("toto"));
    assert(igs_readParameterAsData("toto", &data, &dataSize) == IGS_FAILURE);
    
    //definition - part 1
    assert(igs_loadDefinition("invalid json") == IGS_FAILURE);
    assert(igs_loadDefinitionFromPath("/does not exist") == IGS_FAILURE);
    assert(igs_getDefinition()); //intentional memory leak here
    assert(streq(igs_getDefinitionName(), "tester"));
    assert(igs_getDefinitionDescription() == NULL);
    assert(igs_getDefinitionVersion() == NULL);
    igs_setDefinitionName("");
    igs_setDefinitionDescription("");
    igs_setDefinitionVersion("");
    //TODO: test loading valid string and file definitions
    igs_setDefinitionName("my definition");
    char *defName = igs_getDefinitionName();
    assert(streq(defName, "my definition"));
    free(defName);
    igs_setDefinitionDescription("my description");
    char *defDesc = igs_getDefinitionDescription();
    free(defDesc);
    assert(streq(defDesc, "my description"));
    igs_setDefinitionVersion("version");
    char *defVer = igs_getDefinitionVersion();
    assert(streq(defVer, "version"));
    free(defVer);
    assert(igs_createInput("toto", IGS_BOOL_T, NULL, 0) == IGS_SUCCESS);
    assert(igs_createOutput("toto", IGS_BOOL_T, NULL, 0) == IGS_SUCCESS);
    assert(igs_createParameter("toto", IGS_BOOL_T, NULL, 0) == IGS_SUCCESS);
    assert(igs_createInput("toto", IGS_BOOL_T, NULL, 0) == IGS_FAILURE);
    assert(igs_createOutput("toto", IGS_BOOL_T, NULL, 0) == IGS_FAILURE);
    assert(igs_createParameter("toto", IGS_BOOL_T, NULL, 0) == IGS_FAILURE);
    igs_muteOutput("toto");
    assert(igs_isOutputMuted("toto"));
    igs_unmuteOutput("toto");
    assert(!igs_isOutputMuted("toto"));
    assert(igs_removeInput("toto") == IGS_SUCCESS);
    assert(igs_removeOutput("toto") == IGS_SUCCESS);
    assert(igs_removeParameter("toto") == IGS_SUCCESS);
    assert(igs_removeInput("toto") == IGS_FAILURE);
    assert(igs_removeOutput("toto") == IGS_FAILURE);
    assert(igs_removeParameter("toto") == IGS_FAILURE);
    
    //inputs
    assert(igs_createInput("my impulsion", IGS_IMPULSION_T, NULL, 0) == IGS_SUCCESS);
    assert(igs_createInput("my impulsion", IGS_IMPULSION_T, NULL, 0) == IGS_FAILURE);
    assert(igs_createInput("my bool", IGS_BOOL_T, &myBool, sizeof(bool)) == IGS_SUCCESS);
    assert(igs_createInput("my bool", IGS_BOOL_T, &myBool, sizeof(bool)) == IGS_FAILURE);
    assert(igs_createInput("my int", IGS_INTEGER_T, &myInt, sizeof(int)) == IGS_SUCCESS);
    assert(igs_createInput("my int", IGS_INTEGER_T, &myInt, sizeof(int)) == IGS_FAILURE);
    assert(igs_createInput("my double", IGS_DOUBLE_T, &myDouble, sizeof(double)) == IGS_SUCCESS);
    assert(igs_createInput("my double", IGS_DOUBLE_T, &myDouble, sizeof(double)) == IGS_FAILURE);
    assert(igs_createInput("my string", IGS_STRING_T, myString, strlen(myString) + 1) == IGS_SUCCESS);
    assert(igs_createInput("my string", IGS_STRING_T, myString, strlen(myString) + 1) == IGS_FAILURE);
    assert(igs_createInput("my data", IGS_DATA_T, myData, 32) == IGS_SUCCESS);
    assert(igs_createInput("my data", IGS_DATA_T, myData, 32) == IGS_FAILURE);
    elements = NULL;
    elements = igs_getInputsList(&nbElements);
    assert(elements && nbElements == 6);
    igs_freeIOPList(&elements, nbElements);
    assert(igs_getInputsNumber() == 6);
    assert(igs_getTypeForInput("my_impulsion") == IGS_IMPULSION_T);
    assert(igs_checkInputExistence("my_impulsion"));
    assert(igs_getTypeForInput("my_bool") == IGS_BOOL_T);
    assert(igs_checkInputExistence("my_bool"));
    assert(igs_getTypeForInput("my_int") == IGS_INTEGER_T);
    assert(igs_checkInputExistence("my_int"));
    assert(igs_getTypeForInput("my_double") == IGS_DOUBLE_T);
    assert(igs_checkInputExistence("my_double"));
    assert(igs_getTypeForInput("my_string") == IGS_STRING_T);
    assert(igs_checkInputExistence("my_string"));
    assert(igs_getTypeForInput("my_data") == IGS_DATA_T);
    assert(igs_checkInputExistence("my_data"));
    assert(igs_readInputAsBool("my_bool"));
    assert(igs_readInputAsInt("my_int") == 1);
    assert(igs_readInputAsDouble("my_double") == 1.0);
    char *string = igs_readInputAsString("my_string");
    assert(streq(string, "my string"));
    free(string);
    data = NULL;
    dataSize = 0;
    assert(igs_readInputAsData("my_data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 32 && memcmp(data, myData, dataSize) == 0);
    free(data);
    data = NULL;
    dataSize = 0;
    assert(igs_writeInputAsBool("", false) == IGS_FAILURE);
    assert(igs_writeInputAsBool("my_bool", false) == IGS_SUCCESS);
    assert(!igs_readInputAsBool("my_bool"));
    assert(igs_writeInputAsInt("", 2) == IGS_FAILURE);
    assert(igs_writeInputAsInt("my_int", 2) == IGS_SUCCESS);
    assert(igs_readInputAsInt("my_int") == 2);
    assert(igs_writeInputAsDouble("", 2) == IGS_FAILURE);
    assert(igs_writeInputAsDouble("my_double", 2) == IGS_SUCCESS);
    assert(igs_readInputAsDouble("my_double") == 2);
    assert(igs_writeInputAsString("", "new string") == IGS_FAILURE);
    assert(igs_writeInputAsString("my_string", "new string") == IGS_SUCCESS);
    string = igs_readInputAsString("my_string");
    assert(streq(string, "new string"));
    free(string);
    string = NULL;
    assert(igs_writeInputAsData("", myOtherData, 64) == IGS_FAILURE);
    assert(igs_writeInputAsData("my_data", myOtherData, 64) == IGS_SUCCESS);
    data = NULL;
    dataSize = 0;
    assert(igs_readInputAsData("my_data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 64 && memcmp(data, myOtherData, dataSize) == 0);
    free(data);
    data = NULL;
    dataSize = 0;
    assert(igs_readInput("my_data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 64 && memcmp(data, myOtherData, dataSize) == 0);
    free(data);
    data = NULL;
    dataSize = 0;
    igs_clearDataForInput("my_data");
    data = NULL;
    dataSize = 0;
    assert(igs_readInput("my_data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 0 && data == NULL);
    assert(igs_readInputAsData("my_data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 0 && data == NULL);
    
    //outputs
    assert(igs_createOutput("my impulsion", IGS_IMPULSION_T, NULL, 0) == IGS_SUCCESS);
    assert(igs_createOutput("my impulsion", IGS_IMPULSION_T, NULL, 0) == IGS_FAILURE);
    assert(igs_createOutput("my bool", IGS_BOOL_T, &myBool, sizeof(bool)) == IGS_SUCCESS);
    assert(igs_createOutput("my bool", IGS_BOOL_T, &myBool, sizeof(bool)) == IGS_FAILURE);
    assert(igs_createOutput("my int", IGS_INTEGER_T, &myInt, sizeof(int)) == IGS_SUCCESS);
    assert(igs_createOutput("my int", IGS_INTEGER_T, &myInt, sizeof(int)) == IGS_FAILURE);
    assert(igs_createOutput("my double", IGS_DOUBLE_T, &myDouble, sizeof(double)) == IGS_SUCCESS);
    assert(igs_createOutput("my double", IGS_DOUBLE_T, &myDouble, sizeof(double)) == IGS_FAILURE);
    assert(igs_createOutput("my string", IGS_STRING_T, myString, strlen(myString) + 1) == IGS_SUCCESS);
    assert(igs_createOutput("my string", IGS_STRING_T, myString, strlen(myString) + 1) == IGS_FAILURE);
    assert(igs_createOutput("my data", IGS_DATA_T, myData, 32) == IGS_SUCCESS);
    assert(igs_createOutput("my data", IGS_DATA_T, myData, 32) == IGS_FAILURE);
    elements = NULL;
    elements = igs_getOutputsList(&nbElements);
    assert(elements && nbElements == 6);
    igs_freeIOPList(&elements, nbElements);
    assert(igs_getOutputsNumber() == 6);
    assert(igs_getTypeForOutput("my_impulsion") == IGS_IMPULSION_T);
    assert(igs_checkOutputExistence("my_impulsion"));
    assert(igs_getTypeForOutput("my_bool") == IGS_BOOL_T);
    assert(igs_checkOutputExistence("my_bool"));
    assert(igs_getTypeForOutput("my_int") == IGS_INTEGER_T);
    assert(igs_checkOutputExistence("my_int"));
    assert(igs_getTypeForOutput("my_double") == IGS_DOUBLE_T);
    assert(igs_checkOutputExistence("my_double"));
    assert(igs_getTypeForOutput("my_string") == IGS_STRING_T);
    assert(igs_checkOutputExistence("my_string"));
    assert(igs_getTypeForOutput("my_data") == IGS_DATA_T);
    assert(igs_checkOutputExistence("my_data"));
    assert(igs_readOutputAsBool("my_bool"));
    assert(igs_readOutputAsInt("my_int") == 1);
    assert(igs_readOutputAsDouble("my_double") == 1.0);
    string = igs_readOutputAsString("my_string");
    assert(streq(string, "my string"));
    free(string);
    string = NULL;
    data = NULL;
    dataSize = 0;
    assert(igs_readOutputAsData("my_data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 32 && memcmp(data, myData, dataSize) == 0);
    free(data);
    data = NULL;
    dataSize = 0;
    assert(igs_writeOutputAsBool("", false) == IGS_FAILURE);
    assert(igs_writeOutputAsBool("my_bool", false) == IGS_SUCCESS);
    assert(!igs_readOutputAsBool("my_bool"));
    assert(igs_writeOutputAsInt("", 2) == IGS_FAILURE);
    assert(igs_writeOutputAsInt("my_int", 2) == IGS_SUCCESS);
    assert(igs_readOutputAsInt("my_int") == 2);
    assert(igs_writeOutputAsDouble("", 2) == IGS_FAILURE);
    assert(igs_writeOutputAsDouble("my_double", 2) == IGS_SUCCESS);
    assert(igs_readOutputAsDouble("my_double") == 2);
    assert(igs_writeOutputAsString("", "new string") == IGS_FAILURE);
    assert(igs_writeOutputAsString("my_string", "new string") == IGS_SUCCESS);
    string = igs_readOutputAsString("my_string");
    assert(streq(string, "new string"));
    free(string);
    assert(igs_writeOutputAsData("", myOtherData, 64) == IGS_FAILURE);
    assert(igs_writeOutputAsData("my_data", myOtherData, 64) == IGS_SUCCESS);
    data = NULL;
    dataSize = 0;
    assert(igs_readOutputAsData("my_data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 64 && memcmp(data, myOtherData, dataSize) == 0);
    free(data);
    data = NULL;
    dataSize = 0;
    assert(igs_readOutput("my_data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 64 && memcmp(data, myOtherData, dataSize) == 0);
    free(data);
    data = NULL;
    dataSize = 0;
    igs_clearDataForOutput("my_data");
    data = NULL;
    dataSize = 0;
    assert(igs_readOutput("my_data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 0 && data == NULL);
    assert(igs_readOutputAsData("my_data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 0 && data == NULL);
    
    
    //parameters
    assert(igs_createParameter("my impulsion", IGS_IMPULSION_T, NULL, 0) == IGS_SUCCESS);
    assert(igs_createParameter("my impulsion", IGS_IMPULSION_T, NULL, 0) == IGS_FAILURE);
    assert(igs_createParameter("my bool", IGS_BOOL_T, &myBool, sizeof(bool)) == IGS_SUCCESS);
    assert(igs_createParameter("my bool", IGS_BOOL_T, &myBool, sizeof(bool)) == IGS_FAILURE);
    assert(igs_createParameter("my int", IGS_INTEGER_T, &myInt, sizeof(int)) == IGS_SUCCESS);
    assert(igs_createParameter("my int", IGS_INTEGER_T, &myInt, sizeof(int)) == IGS_FAILURE);
    assert(igs_createParameter("my double", IGS_DOUBLE_T, &myDouble, sizeof(double)) == IGS_SUCCESS);
    assert(igs_createParameter("my double", IGS_DOUBLE_T, &myDouble, sizeof(double)) == IGS_FAILURE);
    assert(igs_createParameter("my string", IGS_STRING_T, myString, strlen(myString) + 1) == IGS_SUCCESS);
    assert(igs_createParameter("my string", IGS_STRING_T, myString, strlen(myString) + 1) == IGS_FAILURE);
    assert(igs_createParameter("my data", IGS_DATA_T, myData, 32) == IGS_SUCCESS);
    assert(igs_createParameter("my data", IGS_DATA_T, myData, 32) == IGS_FAILURE);
    elements = NULL;
    elements = igs_getParametersList(&nbElements);
    assert(elements && nbElements == 6);
    igs_freeIOPList(&elements, nbElements);
    assert(igs_getParametersNumber() == 6);
    assert(igs_getTypeForParameter("my_impulsion") == IGS_IMPULSION_T);
    assert(igs_checkParameterExistence("my_impulsion"));
    assert(igs_getTypeForParameter("my_bool") == IGS_BOOL_T);
    assert(igs_checkParameterExistence("my_bool"));
    assert(igs_getTypeForParameter("my_int") == IGS_INTEGER_T);
    assert(igs_checkParameterExistence("my_int"));
    assert(igs_getTypeForParameter("my_double") == IGS_DOUBLE_T);
    assert(igs_checkParameterExistence("my_double"));
    assert(igs_getTypeForParameter("my_string") == IGS_STRING_T);
    assert(igs_checkParameterExistence("my_string"));
    assert(igs_getTypeForParameter("my_data") == IGS_DATA_T);
    assert(igs_checkParameterExistence("my_data"));
    assert(igs_readParameterAsBool("my_bool"));
    assert(igs_readParameterAsInt("my_int") == 1);
    assert(igs_readParameterAsDouble("my_double") == 1.0);
    string = igs_readParameterAsString("my_string");
    assert(streq(string, "my string"));
    free(string);
    string = NULL;
    data = NULL;
    dataSize = 0;
    assert(igs_readParameterAsData("my_data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 32 && memcmp(data, myData, dataSize) == 0);
    free(data);
    data = NULL;
    dataSize = 0;
    assert(igs_writeParameterAsBool("", false) == IGS_FAILURE);
    assert(igs_writeParameterAsBool("my_bool", false) == IGS_SUCCESS);
    assert(!igs_readParameterAsBool("my_bool"));
    assert(igs_writeParameterAsInt("", 2) == IGS_FAILURE);
    assert(igs_writeParameterAsInt("my_int", 2) == IGS_SUCCESS);
    assert(igs_readParameterAsInt("my_int") == 2);
    assert(igs_writeParameterAsDouble("", 2) == IGS_FAILURE);
    assert(igs_writeParameterAsDouble("my_double", 2) == IGS_SUCCESS);
    assert(igs_readParameterAsDouble("my_double") == 2);
    assert(igs_writeParameterAsString("", "new string") == IGS_FAILURE);
    assert(igs_writeParameterAsString("my_string", "new string") == IGS_SUCCESS);
    string = igs_readParameterAsString("my_string");
    assert(streq(string, "new string"));
    free(string);
    assert(igs_writeParameterAsData("", myOtherData, 64) == IGS_FAILURE);
    assert(igs_writeParameterAsData("my_data", myOtherData, 64) == IGS_SUCCESS);
    data = NULL;
    dataSize = 0;
    assert(igs_readParameterAsData("my_data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 64 && memcmp(data, myOtherData, dataSize) == 0);
    free(data);
    data = NULL;
    dataSize = 0;
    assert(igs_readParameter("my_data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 64 && memcmp(data, myOtherData, dataSize) == 0);
    free(data);
    data = NULL;
    dataSize = 0;
    igs_clearDataForParameter("my_data");
    data = NULL;
    dataSize = 0;
    assert(igs_readParameter("my_data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 0 && data == NULL);
    assert(igs_readParameterAsData("my_data", &data, &dataSize) == IGS_SUCCESS);
    assert(dataSize == 0 && data == NULL);
    
    
    //definition - part 2
    //TODO: compare exported def, saved file and reference file
    char *exportedDef = igs_getDefinition();
    //assert(streq(exportedDef, referenceDefinition));
    free(exportedDef);
    igs_setDefinitionPath("/tmp/simple Demo Agent.json");
    igs_writeDefinitionToPath();
    
    igs_clearDefinition();
    name = igs_getAgentName();
    defName = igs_getDefinitionName();
    assert(streq(name, defName));
    free(name);
    name = NULL;
    free(defName);
    defName = NULL;
    defDesc = igs_getDefinitionDescription();
    assert(defDesc == NULL);
    defVer = igs_getDefinitionVersion();
    assert(defVer == NULL);
    elements = igs_getInputsList(&nbElements);
    assert(elements == NULL && nbElements == 0);
    elements = igs_getOutputsList(&nbElements);
    assert(elements == NULL && nbElements == 0);
    elements = igs_getParametersList(&nbElements);
    assert(elements == NULL && nbElements == 0);
    elements = igs_getCallsList(&nbElements);
    assert(elements == NULL && nbElements == 0);
    
    
    //mapping
    assert(igs_loadMapping("invalid json") == IGS_FAILURE);
    assert(igs_loadMappingFromPath("/does not exist") == IGS_FAILURE);
    assert(igs_getMapping()); //intentional memory leak here
    assert(igs_getMappingEntriesNumber() == 0);
    
    assert(igs_addMappingEntry("toto", "other_agent", "tata") != 0);
    long mapId = igs_addMappingEntry("toto", "other_agent", "tata");
    assert(mapId > 0);
    assert(igs_removeMappingEntryWithId(12345) == IGS_FAILURE);
    assert(igs_removeMappingEntryWithId(mapId) == IGS_SUCCESS);
    assert(igs_addMappingEntry("toto", "other_agent", "tata") != 0);
    assert(igs_removeMappingEntryWithName("toto", "other_agent", "tata") == IGS_SUCCESS);
    assert(igs_removeMappingEntryWithName("toto", "other_agent", "tata") == IGS_FAILURE);
    
    //TODO: compare exported mapping, saved file and reference file
    char *exportedMapping = igs_getMapping();
    //assert(streq(exportedMapping, referenceMapping));
    free(exportedMapping);
    igs_setMappingPath("/tmp/simple Demo Agent mapping.json");
    igs_writeMappingToPath();
    igs_clearMapping();
    char *mappingName = igs_getMappingName();
    assert(mappingName == NULL);
    char *mappingDesc = igs_getMappingDescription();
    assert(mappingDesc == NULL);
    char *mappingVer = igs_getMappingVersion();
    assert(mappingVer == NULL);
    //TODO: compare exported cleared mapping with reference string
//    exportedMapping = igs_getMapping();
//    assert(streq(exportedMapping, referenceClearedMapping));
//    free(exportedMapping);
    
    //calls
    igs_callArgument_t *list = NULL;
    igs_addBoolToArgumentsList(&list, myBool);
    igs_addIntToArgumentsList(&list, myInt);
    igs_addDoubleToArgumentsList(&list, myDouble);
    igs_addStringToArgumentsList(&list, myString);
    igs_addDataToArgumentsList(&list, myData, 32);
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
    assert(list->next->next->d == myDouble);
    assert(list->next->next->next->name == NULL);
    assert(list->next->next->next->type == IGS_STRING_T);
    assert(list->next->next->next->size == strlen(myString) + 1);
    assert(streq(list->next->next->next->c, myString));
    assert(list->next->next->next->next->name == NULL);
    assert(list->next->next->next->next->type == IGS_DATA_T);
    assert(list->next->next->next->next->size == 32);
    assert(memcmp(list->next->next->next->next->data, myData, 32) == 0);
    igs_callArgument_t *listBis = igs_cloneArgumentsList(list);
    igs_destroyArgumentsList(&list);
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
    assert(listBis->next->next->d == myDouble);
    assert(listBis->next->next->next->name == NULL);
    assert(listBis->next->next->next->type == IGS_STRING_T);
    assert(listBis->next->next->next->size == strlen(myString) + 1);
    assert(streq(listBis->next->next->next->c, myString));
    assert(listBis->next->next->next->next->name == NULL);
    assert(listBis->next->next->next->next->type == IGS_DATA_T);
    assert(listBis->next->next->next->next->size == 32);
    assert(memcmp(listBis->next->next->next->next->data, myData, 32) == 0);
    igs_destroyArgumentsList(&listBis);
    igs_destroyArgumentsList(&listBis);
    
    assert(igs_getNumberOfCalls() == 0);
    assert(!igs_checkCallExistence(NULL));
    assert(!igs_checkCallExistence("toto"));
    elements = igs_getCallsList(&nbElements);
    assert(elements == NULL && nbElements == 0);
    assert(igs_getFirstArgumentForCall(NULL) == NULL);
    assert(igs_getFirstArgumentForCall("toto") == NULL);
    assert(igs_getNumberOfArgumentsForCall(NULL) == 0);
    assert(igs_getNumberOfArgumentsForCall("toto") == 0);
    assert(igs_checkCallArgumentExistence(NULL, NULL) == 0);
    assert(igs_checkCallArgumentExistence("toto", NULL) == 0);
    assert(igs_checkCallArgumentExistence(NULL, "toto") == 0);
    assert(igs_checkCallArgumentExistence("toto", "toto") == 0);
    assert(igs_initCall("myCall", testerCallCallback, NULL) == IGS_SUCCESS);
    assert(igs_removeCall("myCall") == IGS_SUCCESS);
    assert(igs_removeCall("myCall") == IGS_FAILURE);
    assert(igs_addArgumentToCall("myCall", "myArg", IGS_BOOL_T) == IGS_FAILURE);
    assert(igs_removeArgumentFromCall("myCall", "myArg") == IGS_FAILURE);
    
    assert(igs_initCall("myCall", testerCallCallback, NULL) == IGS_SUCCESS);
    assert(igs_addArgumentToCall("myCall", "myBool", IGS_BOOL_T) == IGS_SUCCESS);
    assert(igs_addArgumentToCall("myCall", "myInt", IGS_INTEGER_T) == IGS_SUCCESS);
    assert(igs_addArgumentToCall("myCall", "myDouble", IGS_DOUBLE_T) == IGS_SUCCESS);
    assert(igs_addArgumentToCall("myCall", "myString", IGS_STRING_T) == IGS_SUCCESS);
    assert(igs_addArgumentToCall("myCall", "myData", IGS_DATA_T) == IGS_SUCCESS);
    assert(igs_getNumberOfCalls() == 1);
    assert(igs_checkCallExistence("myCall"));
    elements = igs_getCallsList(&nbElements);
    assert(nbElements == 1 && streq(elements[0], "myCall"));
    igs_freeCallsList(&elements, nbElements);
    assert(elements == NULL);
    assert(igs_removeCall("myCall") == IGS_SUCCESS);
    
    assert(igs_initCall("myCall", testerCallCallback, NULL) == IGS_SUCCESS);
    assert(igs_addArgumentToCall("myCall", "myBool", IGS_BOOL_T) == IGS_SUCCESS);
    assert(igs_addArgumentToCall("myCall", "myInt", IGS_INTEGER_T) == IGS_SUCCESS);
    assert(igs_addArgumentToCall("myCall", "myDouble", IGS_DOUBLE_T) == IGS_SUCCESS);
    assert(igs_addArgumentToCall("myCall", "myString", IGS_STRING_T) == IGS_SUCCESS);
    assert(igs_addArgumentToCall("myCall", "myData", IGS_DATA_T) == IGS_SUCCESS);
    assert(igs_getNumberOfArgumentsForCall("myCall") == 5);
    assert(igs_checkCallArgumentExistence("myCall", "myBool"));
    assert(igs_checkCallArgumentExistence("myCall", "myInt"));
    assert(igs_checkCallArgumentExistence("myCall", "myDouble"));
    assert(igs_checkCallArgumentExistence("myCall", "myString"));
    assert(igs_checkCallArgumentExistence("myCall", "myData"));
    list = igs_getFirstArgumentForCall("myCall");
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
    assert(list->next->next->d == 0.0);
    assert(streq(list->next->next->next->name, "myString"));
    assert(list->next->next->next->type == IGS_STRING_T);
    assert(list->next->next->next->size == 0);
    assert(list->next->next->next->c == NULL);
    assert(streq(list->next->next->next->next->name, "myData"));
    assert(list->next->next->next->next->type == IGS_DATA_T);
    assert(list->next->next->next->next->size == 0);
    assert(list->next->next->next->next->data == NULL);
    assert(igs_removeCall("myCall") == IGS_SUCCESS);
    
    //bus
    assert(igs_busAddServiceDescription("publisher", "toto") == IGS_FAILURE);
    assert(igs_busAddServiceDescription("logger", "toto") == IGS_FAILURE);
    assert(igs_busAddServiceDescription("pid", "toto") == IGS_FAILURE);
    assert(igs_busAddServiceDescription("commandline", "toto") == IGS_FAILURE);
    assert(igs_busAddServiceDescription("hostname", "toto") == IGS_FAILURE);
    assert(igs_busRemoveServiceDescription("bogus key") == IGS_FAILURE);
    assert(igs_busRemoveServiceDescription("publisher") == IGS_FAILURE);
    assert(igs_busRemoveServiceDescription("logger") == IGS_FAILURE);
    assert(igs_busRemoveServiceDescription("pid") == IGS_FAILURE);
    assert(igs_busRemoveServiceDescription("commandline") == IGS_FAILURE);
    assert(igs_busRemoveServiceDescription("hostname") == IGS_FAILURE);
    assert(igs_busAddServiceDescription("new key", "toto") == IGS_SUCCESS);
    assert(igs_busRemoveServiceDescription("new key") == IGS_SUCCESS);
    assert(igs_busRemoveServiceDescription("new key") == IGS_FAILURE);
    assert(igs_busJoinChannel("toto") == IGS_FAILURE);
    igs_busLeaveChannel("toto");
    assert(igs_busAddServiceDescription("new key", "toto") == IGS_SUCCESS);
    
    
    //prepare agent for dynamic tests by adding proper complete definitions
    igs_setAgentName(agentName);
    igs_setVerbose(verbose);
    igs_setLicensePath("~/Documents/IngeScape/licenses/");
    igs_observeBus(testerBusCallback, NULL);
    
    igs_setDefinitionName(agentName);
    igs_setDefinitionDescription("One example for each type of IOP and call");
    igs_setDefinitionVersion("1.0");
    igs_createInput("my_impulsion", IGS_IMPULSION_T, NULL, 0);
    igs_createInput("my_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igs_createInput("my_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igs_createInput("my_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igs_createInput("my_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igs_createInput("my_data", IGS_DATA_T, myData, 32);
    igs_createOutput("my_impulsion", IGS_IMPULSION_T, NULL, 0);
    igs_createOutput("my_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igs_createOutput("my_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igs_createOutput("my_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igs_createOutput("my_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igs_createOutput("my_data", IGS_DATA_T, myData, 32);
    igs_createParameter("my_impulsion", IGS_IMPULSION_T, NULL, 0);
    igs_createParameter("my_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igs_createParameter("my_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igs_createParameter("my_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igs_createParameter("my_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igs_createParameter("my_data", IGS_DATA_T, myData, 32);
    igs_initCall("myCall", testerCallCallback, NULL);
    igs_addArgumentToCall("myCall", "myBool", IGS_BOOL_T);
    igs_addArgumentToCall("myCall", "myInt", IGS_INTEGER_T);
    igs_addArgumentToCall("myCall", "myDouble", IGS_DOUBLE_T);
    igs_addArgumentToCall("myCall", "myString", IGS_STRING_T);
    igs_addArgumentToCall("myCall", "myData", IGS_DATA_T);
    
    igs_observeInput("my_impulsion", testerIOPCallback, NULL);
    igs_observeInput("my_bool", testerIOPCallback, NULL);
    igs_observeInput("my_int", testerIOPCallback, NULL);
    igs_observeInput("my_double", testerIOPCallback, NULL);
    igs_observeInput("my_string", testerIOPCallback, NULL);
    igs_observeInput("my_data", testerIOPCallback, NULL);
    
    igs_addMappingEntry("my_impulsion", "partner", "sparing_impulsion");
    igs_addMappingEntry("my_bool", "partner", "sparing_bool");
    igs_addMappingEntry("my_int", "partner", "sparing_int");
    igs_addMappingEntry("my_double", "partner", "sparing_double");
    igs_addMappingEntry("my_string", "partner", "sparing_string");
    igs_addMappingEntry("my_data", "partner", "sparing_data");
    
    
    //IOP writing and types conversions
    igs_writeInputAsImpulsion("my_impulsion");
    igs_writeInputAsImpulsion("my_bool");
    assert(!igs_readInputAsBool("my_bool"));
    igs_writeInputAsImpulsion("my_int");
    assert(igs_readInputAsInt("my_int") == 0);
    igs_writeInputAsImpulsion("my_double");
    assert(igs_readInputAsDouble("my_double") == 0);
    igs_writeInputAsImpulsion("my_string");
    char *readResult = igs_readInputAsString("my_string");
    assert(strlen(readResult) == 0);
    free(readResult);
    igs_writeInputAsImpulsion("my_data");
    igs_readInputAsData("my_data", &data, &dataSize);
    assert(data == NULL && dataSize == 0);
    
    igs_writeInputAsBool("my_impulsion", true);
    igs_writeInputAsBool("my_bool", true);
    assert(igs_readInputAsBool("my_bool"));
    igs_writeInputAsBool("my_int", true);
    assert(igs_readInputAsInt("my_int") == 1);
    igs_writeInputAsBool("my_double", true);
    assert(igs_readInputAsDouble("my_double") == 1.0);
    igs_writeInputAsBool("my_string", true);
    readResult = igs_readInputAsString("my_string");
    assert(streq(readResult, "1"));
    free(readResult);
    igs_writeInputAsBool("my_data", true);
    igs_readInputAsData("my_data", &data, &dataSize);
    assert(*(bool*)data && dataSize == sizeof(bool));
    
    igs_writeInputAsInt("my_impulsion", 3);
    igs_writeInputAsInt("my_bool", 3);
    assert(igs_readInputAsBool("my_bool"));
    igs_writeInputAsInt("my_int", 3);
    assert(igs_readInputAsInt("my_int") == 3);
    igs_writeInputAsInt("my_double", 3);
    assert(igs_readInputAsDouble("my_double") == 3.0);
    igs_writeInputAsInt("my_string", 3);
    readResult = igs_readInputAsString("my_string");
    assert(streq(readResult, "3"));
    free(readResult);
    igs_writeInputAsInt("my_data", 3);
    igs_readInputAsData("my_data", &data, &dataSize);
    assert(*(int*)data == 3 && dataSize == sizeof(int));
    
    igs_writeInputAsDouble("my_impulsion", 3.3);
    igs_writeInputAsDouble("my_bool", 3.3);
    assert(igs_readInputAsBool("my_bool"));
    igs_writeInputAsDouble("my_int", 3.3);
    assert(igs_readInputAsInt("my_int") == 3);
    igs_writeInputAsDouble("my_double", 3.3);
    assert(igs_readInputAsDouble("my_double") == 3.3);
    igs_writeInputAsDouble("my_string", 3.3);
    readResult = igs_readInputAsString("my_string");
    assert(streq(readResult, "3.300000"));
    free(readResult);
    igs_writeInputAsDouble("my_data", 3.3);
    igs_readInputAsData("my_data", &data, &dataSize);
    assert(*(double*)data == 3.3 && dataSize == sizeof(double));
    
    igs_writeInputAsString("my_impulsion", "true");
    igs_writeInputAsString("my_bool", "true");
    assert(igs_readInputAsBool("my_bool"));
    igs_writeInputAsString("my_int", "3.3");
    assert(igs_readInputAsInt("my_int") == 3);
    igs_writeInputAsString("my_double", "3.3");
    assert(igs_readInputAsDouble("my_double") == 3.3);
    igs_writeInputAsString("my_string", "3.3");
    readResult = igs_readInputAsString("my_string");
    assert(streq(readResult, "3.3"));
    free(readResult);
    assert(igs_writeInputAsString("my_data", "toto") == IGS_FAILURE);
    assert(igs_writeInputAsString("my_data", "0123456789abcdef") == IGS_SUCCESS);
    igs_readInputAsData("my_data", &data, &dataSize);
    assert(dataSize == 16);
    
    data = (void*)"my data";
    dataSize = strlen("my data") + 1;
    igs_writeInputAsData("my_impulsion", data, dataSize);
    igs_writeInputAsData("my_bool", data, dataSize);
    igs_writeInputAsData("my_int", data, dataSize);
    igs_writeInputAsData("my_double", data, dataSize);
    igs_writeInputAsData("my_string", data, dataSize);
    igs_writeInputAsData("my_data", data, dataSize);
    igs_readInputAsData("my_data", &data, &dataSize);
    assert(streq("my data", (char*)data) && dataSize == strlen("my data") + 1);
    
    igs_writeInputAsBool("my_bool", true);
    assert(igs_readInputAsBool("my_bool"));
    assert(igs_readInputAsInt("my_bool") == 1);
    assert(igs_readInputAsDouble("my_bool") == 1);
    assert(streq(igs_readInputAsString("my_bool"), "true")); //memory leak here
    igs_writeInputAsBool("my_bool", false);
    assert(!igs_readInputAsBool("my_bool"));
    assert(igs_readInputAsInt("my_bool") == 0);
    assert(igs_readInputAsDouble("my_bool") == 0);
    assert(streq(igs_readInputAsString("my_bool"), "false")); //memory leak here
    assert(igs_readInputAsData("my_bool", &data, &dataSize) == IGS_FAILURE && data == NULL && dataSize == 0);
    
    igs_writeInputAsInt("my_int", 3);
    assert(igs_readInputAsBool("my_int"));
    assert(igs_readInputAsInt("my_int") == 3);
    assert(igs_readInputAsDouble("my_int") == 3);
    assert(streq(igs_readInputAsString("my_int"), "3")); //memory leak here
    igs_writeInputAsInt("my_int", 0);
    assert(!igs_readInputAsBool("my_int"));
    assert(igs_readInputAsInt("my_int") == 0);
    assert(igs_readInputAsDouble("my_int") == 0);
    assert(streq(igs_readInputAsString("my_int"), "0")); //memory leak here
    assert(igs_readInputAsData("my_int", &data, &dataSize) == IGS_FAILURE && data == NULL && dataSize == 0);
    
    igs_writeInputAsDouble("my_double", 3.3);
    assert(igs_readInputAsBool("my_double"));
    assert(igs_readInputAsInt("my_double") == 3);
    assert(igs_readInputAsDouble("my_double") == 3.3);
    assert(streq(igs_readInputAsString("my_double"), "3.300000")); //memory leak here
    igs_writeInputAsDouble("my_double", 0.0);
    assert(!igs_readInputAsBool("my_double"));
    assert(igs_readInputAsInt("my_double") == 0);
    assert(igs_readInputAsDouble("my_double") == 0);
    assert(streq(igs_readInputAsString("my_double"), "0.000000")); //memory leak here
    assert(igs_readInputAsData("my_double", &data, &dataSize) == IGS_FAILURE && data == NULL && dataSize == 0);
    
    igs_writeInputAsString("my_string", "true");
    assert(igs_readInputAsBool("my_string"));
    igs_writeInputAsString("my_string", "false");
    assert(!igs_readInputAsBool("my_string"));
    igs_writeInputAsString("my_string", "10.1");
    assert(igs_readInputAsInt("my_string") == 10);
    assert(igs_readInputAsDouble("my_string") == 10.1);
    assert(streq(igs_readInputAsString("my_string"), "10.1")); //memory leak here
    assert(igs_readInputAsData("my_string", &data, &dataSize) == IGS_FAILURE && data == NULL && dataSize == 0);
    
    data = (void*)"my data";
    dataSize = strlen("my data") + 1;
    igs_writeInputAsData("my_data", NULL, 0);
    assert(!igs_readInputAsBool("my_data"));
    igs_writeInputAsData("my_data", data, dataSize);
    assert(!igs_readInputAsBool("my_data"));
    assert(igs_readInputAsInt("my_data") == 0);
    assert(igs_readInputAsDouble("my_data") == 0);
    assert(igs_readInputAsString("my_data") == NULL);
    assert(igs_readInputAsData("my_data", &data, &dataSize) == IGS_SUCCESS);
    assert(streq((char*)data, "my data") && strlen((char*)data) == dataSize - 1);
    
    
    //add multiple agents to be enabled and disabled on demand
    //first additional agent is activated immediately
    firstAgent = igsAgent_new("firstAgent", true);
    igsAgent_setDefinitionName(firstAgent, "firstAgent");
    igsAgent_setDefinitionDescription(firstAgent, "First virtual agent");
    igsAgent_setDefinitionVersion(firstAgent, "1.0");
    igsAgent_createInput(firstAgent, "first_impulsion", IGS_IMPULSION_T, NULL, 0);
    igsAgent_createInput(firstAgent, "first_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igsAgent_createInput(firstAgent, "first_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igsAgent_createInput(firstAgent, "first_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igsAgent_createInput(firstAgent, "first_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igsAgent_createInput(firstAgent, "first_data", IGS_DATA_T, myData, 32);
    igsAgent_createOutput(firstAgent, "first_impulsion", IGS_IMPULSION_T, NULL, 0);
    igsAgent_createOutput(firstAgent, "first_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igsAgent_createOutput(firstAgent, "first_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igsAgent_createOutput(firstAgent, "first_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igsAgent_createOutput(firstAgent, "first_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igsAgent_createOutput(firstAgent, "first_data", IGS_DATA_T, myData, 32);
    igsAgent_createParameter(firstAgent, "first_impulsion", IGS_IMPULSION_T, NULL, 0);
    igsAgent_createParameter(firstAgent, "first_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igsAgent_createParameter(firstAgent, "first_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igsAgent_createParameter(firstAgent, "first_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igsAgent_createParameter(firstAgent, "first_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igsAgent_createParameter(firstAgent, "first_data", IGS_DATA_T, myData, 32);
    
    igsAgent_initCall(firstAgent, "firstCall", agentCallCallback, NULL);
    igsAgent_addArgumentToCall(firstAgent, "firstCall", "firstBool", IGS_BOOL_T);
    igsAgent_addArgumentToCall(firstAgent, "firstCall", "firstInt", IGS_INTEGER_T);
    igsAgent_addArgumentToCall(firstAgent, "firstCall", "firstDouble", IGS_DOUBLE_T);
    igsAgent_addArgumentToCall(firstAgent, "firstCall", "firstString", IGS_STRING_T);
    igsAgent_addArgumentToCall(firstAgent, "firstCall", "firstData", IGS_DATA_T);
    
    igsAgent_observeInput(firstAgent, "first_impulsion", agentIOPCallback, NULL);
    igsAgent_observeInput(firstAgent, "first_bool", agentIOPCallback, NULL);
    igsAgent_observeInput(firstAgent, "first_int", agentIOPCallback, NULL);
    igsAgent_observeInput(firstAgent, "first_double", agentIOPCallback, NULL);
    igsAgent_observeInput(firstAgent, "first_string", agentIOPCallback, NULL);
    igsAgent_observeInput(firstAgent, "first_data", agentIOPCallback, NULL);
    
    igsAgent_addMappingEntry(firstAgent, "first_impulsion", "partner", "sparing_impulsion");
    igsAgent_addMappingEntry(firstAgent, "first_bool", "partner", "sparing_bool");
    igsAgent_addMappingEntry(firstAgent, "first_int", "partner", "sparing_int");
    igsAgent_addMappingEntry(firstAgent, "first_double", "partner", "sparing_double");
    igsAgent_addMappingEntry(firstAgent, "first_string", "partner", "sparing_string");
    igsAgent_addMappingEntry(firstAgent, "first_data", "partner", "sparing_data");
    
    //second additional agent is NOT activated immediately
    secondAgent = igsAgent_new("secondAgent", false);
    igsAgent_setDefinitionName(secondAgent, "secondAgent");
    igsAgent_setDefinitionDescription(secondAgent, "Second virtual agent");
    igsAgent_setDefinitionVersion(secondAgent, "1.0");
    igsAgent_createInput(secondAgent, "second_impulsion", IGS_IMPULSION_T, NULL, 0);
    igsAgent_createInput(secondAgent, "second_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igsAgent_createInput(secondAgent, "second_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igsAgent_createInput(secondAgent, "second_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igsAgent_createInput(secondAgent, "second_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igsAgent_createInput(secondAgent, "second_data", IGS_DATA_T, myData, 32);
    igsAgent_createOutput(secondAgent, "second_impulsion", IGS_IMPULSION_T, NULL, 0);
    igsAgent_createOutput(secondAgent, "second_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igsAgent_createOutput(secondAgent, "second_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igsAgent_createOutput(secondAgent, "second_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igsAgent_createOutput(secondAgent, "second_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igsAgent_createOutput(secondAgent, "second_data", IGS_DATA_T, myData, 32);
    igsAgent_createParameter(secondAgent, "second_impulsion", IGS_IMPULSION_T, NULL, 0);
    igsAgent_createParameter(secondAgent, "second_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igsAgent_createParameter(secondAgent, "second_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igsAgent_createParameter(secondAgent, "second_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igsAgent_createParameter(secondAgent, "second_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igsAgent_createParameter(secondAgent, "second_data", IGS_DATA_T, myData, 32);
    igsAgent_initCall(secondAgent, "secondCall", agentCallCallback, NULL);
    igsAgent_addArgumentToCall(secondAgent, "secondCall", "secondBool", IGS_BOOL_T);
    igsAgent_addArgumentToCall(secondAgent, "secondCall", "secondInt", IGS_INTEGER_T);
    igsAgent_addArgumentToCall(secondAgent, "secondCall", "secondDouble", IGS_DOUBLE_T);
    igsAgent_addArgumentToCall(secondAgent, "secondCall", "secondString", IGS_STRING_T);
    igsAgent_addArgumentToCall(secondAgent, "secondCall", "secondData", IGS_DATA_T);
    
    igsAgent_observeInput(secondAgent, "second_impulsion", agentIOPCallback, NULL);
    igsAgent_observeInput(secondAgent, "second_bool", agentIOPCallback, NULL);
    igsAgent_observeInput(secondAgent, "second_int", agentIOPCallback, NULL);
    igsAgent_observeInput(secondAgent, "second_double", agentIOPCallback, NULL);
    igsAgent_observeInput(secondAgent, "second_string", agentIOPCallback, NULL);
    igsAgent_observeInput(secondAgent, "second_data", agentIOPCallback, NULL);
    
    igsAgent_addMappingEntry(secondAgent, "second_impulsion", "partner", "sparing_impulsion");
    igsAgent_addMappingEntry(secondAgent, "second_bool", "partner", "sparing_bool");
    igsAgent_addMappingEntry(secondAgent, "second_int", "partner", "sparing_int");
    igsAgent_addMappingEntry(secondAgent, "second_double", "partner", "sparing_double");
    igsAgent_addMappingEntry(secondAgent, "second_string", "partner", "sparing_string");
    igsAgent_addMappingEntry(secondAgent, "second_data", "partner", "sparing_data");
    
    igsAgent_addMappingEntry(secondAgent, "second_impulsion", "tester", "my_impulsion");
    igsAgent_addMappingEntry(secondAgent, "second_bool", "tester", "my_bool");
    igsAgent_addMappingEntry(secondAgent, "second_int", "tester", "my_int");
    igsAgent_addMappingEntry(secondAgent, "second_double", "tester", "my_double");
    igsAgent_addMappingEntry(secondAgent, "second_string", "tester", "my_string");
    igsAgent_addMappingEntry(secondAgent, "second_data", "tester", "my_data");
    
    igsAgent_addMappingEntry(secondAgent, "second_impulsion", "firstAgent", "first_impulsion");
    igsAgent_addMappingEntry(secondAgent, "second_bool", "firstAgent", "first_bool");
    igsAgent_addMappingEntry(secondAgent, "second_int", "firstAgent", "first_int");
    igsAgent_addMappingEntry(secondAgent, "second_double", "firstAgent", "first_double");
    igsAgent_addMappingEntry(secondAgent, "second_string", "firstAgent", "first_string");
    igsAgent_addMappingEntry(secondAgent, "second_data", "firstAgent", "first_data");
    
    //test mapping in same process between second_agent and first_agent
    igsAgent_activate(secondAgent);
    igsAgent_writeOutputAsBool(firstAgent, "first_bool", true);
    assert(igsAgent_readInputAsBool(secondAgent, "second_bool"));
    igsAgent_writeOutputAsBool(firstAgent, "first_bool", false);
    assert(!igsAgent_readInputAsBool(secondAgent, "second_bool"));
    igsAgent_writeOutputAsInt(firstAgent, "first_int", 5);
    assert(igsAgent_readInputAsInt(secondAgent, "second_int") == 5);
    igsAgent_writeOutputAsDouble(firstAgent, "first_double", 5.5);
    assert(igsAgent_readInputAsDouble(secondAgent, "second_double") == 5.5);
    igsAgent_writeOutputAsString(firstAgent, "first_string", "test string mapping");
    assert(streq(igsAgent_readInputAsString(secondAgent, "second_string"), "test string mapping")); //memory leak here
    data = (void*)"my data";
    dataSize = strlen("my data") + 1;
    igsAgent_writeOutputAsData(firstAgent, "first_data", data, dataSize);
    assert(igsAgent_readInputAsData(secondAgent, "second_data", &data, &dataSize) == IGS_SUCCESS);
    assert(streq((char*)data, "my data") && strlen((char*)data) == dataSize - 1);
    
    //test call in the same process
    list = NULL;
    igs_addBoolToArgumentsList(&list, true);
    igs_addIntToArgumentsList(&list, 13);
    igs_addDoubleToArgumentsList(&list, 13.3);
    igs_addStringToArgumentsList(&list, "my string arg");
    igs_addDataToArgumentsList(&list, data, dataSize);
    igsAgent_sendCall(firstAgent, "secondAgent", "secondCall", &list, "token");
    
    
    //test agent events in same process
    igsAgent_deactivate(secondAgent);
    igsAgent_deactivate(firstAgent);
    igs_observeAgentEvents(agentEvent, NULL);
    igsAgent_observeAgentEvents(firstAgent, agentEvent2, NULL);
    igsAgent_observeAgentEvents(secondAgent, agentEvent2, NULL);
    igsAgent_activate(firstAgent);
    assert(tester_firstAgentEntered);
    assert(tester_firstAgentKnowsUs);
    assert(first_testerAgentEntered);
    assert(first_testergentKnowsUs);
    igsAgent_activate(secondAgent);
    assert(second_firstAgentEntered);
    assert(second_firstAgentKnowsUs);
    assert(second_testerAgentEntered);
    assert(second_testergentKnowsUs);
    assert(tester_secondAgentEntered);
    assert(tester_secondAgentKnowsUs);
    assert(first_secondAgentEntered);
    assert(first_secondAgentKnowsUs);
    igsAgent_deactivate(firstAgent);
    assert(tester_firstAgentExited);
    assert(second_firstAgentExited);
    igsAgent_deactivate(secondAgent);
    assert(tester_secondAgentExited);
    
    //elections
    assert(igs_leaveElection("my election") == IGS_FAILURE);
    assert(igs_competeInElection("my election") == IGS_SUCCESS);
    assert(igs_competeInElection("my election") == IGS_FAILURE);
    assert(igs_competeInElection("INGESCAPE_PRIVATE") == IGS_FAILURE);
    assert(igs_leaveElection("my election") == IGS_SUCCESS);
    assert(igs_leaveElection("my election") == IGS_FAILURE);
    assert(igs_leaveElection("my other election") == IGS_FAILURE);
    assert(igs_competeInElection("my other election") == IGS_SUCCESS);
    assert(igs_competeInElection("my other election") == IGS_FAILURE);
    assert(igs_leaveElection("my other election") == IGS_SUCCESS);
    assert(igs_leaveElection("my other election") == IGS_FAILURE);
    
    //replay
    igs_replayTerminate();
//    igs_replayInit("tester_replay.log",
//                   0, NULL, false, 0, NULL);
//    igs_replayInit("tester_replay.log",
//                   10, "11:26:12", false, 0, "firstAgent");
//    igs_replayInit("tester_replay.log",
//                   10, "11:26:12", false, IGS_REPLAY_INPUT, "firstAgent");
//    igs_replayInit("tester_replay.log",
//                   10, "11:26:12", false, IGS_REPLAY_INPUT+IGS_REPLAY_OUTPUT, "firstAgent");
//    igs_replayInit("tester_replay.log",
//                   10, "11:26:12", false, IGS_REPLAY_INPUT+IGS_REPLAY_OUTPUT+IGS_REPLAY_PARAMETER, "firstAgent");
//    igs_replayInit("tester_replay.log",
//                   10, "11:26:12", false, IGS_REPLAY_INPUT+IGS_REPLAY_OUTPUT+IGS_REPLAY_PARAMETER+IGS_REPLAY_RECEIVED_CALL+IGS_REPLAY_SENT_CALL, "firstAgent");
//    igs_replayTerminate();
//    igs_replayInit("tester_replay.log",
//                   100, NULL, false, 0, NULL);
////    igs_replayTerminate();
//    igs_replayInit("tester_replay.log",
//                   100, NULL, true, 0, NULL);
//    igs_replayStart();
//    igs_replayTerminate();
    
    if (autoTests){
        igs_startWithDevice(networkDevice, port);
        igs_busJoinChannel("TEST_CHANNEL");
        zloop_t *loop = zloop_new();
        zsock_t *pipe = igs_getPipeToIngescape();
        zloop_reader(loop, pipe, ingescapeSentMessage, NULL);
        igs_info("ready to start autotests");
        zloop_start(loop);
        zloop_destroy(&loop);
        igs_stop();
        exit(EXIT_SUCCESS);
    }else{
        if (networkDevice == NULL){
            //we have no device to start with: try to find one
            char **devices = NULL;
            char **addresses = NULL;
            int nbD = 0;
            int nbA = 0;
            igs_getNetdevicesList(&devices, &nbD);
            igs_getNetaddressesList(&addresses, &nbA);
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
            igs_freeNetdevicesList(devices, nbD);
            igs_freeNetaddressesList(addresses, nbD);
        }
    }
    
    
    igsAgent_activate(firstAgent);
    igs_startWithDevice(networkDevice, port);
    igs_startWithDevice(networkDevice, port); //testing immediate stop + restart
    igs_stop();
    igs_stop();
    igs_stop();
    igs_startWithDevice(networkDevice, port);
    
    //mainloop management (two modes)
    if (!interactiveloop) {
        //Run the main loop (non-interactive mode):
        //we rely on CZMQ which is an ingeScape dependency and is thus
        //always here.
        zloop_t *loop = zloop_new();
        zsock_t *pipe = igs_getPipeToIngescape();
        zloop_reader(loop, pipe, ingescapeSentMessage, NULL);
        zloop_start(loop);
        zloop_destroy(&loop);
    }else{
        char message[BUFFER_SIZE];
        char command[BUFFER_SIZE];
        char param1[BUFFER_SIZE];
        char param2[BUFFER_SIZE];
        int usedChar = 0;
        print_cli_usage();
        while (igs_isStarted()){
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
                    }else if(streq(command, "call")){
                        callCommand();
                    }else if(streq(command, "bus")){
                        busCommand();
                    }else if(streq(command, "activate")){
                        igsAgent_activate(secondAgent);
                    }else if(streq(command, "deactivate")){
                        igsAgent_deactivate(secondAgent);
                    }else if(streq(command, "call_local")){
                        igs_callArgument_t *args = NULL;
                        igs_addBoolToArgumentsList(&args, true);
                        igs_addIntToArgumentsList(&args, 3);
                        igs_addDoubleToArgumentsList(&args, 3.0);
                        igs_addStringToArgumentsList(&args, "call string test");
                        igs_addDataToArgumentsList(&args, myOtherData, 64);
                        igsAgent_sendCall(firstAgent, "secondAgent", "secondCall", &args, "token");
                    }else if(streq(command, "security")){
                        securityCommand();
                        
                    }else if(streq(command, "gossip")){
                        gossipCommand();
                        
                    }else {
                        printf("unhandled command: %s\n", command);
                    }
                }else if (matches == 2) {
                    if (streq(command, "editor")){
                        editorCommand(param1, "sparing_int");
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

    igsAgent_destroy(&secondAgent);
    igs_stop();
    igsAgent_destroy(&firstAgent);
    igs_cleanContext();

    return EXIT_SUCCESS;
}
