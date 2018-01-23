/*
 *	ZYREPROBE
 *
 *  Copyright (c) 2016 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Stephane Vales <vales@ingenuity.io>
 *
 *
 */


#include <zyre.h>
#include <zyre_event.h>
#include <regex.h>
#include <getopt.h>
#include "uthash/uthash.h"
#include "zregex.h"

//global application options
int port = 5670;
const char *name = "zyreprobe";
char *netdevice = NULL;
bool verbose = false;
bool keepRunning = false;
bool proxy = false;
const char *gossipbind = NULL;//"tcp://10.0.0.7:12345";
const char *gossipconnect = NULL;
const char *endpoint = NULL;


//for message passed as run parameter
char *paramMessage = NULL;
char *paramChannel = NULL;
char *paramPeer = NULL;
char *paramText = NULL;

//data storage
#define NAME_BUFFER_SIZE 256
typedef struct agent {
    char uuid[NAME_BUFFER_SIZE];
    char name[NAME_BUFFER_SIZE];
    int reconnected;
    UT_hash_handle hh;
} agent;

///////////////////////////////////////////////////////////////////////////////
// ZYRE AGENT MANAGEMENT
//
//int initNbFileHandlers = 2;
typedef struct zyreloopElements{
    char *name;
    zyre_t *node;
    agent *agents;
    bool useGossip;
} zyreloopElements_t;

//manage commands entered on the command line from the parent
int manageParent (zloop_t *loop, zmq_pollitem_t *item, void *args){
    zyreloopElements_t *zEl = (zyreloopElements_t *)args;
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
                agent *a = NULL;
                for(a = zEl->agents; a != NULL; a = a->hh.next) {
                    if (strcmp(a->name, peer) == 0 || strcmp(a->uuid, peer) == 0){
                        peer = a->uuid;
                        break;
                    }
                }
                zyre_whispers (node, peer, "%s", string);
            }
            else if (streq (command, "WHISPERALL")) {
                char *string = zmsg_popstr (msg);
                agent *a = NULL;
                for(a = zEl->agents; a != NULL; a = a->hh.next) {
                    zyre_whispers (node, a->uuid, "%s", string);
                }
                
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
                    agent *a = NULL;
                    HASH_FIND_STR(zEl->agents, p, a);
                    if (a != NULL){
                        printf("\t%s -> %s\n", a->name, p);
                    }else{
                        printf("\t? -> %s\n", p);
                    }
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
int manageIncoming (zloop_t *loop, zmq_pollitem_t *item, void *args){
    zyreloopElements_t *zEl = (zyreloopElements_t *)args;
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
        //size_t msg_size = zmsg_content_size(msg);
        
        if (streq (event, "ENTER")){
            if (!zEl->useGossip){
                printf ("B->%s has entered the network with peer id %s and address %s\n", name, peer, address);
            }else{
                printf ("G->%s has entered the network with peer id %s and address %s\n", name, peer, address);
            }
            //printf ("->%s has entered the network with peer id %s and address %s\n", name, peer, address);
            agent *a = NULL;
            HASH_FIND_STR(zEl->agents, peer, a);
            if (a == NULL){
                a = calloc(1, sizeof(agent));
                a->reconnected = 0;
                strncpy(a->uuid, peer, NAME_BUFFER_SIZE);
                HASH_ADD_STR(zEl->agents, uuid, a);
            }else{
                //Agent already exists, we set its reconnected flag
                //(this is used below to avoid agent destruction on EXIT received after timeout)
                a->reconnected++;
            }
            strncpy(a->name, name, NAME_BUFFER_SIZE);
            
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
            if (!zEl->useGossip){
                printf ("B<-%s exited\n", name);
            }else{
                printf ("G<-%s exited\n", name);
            }
            //printf ("<-%s exited\n", name);
            agent *a = NULL;
            HASH_FIND_STR(zEl->agents, peer, a);
            if (a != NULL){
                if (a->reconnected > 0){
                    //do not destroy: we are getting a timemout now whereas
                    //the agent is reconnected
                    a->reconnected--;
                }else{
                    HASH_DEL(zEl->agents, a);
                    free(a);
                }
            }
        }
        zyre_event_destroy(&zyre_event);
    }
    return 0;
}

