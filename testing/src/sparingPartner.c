//
//  sparingPartner.c
//  simpleDemoAgent
//
//  Created by Stephane Vales on 15/07/2020.
//  Copyright © 2020 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <getopt.h> //command line options at statrtup
#include <stdlib.h> //standard C functions such as getenv, atoi, exit, etc.
#include <string.h> //C string handling functions
#include <signal.h> //catching interruptions
#include <czmq.h>
#include <ingescape/ingescape_advanced.h>
#include "common.h"


unsigned int port = 5669;
char *agentName = "sparingPartner";
char *networkDevice = NULL; //can be set to a default device name
bool verbose = false;
bool autoTests = false;

zsock_t *mainThreadPipe = NULL;
zsock_t *toMainThreadPipe = NULL;

//reference values
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

int runAutoTests(zloop_t *loop, int timer_id, void *arg){
    igs_info("starting autotests");
    zclock_sleep(1500);
    igs_busSendStringToAgent("puncher", "starting autotests");
    zclock_sleep(100);
    publishCommandSparing();
    zclock_sleep(250);
    callCommandSparing();
    zclock_sleep(250);
    busCommandSparing();
    zclock_sleep(250);
    igs_busSendStringToAgent("puncher", "STOP_PEER");
    return -1;
}

void agentEvents(igs_agent_event_t event, const char *uuid, const char *name, void *myData){
    igs_info("%s - %d", name, event);
    if (streq(name, "puncher") && event == IGS_AGENT_KNOWS_US){
        igs_info("starting auto tests with %s", name);
        zsock_signal(toMainThreadPipe, 0);
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
                print_usage();
                exit(0);
            default:
                print_usage();
                exit(1);
        }
    }

    igs_setAgentName(agentName);
    igs_setLicensePath("~/Documents/IngeScape/licenses/");
    
    igs_setVerbose(verbose);
    igs_setLogInFile(true);
    igs_setLogLevel(IGS_LOG_TRACE);
    igs_setDefinitionName("sparingPartner");
    igs_setDefinitionDescription("One example for each type of IOP and call");
    igs_setDefinitionVersion("1.0");
    igs_createInput("sparing_impulsion", IGS_IMPULSION_T, NULL, 0);
    igs_createInput("sparing_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igs_createInput("sparing_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igs_createInput("sparing_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igs_createInput("sparing_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igs_createInput("sparing_data", IGS_DATA_T, myData, 32);
    igs_createOutput("sparing_impulsion", IGS_IMPULSION_T, NULL, 0);
    igs_createOutput("sparing_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igs_createOutput("sparing_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igs_createOutput("sparing_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igs_createOutput("sparing_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igs_createOutput("sparing_data", IGS_DATA_T, myData, 32);
    igs_createParameter("sparing_impulsion", IGS_IMPULSION_T, NULL, 0);
    igs_createParameter("sparing_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igs_createParameter("sparing_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igs_createParameter("sparing_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igs_createParameter("sparing_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igs_createParameter("sparing_data", IGS_DATA_T, myData, 32);
    igs_initCall("sparingCall", myCallCallback, NULL);
    igs_addArgumentToCall("sparingCall", "myBool", IGS_BOOL_T);
    igs_addArgumentToCall("sparingCall", "myInt", IGS_INTEGER_T);
    igs_addArgumentToCall("sparingCall", "myDouble", IGS_DOUBLE_T);
    igs_addArgumentToCall("sparingCall", "myString", IGS_STRING_T);
    igs_addArgumentToCall("sparingCall", "myData", IGS_DATA_T);
    
    igs_observeInput("sparing_impulsion", myIOPCallback, NULL);
    igs_observeInput("sparing_bool", myIOPCallback, NULL);
    igs_observeInput("sparing_int", myIOPCallback, NULL);
    igs_observeInput("sparing_double", myIOPCallback, NULL);
    igs_observeInput("sparing_string", myIOPCallback, NULL);
    igs_observeInput("sparing_data", myIOPCallback, NULL);
    
    igs_addMappingEntry("sparing_impulsion", "testAgent", "my_impulsion");
    igs_addMappingEntry("sparing_bool", "testAgent", "my_bool");
    igs_addMappingEntry("sparing_int", "testAgent", "my_int");
    igs_addMappingEntry("sparing_double", "testAgent", "my_double");
    igs_addMappingEntry("sparing_string", "testAgent", "my_string");
    igs_addMappingEntry("sparing_data", "testAgent", "my_data");
    
    igs_observeBus(myBusCallback, NULL);
 
    igs_startWithDevice(networkDevice, port);
    igs_busJoinChannel("TEST_CHANNEL");
    
    //mainloop management (two modes)
    if (!interactiveloop) {
        //Run the main loop (non-interactive mode):
        //we rely on CZMQ which is an ingeScape dependency and is thus
        //always here.
        zloop_t *loop = zloop_new();
        zsock_t *pipe = igs_getPipeToIngescape();
        zloop_reader(loop, pipe, ingescapeSentMessage, NULL);
        if (autoTests){
            igs_observeAgentEvents(agentEvents, NULL);
            char *endpoint = "inproc://sparingPartner";
            mainThreadPipe = zsock_new(ZMQ_PAIR);
            zsock_attach(mainThreadPipe, endpoint, true);
            toMainThreadPipe = zsock_new_pair(endpoint);
            igs_info("waiting for puncher");
            zsock_wait(mainThreadPipe); //wait for signal triggered in agentEvents
            zloop_timer(loop, 100, 1, runAutoTests, NULL);
        }
        zloop_start(loop); //this function is blocking until SIGINT is received
        zloop_destroy(&loop);
    }else{
        char message[BUFFER_SIZE];
        char command[BUFFER_SIZE];
        char param1[BUFFER_SIZE];
        char param2[BUFFER_SIZE];
        int usedChar = 0;
        print_cli_usage();
        while (igs_isStarted()) {
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
                        publishCommandSparing();
                        
                    }else if(streq(command, "call")){
                        callCommandSparing();
                        
                    }else if(streq(command, "bus")){
                        busCommandSparing();
                        
                    }else if(streq(command, "gossip")){
                        gossipCommandSparing();
                        
                    }else if(streq(command, "security")){
                        securityCommandSparing();
                        
                    }else if(streq(command, "toxic_waste")){
                        //busCommandToxicWaste();
                        
                    }else {
                        printf("unhandled command: %s\n", command);
                    }
                }else if (matches == 2) {
                    if (streq(command, "editor")){
                        editorCommand(param1, "my_int");
                        
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

    igs_stop();
    if (mainThreadPipe)
        zsock_destroy(&mainThreadPipe);
    if (toMainThreadPipe)
        zsock_destroy(&toMainThreadPipe);

    return EXIT_SUCCESS;
}
