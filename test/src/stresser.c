//
//  stresser.c
//  testing
//
//  Created by Stephane Vales on 11/08/2024.
//  Copyright © 2024 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <getopt.h> //command line options at statrtup
#include <stdlib.h> //standard C functions such as getenv, atoi, exit, etc.
#include <string.h> //C string handling functions
#include <signal.h> //catching interruptions
#include <czmq.h>
#include <igsagent.h>

/*
 This is a test program dedicated to stressing Ingescape platforms.
 This program runs a user-specifed number of agents, with each one of them:
 - activating and deactivating,
 - changing state (NB: mute and freeze observers are not implemented yet for additional agents),
 - changing definition and mappings,
 - publishing and receiving data (ios and services),
 - trigerring/leaving elections.
 */

#define STRESS_ACTIVATE_DEACTIVATE_PERIOD 2500 //min tried: 150
#define STRESS_CHANGE_STATE_PERIOD 200
#define STRESS_CHANGE_DEFINITION_PERIOD 1000 //NB: s_trigger_definition_update is called every second
#define STRESS_CHANGE_MAPPING_PERIOD 1000 //NB: s_trigger_mapping_update is called every second
#define STRESS_PUBLISH_PERIOD 50 //min tried: 5, must be a minimum of 2ms (see stress_publish) * nb of agents not to saturate the thread's zloop
#define STRESS_ELECTIONS_PERIOD 500

#define PORT 5669
bool verbose = false;
char *agent_name = (char*)"stresser";
bool deactivate = false;
bool change_state = false;
bool change_definition = false;
bool change_mapping = false;
bool publish = false;
bool elections = false;
size_t nb_of_agents = 0;

zlist_t *agents = NULL;
const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

zactor_t *a_stress_activate_deactivate = NULL;
zactor_t *a_stress_change_state = NULL;
zactor_t *a_stress_change_definition = NULL;
zactor_t *a_stress_change_mapping = NULL;
zactor_t *a_stress_publish = NULL;
zactor_t *a_stress_elections = NULL;

int ingescapeSentMessage(zloop_t *loop, zsock_t *reader, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(arg)
    char *message = NULL;
    zsock_recv(reader, "s", &message);
    if (streq(message, "LOOP_STOPPED")){
        igs_info("LOOP_STOPPED received in main app");
        if (a_stress_activate_deactivate)
            zactor_destroy(&a_stress_activate_deactivate);
        if (a_stress_change_state)
            zactor_destroy(&a_stress_change_state);
        if (a_stress_change_definition)
            zactor_destroy(&a_stress_change_definition);
        if (a_stress_change_mapping)
            zactor_destroy(&a_stress_change_mapping);
        if (a_stress_publish)
            zactor_destroy(&a_stress_publish);
        if (a_stress_elections)
            zactor_destroy(&a_stress_elections);
        zclock_sleep(250);
        return -1;
    }else
        return 0;
}

int s_extract_number(const char *string) {
    assert(string);
    size_t length = strlen(string);
    assert(length);
    size_t i = length - 1;
    while (isdigit(string[i]) && string[i] != '-')
        i--;
    char result[32] = "";
    strncpy(result, &string[i+1], 31);
    result[length - i] = '\0';
    int res = atoi(result);
    return res;
}

int s_read_pipe (zloop_t *loop, zsock_t *reader, void *arg){
    IGS_UNUSED(loop)
    assert(arg);
    char *thread_name = (char*)arg;
    char *message = NULL;
    zsock_recv(reader, "s", &message);
    if (message && streq(message, "$TERM")){
        igs_info("thread %s stopping...", thread_name);
        free(message);
        return -1;
    }else
        return 0;
}

///////////////////////////////////////////////////////////////////////////////
// STRESS FUNCTIONS
//
int stress_activate_deactivate (zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(timer_id)
    IGS_UNUSED(arg)
    zlist_t *dup = zlist_dup(agents);
    igsagent_t *a = zlist_first(dup);
    while (a) {
        srand((uint)zclock_usecs());
        int choice = rand()%2;
        if (choice && !igsagent_is_activated(a))
            igsagent_activate(a);
        else if (choice && igsagent_is_activated(a))
            igsagent_deactivate(a);
        a = zlist_next(dup);
    }
    zlist_destroy(&dup);
    return 0;
}

