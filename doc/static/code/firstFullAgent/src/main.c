//
//  main.c
//  firstFullAgent
//
//  Created by Stephane Vales on 09/05/2018.
//  Copyright © 2018 IngeScape. All rights reserved.
//

#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	#define NOMINMAX
	#include <windows.h>
	#include <winsock2.h>
#endif

#include <getopt.h> //command line options at statrtup
#include <stdlib.h> //standard C functions such as getenv, atoi, exit, etc.
#include <string.h> //C string handling functions
#include <signal.h> //catching interruptions
#include <czmq.h>
#include <ingescape/ingescape.h>

//default agent parameters to be overriden by command line paramters
int port = 5670;
char *agentName = "firstFullAgent";
char *networkDevice = NULL;
bool verbose = false;

//definition and mapping as external resources
#define BUFFER_SIZE 1024
#define DEFAULTDEFINITIONPATH "~/Documents/IngeScape/examples/definition.json"
char definitionFile[BUFFER_SIZE];
#define DEFAULTMAPPINGPATH "~/Documents/IngeScape/examples/mapping.json"
char mappingFile[BUFFER_SIZE];

//SIGINT handling
void interruptionReceived(int val) {
	//add code here if needed
}

/*
 This callback is able to display the new value of any IOP it observes.
 In order to use the igs_read* functions, the IOP type must be checked
 first and then the value type.
 */
