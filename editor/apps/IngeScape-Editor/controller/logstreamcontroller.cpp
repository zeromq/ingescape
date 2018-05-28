/*
 *	IngeScape Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "logstreamcontroller.h"



/*static void zyre_actor (zsock_t *pipe, void *args)
{
    zyreloopElements_t *zEl = (zyreloopElements_t *)args;
    zyre_t *node = zyre_new (zEl->name);
    zEl->node = node;


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
            printf("connecting to P2P node at %s\n", gossipconnect);
        }
        if (gossipbind != NULL){
            zyre_gossip_bind(node, "%s", gossipbind);
            printf("creating P2P node %s\n", gossipbind);
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



    // New LOOP
    zloop_t *loop = zloop_new();
    assert (loop);

    zloop_set_verbose(loop, verbose);

    zloop_poller (loop, &zpipePollItem, manageParent, args);
    zloop_poller_set_tolerant(loop, &zpipePollItem);
    zloop_poller (loop, &zyrePollItem, manageIncoming, args);
    zloop_poller_set_tolerant(loop, &zyrePollItem);

    if (paramText != NULL && strlen(paramText) > 0){
        zloop_timer(loop, 10, 1, triggerMessageSend, (void *)args);
    }

    // start returns when one of the pollers returns -1
    zloop_start (loop);

    printf("shutting down...\n");

    // clean
    zloop_destroy (&loop);
    assert (loop == NULL);

    agent *current, *tmp;

    HASH_ITER(hh, zEl->agents, current, tmp) {
        HASH_DEL(zEl->agents,current);
        free(current->name);
        free(current->uuid);
        free(current->endpoint);
        if (current->publisherPort != NULL){
            free(current->publisherPort);
        }
        if (current->subscriber != NULL){
            zsock_destroy(&(current->subscriber));
        }
        if (current->subscriberPoller != NULL){
            free(current->subscriberPoller);
        }
        if (current->logPort != NULL){
            free(current->logPort);
        }
        if (current->logger != NULL){
            zsock_destroy(&(current->logger));
        }
        if (current->loggerPoller != NULL){
            free(current->loggerPoller);
        }
        free(current);
    }
    zyre_stop (node);
    zclock_sleep (100);
    zyre_destroy (&node);
    keepRunning = false;
    free(zEl);
}*/



/**
 * @brief Constructor
 * @param parent
 */
LogStreamController::LogStreamController(QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    /*zactor_t *zActor = NULL;

    zyreloopElements_t *zEl = calloc(1, sizeof(zyreloopElements_t));
    assert(zEl);

    zEl->name = strdup(name);
    zEl->useGossip = true;
    zEl->agents = NULL;

    zActor = zactor_new (zyre_actor, zEl);
    assert (zActor);


    macosAgent has entered the network with peer id 24B7D7C1A62E45B4A10B0C246709B78F and endpoint tcp://10.0.0.104:49154
    @macosAgent's headers are:
        pid -> 9089
        canBeFrozen -> 1
        commandline ->  /Users/Vincent/Library/Developer/Xcode/DerivedData/cocoaAgents-cbddaxwvdoulyofkunbobgtsjxti/Build/Products/Debug/macosAgent.app/Contents/MacOS/macosAgent -NSDocumentRevisionsDebugMode YES
        test -> worked properly
        logger -> 49153

    // tcp:// 10.0.0.104 (address of agent) : 49153 (port of logger)


    a->subscriber = zsock_new_sub(endpointAddress, NULL);

    // Subscribe to all
    zsock_set_subscribe(a->subscriber, "");

    zmq_pollitem_t *poller = a->subscriberPoller = calloc(1, sizeof(zmq_pollitem_t));;

    poller->socket = zsock_resolve(a->subscriber);
    poller->fd = 0;
    poller->events = ZMQ_POLLIN;
    poller->revents = 0;

    zloop_poller (loop, poller, manageSubscription, (void*)a);
    zloop_poller_set_tolerant(loop, poller);

    printf("Subscriber created for %s\n", a->name);*/
}


/**
 * @brief Destructor
 */
LogStreamController::~LogStreamController()
{

}