void stress_activate_deactivate_fn (zsock_t *pipe, void *args){
    IGS_UNUSED(args)
    zsock_signal(pipe, 0);
    zloop_t *loop = zloop_new();
    zloop_timer(loop, STRESS_ACTIVATE_DEACTIVATE_PERIOD, 0, stress_activate_deactivate, NULL);
    zloop_reader(loop, pipe, s_read_pipe, (char*)"stress_activate_deactivate");
    zloop_start(loop);
    zloop_destroy(&loop);
    igs_info("thread %s stopped", "stress_activate_deactivate");
}

int stress_change_state (zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(timer_id)
    IGS_UNUSED(arg)
    zlist_t *dup = zlist_dup(agents);
    igsagent_t *a = zlist_first(dup);
    char state_out[32] = "";
    while (a) {
        char *state = igsagent_state(a);
        int state_nb = atoi(state);
        free(state);
        sprintf(state_out, "%d", ++state_nb);
        igsagent_set_state(a, state_out);
        a = zlist_next(dup);
    }
    zlist_destroy(&dup);
    return 0;
}

void stress_change_state_fn (zsock_t *pipe, void *args){
    IGS_UNUSED(args)
    zsock_signal(pipe, 0);
    zloop_t *loop = zloop_new();
    zloop_timer(loop, STRESS_CHANGE_STATE_PERIOD, 0, stress_change_state, NULL);
    zloop_reader(loop, pipe, s_read_pipe, (char*)"stress_change_state");
    zloop_start(loop);
    zloop_destroy(&loop);
    igs_info("thread %s stopped", "stress_change_state");
}

int stress_change_definition (zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(timer_id)
    IGS_UNUSED(arg)
    zlist_t *dup = zlist_dup(agents);
    igsagent_t *a = zlist_first(dup);
    while (a) {
        srand((uint)zclock_usecs());
        bool choice = rand() % 2;
        if (choice && !igsagent_input_exists(a, "new_input"))
            igsagent_input_create(a, "new_input", IGS_DATA_T, NULL, 0);
        else if (choice && igsagent_input_exists(a, "new_input"))
            igsagent_input_remove(a, "new_input");
        a = zlist_next(dup);
    }
    zlist_destroy(&dup);
    return 0;
}

void stress_change_definition_fn (zsock_t *pipe, void *args){
    IGS_UNUSED(args)
    zsock_signal(pipe, 0);
    zloop_t *loop = zloop_new();
    zloop_timer(loop, STRESS_CHANGE_DEFINITION_PERIOD, 0, stress_change_definition, NULL);
    zloop_reader(loop, pipe, s_read_pipe, (char*)"stress_change_definition");
    zloop_start(loop);
    zloop_destroy(&loop);
    igs_info("thread %s stopped", "stress_change_definition");
}

int stress_change_mapping (zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(timer_id)
    IGS_UNUSED(arg)
    zlist_t *dup = zlist_dup(agents);
    igsagent_t *a = zlist_first(dup);
    while (a) {
        char *name = igsagent_name(a);
        int index = s_extract_number(name);
        if (index == 0)
            index = (uint)nb_of_agents - 1;
        else
            index--;
        char target[IGS_MAX_AGENT_NAME_LENGTH] = "";
        sprintf(target, "%s-%d", agent_name, index);
        //printf("MAP: %s->%s\n", name, target);
        free(name);
        srand((uint)zclock_usecs());
        bool choice = rand() % 2;
        if (choice){
            igsagent_mapping_add(a, "impulsion", target, "impulsion");
            igsagent_mapping_add(a, "int", target, "int");
            igsagent_mapping_add(a, "double", target, "double");
            igsagent_mapping_add(a, "bool", target, "bool");
            igsagent_mapping_add(a, "string", target, "string");
            igsagent_mapping_add(a, "data", target, "data");
        }else{
            igsagent_mapping_remove_with_name(a, "impulsion", target, "impulsion");
            igsagent_mapping_remove_with_name(a, "int", target, "int");
            igsagent_mapping_remove_with_name(a, "double", target, "double");
            igsagent_mapping_remove_with_name(a, "bool", target, "bool");
            igsagent_mapping_remove_with_name(a, "string", target, "string");
            igsagent_mapping_remove_with_name(a, "data", target, "data");
        }
        a = zlist_next(dup);
    }
    zlist_destroy(&dup);
    return 0;
}

