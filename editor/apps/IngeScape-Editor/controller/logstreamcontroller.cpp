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

bool verbose = false;


/**
 * @brief Incoming event callback
 * @param loop
 * @param item
 * @param args
 * @return
 */
int incomingEventCallback(zloop_t *loop, zmq_pollitem_t *item, void *args)
{
    Q_UNUSED(loop)

    if ((item != NULL) && (args != NULL))
    {
        //if (item->revents & ZMQ_POLLIN)

        LogStreamController* logStreamC = (LogStreamController*)args;
        if (logStreamC != NULL)
        {
            zmsg_t *msg = zmsg_recv((zsock_t *)item->socket);

            QString log = zmsg_popstr(msg);
            //qDebug() << "Incoming event:" << log;

            Q_EMIT logStreamC->logReceived(log);

            free(msg);
        }
    }

    return 0;
}


/**
 * @brief zactor callback
 * @param pipe
 * @param args
 */
static void zactorCallback(zsock_t *pipe, void *args)
{
    if (args != NULL)
    {
        LogStreamController* logStreamC = (LogStreamController*)args;
        if (logStreamC != NULL)
        {
            // Make a copy of the "char*"
            char *subscriberAddress = strdup(logStreamC->subscriberAddress().toStdString().c_str());

            // Notify main thread that we are ready
            zsock_signal(pipe, 0);

            // New Zyre LOOP
            zloop_t *loop = zloop_new();
            assert(loop);

            zloop_set_verbose(loop, verbose);

            // New Zyre Socket
            zsock_t *zSocket = zsock_new_sub(subscriberAddress, NULL);

            // Subscribe to all
            zsock_set_subscribe(zSocket, "");


            // Init Zyre poll item
            zmq_pollitem_t zyrePollItem;
            //zyrePollItem.socket = zSocket;
            zyrePollItem.socket = zsock_resolve(zSocket);
            zyrePollItem.fd = 0;
            zyrePollItem.events = ZMQ_POLLIN;
            zyrePollItem.revents = 0;

            // Register the callback about "incoming event"
            zloop_poller(loop, &zyrePollItem, incomingEventCallback, args);
            zloop_poller_set_tolerant(loop, &zyrePollItem);


            // Start returns when one of the pollers returns -1
            zloop_start(loop);

            printf("shutting down...\n");

            // Clean
            zloop_destroy(&loop);
            assert(loop == NULL);

            //free(subscriberAddress);

            // Free zSocket ?
        }
    }
}


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

    connect(this, &LogStreamController::logReceived, this, &LogStreamController::_onLogReceived);

    // Create a new zactor and register the callback
    zactor_t *zActor = zactor_new(zactorCallback, this);
    assert(zActor);
}


/**
 * @brief Destructor
 */
LogStreamController::~LogStreamController()
{
    qInfo() << "Delete Log Stream Controller for" << _agentName << "on" << _subscriberAddress;
}


/**
 * @brief Slot called when a log has been received from the agent
 * @param log
 */
void LogStreamController::_onLogReceived(QString log)
{
    if (!log.isEmpty())
    {
        QStringList newLogs = _logs;
        newLogs.prepend(log);

        setlogs(newLogs);
    }
}
