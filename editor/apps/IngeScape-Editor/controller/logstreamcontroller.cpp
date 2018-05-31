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


/**
 * @brief Incoming event callback from the parent thread
 * @param loop
 * @param item
 * @param args
 * @return
 */
int incomingEventFromParentThread(zloop_t *loop, zmq_pollitem_t *item, void *args)
{
    Q_UNUSED(loop)
    Q_UNUSED(args)

    if (item->revents & ZMQ_POLLIN)
    {
        zmsg_t *msg = zmsg_recv((zsock_t *)item->socket);

        if (!msg) {
            printf("Error while reading message from main thread... exiting.");

            // Interrupted
            exit(EXIT_FAILURE);
        }

        char *command = zmsg_popstr(msg);
        /*if (streq(command, "$TERM")) {
            return -1;
        }*/
        if (streq(command, "STOP")) {
            return -1;
        }
        else {
            // Nothing to do so far
        }

        free(command);
        zmsg_destroy(&msg);
    }
    return 0;
}


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

    if (item->revents & ZMQ_POLLIN)
    {
        if ((item != NULL) && (args != NULL))
        {
            LogStreamController* logStreamC = (LogStreamController*)args;
            if (logStreamC != NULL)
            {
                zmsg_t *msg = zmsg_recv((zsock_t *)item->socket);

                if (!msg) {
                    printf("Error while reading message from agent.");

                    // Interrupted
                    //exit(EXIT_FAILURE);
                }

                char *message = zmsg_popstr(msg);

                QString log = QString(message);
                if (!log.isEmpty())
                {
                    //qDebug() << "Incoming log:" << log;

                    //Q_EMIT logStreamC->logReceived(message);

                    Q_EMIT logStreamC->logReceived(QDateTime::currentDateTime(), log.split(';'));
                }

                free(message);
                zmsg_destroy(&msg);
            }
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

            //zloop_set_verbose(loop, false);

            // New Zyre Socket
            zsock_t *zSocket = zsock_new_sub(subscriberAddress, NULL);

            // Subscribe to all
            zsock_set_subscribe(zSocket, "");

            // Init zpipe poll item
            zmq_pollitem_t zpipePollItem;
            void *zpipe = zsock_resolve(pipe);
            if (zpipe == NULL) {
                printf("Error: could not get the pipe descriptor for polling... exiting.\n");
                exit(EXIT_FAILURE);
            }
            zpipePollItem.socket = zpipe;
            zpipePollItem.fd = 0;
            zpipePollItem.events = ZMQ_POLLIN;
            zpipePollItem.revents = 0;

            zloop_poller(loop, &zpipePollItem, incomingEventFromParentThread, NULL);
            zloop_poller_set_tolerant(loop, &zpipePollItem);

            // Init zyre poll item
            zmq_pollitem_t zyrePollItem;
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

            zloop_poller_end(loop, &zyrePollItem);

            zsock_destroy(&zSocket);
            zSocket = NULL;

            // Clean
            zloop_destroy(&loop);
            assert(loop == NULL);

            //free(subscriberAddress);
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
    _subscriberAddress(subscriberAddress),
    _zActor(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Log Stream Controller for" << _agentName << "on" << _subscriberAddress;

    connect(this, &LogStreamController::logReceived, this, &LogStreamController::_onLogReceived);

    // Create a new zactor and register the callback
    _zActor = zactor_new(zactorCallback, this);
    assert(_zActor);
}


/**
 * @brief Destructor
 */
LogStreamController::~LogStreamController()
{
    qInfo() << "Delete Log Stream Controller for" << _agentName << "on" << _subscriberAddress;

    _logs.deleteAllItems();

    if (_zActor != NULL)
    {
        zstr_send(_zActor, "STOP");

        //zactor_destroy(&_zActor);
        //_zActor = NULL;
    }
}


/**
 * @brief Slot called when a log has been received from the agent
 * @param logDateTime
 * @param parametersOfLog
 */
void LogStreamController::_onLogReceived(QDateTime logDateTime, QStringList parametersOfLog)
{
    if (parametersOfLog.count() > 1)
    {
        QString logType = parametersOfLog.first();
        parametersOfLog.removeFirst();

        QString logContent = parametersOfLog.join(' ');

        // Create a new log
        LogM* log = new LogM(logDateTime, logType, logContent, this);

        // Add to the list
        _logs.prepend(log);
    }
}