void stress_change_mapping_fn (zsock_t *pipe, void *args){
    IGS_UNUSED(args)
    zsock_signal(pipe, 0);
    zloop_t *loop = zloop_new();
    zloop_timer(loop, STRESS_CHANGE_MAPPING_PERIOD, 0, stress_change_mapping, NULL);
    zloop_reader(loop, pipe, s_read_pipe, (char*)"stress_change_mapping");
    zloop_start(loop);
    zloop_destroy(&loop);
    igs_info("thread %s stopped", "stress_change_mapping");
}

int stress_publish (zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(timer_id)
    IGS_UNUSED(arg)
    zlist_t *dup = zlist_dup(agents);
    igsagent_t *a = zlist_first(dup);
    while (a) {
        srand((uint)zclock_usecs());
        
        igsagent_output_set_impulsion(a, "impulsion");
        bool b = igsagent_input_bool(a, "bool");
        igsagent_output_set_bool(a, "bool", !b);
        int i = igsagent_input_int(a, "int");
        igsagent_output_set_int(a, "int", ++i);
        double d = igsagent_input_double(a, "double");
        igsagent_output_set_double(a, "double", ++d);
        char s[17] = "";
        size_t charset_size = sizeof(charset) - 1;
        for (size_t si = 0; si < 16; si++) {
            int k = rand() % charset_size;
            s[si] = charset[k];
        }
        s[16] = '\0';
        igsagent_output_set_string(a, "string", s);
        igsagent_output_set_data(a, "data", s, 17);
        
        char *name = igsagent_name(a);
        size_t index = s_extract_number(name);
        free(name);
        if (index == 0)
            index = nb_of_agents - 1;
        else
            index--;
        char target[IGS_MAX_AGENT_NAME_LENGTH] = "";
        sprintf(target, "%s-%zu", agent_name, index);
        igs_service_arg_t *args = NULL;
        igs_service_args_add_bool(&args, b);
        igs_service_args_add_int(&args, i);
        igs_service_args_add_double(&args, d);
        igs_service_args_add_string(&args, s);
        igs_service_args_add_data(&args, s, 17);
        igsagent_service_call(a, target, "service", &args, s);
        
        a = zlist_next(dup);
    }
    zlist_destroy(&dup);
    return 0;
}

void stress_publish_fn (zsock_t *pipe, void *args){
    IGS_UNUSED(args)
    zsock_signal(pipe, 0);
    zloop_t *loop = zloop_new();
    zloop_timer(loop, STRESS_PUBLISH_PERIOD, 0, stress_publish, NULL);
    zloop_reader(loop, pipe, s_read_pipe, (char*)"stress_publish");
    zloop_start(loop);
    zloop_destroy(&loop);
    igs_info("thread %s stopped", "stress_publish");
}

int stress_elections (zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(timer_id)
    IGS_UNUSED(arg)
    zlist_t *dup = zlist_dup(agents);
    igsagent_t *a = zlist_first(dup);
    while (a) {
        srand((uint)zclock_usecs());
        bool choice = rand() % 2;
        if (choice)
            igsagent_election_join(a, "stress_elections");
        else
            igsagent_election_leave(a, "stress_elections");
        a = zlist_next(dup);
    }
    zlist_destroy(&dup);
    return 0;
}

void stress_elections_fn (zsock_t *pipe, void *args){
    IGS_UNUSED(args)
    zsock_signal(pipe, 0);
    zloop_t *loop = zloop_new();
    zloop_timer(loop, STRESS_ELECTIONS_PERIOD, 0, stress_elections, NULL);
    zloop_reader(loop, pipe, s_read_pipe, (char*)"stress_elections");
    zloop_start(loop);
    zloop_destroy(&loop);
    igs_info("thread %s stopped", "stress_elections");
}


