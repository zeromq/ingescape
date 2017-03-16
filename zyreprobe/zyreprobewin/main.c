//
//  main.c
//  zyreprobe
//
//  Created by Stephane Vales on 04/04/2016.
//  Copyright © 2016 Ingenuity i/o. All rights reserved.
//

#include <zyre.h>
#include <zyre_event.h>
#include <regex.h>
#include <getopt.h>

#include "zregex.h"

//global application options
int port = 5670;
const char *name = "zyreprobe";
bool verbose = false;


///////////////////////////////////////////////////////////////////////////////
// ZYRE AGENT MANAGEMENT
//
//int initNbFileHandlers = 2;
typedef struct zyreloopElements{
    zyre_t *node;
} zyreloopElements_t;


//manage commands entered on the command line from the parent
int manageParent (zloop_t *loop, zmq_pollitem_t *item, void *arg){
    zyreloopElements_t *zEl = (zyreloopElements_t *)arg;
    zyre_t *node = zEl->node;
    if (item->revents & ZMQ_POLLIN)
    {
        zmsg_t *msg = zmsg_recv ((zsock_t *)item->socket);
        if (!msg){
            printf("Error while reading message from main thread");
            exit(EXIT_FAILURE); //Interrupted
        }
        char *command = zmsg_popstr (msg);
        if (streq (command, "$TERM")){
            return -1;
        }
        else
        {
            if (streq (command, "SHOUT")) {
                char *group = zmsg_popstr (msg);
                char *string = zmsg_popstr (msg);
                zyre_shouts (node, group, "%s", string);
            }
            else if (streq (command, "WHISPER")) {
                char *peer = zmsg_popstr (msg);
                char *string = zmsg_popstr (msg);
                zyre_whispers (node, peer, "%s", string);
            }
            else if(streq (command, "LEAVE")){
                char *group = zmsg_popstr (msg);
                zyre_leave (node, group);
            }
            else if(streq (command, "LEAVEALL")){
                char *p;
                zlist_t *all_groups = zyre_peer_groups(node);
                while ((p = (char *)zlist_pop(all_groups))) {
                    zyre_leave (node, p);
                    free(p);
                }
                zlist_destroy(&all_groups);
            }
            else if(streq (command, "JOIN")){
                char *group = zmsg_popstr (msg);
                zyre_join (node, group);
            }
            else if(streq (command, "JOINALL")){
                char *p;
                zlist_t *all_groups = zyre_peer_groups(node);
                while ((p = (char *)zlist_pop(all_groups))) {
                    zyre_join (node, p);
                    free(p);
                }
                zlist_destroy(&all_groups);
            }
            else if(streq (command, "PEERS")){
                zlist_t *peers = zyre_peers(node);
                char *p;
                printf("@peers:\n");
                while ((p = (char *)zlist_pop(peers))) {
                    printf("\t%s\n", p);
                    free(p);
                }
                zlist_destroy(&peers);
            }
            else if(streq (command, "GROUPS")){
                zlist_t *my_groups = zyre_own_groups(node);
                zlist_t *all_groups = zyre_peer_groups(node);
                char *p;
                printf("@my groups:\n");
                while ((p = (char *)zlist_pop(my_groups))) {
                    printf("\t%s\n", p);
                    free(p);
                }
                printf("@all groups:\n");
                while ((p = (char *)zlist_pop(all_groups))) {
                    printf("\t%s\n", p);
                    free(p);
                }
                zlist_destroy(&my_groups);
                zlist_destroy(&all_groups);
            }
            else if(streq (command, "RESTART")){
                //FIXME: find out what needs to be done for a proper stop+start
                zyre_stop (node);
                sleep(1);
                zyre_start (node);
            }
            else if(streq (command, "VERBOSE")){
                zyre_set_verbose(node);
            }
            else {
                printf("Error: invalid command to actor\n%s\n", command);
                assert (false);
            }
        }
        
        free (command);
        zmsg_destroy (&msg);
    }
    return 0;
}

