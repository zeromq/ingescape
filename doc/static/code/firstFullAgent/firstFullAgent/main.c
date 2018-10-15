//
//  main.c
//  firstFullAgent
//
//  Created by Stephane Vales on 09/05/2018.
//  Copyright © 2018 IngeScape. All rights reserved.
//

#include <getopt.h> //command line options at statrtup
#include <stdlib.h> //standard C functions such as getenv, atoi, exit, etc.
#include <string.h> //C string handling functions
#include <signal.h> //catching interruptions
#include <czmq.h> //access to the ZeroMQ mainloop
#include <ingescape/ingescape.h> //main ingeScape header

#include "regexp.h" //regexp utilities for the command line

//default agent parameters to be overriden by command line paramters
int port = 5670;
char *agentName = "firstFullAgent"; //default agent name
char *networkDevice = "en0"; //default network device value
bool verbose = false;

//definition and mapping as external resources
//(used optionally)
#define BUFFER_SIZE 1024
#define DEFAULTDEFINITIONPATH "~/Documents/IngeScape/agents/firstFullAgent_definition.json"
char definitionFile[BUFFER_SIZE];
#define DEFAULTMAPPINGPATH "~/Documents/IngeScape/agents/firstFullAgent_mapping.json"
char mappingFile[BUFFER_SIZE];

//SIGINT handling
void interruptionReceived(int val) {
    IGS_UNUSED(val);
    //do something here if needed
}

/*
 This callback is able to display the new value of any IOP it observes.
 In order to use the igs_read* functions, the IOP type must be checked
 first and then the value type.
 */
