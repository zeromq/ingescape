//
//  partner.c
//  simpleDemoAgent
//
//  Created by Stephane Vales on 15/07/2020.
//  Copyright © 2020 Ingenuity i/o. All rights reserved.
//
#include "common.h"

#include <stdio.h>
#include <getopt.h> //command line options at statrtup
#include <stdlib.h> //standard C functions such as getenv, atoi, exit, etc.
#include <string.h> //C string handling functions
#include <signal.h> //catching interruptions
#include <czmq.h>
#include <ingescape.h>


unsigned int port = 5670;
const char *p_agentName = "partner";
const char *p_networkDevice = "en0";
bool verbose = false;
bool autoTests = false;

zsock_t *mainThreadPipe = NULL;
zsock_t *toMainThreadPipe = NULL;

//reference values
bool myBool = true;
int myInt = 1;
double myDouble = 1.0;
char *myString = (char*)"my string";
void *myData = NULL;
void *myOtherData = NULL;

int ingescapeSentMessage(zloop_t *loop, zsock_t *reader, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(arg)
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
    IGS_UNUSED(loop)
    IGS_UNUSED(timer_id)
    IGS_UNUSED(arg)
    igs_info("starting autotests");
    zclock_sleep(1500);
    igs_channel_whisper_str("tester", "starting autotests");
    zclock_sleep(100);
    publishCommandSparing();
    zclock_sleep(250);
    servicesCommandSparing();
    zclock_sleep(250);
    channelsCommandSparing();
    zclock_sleep(250);
    igs_channel_whisper_str("tester", "STOP_PEER");
    igs_info("autotests completed");
    return -1;
}

void agentEvents(igs_agent_event_t event, const char *uuid, const char *name, void *eventData, void *myCbData){
    IGS_UNUSED(uuid)
    IGS_UNUSED(eventData)
    IGS_UNUSED(myCbData)
    igs_info("%s - %d", name, event);
    if (streq(name, "tester") && event == IGS_AGENT_KNOWS_US){
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
                p_networkDevice = optarg;
                break;
            case 'n':
                p_agentName = optarg;
                break;
            case 'a':
                autoTests = true;
                break;
            case 's':
                staticTests = true;
                break;
            case 'h':
                print_usage(p_agentName);
                exit(0);
            default:
                print_usage(p_agentName);
                exit(1);
        }
    }

    igs_agent_set_name(p_agentName);

    igs_log_set_console(verbose);
    igs_log_set_file(true, NULL);
    igs_log_set_console_level(IGS_LOG_TRACE);
    igs_definition_set_description("One example for each type of IOP and call");
    igs_definition_set_version("1.0");
    igs_input_create("sparing_impulsion", IGS_IMPULSION_T, NULL, 0);
    igs_input_create("sparing_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igs_input_create("sparing_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igs_input_create("sparing_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igs_input_create("sparing_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igs_input_create("sparing_data", IGS_DATA_T, myData, 32);
    igs_output_create("sparing_impulsion", IGS_IMPULSION_T, NULL, 0);
    igs_output_create("sparing_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igs_output_create("sparing_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igs_output_create("sparing_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igs_output_create("sparing_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igs_output_create("sparing_data", IGS_DATA_T, myData, 32);
    igs_parameter_create("sparing_impulsion", IGS_IMPULSION_T, NULL, 0);
    igs_parameter_create("sparing_bool", IGS_BOOL_T, &myBool, sizeof(bool));
    igs_parameter_create("sparing_int", IGS_INTEGER_T, &myInt, sizeof(int));
    igs_parameter_create("sparing_double", IGS_DOUBLE_T, &myDouble, sizeof(double));
    igs_parameter_create("sparing_string", IGS_STRING_T, myString, strlen(myString) + 1);
    igs_parameter_create("sparing_data", IGS_DATA_T, myData, 32);
    igs_service_init("sparingService", myServiceCallback, NULL);
    igs_service_arg_add("sparingService", "myBool", IGS_BOOL_T);
    igs_service_arg_add("sparingService", "myInt", IGS_INTEGER_T);
    igs_service_arg_add("sparingService", "myDouble", IGS_DOUBLE_T);
    igs_service_arg_add("sparingService", "myString", IGS_STRING_T);
    igs_service_arg_add("sparingService", "myData", IGS_DATA_T);

    igs_observe_input("sparing_impulsion", myIOPCallback, NULL);
    igs_observe_input("sparing_bool", myIOPCallback, NULL);
    igs_observe_input("sparing_int", myIOPCallback, NULL);
    igs_observe_input("sparing_double", myIOPCallback, NULL);
    igs_observe_input("sparing_string", myIOPCallback, NULL);
    igs_observe_input("sparing_data", myIOPCallback, NULL);

    igs_mapping_add("sparing_impulsion", "testAgent", "my_impulsion");
    igs_mapping_add("sparing_bool", "testAgent", "my_bool");
    igs_mapping_add("sparing_int", "testAgent", "my_int");
    igs_mapping_add("sparing_double", "testAgent", "my_double");
    igs_mapping_add("sparing_string", "testAgent", "my_string");
    igs_mapping_add("sparing_data", "testAgent", "my_data");

    igs_observe_channels(myChannelsCallback, NULL);
    
    if (staticTests)
        exit(EXIT_SUCCESS);

    igs_start_with_device(p_networkDevice, port);
    igs_channel_join("TEST_CHANNEL");

    //mainloop management (two modes)
    if (!interactiveloop) {
        //Run the main loop (non-interactive mode):
        //we rely on CZMQ which is an ingeScape dependency and is thus
        //always here.
        zloop_t *loop = zloop_new();
        zsock_t *pipe = igs_pipe_to_ingescape();
        zloop_reader(loop, pipe, ingescapeSentMessage, NULL);
        if (autoTests){
            igs_observe_agent_events(agentEvents, NULL);
            const char *endpoint = "inproc://partner";
            mainThreadPipe = zsock_new(ZMQ_PAIR);
            zsock_attach(mainThreadPipe, endpoint, true);
            toMainThreadPipe = zsock_new_pair(endpoint);
            igs_info("waiting for tester");
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
        while (igs_is_started()) {
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

                    }else if(streq(command, "services")){
                        servicesCommandSparing();

                    }else if(streq(command, "channels")){
                        channelsCommandSparing();

                    }else if(streq(command, "toxic_waste")){
                        //busCommandToxicWaste();

                    }else {
                        printf("unhandled command: %s\n", command);
                    }
                }else if (matches == 2) {
                    if (streq(command, "editor")){
                        editorCommand(param1, "my_int");

                    }else if(streq(command, "broker")){
                        brokerCommandSparing(param1);

                    }else if(streq(command, "security")){
                        securityCommandSparing(param1);

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
    
    if (mainThreadPipe)
        zsock_destroy(&mainThreadPipe);
    if (toMainThreadPipe)
        zsock_destroy(&toMainThreadPipe);
    igs_stop();

    return EXIT_SUCCESS;
}