//manage messages received on the bus
int manageIncoming (zloop_t *loop, zmq_pollitem_t *item, void *arg){
    zyreloopElements_t *zEl = (zyreloopElements_t *)arg;
    zyre_t *node = zEl->node;
    if (item->revents & ZMQ_POLLIN)
    {
        zyre_event_t *zyre_event = zyre_event_new (node);
        const char *event = zyre_event_type(zyre_event);
        const char *peer = zyre_event_peer_uuid(zyre_event);
        const char *name = zyre_event_peer_name (zyre_event);
        const char *address = zyre_event_peer_addr (zyre_event);
        zhash_t *headers = zyre_event_headers (zyre_event);
        const char *group = zyre_event_group (zyre_event);
        zmsg_t *msg = zyre_event_msg (zyre_event);
        
        if (streq (event, "ENTER")){
            printf ("->%s has entered the network with peer id %s and address %s\n", name, peer, address);
            assert(headers);
            char *k;
            const char *v;
            zlist_t *keys = zhash_keys(headers);
            size_t s = zlist_size(keys);
            if (s > 0){
                printf("@%s's headers are:\n", name);
            }
            while ((k = (char *)zlist_pop(keys))) {
                v = zyre_event_header (zyre_event,k);
                printf("\t%s -> %s\n", k, v);
                free(k);
            }
            zlist_destroy(&keys);
        } else if (streq (event, "JOIN")){
            printf ("+%s has joined %s\n", name, group);
        } else if (streq (event, "LEAVE")){
            printf ("-%s has left %s\n", name, group);
        } else if (streq (event, "SHOUT")){
            char *message;
            while ((message = zmsg_popstr(msg))) {
                printf ("#%s[%s]: %s\n", group, name, message);
                free (message);
            }
        } else if (streq (event, "WHISPER")){
            char *message;
            while ((message = zmsg_popstr(msg))) {
                printf ("#whisper[%s] : %s\n", name, message);
                free (message);
            }
        } else if (streq (event, "EXIT")){
            printf ("<-%s exited\n", name);
        }
        
        zyre_event_destroy(&zyre_event);
    }
    return 0;
}

static void
chat_actor (zsock_t *pipe, void *args)
{
    zyre_t *node = zyre_new ((char *) args);
    zyre_set_port(node, port);
    if (verbose){
        zyre_set_verbose(node);
    }
    if (!node)
        return;
    zyre_start (node);
    zsock_signal (pipe, 0); //notify main thread that we are ready
    
    //preparing and running zyre mainloop
    zyreloopElements_t *zEl = calloc(1, sizeof(zyreloopElements_t));
    assert(zEl);
    zEl->node = node;
    
    zmq_pollitem_t zpipePollItem;
    zmq_pollitem_t zyrePollItem;
    
    //main zmq socket (i.e. main thread)
    void *zpipe = zsock_resolve(pipe);
    if (zpipe == NULL){
        printf("Error : could not get the pipe descriptor for polling... exiting.\n");
        exit(EXIT_FAILURE);
    }
    zpipePollItem.socket = zpipe;
    zpipePollItem.fd = 0;
    zpipePollItem.events = ZMQ_POLLIN;
    zpipePollItem.revents = 0;
    
    //zyre socket
    void *zsock = zsock_resolve(zyre_socket (node));
    if (zsock == NULL){
        printf("Error : could not get the zyre socket for polling... exiting.\n");
        exit(EXIT_FAILURE);
    }
    zyrePollItem.socket = zsock;
    zyrePollItem.fd = 0;
    zyrePollItem.events = ZMQ_POLLIN;
    zyrePollItem.revents = 0;
    
    zloop_t *loop = zloop_new ();
    assert (loop);
    zloop_set_verbose (loop, verbose);
    
    zloop_poller (loop, &zpipePollItem, manageParent, zEl);
    zloop_poller_set_tolerant(loop, &zpipePollItem);
    zloop_poller (loop, &zyrePollItem, manageIncoming, zEl);
    zloop_poller_set_tolerant(loop, &zyrePollItem);
    
    zloop_start (loop); //start returns when one of the pollers returns -1
    
    zloop_destroy (&loop);
    assert (loop == NULL);
    
    
    //clean
    zloop_destroy (&loop);
    assert (loop == NULL);
    zyre_stop (node);
    zclock_sleep (100);
    zyre_destroy (&node);
}

///////////////////////////////////////////////////////////////////////////////
// COMMAND LINE AND INTERPRETER OPTIONS
//
void print_usage(){
    printf("Usage example: zyreprobe --verbose --port 5670 --name zyreprobe\n");
    printf("(all parameters are optional)\n");
    printf("--verbose : enable verbose mode in the application\n");
    printf("--port port_number : port used for autodiscovery between peers\n");
    printf("--name peer_name : published name of this peer\n");
    
}

void print_commands(){
    printf("---------------------------------\n");
    printf("Supported commands:\n");
    printf("/quit : cleanly stop the application\n");
    printf("/restart : stop and start connection (still buggy)\n");
    printf("/verbose : triggers verbose mode for detailed activity information\n");
    printf("/peers : list all connected peers at the time\n");
    printf("/groups : list all existing groups at the time\n");
    printf("/join group_name : joins a specific group\n");
    printf("/joinall : join all existing groups at the time\n");
    printf("/leave group_name : leave a specific group\n");
    printf("/leaveall : leave all existing groups at the time\n");
    printf("/whisper peer_name message : sends a message to a specific peer\n");
    printf("/shout channel_name message : sends a message to a specific group\n\n");
}