void myIOPCallback(iop_t iopType, const char* name, iopType_t valueType,
	void* value, size_t valueSize, void* myData) {

	if (valueType == IGS_IMPULSION_T) {
		printf("%s changed (impulsion)\n", name);
	}
	else {
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
	}

	/*
	 In the example of an Input, here is the code that could be used
	 to get the actual value.
	 Please not that igs_read*AsString require memory management.
	*/
	if (iopType == IGS_INPUT_T) {
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
}

///////////////////////////////////////////////////////////////////////////////
// COMMAND LINE AND INTERPRETER OPTIONS
//
void print_usage() {
	printf("Usage example: firstFullAgent --verbose --port %d --device device_name --name firstFullAgent\n", port);
	printf("\nthese parameters have default value (indicated here above):\n");
	printf("--definition : path to the definition file (default: %s)\n", DEFAULTDEFINITIONPATH);
	printf("--mapping : path to the mapping file (default: %s)\n", DEFAULTMAPPINGPATH);
	printf("--verbose : enable verbose mode in the application (default is disabled)\n");
	printf("--port port_number : port used for autodiscovery between agents (default: %d)\n", port);
	printf("--device device_name : name of the network device to be used (useful if several devices available)\n");
	printf("--name agent_name : published name for this agent (default: %s)\n", agentName);
	printf("--interactiveloop : enables interactive loop to pass commands in CLI (default: false)\n");
}

//helper to convert paths starting with ~ to absolute paths
void makeFilePath(char *from, char *to, size_t size_of_to) {
	if (from[0] == '~') {
		from++;
#ifdef _WIN32
        char *home = getenv("HOMEPATH");
#else
        char *home = getenv("HOME");
#endif
        if (home == NULL){
            igs_error("could not find path for home directory");
        }else{
            strncpy(to, home, size_of_to);
            strncat(to, from, size_of_to);
        }
	}
	else {
		strncpy(to, from, size_of_to);
	}
}

///////////////////////////////////////////////////////////////////////////////
// MAIN & OPTIONS & COMMAND INTERPRETER
//
//
int main(int argc, const char * argv[]) {

	//register to SIGINT
	signal(SIGINT, interruptionReceived);

	//manage options
	int opt = 0;
	bool interactiveloop = false;

	char definitionPath[BUFFER_SIZE];
	strncpy(definitionPath, DEFAULTDEFINITIONPATH, BUFFER_SIZE * sizeof(char));
	char mappingPath[BUFFER_SIZE];
	strncpy(mappingPath, DEFAULTMAPPINGPATH, BUFFER_SIZE * sizeof(char));

	static struct option long_options[] = {
		{"verbose",     no_argument, 0,  'v' },
		{"interactiveloop",     no_argument, 0,  'l' },
		{"definition",  required_argument, 0,  'f' },
		{"mapping",  required_argument, 0,  'm' },
		{"device",      required_argument, 0,  'd' },
		{"port",        required_argument, 0,  'p' },
		{"name",        required_argument, 0,  'n' },
		{"help",        no_argument, 0,  'h' }
	};

	int long_index = 0;
	while ((opt = getopt_long(argc, (char *const *)argv, "p", long_options, &long_index)) != -1) {
		switch (opt) {
		case 'v':
			verbose = true;
			break;
		case 'l':
			interactiveloop = true;
			break;
		case 'f':
			strncpy(definitionPath, optarg, BUFFER_SIZE);
			break;
		case 'm':
			strncpy(mappingPath, optarg, BUFFER_SIZE);
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 'd':
			networkDevice = optarg;
			break;
		case 'n':
			agentName = optarg;
			break;
		case 'h':
			print_usage();
			exit(0);
			break;
		default:
			print_usage();
			exit(1);
		}
	}

	makeFilePath(definitionPath, definitionFile, BUFFER_SIZE * sizeof(char));
	makeFilePath(mappingPath, mappingFile, BUFFER_SIZE * sizeof(char));

    if (networkDevice == NULL){
        //we have no device to start with: try to find one
        char **devices = NULL;
        int nb = 0;
        igs_getNetdevicesList(&devices, &nb);
        if (nb == 1){
            //we have exactly one compliant network device available: we use it
            networkDevice = strdup(devices[0]);
            igs_info("using %s as default network device (this is the only one available)", networkDevice);
        }else{
            igs_error("No network device passed as command line parameter and several are available.");
            printf("Please use one of these network devices:\n");
            for (int i = 0; i < nb; i++){
                printf("\t%s\n", devices[i]);
            }
            printf("\n");
            print_usage();
            exit(1);
        }
        igs_freeNetdevicesList(devices, nb);
    }
    
	igs_setLogLevel(IGS_LOG_TRACE);
	igs_setVerbose(verbose);
	igs_setUseColorVerbose(verbose);
	igs_setLogInFile(verbose);
	igs_setLogStream(verbose);
	igs_setLogPath("./log.csv");

	char *example = "example log message";
	igs_trace("this is a trace %s", example);
	igs_debug("this is a debug %s", example);
	igs_info("this is an info %s", example);
	igs_warn("this is a warning %s", example);
	igs_error("this is an error %s", example);
	igs_fatal("this is a fatale %s", example);

	igs_setAgentName(agentName);

	//load definition
	igs_loadDefinitionFromPath(definitionFile);
	//    char *definition = igs_getDefinition();
	//    printf("%s\n", definition);
	//    free(definition);

	//load mapping
	igs_loadMappingFromPath(mappingFile);
	//    char *mapping = igs_getMapping();
	//    printf("%s\n", mapping);
	//    free(mapping);

	//explore and print definition
	long numberOfEntries;
	char **myEntries = igs_getInputsList(&numberOfEntries);
	printf("Inputs :\n");
	for (int i = 0; i < numberOfEntries; i++) {
		printf("\t%s\n", myEntries[i]);
		igs_observeInput(myEntries[i], myIOPCallback, NULL);
	}
	igs_freeIOPList(&myEntries, numberOfEntries);
	myEntries = igs_getOutputsList(&numberOfEntries);
	printf("Outputs :\n");
	for (int i = 0; i < numberOfEntries; i++) {
		printf("\t%s\n", myEntries[i]);
		igs_observeOutput(myEntries[i], myIOPCallback, NULL);
	}
	igs_freeIOPList(&myEntries, numberOfEntries);
	myEntries = igs_getParametersList(&numberOfEntries);
	printf("Parameters :\n");
	for (int i = 0; i < numberOfEntries; i++) {
		printf("\t%s\n", myEntries[i]);
		igs_observeParameter(myEntries[i], myIOPCallback, NULL);
	}
	igs_freeIOPList(&myEntries, numberOfEntries);

	igs_startWithDevice(networkDevice, port);

	//mainloop management (two modes)
	if (!interactiveloop) {
		//Run the main loop (non-interactive mode):
		//we rely on CZMQ which is an ingeScape dependency and is thus
		//always here.
		zloop_t *loop = zloop_new();
		zloop_start(loop); //this function is blocking until SIGINT is received
		zloop_destroy(&loop);
	}
	else {
		char message[BUFFER_SIZE];
		char command[BUFFER_SIZE];
		char param1[BUFFER_SIZE];
		char param2[BUFFER_SIZE];
		int usedChar = 0;

		while (!igs_Interrupted) {
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
					if (strncmp(command, "quit", BUFFER_SIZE) == 0) {
						break;
					}
					else {
						printf("Received command: %s\n", command);
					}
				}else if (matches == 2) {
					printf("Received command: %s + %s\n", command, param1);
				}else if (matches == 3) {
					printf("Received command: %s + %s + %s\n", command, param1, param2);
				}else{
					printf("Error: message returned %d matches (%s)\n", matches, message);
				}
			}
		}
	}

	igs_stop();

	return 0;
}