///////////////////////////////////////////////////////////////////////////////
// AGENT CALLBACKS
//
void observe_agent_events (igsagent_t *agent,
                           igs_agent_event_t event,
                           const char *uuid,
                           const char *name,
                           void *event_data,
                           void *data){
    IGS_UNUSED(event_data)
    IGS_UNUSED(data)
    const char *event_name = NULL;
    switch (event) {
        case IGS_PEER_ENTERED:
            event_name = "IGS_PEER_ENTERED";
            break;
        case IGS_PEER_EXITED:
            event_name = "IGS_PEER_EXITED";
            break;
        case IGS_AGENT_ENTERED:
            event_name = "IGS_AGENT_ENTERED";
            break;
        case IGS_AGENT_UPDATED_DEFINITION:
            event_name = "IGS_AGENT_UPDATED_DEFINITION";
            break;
        case IGS_AGENT_KNOWS_US:
            event_name = "IGS_AGENT_KNOWS_US";
            break;
        case IGS_AGENT_EXITED:
            event_name = "IGS_AGENT_EXITED";
            break;
        case IGS_AGENT_UPDATED_MAPPING:
            event_name = "IGS_AGENT_UPDATED_MAPPING";
            break;
        case IGS_AGENT_WON_ELECTION:
            event_name = "IGS_AGENT_WON_ELECTION";
            break;
        case IGS_AGENT_LOST_ELECTION:
            event_name = "IGS_AGENT_LOST_ELECTION";
            break;
            
        default:
            break;
    }
    if (verbose)
        igsagent_fatal(agent, "received event %s from %s (%s)", event_name, name, uuid);
}

void observe_input (igsagent_t *agent,
                    igs_io_type_t type,
                    const char *name,
                    igs_io_value_type_t value_type,
                    void *value,
                    size_t value_size,
                    void *data){
    IGS_UNUSED(type)
    IGS_UNUSED(data)
    if (verbose){
        switch (value_type) {
            case IGS_IMPULSION_T:
                igsagent_fatal(agent, "%s received impulsion", name);
                break;
            case IGS_INTEGER_T:
                igsagent_fatal(agent, "%s received %d", name, *(int*)value);
                break;
            case IGS_DOUBLE_T:
                igsagent_fatal(agent, "%s received %f", name, *(double*)value);
                break;
            case IGS_BOOL_T:
                igsagent_fatal(agent, "%s received %d", name, *(bool*)value);
                break;
            case IGS_STRING_T:
                igsagent_fatal(agent, "%s received '%s'", name, (char*)value);
                break;
            case IGS_DATA_T:
                igsagent_fatal(agent, "%s received %zu bytes", name, value_size);
                break;
                
            default:
                break;
        }
    }
}

void service (igsagent_t *agent,
              const char *sender_agent_name,
              const char *sender_agent_uuid,
              const char *service_name,
              igs_service_arg_t *first_argument,
              size_t args_nbr,
              const char *token,
              void *data){
    IGS_UNUSED(data)
    if (verbose)
        igsagent_fatal(agent, "%s called by %s (%s)", service_name, sender_agent_name, sender_agent_uuid);
    assert(args_nbr == 5);
    igs_service_arg_t *args = igs_service_args_clone(first_argument);
    igsagent_service_call(agent, sender_agent_uuid, "reply", &args, token);
}

void reply (igsagent_t *agent,
            const char *sender_agent_name,
            const char *sender_agent_uuid,
            const char *service_name,
            igs_service_arg_t *first_argument,
            size_t args_nbr,
            const char *token,
            void *data){
    IGS_UNUSED(args_nbr)
    IGS_UNUSED(data)
    if (verbose)
        igsagent_fatal(agent, "%s called by %s (%s)", service_name, sender_agent_name, sender_agent_uuid);
    assert(streq(first_argument->next->next->next->c, token));
}

///////////////////////////////////////////////////////////////////////////////
// COMMAND LINE AND INTERPRETER OPTIONS
//
void print_usage(void){
    printf("Usage examples:\n");
    printf("    ./Files --verbose --device en0 --port 5670 --rootDir /exemple/of/path\n");
    printf("\nIngescape parameters:\n");
    printf("--verbose : enable verbose mode in the application (default is disabled)\n");
    printf("--device device_name : name of the network device to be used (useful if several devices are available)\n");
    printf("--port port_number : port used for autodiscovery between agents (default: %d)\n", PORT);
    printf("--name agent_name : published name of this agent (default: %s)\n", agent_name);
    printf("\n\nSpecific parameters:\n");
    printf("--agents nb_of_agents : number of agents instanciated for the stresser (default: %zu)\n", nb_of_agents);
    printf("--stress-peer : triggers a series of igs start/stop when starting the programm\n");
    printf("--deactivate : runs the activate/deactivate thread with a period of %d milliseconds\n", STRESS_ACTIVATE_DEACTIVATE_PERIOD);
    printf("--change_state : runs the change_state thread, publishing new states every %d milliseconds\n", STRESS_CHANGE_STATE_PERIOD);
    printf("--change_definition : runs the change_definition thread with a period of %d milliseconds\n", STRESS_CHANGE_DEFINITION_PERIOD);
    printf("--change_mapping : runs the change_mapping thread with a period of %d milliseconds\n", STRESS_CHANGE_MAPPING_PERIOD);
    printf("--publish : runs the publish thread (outpus and services) with a period of %d milliseconds\n", STRESS_PUBLISH_PERIOD);
    printf("--elections : runs the elections thread joining and leaving elections with a period of %d milliseconds\n", STRESS_ELECTIONS_PERIOD);
    printf("\n");
}