///////////////////////////////////////////////////////////////////////////////
// MAIN & OPTIONS & COMMAND INTERPRETER
//
//
int main (int argc, char *argv [])
{
    //manage options
    int opt= 0;
    
    //Specifying the expected options
    //The two options l and b expect numbers as argument
    static struct option long_options[] = {
        {"verbose",   no_argument, 0,  'v' },
        {"port",      required_argument, 0,  'p' },
        {"name",      required_argument, 0,  'n' },
        {"help",      no_argument, 0,  'h' }
    };
    
    int long_index =0;
    while ((opt = getopt_long(argc, argv,"p",long_options, &long_index )) != -1) {
        switch (opt) {
            case 'p' :
                port = atoi(optarg);
                //printf("port: %i\n", port);
                break;
            case 'n' :
                name = optarg;
                //printf("name: %s\n", name);
                break;
            case 'v' :
                verbose = true;
                //printf("verbose\n");
                break;
            case 'h' :
                print_usage();
                exit (0);
                break;
            default:
                print_usage();
                exit(EXIT_FAILURE);
        }
    }
    
    //init zyre
    zactor_t *actor = zactor_new (chat_actor, (void *)name);
    assert (actor);
    
    //mainloop
    printf("%s is running...\nType /help for available commands\n", name);
    while (!zsys_interrupted) {
        char message [BUFFER_SIZE];
        if (!fgets (message, BUFFER_SIZE, stdin))
            break;
        if ((message[0] == '/')&&(strlen (message) > 1)) {
            //we have something to parse!
            
            //matches collector
            matches my_matches;
            //compiled regex
            regex_t r;
            
            const char *reg1 = "/([[:alnum:]]+)"; //single command
            const char *reg2 = "/([[:alnum:]]+)[[:space:]]{1}([[:alnum:]]+)"; //command + parameter (single word)
            const char *reg3 = "/([[:alnum:]]+)[[:space:]]{1}([[:alnum:]]+)[[:space:]]{1}([[:print:]]+)"; //command + parameter + message
            compile_regex(&r, reg3);
            match_regex(&r, message, &my_matches);
            regfree (&r);
            if (my_matches.nb > 0){
                char *command = my_matches.results[0];
                char *parameter = my_matches.results[1];
                char *message = my_matches.results[2];
                //printf("command: %s\nparameter: %s\nmessage: %s\n",command,parameter,message);
                if (strcmp(command, "whisper") == 0){
                    zstr_sendx (actor, "WHISPER", parameter, message, NULL);
                } else if (strcmp(command, "shout") == 0){
                    zstr_sendx (actor, "SHOUT", parameter, message, NULL);
                }
                clean_matches(&my_matches);
                continue;
            }
            compile_regex(&r, reg2);
            match_regex(&r, message, &my_matches);
            regfree (&r);
            if (my_matches.nb > 0){
                char *command = my_matches.results[0];
                char *parameter = my_matches.results[1];
                //printf("command: %s\nparameter: %s\n",command,parameter);
                if (strcmp(command, "join") == 0){
                    zstr_sendx (actor, "JOIN", parameter, NULL);
                } else if (strcmp(command, "leave") == 0){
                    zstr_sendx (actor, "LEAVE", parameter, NULL);
                }
                clean_matches(&my_matches);
                continue;
            }
            compile_regex(&r, reg1);
            match_regex(&r, message, &my_matches);
            regfree (&r);
            if (my_matches.nb > 0){
                char *command = my_matches.results[0];
                //printf("command: %s\n",command);
                if (strcmp(command, "restart") == 0){
                    zstr_sendx (actor, "RESTART", NULL);
                }else if (strcmp(command, "verbose") == 0){
                    zstr_sendx (actor, "VERBOSE", NULL);
                }else if (strcmp(command, "peers") == 0){
                    zstr_sendx (actor, "PEERS", NULL);
                }else if (strcmp(command, "groups") == 0){
                    zstr_sendx (actor, "GROUPS", NULL);
                }else if (strcmp(command, "joinall") == 0){
                    zstr_sendx (actor, "JOINALL", NULL);
                }else if (strcmp(command, "leaveall") == 0){
                    zstr_sendx (actor, "LEAVEALL", NULL);
                }else if (strcmp(command, "help") == 0){
                    print_commands();
                }else if (strcmp(command, "quit") == 0){
                    zstr_sendx (actor, "$TERM", NULL);
                    break;
                }
                clean_matches(&my_matches);
                continue;
            }
        }
    }
    zactor_destroy (&actor);
    return 0;
}