//manage message passed as run parameter
int triggerMessageSend(zloop_t *loop, int timer_id, void *args){
    zyreloopElements_t *zEl = (zyreloopElements_t *)args;
    zyre_t *node = zEl->node;
    if (paramChannel != NULL && strlen(paramChannel) > 0){
        zyre_shouts(node, paramChannel, "%s", paramText);
    }else if (paramPeer != NULL && strlen(paramPeer) > 0){
        zyre_whispers(node, paramPeer, "%s", paramText);
    }
    //this return will stop the loop and terminate the app
    return -1;
}

static void
zyre_actor (zsock_t *pipe, void *args)
{
    zyreloopElements_t *zEl = (zyreloopElements_t *)args;
    zyre_t *node = zyre_new (zEl->name);
    zEl->node = node;
    
    if (!zEl->useGossip){
        //beacon
        zyre_set_port(node, port);
        printf("using broadcast discovery with port %i", port);
        if (netdevice != NULL){
            zyre_set_interface(node, netdevice);
            printf(" on device %s", netdevice);
        }
        printf("\n");
    }else{
        //gossip
        if (endpoint != NULL){
            int res = zyre_set_endpoint(node, "%s", endpoint);
            if (res != 0){
                printf("impossible to create our endpoint %s ...exiting.", endpoint);
            }
            printf("using endpoint %s\n", endpoint);
            if (gossipconnect == NULL && gossipbind == NULL){
                printf("warning : endpoint specified but no attached gossip information, %s won't reach any other agent", name);
            }
        }
        if (gossipconnect != NULL){
            zyre_gossip_connect(node, "%s", gossipconnect);
            printf("connecting to gossip node at %s\n", gossipconnect);
        }
        if (gossipbind != NULL){
            zyre_gossip_bind(node, "%s", gossipbind);
            printf("creating gossip node %s\n", gossipbind);
        }
    }
    
    if (verbose){
        zyre_set_verbose(node);
    }
    if (!node)
        return;
    zyre_start (node);
    zsock_signal (pipe, 0); //notify main thread that we are ready
    zyre_print(node);
    
    //preparing and running zyre mainloop
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
    
    zloop_poller (loop, &zpipePollItem, manageParent, args);
    zloop_poller_set_tolerant(loop, &zpipePollItem);
    zloop_poller (loop, &zyrePollItem, manageIncoming, args);
    zloop_poller_set_tolerant(loop, &zyrePollItem);
    
    if (paramText != NULL && strlen(paramText) > 0){
        zloop_timer(loop, 10, 1, triggerMessageSend, (void *)args);
    }
    
    zloop_start (loop); //start returns when one of the pollers returns -1
    
    printf("shutting down...\n");
    //clean
    zloop_destroy (&loop);
    assert (loop == NULL);
    agent *current, *tmp;
    HASH_ITER(hh, zEl->agents, current, tmp) {
        HASH_DEL(zEl->agents,current);
        free(current);
    }
    zyre_stop (node);
    zclock_sleep (100);
    zyre_destroy (&node);
    keepRunning = false;
    free(zEl);
}

///////////////////////////////////////////////////////////////////////////////
// COMMAND LINE AND INTERPRETER OPTIONS
//
void print_usage(){
    printf("Usage example: zyreprobe --verbose --port 5670 --name zyreprobe\n");
    printf("(all parameters are optional)\n");
    printf("--verbose : enable verbose mode in the application\n");
    printf("--name peer_name : published name of this peer (default : zyreprobe)\n");
    printf("--noninteractiveloop : non-interactive loop for use as a background application\n");
    printf("--message \"[#channel|peer] message\" : message to send to a channel (indicated with #) or a peer (peer id) at startup and then stop\n");
    //printf("\n--proxy : run both beacon and gossip instances (default : false)\n");
    printf("beacon broadcast configuration :\n");
    printf("--netdevice : name of the network device to be used (shall be used if several devices available)\n");
    printf("--port port_number : port used for autodiscovery between peers (default : 5670)\n");
    printf("OR\n");
    printf("gossip and fixed endepoint configuration :\n");
    printf("an endpoint looks like : tcp://10.0.0.7:49155\n");
    printf("--gossipbind endpoint : our address as a gossip endpoint\n");
    printf("--gossipconnect endpoint : address of a gossip endpoint to use\n");
    printf("NB: if gossip endpoint restarts, others depending on it will not see it coming back\n");
    printf("NB: in gossip mode, leaving agents are not detected by others\n");
    printf("--endpoint endpoint : optional custom zyre endpoint address (overrides --netdevice and --port)\n");
}

