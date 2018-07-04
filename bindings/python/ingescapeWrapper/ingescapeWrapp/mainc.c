//
//  Created by vaugien on 12/02/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ingescape/ingescape.h>
#include "regex.h"

int port = 5679;
char *agentName = "ingescapeCAgent";
char *networkDevice = "en0";
bool verbose = false;

//SIGINT handling
static volatile bool interrupted = false;
void interruption(int val)
{
    interrupted = true;
}

typedef struct Coord
{
    int y;
    int x;
}Coord;

void observeint(iop_t iopType, const char* name, iopType_t valueType, void* value, long valueSize, void* myData)
{
    int r = igs_readInputAsInt(name);
    printf("%s received INT : %d\n", name, r);
}

void observedouble(iop_t iopType, const char* name, iopType_t valueType, void* value, long valueSize, void* myData)
{
    double r = igs_readInputAsDouble(name);
    printf("%s received DOUBLE : %f\n", name, r);
}

void observestring(iop_t iopType, const char* name, iopType_t valueType, void* value, long valueSize, void* myData)
{
    char * r = igs_readInputAsString(name);
    printf("%s received STRING : %s\n", name, r);
}

void observebool(iop_t iopType, const char* name, iopType_t valueType, void* value, long valueSize, void* myData)
{
    bool r = igs_readInputAsBool(name);
    printf("%s received BOOL : %d\n", name, r);
}

void observeimp(iop_t iopType, const char* name, iopType_t valueType, void* value, long valueSize, void* myData)
{
    printf("%s received IMP \n", name);
}

void observedata(iop_t iopType, const char* name, iopType_t valueType, void* value, long valueSize, void* myData)
{
    igs_readInputAsData("inputData", &myData, &valueSize );
    Coord point = *((struct Coord *)myData);
    printf("message received Point (%i, %i)\n", point.x, point.y);
}

///////////////////////////////////////////////////////////////////////////////
// COMMAND LINE AND INTERPRETER OPTIONS
//
void print_usage()
{
    printf("Usage example: pgimCondif --verbose --zyreport 5670 --zyrename pgimCondif\n");
    printf("\nthese parameters have default value (indicated here above):\n");
    printf("--verbose : enable verbose mode in the application (default: 0)\n");
    printf("--port port_number : port used for autodiscovery between peers on IngeScape (default: %d)\n", port);
    printf("--device device_name : name of the network device to be used (useful if several devices available)\n");
    printf("--name agent_name : published name of this agent on IngeScape (default: %s)\n", agentName);
    printf("--noninteractiveloop : non-interactive loop for use as a deamon (default: false)\n");
}

void makeFilePath(char *from, char *to, size_t size_of_to)
{
    if (from[0] == '~')
    {
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

int main(int argc, const char * argv[])
{
    signal(SIGINT, interruption);
    
    //manage options
    int opt= 0;
    bool noninteractiveloop = false;
    
    //Specifying the expected options
    //The two options l and b expect numbers as argument
    static struct option long_options[] =
    {
        {"verbose",     no_argument, 0,  'v' },
        {"device",      required_argument, 0,  'd' },
        {"port",        required_argument, 0,  'p' },
        {"name",        required_argument, 0,  'n' },
        {"help",        no_argument, 0,  'h' }
    };
    
    int long_index =0;
    while ((opt = getopt_long(argc, (char *const *)argv,"p",long_options, &long_index )) != -1)
    {
        switch (opt)
        {
            case 'v' :
                verbose = true;
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
    
    igs_setAgentName(agentName);
    igs_setDefinitionName("definitionForAgent");
    igs_setDefinitionVersion("0.0.1");
    
    igs_createInput("inputInt", IGS_IGS_INTEGER_T, 0, 0);
    igs_createInput("inputString", IGS_IGS_STRING_T, "", 0);
    igs_createInput("inputBol", IGS_IGS_BOOL_T, 0, 0);
    igs_createInput("inputDouble", IGS_IGS_DOUBLE_T, 0, 0);
    igs_createInput("inputImp", IGS_IGS_IMPULSION_T, 0, 0);
    igs_createInput("inputData", IGS_DATA_T, 0, 0);
    
    igs_addMappingEntry("inputInt", "sendAgent", "outputInt");
    igs_addMappingEntry("inputString", "sendAgent", "outputString");
    igs_addMappingEntry("inputBol", "sendAgent", "outputBol");
    igs_addMappingEntry("inputDouble", "sendAgent", "outputDouble");
    igs_addMappingEntry("inputImp", "sendAgent", "outputImp");
    igs_addMappingEntry("inputData", "sendAgent", "outputData");
    
    igs_observeInput("inputInt", observeint, NULL);
    igs_observeInput("inputData", observedata, NULL);
    igs_observeInput("inputDouble", observedouble, NULL);
    igs_observeInput("inputString", observestring, NULL);
    igs_observeInput("inputImp", observeimp, NULL);
    igs_observeInput("inputBol", observebool, NULL);
    
    igs_setVerbose(true);

    igs_startWithDevice("en0", 5679);

    
    if (noninteractiveloop)
    {
        while (1)
        {
            if (interrupted)
            {
                printf("Interruption signal received : stopping.\n");
                break;
            }
            sleep(2);
        }
    }else{
        while (!interrupted)
        {
            char message [1024];
            if (!fgets (message, 1024, stdin))
                break;
            if ((message[0] == '/')&&(strlen (message) > 1))
            {
                matches my_matches;
                clean_matches(&my_matches);
                regex_t r;
                
                const char *reg1 = "/([[:alnum:]]+)";
                compile_regex(&r, reg1);
                match_regex(&r, message, &my_matches);
                regfree (&r);
                if (my_matches.nb > 0)
                {
                    char *command = my_matches.results[0];
                    if (strcmp(command, "quit") == 0)
                    {
                        igs_stop();
                        break;
                    }
                    clean_matches(&my_matches);
                    continue;
                }
            }
        }
    }
    return 0;
}

