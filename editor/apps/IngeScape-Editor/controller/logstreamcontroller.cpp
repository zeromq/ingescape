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

extern "C" {
//#include <ingescape_advanced.h>
#include <ingescape_private.h>
//#include <czmq.h>
}

bool verbose = true;


/*static void zyre_actor(zsock_t *pipe, void *args)
{
    zyreloopElements_t *zLoopElement = (zyreloopElements_t *)args;

    zyre_t *node = zyre_new("FIXME_TO_RENAME");

    //zLoopElement->node = node;


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


    if (verbose) {
        zyre_set_verbose(node);
    }

    if (!node) {
        return;
    }

    zyre_start(node);
    zsock_signal(pipe, 0); //notify main thread that we are ready
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

    //zloop_poller (loop, &zpipePollItem, manageParent, args);
    //zloop_poller_set_tolerant(loop, &zpipePollItem);
    zloop_poller (loop, &zyrePollItem, manageIncoming, args);
    zloop_poller_set_tolerant(loop, &zyrePollItem);

    if (paramText != NULL && strlen(paramText) > 0){
        zloop_timer(loop, 10, 1, triggerMessageSend, (void *)args);
    }

    // start returns when one of the pollers returns -1
    zloop_start(loop);

    printf("shutting down...\n");

    // clean
    zloop_destroy(&loop);
    assert(loop == NULL);

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
    zyre_stop(node);
    zclock_sleep(100);
    zyre_destroy(&node);

    free(zLoopElement);
}*/



/**
 * @brief Constructor
 * @param agentName
 * @param subscriberAddress
 * @param parent
 */
LogStreamController::LogStreamController(QString agentName,
                                         QString subscriberAddress,
                                         QObject *parent) : QObject(parent),
    _agentName(agentName),
    _subscriberAddress(subscriberAddress)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Log Stream Controller for" << _agentName << "on" << _subscriberAddress;

    /*zactor_t *zActor = NULL;

    zyreloopElements_t *zLoopElement = calloc(1, sizeof(zyreloopElements_t));
    assert(zLoopElement);

    //zLoopElement->name = strdup(name);
    //zLoopElement->useGossip = true;
    //zLoopElement->agents = NULL;

    zActor = zactor_new(zyre_actor, zLoopElement);
    assert(zActor);*/




    /*//agent *a = NULL;
    zyreAgent_t *a = NULL;
    //zyreloopElements_t *a = NULL;
    //subscriber_t *a = NULL;

    a->subscriber = zsock_new_sub(subscriberAddress.toStdString().c_str(), NULL);

    // Subscribe to all
    zsock_set_subscribe(a->subscriber, "");

    a->subscriberPoller = calloc(1, sizeof(zmq_pollitem_t));
    zmq_pollitem_t *poller = a->subscriberPoller;

    poller->socket = zsock_resolve(a->subscriber);
    poller->fd = 0;
    poller->events = ZMQ_POLLIN;
    poller->revents = 0;

    zloop_poller(loop, poller, manageSubscription, (void*)a);
    zloop_poller_set_tolerant(loop, poller);

    printf("Subscriber created for %s\n", a->name);*/
}


/**
 * @brief Destructor
 */
LogStreamController::~LogStreamController()
{
    qInfo() << "Delete Log Stream Controller for" << _agentName << "on" << _subscriberAddress;
}