//resolve paths starting with ~ to absolute paths
void resolveUserPathIn(char path[], size_t maxSize) {
    if (path && strlen(path) && path[0] == '~') {
        char *temp = strdup(path+1);
#ifdef _WIN32
        char *home = getenv("USERPROFILE");
#else
        char *home = getenv("HOME");
#endif
        if (!home)
            igs_error("could not find path for home directory");
        else{
            strncpy(path, home, maxSize);
            strncat(path, temp, maxSize);
        }
        free(temp);
    }
}

///////////////////////////////////////////////////////////////////////////////
// MAIN & OPTIONS & COMMAND INTERPRETER
//
int main(int argc, const char * argv[]) {

    //manage options
    int opt = 0;
    char *networkDevice = NULL;
    unsigned int port = PORT;
    bool stress_peer = false;

    static struct option long_options[] = {
        {"verbose",     no_argument, 0,  'v' },
        {"device",      required_argument, 0,  'd' },
        {"port",        required_argument, 0,  'p' },
        {"name",        required_argument, 0,  'n' },
        {"agents",      required_argument, 0,  'a' },
        {"stress-peer", no_argument, 0,  's' },
        {"deactivate", no_argument, 0,  '1' },
        {"change_state", no_argument, 0,  '2' },
        {"change_definition", no_argument, 0,  '3' },
        {"change_mapping", no_argument, 0,  '4' },
        {"publish", no_argument, 0,  '5' },
        {"elections", no_argument, 0,  '6' },
        {"help",        no_argument, 0,  'h' },
        {0, 0, 0, 0}
    };

    int long_index = 0;
    while ((opt = getopt_long(argc, (char *const *)argv, "p", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'v':
                verbose = true;
                break;
            case 'p':
                port = (unsigned int)atoi(optarg);
                break;
            case 'a':
                nb_of_agents = (unsigned int)atoi(optarg);
                break;
            case 'd':
                networkDevice = optarg;
                break;
            case 'n':
                agent_name = optarg;
                break;
            case 's':
                stress_peer = true;
                break;
            case '1':
                deactivate = true;
                break;
            case '2':
                change_state = true;
                break;
            case '3':
                change_definition = true;
                break;
            case '4':
                change_mapping = true;
                break;
            case '5':
                publish = true;
                break;
            case '6':
                elections = true;
                break;
            case 'h':
                print_usage();
                exit(0);
            default:
                print_usage();
                exit(1);
        }
    }

    
    igs_agent_set_name(agent_name);
    igs_log_set_console(true);
    igs_log_set_file(true, NULL);
    igs_log_set_stream(true);
    igs_set_command_line_from_args(argc, argv);
    igs_log_set_console(true);
    igs_log_set_file(true, NULL);
    if (verbose)
        igs_log_set_console_level(IGS_LOG_TRACE);
    else
        igs_log_set_console_level(IGS_LOG_INFO);
    
    igs_log_set_console_level(IGS_LOG_TRACE);
    
    igs_unbind_pipe();
    igs_monitor_pipe_stack(verbose);
    
    if (stress_peer){
        //TODO: add start/stop stress here
    }
    
    
    agents = zlist_new();
    for (size_t i = 0; i < nb_of_agents; i++) {
        char additionalAgentName[IGS_MAX_AGENT_NAME_LENGTH] = "";
        sprintf(additionalAgentName, "%s-%zu", agent_name, i);
        igsagent_t *agent = igsagent_new(additionalAgentName, true);
        zlist_append(agents, agent);
        
        igsagent_set_state(agent, "0");
        igsagent_observe_agent_events(agent, observe_agent_events, NULL);
        
        igsagent_input_create(agent, "impulsion", IGS_IMPULSION_T, NULL, 0);
        igsagent_observe_input(agent, "impulsion", observe_input, NULL);
        igsagent_input_create(agent, "bool", IGS_BOOL_T, NULL, 0);
        igsagent_observe_input(agent, "bool", observe_input, NULL);
        igsagent_input_create(agent, "int", IGS_INTEGER_T, NULL, 0);
        igsagent_observe_input(agent, "int", observe_input, NULL);
        igsagent_input_create(agent, "double", IGS_DOUBLE_T, NULL, 0);
        igsagent_observe_input(agent, "double", observe_input, NULL);
        igsagent_input_create(agent, "string", IGS_STRING_T, NULL, 0);
        igsagent_observe_input(agent, "string", observe_input, NULL);
        igsagent_input_create(agent, "data", IGS_DATA_T, NULL, 0);
        igsagent_observe_input(agent, "data", observe_input, NULL);
        igsagent_output_create(agent, "impulsion", IGS_IMPULSION_T, NULL, 0);
        igsagent_output_create(agent, "bool", IGS_BOOL_T, NULL, 0);
        igsagent_output_create(agent, "int", IGS_INTEGER_T, NULL, 0);
        igsagent_output_create(agent, "double", IGS_DOUBLE_T, NULL, 0);
        igsagent_output_create(agent, "string", IGS_STRING_T, NULL, 0);
        igsagent_output_create(agent, "data", IGS_DATA_T, NULL, 0);
        igsagent_attribute_create(agent, "bool", IGS_BOOL_T, NULL, 0);
        igsagent_attribute_create(agent, "int", IGS_INTEGER_T, NULL, 0);
        igsagent_attribute_create(agent, "double", IGS_DOUBLE_T, NULL, 0);
        igsagent_attribute_create(agent, "string", IGS_STRING_T, NULL, 0);
        igsagent_attribute_create(agent, "data", IGS_DATA_T, NULL, 0);
        
        igsagent_service_init(agent, "service", service, NULL);
        igsagent_service_arg_add(agent, "service", "bool", IGS_BOOL_T);
        igsagent_service_arg_add(agent, "service", "int", IGS_INTEGER_T);
        igsagent_service_arg_add(agent, "service", "double", IGS_DOUBLE_T);
        igsagent_service_arg_add(agent, "service", "string", IGS_STRING_T);
        igsagent_service_arg_add(agent, "service", "data", IGS_DATA_T);
        igsagent_service_init(agent, "reply", reply, NULL);
        igsagent_service_arg_add(agent, "reply", "bool", IGS_BOOL_T);
        igsagent_service_arg_add(agent, "reply", "int", IGS_INTEGER_T);
        igsagent_service_arg_add(agent, "reply", "double", IGS_DOUBLE_T);
        igsagent_service_arg_add(agent, "reply", "string", IGS_STRING_T);
        igsagent_service_arg_add(agent, "reply", "data", IGS_DATA_T);
    }
    
    zloop_t *loop = zloop_new();
    igs_start_with_device(networkDevice, port);
    zsock_t *pipe = igs_pipe_to_ingescape();
    zloop_reader(loop, pipe, ingescapeSentMessage, NULL);
    
    if (deactivate)
        a_stress_activate_deactivate = zactor_new(stress_activate_deactivate_fn, NULL);
    if (change_state)
        a_stress_change_state = zactor_new(stress_change_state_fn, NULL);
    if (change_definition)
        a_stress_change_definition = zactor_new(stress_change_definition_fn, NULL);
    if (change_mapping)
        a_stress_change_mapping = zactor_new(stress_change_mapping_fn, NULL);
    if (publish)
        a_stress_publish = zactor_new(stress_publish_fn, NULL);
    //NB: elections only make sense between separated peers => to be used with several igsStresser instances
    if (elections)
        a_stress_elections = zactor_new(stress_elections_fn, NULL);
    
    zloop_start(loop);
    
    zloop_destroy(&loop);
    igs_stop();
    igsagent_t *a = zlist_first(agents);
    while (a) {
        igsagent_destroy(&a);
        a = zlist_next(agents);
    }
    zlist_destroy(&agents);

    return EXIT_SUCCESS;
}