void myIOPCallback(iop_t iopType, const char* name, iopType_t valueType,
                  void* value, size_t valueSize, void* myData){
    
    //NB: value is a pointer to the new IOP value. If you are sure of
    //what you are doing, you can use it directly. If not, use the rest
    //of the code in this function.
    //valueSize is the actual size of the new IOP value.
    
    if (valueType == IGS_IMPULSION_T){
        printf("%s changed (impulsion)\n", name);
    } else {
        char *convertedValue = NULL;
        switch (iopType) {
            case IGS_INPUT_T:
                convertedValue = igs_readInputAsString(name);
                break;
            case IGS_OUTPUT_T:
                convertedValue = igs_readOutputAsString(name);
                break;
            case IGS_PARAMETER_T:
                convertedValue = igs_readParameterAsString(name);
                break;
            default:
                break;
        }
        printf("%s changed to %s", name, convertedValue);
        if (convertedValue != NULL){
            free(convertedValue);
        }
    }
    
    /*
     In the example of an Input, here is the code that could be used
     to get the actual value.
     Please not that igs_read*AsString require memory management.
    */
    if (iopType == IGS_INPUT_T){
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
    //NB: exactly the same could be done for outputs and parameters
    
    //flag unused parameters to avoid compilation warnings with some compilers
    //NB: in real-world cases, theses parameters would be used most of
    //the time.
    IGS_UNUSED(value);
    IGS_UNUSED(myData);
}

///////////////////////////////////////////////////////////////////////////////
// COMMAND LINE AND INTERPRETER OPTIONS
//
void print_usage(){
    printf("Usage example: firstFullAgent --verbose --port 5670 --name firstFullAgent\n");
    printf("\nthese parameters have default value (indicated here above):\n");
    printf("--definition : optional path to the definition file (default: %s)\n", DEFAULTDEFINITIONPATH);
    printf("--mapping : optional path to the mapping file (default: %s)\n", DEFAULTMAPPINGPATH);
    printf("--verbose : enable verbose mode in the application (default is disabled)\n");
    printf("--port port_number : port used for autodiscovery between agents (default: %d)\n", port);
    printf("--device device_name : name of the network device to be used (useful if several devices available)\n");
    printf("--name agent_name : published name of this agent (default: %s)\n", agentName);
    printf("--noninteractiveloop : non-interactive loop for use as a deamon (default is false)\n");
}

//helper to convert paths starting with ~ to absolute paths
void makeFilePath(char *from, char *to, size_t size_of_to){
    if (from[0] == '~'){
        from++;
        char *home = getenv("HOME");
        strncpy(to, home, size_of_to);
        strncat(to, from, size_of_to);
    }else{
        strncpy(to, from, size_of_to);
    }
}

///////////////////////////////////////////////////////////////////////////////
// MAIN & OPTIONS & COMMAND INTERPRETER
//
//
int main(int argc, const char * argv[]) {
    
    //register to SIGINT (optional)
    signal(SIGINT, interruptionReceived);
    
    //manage options
    int opt= 0;
    bool noninteractiveloop = false; //by default, we use interacive loop
    
    char definitionPath[BUFFER_SIZE];
    strncpy(definitionPath, DEFAULTDEFINITIONPATH, BUFFER_SIZE*sizeof(char));
    char mappingPath[BUFFER_SIZE];
    strncpy(mappingPath, DEFAULTMAPPINGPATH, BUFFER_SIZE*sizeof(char));
    
    static struct option long_options[] = {
        {"verbose",     no_argument, 0,  'v' },
        {"noninteractiveloop",     no_argument, 0,  'l' },
        {"definition",  required_argument, 0,  'f' },
        {"mapping",  required_argument, 0,  'm' },
        {"device",      required_argument, 0,  'd' },
        {"port",        required_argument, 0,  'p' },
        {"name",        required_argument, 0,  'n' },
        {"help",        no_argument, 0,  'h' },
        {0, 0, 0, 0}
    };
    
    int long_index =0;
    while ((opt = getopt_long(argc, (char *const *)argv,"",long_options, &long_index )) != -1) {
        switch (opt) {
            case 'v' :
                verbose = true;
                break;
            case 'l' :
                noninteractiveloop = true;
                break;
            case 'f' :
                strncpy(definitionPath, optarg, BUFFER_SIZE);
                break;
            case 'm' :
                strncpy(mappingPath, optarg, BUFFER_SIZE);
                break;
            case 'p' :
                port = atoi(optarg);
                break;
            case 'd' :
                networkDevice = optarg;
                break;
            case 'n' :
                agentName = optarg;
                break;
            case 'h' :
                print_usage();
                exit (0);
                break;
            default:
                print_usage();
                exit(1);
        }
    }
    
    //transform definition and mapping path to absolute if necessary
    makeFilePath(definitionPath, definitionFile, BUFFER_SIZE*sizeof(char));
    makeFilePath(mappingPath, mappingFile, BUFFER_SIZE*sizeof(char));
    
    igs_setAgentName(agentName);
    igs_setCommandLineFromArgs(argc, argv);
    
    //logs configuration (optional)
    //NB: file log and stream log are enabled optionnaly
    igs_setLogLevel(IGS_LOG_TRACE); //set log level to TRACE (default is INFO)
    igs_setVerbose(verbose);
    igs_setUseColorVerbose(verbose);
    igs_setLogInFile(verbose);
    igs_setLogStream(verbose);
    igs_setLogPath("./log.csv");
    
    //here is the place to intialize the agent before actually starting it
    //ex: create IOPs, add callbacks, intitiate your agent specific code, etc.
    
    
    //NB: loading external definition and mapping is optional and is a commodity if
    //you want to avoid harcoding them.
    //load definition (optional )
    //igs_loadDefinitionFromPath(definitionFile);
    //print definition if needed
    //    char *definition = igs_getDefinition();
    //    printf("%s\n", definition);
    //    free(definition);
    
    //load mapping (optional)
    //igs_loadMappingFromPath(mappingFile);
    //print mapping if needed
    //    char *mapping = igs_getMapping();
    //    printf("%s\n", mapping);
    //    free(mapping);
    
    igs_startWithDevice(networkDevice, port);
    
    
    //show logging examples
    char *example = "example log message";
    igs_trace("this is a trace %s", example);
    igs_debug("this is a debug %s", example);
    igs_info("this is an info %s", example);
    igs_warn("this is a warning %s", example);
    igs_error("this is an error %s", example);
    igs_fatal("this is a fatale %s", example);
    
    //explore and print definition (definitely optional)
    long numberOfEntries;
    char **myEntries = igs_getInputsList(&numberOfEntries);
    printf("Inputs :\n");
    for (int i = 0; i< numberOfEntries; i++){
        printf("\t%s\n", myEntries[i]);
        igs_observeInput(myEntries[i], myIOPCallback, NULL);
    }
    igs_freeIOPList(&myEntries, numberOfEntries);
    myEntries = igs_getOutputsList(&numberOfEntries);
    printf("Outputs :\n");
    for (int i = 0; i< numberOfEntries; i++){
        printf("\t%s\n", myEntries[i]);
        igs_observeOutput(myEntries[i], myIOPCallback, NULL);
    }
    igs_freeIOPList(&myEntries, numberOfEntries);
    myEntries = igs_getParametersList(&numberOfEntries);
    printf("Parameters :\n");
    for (int i = 0; i< numberOfEntries; i++){
        printf("\t%s\n", myEntries[i]);
        igs_observeParameter(myEntries[i], myIOPCallback, NULL);
    }
    igs_freeIOPList(&myEntries, numberOfEntries);
   
    
    //mainloop management (two modes: interactive or not)
    if (noninteractiveloop){
        //Run the main loop (non-interactive mode):
        //we rely on CZMQ which is an ingeScape dependency and is thus
        //always here.
        zloop_t *loop = zloop_new();
        zloop_start(loop); //this function is blocking until SIGINT is received
        zloop_destroy(&loop);
    }else{
        while (!igs_Interrupted) {
            char message [1024];
            if (!fgets (message, 1024, stdin))
                break;
            if ((message[0] == '/')&&(strlen (message) > 1)) {
                //we have something to parse!
                matches_t my_matches;
                clean_matches(&my_matches); //initializes the matches_t structure
                regex_t r;
                
                const char *reg1 = "/([[:alnum:]]+)"; //single command
                const char *reg2 = "/([[:alnum:]]+)[[:space:]]{1}([^ ]+)"; //command + parameter (single word)
                const char *reg3 = "/([[:alnum:]]+)[[:space:]]{1}([^ ]+)[[:space:]]{1}([[:print:]]+)"; //command + parameter + string
                
                //command + parameter + string
                compile_regex(&r, reg3);
                match_regex(&r, message, &my_matches);
                regfree (&r);
                if (my_matches.nb > 0){
                    char *command = my_matches.results[0];
                    char *parameter = my_matches.results[1];
                    char *message = my_matches.results[2];
                    printf("command: %s\nparameter: %s\nmessage: %s\n",command,parameter,message);
                    clean_matches(&my_matches);
                    continue;
                }
                //command + parameter (single word)
                compile_regex(&r, reg2);
                match_regex(&r, message, &my_matches);
                regfree (&r);
                if (my_matches.nb > 0){
                    char *command = my_matches.results[0];
                    char *parameter = my_matches.results[1];
                    printf("command: %s\nparameter: %s\n",command,parameter);
                    clean_matches(&my_matches);
                    continue;
                }
                //single command
                compile_regex(&r, reg1);
                match_regex(&r, message, &my_matches);
                regfree (&r);
                if (my_matches.nb > 0){
                    char *command = my_matches.results[0];
                    //printf("command: %s\n",command);
                    if (strcmp(command, "quit") == 0){
                        break; //simply stops the main loop
                    }
                    clean_matches(&my_matches);
                    continue;
                }
            }
        }
    }
    
    igs_stop();
    
    return 0;
}

