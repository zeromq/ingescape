/*
 *	IngeScape Editor
 *
 *  Copyright (c) 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort    <lemort@ingenuity.io>
 *
 */

#include "terminationsignalwatcher.h"




/**
 * @brief Constructor
 * @param parent
 */
TerminationSignalWatcher::TerminationSignalWatcher(QObject *parent) : QObject(parent)
{
    //
    // Unix
    //
#ifdef Q_OS_UNIX
    _socketNotifier = NULL;

    // Create socket pair
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, _sockpair))
    {
        qCritical() << Q_FUNC_INFO << ": socketpair error " << strerror(errno);
        return;
    }

    // Create a socket notifier
    _socketNotifier = new QSocketNotifier(_sockpair[1], QSocketNotifier::Read, this);
    // - subscribe and enable
    if (_socketNotifier != NULL)
    {
        QObject::connect(_socketNotifier, &QSocketNotifier::activated, this, &TerminationSignalWatcher::_onSocketNotifierActivated);
        _socketNotifier->setEnabled(true);
    }

    // Subscribe to signals
    // - save previous handlers
    sigaction (SIGINT, NULL, &_defaultHandlerSIGINT);
    sigaction (SIGTERM, NULL, &_defaultHandlerSIGTERM);

    // - register a new sigaction for
    struct sigaction sigact;
    sigact.sa_handler = TerminationSignalWatcher::unixSignalHandler;
    sigact.sa_flags = 0;
    sigemptyset(&sigact.sa_mask);

    sigaction(SIGINT, &sigact, NULL);
    sigaction(SIGTERM, &sigact, NULL);

#endif


    //
    // Windows
    //
#ifdef Q_OS_WIN
    // TODO

#endif
}


/**
 * @brief Destructor
 */
TerminationSignalWatcher::~TerminationSignalWatcher()
{
    //
    // Unix specific clean-up
    //
#ifdef Q_OS_UNIX
    // Restore default handlers
    sigaction (SIGINT, &_defaultHandlerSIGINT, NULL);
    sigaction (SIGTERM, &_defaultHandlerSIGTERM, NULL);
    _defaultHandlerSIGINT.sa_handler = NULL;
    _defaultHandlerSIGTERM.sa_handler = NULL;

    // Clean-up our socket notifier
    if (_socketNotifier != NULL)
    {
        disconnect(_socketNotifier, 0, this, 0);
        delete _socketNotifier;
        _socketNotifier = NULL;
    }

#endif


    //
    // Window specific clean-up
    //
#ifdef Q_OS_WIN
    // TODO
#endif
}




/**
 *--------------------------------------------------------------------------------
 *
 * Unix specific API
 *
 *--------------------------------------------------------------------------------
 */

#ifdef Q_OS_UNIX

// init our static variables
int TerminationSignalWatcher::_sockpair[2];
struct sigaction TerminationSignalWatcher::_defaultHandlerSIGINT;
struct sigaction TerminationSignalWatcher::_defaultHandlerSIGTERM;


/**
 * @brief sigaction handler
 * @param signal
 */
void TerminationSignalWatcher::unixSignalHandler(int signal)
{
    ssize_t bytes = write(_sockpair[0], &signal, sizeof(signal));
    Q_UNUSED(bytes);
}


/**
 * @brief Called when our socket notifier is activated
 * @param sockfd
 */
void TerminationSignalWatcher::_onSocketNotifierActivated(int sockfd)
{
    int signal;
    ssize_t nBytes = ::read(sockfd, &signal, sizeof(signal));
    Q_UNUSED(nBytes);

    Q_EMIT terminationSignal();
}

#endif


/**
 *--------------------------------------------------------------------------------
 *
 * Windows specific API
 *
 *--------------------------------------------------------------------------------
 */

#ifdef Q_OS_WIN
// TODO
#endif