void print_commands(){
    printf("---------------------------------\n");
    printf("Supported commands:\n");
    printf("/quit : cleanly stop the application\n");
    printf("/restart : stop and start connection (still buggy)\n");
    printf("/verbose : triggers verbose mode for detailed activity information\n");
    printf("/peers : list all connected peers at the time (name -> uuid)\n");
    printf("/groups : list all existing groups at the time\n");
    printf("/join group_name : joins a specific group\n");
    printf("/joinall : join all existing groups at the time\n");
    printf("/leave group_name : leave a specific group\n");
    printf("/leaveall : leave all existing groups at the time\n");
    printf("/whisper peer_name message : sends a message to a specific peer\n\t(peer can be designated by name or uuid)\n");
    printf("/shout channel_name message : sends a message to a specific group\n");
    printf("/whisperall message : sends a message to all peers individually\n");
    printf("\n");
}

///////////////////////////////////////////////////////////////////////////////
// MAIN & OPTIONS & COMMAND INTERPRETER
//
//
int main (int argc, char *argv [])
{
    //manage options
    int opt= 0;
    bool noninteractiveloop = false;
    
    //Specifying the expected options
    //The two options l and b expect numbers as argument
    static struct option long_options[] = {
        {"verbose",   no_argument, 0,  'v' },
        {"proxy",   no_argument, 0,  'r' },
        {"netdevice",      required_argument, 0,  'd' },
        {"port",      required_argument, 0,  'p' },
        {"name",      required_argument, 0,  'n' },
        {"message",      required_argument, 0,  'm' },
        {"noninteractiveloop",      no_argument, 0,  'i' },
        {"help",      no_argument, 0,  'h' },
        {"gossipbind",      required_argument, 0,  's' },
        {"gossipconnect",      required_argument, 0,  'g' },
        {"endpoint",      required_argument, 0,  'e' },
    };
    
    int long_index =0;
    while ((opt = getopt_long(argc, argv,"p",long_options, &long_index )) != -1) {
        switch (opt) {
            case 'p' :
                port = atoi(optarg);
                //printf("port: %i\n", port);
                break;
            case 'd' :
                netdevice = optarg;
                //printf("device: %s\n", netdevice);
                break;
            case 'n' :
                name = optarg;
                //printf("name: %s\n", name);
                break;
            case 's' :
                gossipbind = optarg;
                //printf("name: %s\n", name);
                break;
            case 'g' :
                gossipconnect = optarg;
                //printf("name: %s\n", name);
                break;
            case 'e' :
                endpoint = optarg;
                //printf("name: %s\n", name);
                break;
            case 'v' :
                verbose = true;
                //printf("verbose\n");
                break;
            case 'm' :
                paramMessage = strdup(optarg);
                break;
            case 'i' :
                noninteractiveloop = true;
                break;
            case 'r' :
                proxy = true;
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
    
    //try to parse message passed as parameter
    if (paramMessage != NULL && strlen(paramMessage) > 0){
        //printf("paramMessage: %s\n", paramMessage);
        noninteractiveloop = true;
        const char *reg = "([^ ]+)[[:space:]]{1}([[:print:]]+)";
        regex_t r;
        matches my_matches;
        compile_regex(&r, reg);
        match_regex(&r, paramMessage, &my_matches);
        regfree (&r);
        if (my_matches.nb > 0){
            if (my_matches.results[0][0] == '#' && strlen(my_matches.results[0]) > 1){
                paramChannel = strdup(my_matches.results[0] + 1);
                printf("matched channel: %s\n", paramChannel);
            }else{
                paramPeer = strdup(my_matches.results[0]);
                printf("matched peer: %s\n", paramPeer);
            }
            paramText = strdup(my_matches.results[1]);
            printf("matched text: %s\n", paramText);
            clean_matches(&my_matches);
        }
    }
    
   
    
    
    //init zyre
    zactor_t *beaconActor = NULL;
    zactor_t *gossipActor = NULL;
    if (!proxy || (gossipconnect == NULL && gossipbind == NULL && endpoint == NULL)){
        zyreloopElements_t *zEl = calloc(1, sizeof(zyreloopElements_t));
        assert(zEl);
        zEl->name = strdup(name);
        zEl->useGossip = false;
        zEl->agents = NULL;
        beaconActor = zactor_new (zyre_actor, zEl);
        assert (beaconActor);
    }
    if (proxy || gossipconnect != NULL || gossipbind != NULL || endpoint != NULL){
        if (endpoint != NULL && gossipconnect == NULL && gossipbind == NULL){
            printf("warning : endpoint specified but no attached gossip information, %s won't reach any other agent", name);
        }
        if (endpoint == NULL && gossipconnect == NULL && gossipbind == NULL){
            printf("warning : running in proxy mode without gossip parameters, gossip won't be started for %s", name);
        }else{
            zyreloopElements_t *zEl = calloc(1, sizeof(zyreloopElements_t));
            assert(zEl);
            zEl->name = strdup(name);
            zEl->useGossip = true;
            zEl->agents = NULL;
            gossipActor = zactor_new (zyre_actor, zEl);
            assert (gossipActor);
        }
    }
    
    if (noninteractiveloop){
        keepRunning = true;
        while (keepRunning) {
            if (zsys_interrupted){
                break;
            }
            sleep(1);
        }
    }else{
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
                clean_matches(&my_matches);
                //compiled regex
                regex_t r;
                
                const char *reg1 = "/([[:alnum:]]+)"; //single command
                const char *reg2 = "/([[:alnum:]]+)[[:space:]]{1}([^ ]+)"; //command + parameter (single word)
                const char *reg3 = "/([[:alnum:]]+)[[:space:]]{1}([^ ]+)[[:space:]]{1}([[:print:]]+)"; //command + parameter + message
                compile_regex(&r, reg3);
                match_regex(&r, message, &my_matches);
                regfree (&r);
                if (my_matches.nb > 0){
                    char *command = my_matches.results[0];
                    char *parameter = my_matches.results[1];
                    char *message = my_matches.results[2];
                    //printf("command: %s\nparameter: %s\nmessage: %s\n",command,parameter,message);
                    if (strcmp(command, "whisper") == 0){
                        //FIXME: check to which actor UUID belongs
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "WHISPER", parameter, message, NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "WHISPER", parameter, message, NULL);
                        }
                    } else if (strcmp(command, "shout") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "SHOUT", parameter, message, NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "SHOUT", parameter, message, NULL);
                        }
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
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "JOIN", parameter, NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "JOIN", parameter, NULL);
                        }
                    } else if (strcmp(command, "leave") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "LEAVE", parameter, NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "LEAVE", parameter, NULL);
                        }
                    }else if (strcmp(command, "whisperall") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "WHISPERALL", parameter, NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "WHISPERALL", parameter, NULL);
                        }
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
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "RESTART", NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "RESTART", NULL);
                        }
                    }else if (strcmp(command, "verbose") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "VERBOSE", NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "VERBOSE", NULL);
                        }
                    }else if (strcmp(command, "peers") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "PEERS", NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "PEERS", NULL);
                        }
                    }else if (strcmp(command, "groups") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "GROUPS", NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "GROUPS", NULL);
                        }
                    }else if (strcmp(command, "joinall") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "JOINALL", NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "JOINALL", NULL);
                        }
                    }else if (strcmp(command, "leaveall") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "LEAVEALL", NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "LEAVEALL", NULL);
                        }
                    }else if (strcmp(command, "help") == 0){
                        print_commands();
                    }else if (strcmp(command, "quit") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "$TERM", NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "$TERM", NULL);
                        }
                        break;
                    }
                    clean_matches(&my_matches);
                    continue;
                }
            }
        }
    }
    zactor_destroy (&beaconActor);
    zactor_destroy (&gossipActor);
    return 0;
}
